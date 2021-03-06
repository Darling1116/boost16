#include "searcher.h"

#include "boost/algorithm/string/case_conv.hpp"
#include "../common/util.hpp"

#include "fstream"
#include "string"
#include "algorithm"

#include "jsoncpp/json/json.h"

namespace searcher{

const char* const DICT_PATH = "../jieba_dict/jieba.dict.utf8";
const char* const HMM_PATH = "../jieba_dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "../jieba_dict/user.dict.utf8";
const char* const IDF_PATH = "../jieba_dict/idf.utf8";
const char* const STOP_WORD_PATH = "../jieba_dict/stop_words.utf8";

Index::Index() : jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH){
        //forward_index.clear();
        //inverted_index.clear();
}


//查找正排索引
const DocInfo *Index::GetDocInfo(const int64_t doc_id){
	if(doc_id < 0 || doc_id >= forward_index.size()){
		return nullptr;
    }
    return &forward_index[doc_id];
}


//查倒排索引
const InvertedList *Index::GetInvertedList(const string& key){
     auto it = inverted_index.find(key);
     if(it == inverted_index.end()){
         return nullptr;
     }
     return &(it->second);
    }


 // 建立索引
bool Index::Build(const string& input_path){
//按行读取输入文件内容，即预处理模块生成的raw_input文件
	std::cerr <<input_path<<"开始构建索引"<< std::endl;
    std::ifstream file(input_path.c_str());
    if(!file.is_open()){
        std::cout <<input_path<< "raw_input文件打开失败" << std::endl;
		return false;
	}

	string line;//按行读取

	while(std::getline(file,line)){
		//针对当前行数据，解析成DocInfo对象，并构造正排索引
        DocInfo* doc_info = BuildForward(line);
        if(doc_info == nullptr){
             cout<< "构造正排索引失败"<<endl;
             continue;
        }
		//根据当前的DocInfo对象，进行解析，并构造成倒排索引
		BuildInverted(*doc_info);
		
		//此处循环可能会影响整个程序的执行效率，要尽量减少影响
		if (doc_info->doc_id % 100 == 0){
			std::cerr << doc_info->doc_id << std::endl;
		}
		
    }
	std::cerr <<"正排索引构造成功"<< std::endl;
    file.close();
    return true;
}


//根据一行 预处理 解析的文件，得到一个正排索引的节点,并插入到正排数组中
DocInfo *Index::BuildForward(const string& line){
// 按照\3对line进行切分，依次为 title url content
        vector<string> tokes;
        common::Util::Split(line,"\3",&tokes);
        if(tokes.size() != 3){//切分结果不是3份，就表示该行存在问题
            cout<<"文件构造失败"<< tokes.size()<<endl;
            return nullptr;
        }
		//把切分结果放到doc_info中
        DocInfo doc_info;
        doc_info.doc_id     = forward_index.size();
        doc_info.title     = tokes[0];
        doc_info.url       = tokes[1];
        doc_info.content   = tokes[2];
        forward_index.push_back(std::move(doc_info)); //////////
		//vector中默认存放的是内容，是对象的副本：要把DocInfo拷贝构造才能push_back
		//用move可把doc_info对象移动到vector中，避免了拷贝构造

		//返回结果
        return &forward_index.back();///////
		//存在C++的经典错误
		//不能return &doc_info，改变了临时变量的地址，函数结束后其就被释放了，此时函数外部拿到的为野指针，未定义
}


