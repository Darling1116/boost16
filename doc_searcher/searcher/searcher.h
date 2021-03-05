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


//����һ���ṹ���ýṹ�����������Ļ�����������doc_idӳ�䵽�ĵ�����
namespace searcher{

struct DocInfo{
	int64_t doc_id;
	string title;
	string url;
	string content;
};

//����һ���ṹ���ýṹ�ǵ��������Ļ�����ӳ�䵽�����ôʵ��ĵ���doc_id
 struct Weight{
     int64_t doc_id;
     int     weight;//��Ӧ��Ȩ��
     string  word;
    };


///////////////////////
////////��������:������������ɸ�Weight�ṹ
typedef vector<Weight> InvertedList;


//Index���������������ṹ�������ṩһЩ���ⲿ���õ�API
class Index{
     private:
        //��������:�����±�Ӧ�õ�doc_id
        vector<DocInfo> forward_index;
        //��������:ʹ�ù�ϣ��
        unordered_map<string, InvertedList> inverted_index;
        

	public:
		//�ṩһЩ������õĺ���
		Index();
	    //����������
        const DocInfo *GetDocInfo(int64_t doc_id);
        //�鵹������
        const InvertedList *GetInvertedList(const string& key);
		//��������
        bool Build(const string& input_path);


     private:
        //����һ�� Ԥ���� �������ļ����õ�һ�����������Ľڵ�
        DocInfo *BuildForward(const string& line);
        //�������������ڵ㣬���쵹�������ڵ�
        void BuildInverted(const DocInfo& doc_info);
		void CutWord(const string& input, vector<string>* output);
		
		cppjieba::Jieba jieba;//////
    };



//����ģ��






//����ģ��


}





