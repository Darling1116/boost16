#include "iostream"
#include "string"
#include "unordered_map"
#include "vector"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

#include "../common/util.hpp"


using std::cout;
using std::endl;
using std::string;
using std::unordered_map;
using std::vector;

//读取并分析Boost文档的.html内容，最终把结果输出为一个行文本文件

//boost 中 .html文件的路径
string g_input_path = "../data/input/";

//解析后的每个html文件的内容，所存放的路径
string g_output_path = "../data/tmp/raw_input.txt";

// 在线文档的路径 公共前缀
string g_url_head = "https://www.boost.org/doc/libs/1_53_0/doc/";


//创建一个结构体，表示一个文档（html）
struct DocInfo{
	string title;
	string url;
	string content;
};


bool EnumFile(const string &input_path, vector<string> *file_list){
	//枚举目录要用boost文档
	namespace fs = boost::filesystem;//命名空间重命名
	fs::path root_path(input_path);
	if(!fs::exists(root_path)){
		std::cout<< "当前目录不存在" <<std::endl;
		return false;
	}


	//迭代器使用循环实现的时候可以自动完成递归
	fs::recursive_directory_iterator end_iter;
	for(fs::recursive_directory_iterator iter(root_path); 
        iter != end_iter; ++iter){
            //当前路径为目录时，直接跳过
            if(fs::is_regular_file(*iter) == false){
                continue;
            }
            //当前文件不是 .html 文件，直接跳过，如果是其他文件也跳过
            if(iter->path().extension() != ".html"){
                continue;
            }
            //得到的路径加入到 最终结果的vector数组中
            file_list->push_back(iter->path().string());
        }
    
    return true;
}


//找到html中的title标签
bool ParseTitle(const string& html,string* title){
    size_t begin = html.find("<title>");//开始标签
    if(begin == string::npos){
        cout<<"标题没找到"<<endl;
        return false;
    }

    size_t end = html.find("</title>",begin);//结束标签
    if(end == string::npos){
        cout<<"标题没找到"<<endl;
        return false;
    }

    begin += string("<title>").size();
    if(begin >= end){
        cout<<"标题位置不合法"<<endl;
        return false;
    }

    *title = html.substr(begin,end - begin);
    return true;
}


//解析url：根据本地路径获取在线文档的路径
// 本地路径形如:
// ../data/input/html/thread.html
// 在线路径形如:
// https://www.boost.org/doc/libs/1_53_0/doc/html/thread.html
//把本地路径的后半部分截取出来，再拼装上在线路径的前缀
bool ParseUrl(const string& file_path,string* url){
	string url_head = "https://www.boost.org/doc/libs/1_53_0/doc/";
    string url_tail = file_path.substr(g_input_path.size());
    *url = g_url_head + url_tail;
    return true;
}


//解析内容:以< >为标记进行具体的判断
bool ParseContent(const string& html,string* content){
    bool is_content = true;
    for(auto c : html){
        if(is_content){
            if(c == '<'){
                //遇到标签：把下面的内容忽视掉
                is_content = false;
            }
            else{
                if(c == '\n'){//去换行符
                    c = ' ';
                }
				//遇到普通字符，结果写入coontent
                content->push_back(c);
            }
        }
        else{
            if(c == '>'){//标签结束
                is_content = true;
            }
			//忽略标签中的内容
        }
    }
    return true;
}


bool ParseFile(const string& file_path, DocInfo* doc_info){
	/*
	string html;
    bool ret = common::Util::Read(file_path, &html);//1.读取文件内容
    if(ret == false){
        cout<< file_path << " file read error" <<endl;
        return false;
    }
	*/

	//构造Read（）函数，把该函数方法到common目录下
	string html;
	bool ret = common::Util::Read(file_path, &html);
	if(!ret){
		std::cout << "解析文件失败" << file_path << std::endl;
		return false;
	}

    ret = ParseTitle(html,&doc_info->title);//2.根据文件内容解析标题
    if(!ret){
        std::cout << "解析标题失败" << std::endl;
        return false;
    }

    ret = ParseUrl(file_path,&doc_info->url);//3.根据文件路径，构造出对应在线文档的url
    if(!ret){
        std::cout << "解析url失败 " << std::endl;
        return false;
    }

    ret = ParseContent(html,&doc_info->content);
	//4.根据文件内容，进行去标签，作为doc_info中的content字段
    if(!ret){
        std::cout << "解析内容失败" << std::endl;
        return false;
    }
    return true;
	
}

//ofstream类是没有拷贝构造哈数的，只能传引用或指针
//此处也不能是const引用，否则无法执行里面的写文件操作
void WriteOutput(const DocInfo& doc_info, std::ofstream& ofstream){
    ofstream << doc_info.title << "\3" << doc_info.url
            << "\3" << doc_info.content << std::endl;
}
	//会产生粘包问题：和TCP不是直接相关，而是和面向字节流相关，文件、管道、TCP都是面向字节流的




//预处理过程的核心：把input目录中所有的html路径都枚举出来；
//根据枚举出来的路径依次读取每个文件内容，并进行解析
//解析结果写入最终的输出文件
int main( ){
	vector<string> file_list;
	bool ret = EnumFile(g_input_path, &file_list);
	if(!ret){
		std::cout << "枚举路径失败" << std::endl;
		return 1;
	}


	//遍历枚举出来的路径，针对每个文件，单独进行处理
	std::ofstream output_file(g_output_path.c_str());
	if(!output_file.is_open()){
		std::cout << "打开output文件失败" << std::endl;
		return 1;
	}


	for(const auto& file_path : file_list){
		std::cout<< file_path <<std::endl;

		DocInfo doc_info;
		ret = ParseFile(file_path, &doc_info);
		//通过ParseFile（ ）函数负责此处的解析工作
		if(!ret){
			std::cout << "解析该文件失败" << file_path << std::endl;
			continue;
		}

		//把解析出来的结果写入到最终文件
		WriteOutput(doc_info, output_file);
	}
	output_file.close();
	return 0;
}




