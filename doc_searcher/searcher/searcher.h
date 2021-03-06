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



//////////////����ģ��/////////////
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
		//�ִʺ���
		void CutWord(const string& input, vector<string>* output);


     private:
        //����һ�� Ԥ���� �������ļ����õ�һ�����������Ľڵ�
        DocInfo *BuildForward(const string& line);
        //�������������ڵ㣬���쵹�������ڵ�
        void BuildInverted(const DocInfo& doc_info);
		


		
		cppjieba::Jieba jieba;
    };





//////////////����ģ��/////////////
class Searcher{
     private:
		 //��Ҫ������������
         Index* index;

         //�õ��ؼ���ǰ������ݣ���ǰ��ҳ����ʾ���ı�
         string GenerateDesc(const string& content, const string& word);

	 public:
         Searcher() : index(new Index())  { }
            //  ��ʼ��������ָ���ĵ�������
         bool Init(const string& input_path);
            //  ָ���ı���������
         bool Search(const string& query,string* results);
};


}





