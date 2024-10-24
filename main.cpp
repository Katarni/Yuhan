#include "include/LexicalAnalyzer.h"

int main() {
    std::ifstream reserved_words_fin("../reserved-words.txt");
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

    auto reserved_words_trie = new Trie;
    for (auto [start, length]: words_ptrs) {
        if (length == 0) continue;

        reserved_words_trie->insert(start, length);
    }

    delete[] reserved_words_text;

    std::string file_path;
    std::cin >> file_path;

    std::ifstream file_fin(file_path);
    file_fin.seekg(0, std::ios::end);
    std::streamsize file_size = file_fin.tellg();
    file_fin.seekg(0, std::ios::beg);

    auto file_text = new char[file_size];
    file_fin.read(file_text, file_size);

    LexicalAnalyzer lexical_analyzer(reserved_words_trie, file_text, file_size);

    Token cur_token;
    Token eof;
    eof.setType(Token::Type::EndOfFile);

    std::map < Token::Type, std::string > descriptions = {
            {Token::Type::ReservedWord, "reserved word"},
            {Token::Type::Identifier, "identifier"},
            {Token::Type::LvalueBinaryOperator, "lvalue binary operator"},
            {Token::Type::RvalueBinaryOperator, "rvalue binary operator"},
            {Token::Type::Dot, "dot"},
            {Token::Type::Comma, "comma"},
            {Token::Type::NumericLiteral, "numeric literal"},
            {Token::Type::Semicolon, "semicolon"},
            {Token::Type::OpenParenthesis, "open parenthesis"},
            {Token::Type::CloseParenthesis, "close parenthesis"},
            {Token::Type::OpenCurlyBrace, "open curly brace"},
            {Token::Type::CloseCurlyBrace, "close curly brace"},
            {Token::Type::OpenSquareBracket, "open square bracket"},
            {Token::Type::CloseSquareBracket, "close square bracket"},
            {Token::Type::LvalueUnaryOperator, "lvalue unary operator"},
            {Token::Type::RvalueUnaryOperator, "rvalue unary operator"},
            {Token::Type::StringLiteral, "string literal"},
            {Token::Type::EndOfFile, "endl of file"},
            {Token::Type::Another, "another"},
            {Token::Type::ExponentialLiteral, "exponential literal"},
            {Token::Type::PlusOrMinusOperator, "plus or minus operator"}
    };

    std::ofstream tokens_fos("../tokens-file.txt");

    while (cur_token = lexical_analyzer.getToken(), cur_token != eof) {
        tokens_fos << "content: \"" << cur_token.getContent();
        tokens_fos << "\" type: \"" << descriptions[cur_token.getType()] << "\" in pos ";
        tokens_fos << cur_token.getLine() << ":" << cur_token.getColumn() << std::endl;
    }

    tokens_fos.close();

    return 0;
}
