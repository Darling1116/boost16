#include "iostream"
#include "string"
#include "vector"
#include "stdint.h" 
#include "algorithm"
#include "fstream"
#include "unordered_map"
#include "../searcher/searcher.h"
#include "../searcher/searcher.cc"
#include "/home/gxr116/Downloads/cpp-httplib/httplib.h"
#include "jsoncpp/json/json.h"

using std::cout;
using std::endl;
using std::string;
using std::unordered_map;
using std::vector;

int main( ){
	using namespace httplib;

	//����һ��searcher����
	searcher::Searcher searcher;
    bool ret = searcher.Init("../data/tmp/raw_input.txt");
    if(!ret){
        std::cout << "Searcher��ʼ��ʧ��" << std::endl;
        return 1;
	}

	//����һ��Server����
	Server server;
	server.Get("/searcher", [&searcher](const Request& req, Response& resp){
		if(!req.has_param("query")){
			resp.set_content("�����������","text/plain;charset=utf-8");
			return;
			
		}
		string query = req.get_param_value("query");
		string results;
	    searcher.Search(query,&results);
		resp.set_content(results,"application/json;charset=utf-8");
		cout<<results<<endl;	
	});


	//���߷���������̬��Դ��wwwrootĿ¼��
	//������������ͨ������http://127.0.0.1:10001/index.html����ҳ��
	server.set_base_dir("./wwwroot");
	
	//����������
	server.listen("0.0.0.0",10001);
	return 0;
}