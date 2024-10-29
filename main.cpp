#include "include/Starter.h"

int main() {
    std::string file_path;
    std::cin >> file_path;

    std::ifstream file_fin(file_path);
    file_fin.seekg(0, std::ios::end);
    std::streamsize file_size = file_fin.tellg();
    file_fin.seekg(0, std::ios::beg);

    auto file_text = new char[file_size];
    file_fin.read(file_text, file_size);

    Starter starter(file_text, file_size, "../reserved-words.txt");

    return 0;
}
