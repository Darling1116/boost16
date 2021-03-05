FLAG=-std=c++11

demo:demo.cc
	g++ $^ -o $@ $(FLAG) -I /home/gxr116/jieba/cppjieba/include  -lboost_filesystem -lboost_system -ljsoncpp

.PHONY:clean
clean:
	rm demo