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
			//负责从指定的路径中，读取文件的整体内容，读到output这个string里
            static bool Read(const string& input_path,string* output){
                std::ifstream file(input_path.c_str());
                if(!file.is_open()){
                    return false;
                }
				//把读取到的每行结果追加到Output中
                string line;
                while(std::getline(file,line)){
                    *output += (line + "\n");
                }
				//getline把读取到的文件放到line中，并返回true
                file.close();
                return true;
            }

			//基于Boost中的字符串切分，然后封装，delimiter表示分隔符
            static void Split(const string& input,const string& delimiter,
                        vector<string>* output){
                boost::split(*output,input,boost::is_any_of(delimiter),
                            boost::token_compress_off);
            }
    };
}