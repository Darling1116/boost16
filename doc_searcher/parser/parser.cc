#include "iostream"
#include "string"
#include "unordered_map"
#include "vector"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"



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
	for(const auto& file_path : file_list){
		std::cout<< file_path <<std::endl;
	}
	return 0;
}




