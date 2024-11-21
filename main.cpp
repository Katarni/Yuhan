#include "include/SyntacticAnalyzer.h"
#include "include/TIDTree.h"

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
    std::cout << "OK" << std::endl;

    std::ofstream os("../prn-file.txt");

    for (int i = 0; i < generator->size(); ++i) {
        os << i << ' ';

        auto state = generator->getById(i);

        switch (state.second) {
            case PRNGenerator::PRNType::Identifier:
                os << "Identifier: " << std::get<Identifier>(state.first).getName();
                break;
            case PRNGenerator::PRNType::Operation:
                os << "Operator: " << std::get<Token>(state.first).getContent();
                break;
            case PRNGenerator::PRNType::Literal:
                // get val here
                os << "Literal: " << std::get<Literal>(state.first).getType().getName();
                break;
            case PRNGenerator::PRNType::Address:
                os << "Address: " << std::get<size_t>(state.first);
                break;
            case PRNGenerator::PRNType::Function:
                os << "Function: " << std::get<std::string>(state.first);
                break;
            case PRNGenerator::PRNType::FieldName:
                os << "Struct Field: " << std::get<std::string>(state.first);
                break;
            case PRNGenerator::PRNType::SystemValue:
                switch (std::get<PRNGenerator::SysVals>(state.first)) {
                    case PRNGenerator::SysVals::FuncEnd:
                        os << "Function End";
                        break;
                    case PRNGenerator::SysVals::FuncStart:
                        os << "Function Start";
                        break;
                    case PRNGenerator::SysVals::Semicolon:
                        os << "Semicolon";
                        break;
                    case PRNGenerator::SysVals::Return:
                        os << "Return";
                        break;
                    case PRNGenerator::SysVals::GoTo:
                        os << "GoTo";
                        break;
                    case PRNGenerator::SysVals::GoToByFalse:
                        os << "GoTo by False";
                        break;
                    case PRNGenerator::SysVals::Cmp:
                        os << "Switch Cmp";
                        break;
                }
                break;
        }

        os << std::endl;
    }

    os.close();

    return 0;
}