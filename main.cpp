#include "includes.h"

int main() {
    std::ifstream fin("../service-words.txt");
    fin.seekg(0, std::ios::end);
    size_t file_size = fin.tellg();
    fin.seekg(0, std::ios::beg);
    char * text = new char[file_size];
    fin.read(text, file_size);
    std::vector<std::pair<char *, char *>> words_ptrs;
    auto now_it = text;
    words_ptrs.emplace_back(now_it, now_it);
    int cnt_lines = 1;
    for (int i = 0; i < file_size; ++i) {
        if (*(text + i) == '\n' || *(text + i) == '\r' || *(text + i) == ' ' || *(text + i) == '\t') {
            words_ptrs[cnt_lines - 1].second = text + i - 1;
            if (i != file_size - 1) {
                words_ptrs.emplace_back((text + i + 1), (text + i + 1));
                cnt_lines++;
            }
        }
    }
    Trie bor;
    for (auto [start, end] : words_ptrs) {
        size_t length = end - start + 1;
        if (length == 0) continue;
        bor.insert(start, length);
    }
    delete[] text;
}