//分词的时候，要同时考虑标题和正文；构造Weight对象的时候要考虑权重字段如何计算
//权重=标题出现的次数*10+正文出现的次数
//倒排是一个hash表
void Index::BuildInverted(const DocInfo& doc_info){
       //创建一个专门用来统计关键字的结构记录 标题 和正文的出现次数
        struct WordCnt {
            int title_cnt;
            int content_cnt;
            WordCnt()
                :title_cnt(0), content_cnt(0) {} 
        };

        unordered_map<string, WordCnt> word_cnt_map;

        //针对标题进行分词：遍历分词结果，统计每个词出现的次数
        vector<string> title_token;
        CutWord(doc_info.title, &title_token);

        for(string word : title_token){//保证word是一个可修改变量
            //全部转为小写
            boost::to_lower(word);
            ++word_cnt_map[word].title_cnt;
        }

        //针对正文进行分词：遍历分词结果，统计每个词出现的次数
        vector<string> content_token;
        CutWord(doc_info.content,&content_token);
        for(string word : content_token){
            boost::to_lower(word);
            ++word_cnt_map[word].content_cnt;
        }

        //统计结果，插入到倒排索引中
        for(const auto& word_pair : word_cnt_map){
            Weight weight;
            weight.doc_id  = doc_info.doc_id;

            //自定义 权值 = 10 * titleCnt + contentCnt
            weight.weight = 10 * word_pair.second.title_cnt + word_pair.second.content_cnt;
            weight.word   = word_pair.first;

			//weight对象插入到倒排索引中，需要先找到对应的倒排拉链，然后追加到拉链末尾即可
            InvertedList &inverted_list = inverted_index[word_pair.first];
            inverted_list.push_back(std::move(weight));
        }
		
    }


void Index::CutWord(const string &input, vector<string> *output){
	jieba.CutForSearch(input,*output);
}




///////////////////////////////////////
//初始化，构建指定文档的索引
bool Searcher::Init(const string& input_path){
        return index->Build(input_path);
    }


//指定文本进行搜索
bool Searcher::Search(const string& query,string* output){
        //分词：针对查询词进行分词
        vector<string> tokens;
        index->CutWord(query, &tokens);

        //触发：根据分词的结果，进行倒排索引，得到相关文档
        vector<Weight> all_token_result;
        for(string word : tokens){
            boost::to_lower(word);
            //const vector<backwardIdx>*
            auto* inverted_list = index->GetInvertedList(word);
            if(inverted_list == nullptr){
                //  没有这个关键词
                continue;
            }
            //token包含多个结果，把多个结果合并到一起，才能进行统一的排序
            all_token_result.insert(all_token_result.end(), inverted_list->begin(), inverted_list->end());
        }


		//排序：把查到的文档倒排拉链合并到一起并按照权重进行降序排序
       std::sort(all_token_result.begin(), all_token_result.end(), [](const Weight& w1, const Weight& w2){return w1.weight > w2.weight;});
					//实现降序排序
		

        //包装：把doc_info的内容构造成最终预期的结果
        Json::Value results;
		//每个搜索结果就是一个JSON
        for(const auto& weight : all_token_result){
            //根据weight中的doc_id查找正排索引
            const DocInfo* doc_info = index->GetDocInfo(weight.doc_id);
			//把doc_info对象再进一步包装成一个JSON对象
            Json::Value result;
            result["title"]    = doc_info->title;
            result["url"]      = doc_info->url;
            result["desc"]     = GenerateDesc(doc_info->content, weight.word);
            results.append(result); 
        }
		//把得到的results这个JSON对象序列化成字符串，写入output中
        Json::FastWriter writer;
        *output = writer.write(results);
        return true;
   }



   //得到关键字前后的数据，在前端页面显示的文本
    string Searcher::GenerateDesc(const string& content,const string& word){

        size_t first_pos = content.find(word);
		size_t desc_beg = 0;    //显示文本开始的位置

        if (first_pos == string::npos){
            //关键字不存在,就从头开始作为起始位置
			if (content.size() < 160){
				return content;
			}
			string desc = content.substr(0, 160);
			desc[desc.size() -1 ] = '.';
			desc[desc.size() -2 ] = '.';
			desc[desc.size() -3 ] = '.';
			return desc;
		}
		//关键字找到了，就以first_pos为基准，往前找一些字节
		desc_beg = first_pos < 60 ? 0 :first_pos - 60;
		if (desc_beg + 160 >= content.size()){
			return content.substr(desc_beg);
		}
		else{
			string desc = content.substr(desc_beg, 160);
			desc[desc.size() -1 ] = '.';
			desc[desc.size() -2 ] = '.';
			desc[desc.size() -3 ] = '.';
			return desc;
		}
    }




}