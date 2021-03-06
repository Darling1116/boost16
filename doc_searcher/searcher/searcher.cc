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


//������������
const DocInfo *Index::GetDocInfo(const int64_t doc_id){
	if(doc_id < 0 || doc_id >= forward_index.size()){
		return nullptr;
    }
    return &forward_index[doc_id];
}


//�鵹������
const InvertedList *Index::GetInvertedList(const string& key){
     auto it = inverted_index.find(key);
     if(it == inverted_index.end()){
         return nullptr;
     }
     return &(it->second);
    }


 // ��������
bool Index::Build(const string& input_path){
//���ж�ȡ�����ļ����ݣ���Ԥ����ģ�����ɵ�raw_input�ļ�
	std::cerr <<input_path<<"��ʼ��������"<< std::endl;
    std::ifstream file(input_path.c_str());
    if(!file.is_open()){
        std::cout <<input_path<< "raw_input�ļ���ʧ��" << std::endl;
		return false;
	}

	string line;//���ж�ȡ

	while(std::getline(file,line)){
		//��Ե�ǰ�����ݣ�������DocInfo���󣬲�������������
        DocInfo* doc_info = BuildForward(line);
        if(doc_info == nullptr){
             cout<< "������������ʧ��"<<endl;
             continue;
        }
		//���ݵ�ǰ��DocInfo���󣬽��н�����������ɵ�������
		BuildInverted(*doc_info);
		
		//�˴�ѭ�����ܻ�Ӱ�����������ִ��Ч�ʣ�Ҫ��������Ӱ��
		if (doc_info->doc_id % 100 == 0){
			std::cerr << doc_info->doc_id << std::endl;
		}
		
    }
	std::cerr <<"������������ɹ�"<< std::endl;
    file.close();
    return true;
}


//����һ�� Ԥ���� �������ļ����õ�һ�����������Ľڵ�,�����뵽����������
DocInfo *Index::BuildForward(const string& line){
// ����\3��line�����з֣�����Ϊ title url content
        vector<string> tokes;
        common::Util::Split(line,"\3",&tokes);
        if(tokes.size() != 3){//�зֽ������3�ݣ��ͱ�ʾ���д�������
            cout<<"�ļ�����ʧ��"<< tokes.size()<<endl;
            return nullptr;
        }
		//���зֽ���ŵ�doc_info��
        DocInfo doc_info;
        doc_info.doc_id     = forward_index.size();
        doc_info.title     = tokes[0];
        doc_info.url       = tokes[1];
        doc_info.content   = tokes[2];
        forward_index.push_back(std::move(doc_info)); //////////
		//vector��Ĭ�ϴ�ŵ������ݣ��Ƕ���ĸ�����Ҫ��DocInfo�����������push_back
		//��move�ɰ�doc_info�����ƶ���vector�У������˿�������

		//���ؽ��
        return &forward_index.back();///////
		//����C++�ľ������
		//����return &doc_info���ı�����ʱ�����ĵ�ַ��������������ͱ��ͷ��ˣ���ʱ�����ⲿ�õ���ΪҰָ�룬δ����
}


