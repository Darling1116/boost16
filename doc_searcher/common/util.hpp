#pragma once

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::unordered_map;
using std::vector;

namespace common{
    class Util{
        public:
			//�����ָ����·���У���ȡ�ļ����������ݣ�����output���string��
            static bool Read(const string& input_path,string* output){
                std::ifstream file(input_path.c_str());
                if(!file.is_open()){
                    return false;
                }
				//�Ѷ�ȡ����ÿ�н��׷�ӵ�Output��
                string line;
                while(std::getline(file,line)){
                    *output += (line + "\n");
                }
				//getline�Ѷ�ȡ�����ļ��ŵ�line�У�������true
                file.close();
                return true;
            }

			//����Boost�е��ַ����з֣�Ȼ���װ��delimiter��ʾ�ָ���
            static void Split(const string& input,const string& delimiter,
                        vector<string>* output){
                boost::split(*output,input,boost::is_any_of(delimiter),
                            boost::token_compress_off);
            }
    };
}