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


//�ҵ�html�е�title��ǩ
bool ParseTitle(const string& html,string* title){
    size_t begin = html.find("<title>");//��ʼ��ǩ
    if(begin == string::npos){
        cout<<"����û�ҵ�"<<endl;
        return false;
    }

    size_t end = html.find("</title>",begin);//������ǩ
    if(end == string::npos){
        cout<<"����û�ҵ�"<<endl;
        return false;
    }

    begin += string("<title>").size();
    if(begin >= end){
        cout<<"����λ�ò��Ϸ�"<<endl;
        return false;
    }

    *title = html.substr(begin,end - begin);
    return true;
}


//����url�����ݱ���·����ȡ�����ĵ���·��
// ����·������:
// ../data/input/html/thread.html
// ����·������:
// https://www.boost.org/doc/libs/1_53_0/doc/html/thread.html
//�ѱ���·���ĺ�벿�ֽ�ȡ��������ƴװ������·����ǰ׺
bool ParseUrl(const string& file_path,string* url){
	string url_head = "https://www.boost.org/doc/libs/1_53_0/doc/";
    string url_tail = file_path.substr(g_input_path.size());
    *url = g_url_head + url_tail;
    return true;
}


//��������:��< >Ϊ��ǽ��о�����ж�
bool ParseContent(const string& html,string* content){
    bool is_content = true;
    for(auto c : html){
        if(is_content){
            if(c == '<'){
                //������ǩ������������ݺ��ӵ�
                is_content = false;
            }
            else{
                if(c == '\n'){//ȥ���з�
                    c = ' ';
                }
				//������ͨ�ַ������д��coontent
                content->push_back(c);
            }
        }
        else{
            if(c == '>'){//��ǩ����
                is_content = true;
            }
			//���Ա�ǩ�е�����
        }
    }
    return true;
}


bool ParseFile(const string& file_path, DocInfo* doc_info){
	/*
	string html;
    bool ret = common::Util::Read(file_path, &html);//1.��ȡ�ļ�����
    if(ret == false){
        cout<< file_path << " file read error" <<endl;
        return false;
    }
	*/

	//����Read�����������Ѹú���������commonĿ¼��
	string html;
	bool ret = common::Util::Read(file_path, &html);
	if(!ret){
		std::cout << "�����ļ�ʧ��" << file_path << std::endl;
		return false;
	}

    ret = ParseTitle(html,&doc_info->title);//2.�����ļ����ݽ�������
    if(!ret){
        std::cout << "��������ʧ��" << std::endl;
        return false;
    }

    ret = ParseUrl(file_path,&doc_info->url);//3.�����ļ�·�����������Ӧ�����ĵ���url
    if(!ret){
        std::cout << "����urlʧ�� " << std::endl;
        return false;
    }

    ret = ParseContent(html,&doc_info->content);
	//4.�����ļ����ݣ�����ȥ��ǩ����Ϊdoc_info�е�content�ֶ�
    if(!ret){
        std::cout << "��������ʧ��" << std::endl;
        return false;
    }
    return true;
	
}

//ofstream����û�п�����������ģ�ֻ�ܴ����û�ָ��
//�˴�Ҳ������const���ã������޷�ִ�������д�ļ�����
void WriteOutput(const DocInfo& doc_info, std::ofstream& ofstream){
    ofstream << doc_info.title << "\3" << doc_info.url
            << "\3" << doc_info.content << std::endl;
}
	//�����ճ�����⣺��TCP����ֱ����أ����Ǻ������ֽ�����أ��ļ����ܵ���TCP���������ֽ�����




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
	std::ofstream output_file(g_output_path.c_str());
	if(!output_file.is_open()){
		std::cout << "��output�ļ�ʧ��" << std::endl;
		return 1;
	}


	for(const auto& file_path : file_list){
		std::cout<< file_path <<std::endl;

		DocInfo doc_info;
		ret = ParseFile(file_path, &doc_info);
		//ͨ��ParseFile�� ����������˴��Ľ�������
		if(!ret){
			std::cout << "�������ļ�ʧ��" << file_path << std::endl;
			continue;
		}

		//�ѽ��������Ľ��д�뵽�����ļ�
		WriteOutput(doc_info, output_file);
	}
	output_file.close();
	return 0;
}




