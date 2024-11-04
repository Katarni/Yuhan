#include "../../include/Trie.h"

int main() {
    std::ifstream reserved_words_fin("../../reserved-words.txt");
    reserved_words_fin.seekg(0, std::ios::end);
    std::streamsize reserved_words_file_size = reserved_words_fin.tellg();
    reserved_words_fin.seekg(0, std::ios::beg);

    auto reserved_words_text = new char[reserved_words_file_size];
    reserved_words_fin.read(reserved_words_text, reserved_words_file_size);

    std::vector<std::pair<char *, size_t>> words_ptrs;
    words_ptrs.emplace_back(reserved_words_text, 0);
    for (int i = 0; i < reserved_words_file_size; ++i) {
        if (*(reserved_words_text + i) == '\n') {
            words_ptrs.back().second = reserved_words_text + i - words_ptrs.back().first;

            if (i != reserved_words_file_size - 1) {
                words_ptrs.emplace_back((reserved_words_text + i + 1), 0);
            }
        }
    }

    auto reserved_words_ = new Trie;
    for (auto [start, length]: words_ptrs) {
        if (length == 0) continue;

        reserved_words_->insert(start, length);
    }

    delete[] reserved_words_text;
    std::string find_word;
    std::cin >> find_word;

    if (reserved_words_->isInTrie(find_word)) {
        std::cout << "true";
    } else {
        std::cout << "false";
    }

    delete reserved_words_;
}
