#include "include/Starter.h"
#include "include/SyntacticAnalyzer.h"
#include "include/TIDTree.h"

int main() {
    std::string file_path = "test.txt";
//    std::cin >> file_path;

    std::ifstream file_fin(file_path);
    file_fin.seekg(0, std::ios::end);
    std::streamsize file_size = file_fin.tellg();
    file_fin.seekg(0, std::ios::beg);

    auto file_text = new char[file_size];
    file_fin.read(file_text, file_size);

//    Starter starter(file_text, file_size, "../reserved-words.txt");

//    LexicalAnalyzer *lexer;
//    lexer = new LexicalAnalyzer(file_text, file_size, "../reserved-words.txt");
//
//    SyntacticAnalyzer sintex(lexer);
//
//    sintex.startAnalysis();
//    std::cout << "OK";
    TIDTree tree;
    std::string name = "a", name2 = "b";
    name = name2;
    std::vector<std::pair<std::string, Type>> variables;
    variables = {{"a", Type("int", true)}, {"aa", Type("float", true)}, {"str", Type("string", true)}, {"arr", Type("array", true)}};
    variables[3].second.setArraySize(5);
    variables[3].second.setArrayType(new Type("string", true));
    for (auto & variable : variables) {
        tree.pushVariable(variable.first, variable.second);
    }
    for (auto & variable : variables) {
        std::cout << variable.first << ": " << tree.checkVariable(variable.first).getName() << "\n";
    }
    tree.createScope(false, true, "name");
    tree.pushVariable(variables[3].first, variables[2].second);
    std::cout << variables[3].first << ": " << tree.checkVariable(variables[3].first).getName() << "\n";
    tree.closeScope();
    std::cout << variables[3].first << ": " << tree.checkVariable(variables[3].first).getName() << "\n";
    name = "name::" + variables[3].first;
    std::cout << name << ": " << tree.checkVariable(name).getName() << "\n";
    std::string name_struct = "name_struct", name_struct2 = "name_struct2", name_struct3 = "name_struct3";
    tree.pushStruct(name_struct);
    tree.createScope(true, false, name_struct);
    tree.pushVariable(variables[3].first, variables[0].second);
    tree.pushStruct(name_struct2);
    tree.createScope(true, false, name_struct2);
    std::cout << variables[3].first << ": " << tree.checkVariable(variables[3].first).getName() << "\n";
    tree.closeScope();
    tree.pushStruct(name_struct3);
    tree.createScope(true, false, name_struct3);
    std::cout << name_struct2 << ": " << tree.checkStruct(name_struct2) << "\n";
    tree.closeScope();
    tree.closeScope();
    std::cout << "Field of " << name_struct << "." << variables[3].first << ": " << tree.checkField(name_struct, variables[3].first).getName() << "\n";
    tree.createScope();
    std::string name_func = "function";
    Type type("int", false);
    std::vector<Variable> vars_func = {Variable("x", Type("int", true))};
    tree.pushFunction(name_func, type, vars_func);
    tree.createScope();
    tree.closeScope();
    std::cout << name_func << ": " << tree.checkFunction(name_func, vars_func).getName() << "\n";
    return 0;
}
