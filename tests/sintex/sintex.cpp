#include "../../include/SyntacticAnalyzer.h"

int main() {
    std::string file_path;
    std::cin >> file_path;

    std::ifstream file_fin(file_path);
    file_fin.seekg(0, std::ios::end);
    std::streamsize file_size = file_fin.tellg();
    file_fin.seekg(0, std::ios::beg);

    auto file_text = new char[file_size];
    file_fin.read(file_text, file_size);

    auto generator = new PRNGenerator;

    LexicalAnalyzer *lexer;
    lexer = new LexicalAnalyzer(file_text, file_size, "../../reserved-words.txt");

    SyntacticAnalyzer sintex(lexer, generator);

    try {
        sintex.startAnalysis();
    } catch (const Token& e) {
        std::cout << e.what();
        return 0;
    }
    std::cout << "OK";
    return 0;
}
