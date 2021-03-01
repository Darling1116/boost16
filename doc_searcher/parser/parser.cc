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

//��ȡ������Boost�ĵ���.html���ݣ����հѽ�����Ϊһ�����ı��ļ�

//boost �� .html�ļ���·��
string g_input_path = "../data/input/";

//�������ÿ��html�ļ������ݣ�����ŵ�·��
string g_output_path = "../data/tmp/raw_input.txt";

// �����ĵ���·�� ����ǰ׺
string g_url_head = "https://www.boost.org/doc/libs/1_53_0/doc/";


//����һ���ṹ�壬��ʾһ���ĵ���html��
struct DocInfo{
	string title;
	string url;
	string content;
};


bool EnumFile(const string &input_path, vector<string> *file_list){
	//ö��Ŀ¼Ҫ��boost�ĵ�
	namespace fs = boost::filesystem;//�����ռ�������
	fs::path root_path(input_path);
	if(!fs::exists(root_path)){
		std::cout<< "��ǰĿ¼������" <<std::endl;
		return false;
	}


	//������ʹ��ѭ��ʵ�ֵ�ʱ������Զ���ɵݹ�
	fs::recursive_directory_iterator end_iter;
	for(fs::recursive_directory_iterator iter(root_path); 
        iter != end_iter; ++iter){
            //��ǰ·��ΪĿ¼ʱ��ֱ������
            if(fs::is_regular_file(*iter) == false){
                continue;
            }
            //��ǰ�ļ����� .html �ļ���ֱ������������������ļ�Ҳ����
            if(iter->path().extension() != ".html"){
                continue;
            }
            //�õ���·�����뵽 ���ս����vector������
            file_list->push_back(iter->path().string());
        }
    
    return true;
}


//Ԥ������̵ĺ��ģ���inputĿ¼�����е�html·����ö�ٳ�����
//����ö�ٳ�����·�����ζ�ȡÿ���ļ����ݣ������н���
//�������д�����յ�����ļ�
int main( ){
	vector<string> file_list;
	bool ret = EnumFile(g_input_path, &file_list);
	if(!ret){
		std::cout << "ö��·��ʧ��" << std::endl;
		return 1;
	}
	//����ö�ٳ�����·�������ÿ���ļ����������д���
	for(const auto& file_path : file_list){
		std::cout<< file_path <<std::endl;
	}
	return 0;
}




