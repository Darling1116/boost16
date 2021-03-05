#include "searcher.cc"
#include "ostream"

int main( ){

	searcher::Index index;//�ȴ���һ��indexʵ��
    bool ret = index.Build("../data/tmp/raw_input.txt");
    if (!ret) {
        std::cout << "��������ʧ��" << std::endl;
        return 1;
    }
	//Ϊʲô�ļ�����ʧ�ܣ�����1��������-1
	//��Ϊmain�����ķ���ֵ�������Ǹ�ֵ���ǽ��̵��˳���


    //���������ɹ�, �͵��������е���غ���. (������+�鵹��)
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
