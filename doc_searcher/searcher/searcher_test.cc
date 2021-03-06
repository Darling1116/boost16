#include "searcher.h"


int main ( ){
    searcher::Searcher searcher;
    bool ret = searcher.Init("../data/tmp/raw_input.txt");
    if (!ret) {
        std::cout << "Searcher ��ʼ��ʧ��" << std::endl;
		//std::flush�൱��fflush(stdout)
        return 1;
    }
    while (true) {
        std::cout << "searcher> " << std::flush;
        string query;
        std::cin >> query;
        if (!std::cin.good()) {
            std::cout << "goodbye" << std::endl;
            break;
        }
        string results;
        searcher.Search(query, &results);
        std::cout << results << std::endl;
    }

	return 0;
}
