FLAG=-std=c++11  -I /home/gxr116/jieba/cppjieba/include -lpthread  -ljsoncpp

http_server:http_server.cc 
	g++ $^ -o $@ $(FLAG) 



.PHONY:clean
clean:
	rm http_server