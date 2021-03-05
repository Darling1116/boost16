#include "searcher.cc"
#include "ostream"

int main( ){

	searcher::Index index;//先创建一个index实例
    bool ret = index.Build("../data/tmp/raw_input.txt");
    if (!ret) {
        std::cout << "索引构建失败" << std::endl;
        return 1;
    }
	//为什么文件构造失败，返回1，而不是-1
	//因为main函数的返回值不可能是负值，是进程的退出码


    //索引构建成功, 就调用索引中的相关函数. (查正排+查倒排)
    auto* inverted_list = index.GetInvertedList("filesystem");
    for (const auto& weight : *inverted_list) {
        std::cout << "doc_id:" << weight.doc_id << "weight:" << weight.weight << std::endl;

        auto* doc_info = index.GetDocInfo(weight.doc_id);
        std::cout << "title:" << doc_info->title << std::endl;
        std::cout << "url:" << doc_info->url << std::endl;
        std::cout << "content:" << doc_info->content << std::endl;
        std::cout << "=============================================" << std::endl;
    }

	return 0;
}
