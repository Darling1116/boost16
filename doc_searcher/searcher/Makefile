FLAG=-std=c++11

.PHONY:all
all:index_test searcher_test libsaercher.so

index_test:searcher.cc index_test.cc
	g++ $^ -o $@ $(FLAG) -I /home/gxr116/jieba/cppjieba/include  -lboost_filesystem -lboost_system -ljsoncpp

searcher_test:searcher.cc searcher_test.cc
	g++ $^ -o $@ $(FLAG) -I /home/gxr116/jieba/cppjieba/include  -lboost_filesystem -lboost_system -ljsoncpp

libsaercher.so:searcher.cc searcher.h
	g++ searcher.cc -o $@ -shared -fPIC $(FLAG) -I /home/gxr116/jieba/cppjieba/include  -lboost_filesystem -lboost_system -ljsoncpp

.PHONY:clean
clean:
	rm index_test searcher_test libsaercher.so