#include "../../include/Starter.h"

int main() {
    std::string file_path;
    std::cin >> file_path;

    std::ifstream file_fin(file_path);
    file_fin.seekg(0, std::ios::end);
    std::streamsize file_size = file_fin.tellg();
    file_fin.seekg(0, std::ios::beg);

    auto file_text = new char[file_size];
    file_fin.read(file_text, file_size);

    Starter starter(file_text, file_size, "../../reserved-words.txt");

    auto tokens = starter.getAllTokens();

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
            {Token::Type::PlusOrMinusOperator, "plus or minus operator"},
            {Token::Type::FloatLiteral, "float literal"}
    };

    for (auto& token : tokens) {
        std::cout << descriptions[token.getType()] << ' ';
        std::cout << token.getContent() << ' ';
        std::cout << token.getLine() << ':' << token.getColumn() << std::endl;
    }

    return 0;
}