//�ִʵ�ʱ��Ҫͬʱ���Ǳ�������ģ�����Weight�����ʱ��Ҫ����Ȩ���ֶ���μ���
//Ȩ��=������ֵĴ���*10+���ĳ��ֵĴ���
//������һ��hash��
void Index::BuildInverted(const DocInfo& doc_info){
       //����һ��ר������ͳ�ƹؼ��ֵĽṹ��¼ ���� �����ĵĳ��ִ���
        struct WordCnt {
            int title_cnt;
            int content_cnt;
            WordCnt()
                :title_cnt(0), content_cnt(0) {} 
        };

        unordered_map<string, WordCnt> word_cnt_map;

        //��Ա�����зִʣ������ִʽ����ͳ��ÿ���ʳ��ֵĴ���
        vector<string> title_token;
        CutWord(doc_info.title, &title_token);

        for(string word : title_token){//��֤word��һ�����޸ı���
            //ȫ��תΪСд
            boost::to_lower(word);
            ++word_cnt_map[word].title_cnt;
        }

        //������Ľ��зִʣ������ִʽ����ͳ��ÿ���ʳ��ֵĴ���
        vector<string> content_token;
        CutWord(doc_info.content,&content_token);
        for(string word : content_token){
            boost::to_lower(word);
            ++word_cnt_map[word].content_cnt;
        }

        //ͳ�ƽ�������뵽����������
        for(const auto& word_pair : word_cnt_map){
            Weight weight;
            weight.doc_id  = doc_info.doc_id;

            //�Զ��� Ȩֵ = 10 * titleCnt + contentCnt
            weight.weight = 10 * word_pair.second.title_cnt + word_pair.second.content_cnt;
            weight.word   = word_pair.first;

			//weight������뵽���������У���Ҫ���ҵ���Ӧ�ĵ���������Ȼ��׷�ӵ�����ĩβ����
            InvertedList &inverted_list = inverted_index[word_pair.first];
            inverted_list.push_back(std::move(weight));
        }
		
    }


void Index::CutWord(const string &input, vector<string> *output){
	jieba.CutForSearch(input,*output);
}




///////////////////////////////////////
//��ʼ��������ָ���ĵ�������
bool Searcher::Init(const string& input_path){
        return index->Build(input_path);
    }


//ָ���ı���������
bool Searcher::Search(const string& query,string* output){
        //�ִʣ���Բ�ѯ�ʽ��зִ�
        vector<string> tokens;
        index->CutWord(query, &tokens);

        //���������ݷִʵĽ�������е����������õ�����ĵ�
        vector<Weight> all_token_result;
        for(string word : tokens){
            boost::to_lower(word);
            //const vector<backwardIdx>*
            auto* inverted_list = index->GetInvertedList(word);
            if(inverted_list == nullptr){
                //  û������ؼ���
                continue;
            }
            //token�������������Ѷ������ϲ���һ�𣬲��ܽ���ͳһ������
            all_token_result.insert(all_token_result.end(), inverted_list->begin(), inverted_list->end());
        }


		//���򣺰Ѳ鵽���ĵ����������ϲ���һ�𲢰���Ȩ�ؽ��н�������
       std::sort(all_token_result.begin(), all_token_result.end(), [](const Weight& w1, const Weight& w2){return w1.weight > w2.weight;});
					//ʵ�ֽ�������
		

        //��װ����doc_info�����ݹ��������Ԥ�ڵĽ��
        Json::Value results;
		//ÿ�������������һ��JSON
        for(const auto& weight : all_token_result){
            //����weight�е�doc_id������������
            const DocInfo* doc_info = index->GetDocInfo(weight.doc_id);
			//��doc_info�����ٽ�һ����װ��һ��JSON����
            Json::Value result;
            result["title"]    = doc_info->title;
            result["url"]      = doc_info->url;
            result["desc"]     = GenerateDesc(doc_info->content, weight.word);
            results.append(result); 
        }
		//�ѵõ���results���JSON�������л����ַ�����д��output��
        Json::FastWriter writer;
        *output = writer.write(results);
        return true;
   }



   //�õ��ؼ���ǰ������ݣ���ǰ��ҳ����ʾ���ı�
    string Searcher::GenerateDesc(const string& content,const string& word){

        size_t first_pos = content.find(word);
		size_t desc_beg = 0;    //��ʾ�ı���ʼ��λ��

        if (first_pos == string::npos){
            //�ؼ��ֲ�����,�ʹ�ͷ��ʼ��Ϊ��ʼλ��
			if (content.size() < 160){
				return content;
			}
			string desc = content.substr(0, 160);
			desc[desc.size() -1 ] = '.';
			desc[desc.size() -2 ] = '.';
			desc[desc.size() -3 ] = '.';
			return desc;
		}
		//�ؼ����ҵ��ˣ�����first_posΪ��׼����ǰ��һЩ�ֽ�
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