#include "include/SyntacticAnalyzer.h"
#include "include/TIDTree.h"
#include "include/Interpreter.h"

int main() {
    std::string file_path = "../main.y";
//    std::cin >> file_path;

    std::ifstream file_fin(file_path);
    file_fin.seekg(0, std::ios::end);
    std::streamsize file_size = file_fin.tellg();
    file_fin.seekg(0, std::ios::beg);

    auto file_text = new char[file_size];
    file_fin.read(file_text, file_size);

    auto generator = new PRNGenerator;

    LexicalAnalyzer *lexer;
    lexer = new LexicalAnalyzer(file_text, file_size, "../reserved-words.txt");

    SyntacticAnalyzer sintex(lexer, generator);

    sintex.startAnalysis();

    auto interpreter = new Interpreter(generator);
    interpreter->global();
    interpreter->callFunc("main", {});

    return 0;
}