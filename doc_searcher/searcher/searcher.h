#pragma once


#include "cppjieba/Jieba.hpp"
#include "iostream"
#include "string"
#include "unordered_map"
#include "vector"
#include "stdint.h" 
#include "algorithm"
#include "fstream"

using namespace std;

using std::cout;
using std::endl;
using std::string;
using std::unordered_map;
using std::vector;


//定义一个结构，该结构是正排索引的基础：给定的doc_id映射到文档内容
namespace searcher{

struct DocInfo{
	int64_t doc_id;
	string title;
	string url;
	string content;
};

//定义一个结构，该结构是倒排索引的基础：映射到包含该词的文档的doc_id
 struct Weight{
     int64_t doc_id;
     int     weight;//对应的权重
     string  word;
    };


///////////////////////
////////倒排拉链:里面包含了若干个Weight结构
typedef vector<Weight> InvertedList;


//Index类用于整个索引结构，并且提供一些供外部调用的API
class Index{
     private:
        //正排索引:数组下标应用到doc_id
        vector<DocInfo> forward_index;
        //倒排索引:使用哈希表
        unordered_map<string, InvertedList> inverted_index;
        

	public:
		//提供一些对外调用的函数
		Index();
	    //查正排索引
        const DocInfo *GetDocInfo(int64_t doc_id);
        //查倒排索引
        const InvertedList *GetInvertedList(const string& key);
		//建立索引
        bool Build(const string& input_path);


     private:
        //根据一行 预处理 解析的文件，得到一个正排索引的节点
        DocInfo *BuildForward(const string& line);
        //根据正排索引节点，构造倒排索引节点
        void BuildInverted(const DocInfo& doc_info);
		void CutWord(const string& input, vector<string>* output);
		
		cppjieba::Jieba jieba;//////
    };



//索引模块






//搜索模块


}





