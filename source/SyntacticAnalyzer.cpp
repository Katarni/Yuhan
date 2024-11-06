#include "../include/SyntacticAnalyzer.h"

SyntacticAnalyzer::SyntacticAnalyzer(LexicalAnalyzer *lexer) : lexer_(lexer) {}


void SyntacticAnalyzer::getLex() {
    lex_ = lexer_->getToken();
}

void SyntacticAnalyzer::exp14() {
    exp13();
    while (lex_.getType() == Token::Type::Comma) {
        getLex();
        exp13();
    }
}

void SyntacticAnalyzer::literal() {
    if (lex_.getType() != Token::Type::CharLiteral &&
        lex_.getType() != Token::Type::ExponentialLiteral &&
        lex_.getType() != Token::Type::FloatLiteral &&
        lex_.getType() != Token::Type::NumericLiteral &&
        lex_.getType() != Token::Type::StringLiteral) {
        throw lex_;
    }
    getLex();
}

void SyntacticAnalyzer::B() {
    for (;;) {
        if (lex_.getType() == Token::Type::Dot) {
            getLex();
            if (lex_.getType() != Token::Type::Identifier) {
                throw lex_;
            }
            getLex();
            if (lex_.getType() == Token::Type::OpenParenthesis) {
                getLex();
                if (lex_.getType() != Token::Type::CloseParenthesis) {
                    vars();
                }
                if (lex_.getType() != Token::Type::CloseParenthesis) {
                    throw lex_;
                }
                getLex();
            }
        } else if (lex_.getType() == Token::Type::OpenSquareBracket) {
            getLex();
            exp14();
            if (lex_.getType() != Token::Type::CloseSquareBracket) {
                throw lex_;
            }
            getLex();
        } else {
            break;
        }
    }
}

void SyntacticAnalyzer::exp1() {
    if (lex_.getType() == Token::Type::OpenParenthesis) {
        getLex();
        exp14();
        if (lex_.getType() != Token::Type::CloseParenthesis) {
            throw lex_;
        }
        getLex();
        B();
        return;
    }
    if (lex_.getType() == Token::Type::Identifier) {
        getLex();
        while (lex_.getType() == Token::Type::DoubleColon) {
            getLex();
            if (lex_.getType() != Token::Type::Identifier &&
                lex_.getType() != Token::Type::ReservedWord) {
                throw lex_;
            }
            getLex();
        }
        if (lex_.getType() == Token::Type::OpenParenthesis) {
            getLex();
            if (lex_.getType() != Token::Type::CloseParenthesis) {
                vars();
            }
            if (lex_.getType() != Token::Type::CloseParenthesis) {
                throw lex_;
            }
            getLex();
        }
        B();
        return;
    }
    literal();
}

bool SyntacticAnalyzer::isOp2() {
    if (lex_.getType() == Token::Type::LvalueUnaryOperator ||
        lex_.getType() == Token::Type::RvalueUnaryOperator ||
        lex_.getType() == Token::Type::PlusOrMinusOperator) {
        return true;
    }
    return false;
}

void SyntacticAnalyzer::exp2() {
    while (isOp2()) {
        getLex();
    }
    exp1();
}

bool SyntacticAnalyzer::isOp3() {
    if (lex_.getContent() == "*" ||
        lex_.getContent() == "/" ||
        lex_.getContent() == "%") {
        return true;
    }
    return false;
}

void SyntacticAnalyzer::exp3() {
    exp2();
    while (isOp3()) {
        getLex();
        exp2();
    }
}

void SyntacticAnalyzer::exp4() {
    exp3();
    while (lex_.getType() == Token::Type::PlusOrMinusOperator) {
        getLex();
        exp3();
    }
}

bool SyntacticAnalyzer::isOp5() {
    if (lex_.getContent() == ">>" || lex_.getContent() == "<<") {
        return true;
    }
    return false;
}

void SyntacticAnalyzer::exp5() {
    exp4();
    while (isOp5()) {
        getLex();
        exp4();
    }
}

bool SyntacticAnalyzer::isOp6() {
    if (lex_.getContent() == ">" ||
        lex_.getContent() == "<" ||
        lex_.getContent() == ">=" ||
        lex_.getContent() == "<=") {
        return true;
    }
    return false;
}

void SyntacticAnalyzer::exp6() {
    exp5();
    while (isOp6()) {
        getLex();
        exp5();
    }
}

bool SyntacticAnalyzer::isOp7() {
    if (lex_.getContent() == "==" ||
        lex_.getContent() == "!=") {
        return true;
    }
    return false;
}

void SyntacticAnalyzer::exp7() {
    exp6();
    while (isOp7()) {
        getLex();
        exp6();
    }
}

void SyntacticAnalyzer::exp8() {
    exp7();
    while (lex_.getContent() == "&") {
        getLex();
        exp7();
    }
}

void SyntacticAnalyzer::exp9() {
    exp8();
    while (lex_.getContent() == "^") {
        getLex();
        exp8();
    }
}

void SyntacticAnalyzer::exp10() {
    exp9();
    while (lex_.getContent() == "|") {
        getLex();
        exp9();
    }
}

void SyntacticAnalyzer::exp11() {
    exp10();
    while (lex_.getContent() == "and") {
        getLex();
        exp10();
    }
}

void SyntacticAnalyzer::exp12() {
    exp11();
    while (lex_.getContent() == "or") {
        getLex();
        exp11();
    }
}

void SyntacticAnalyzer::exp13() {
    exp12();
    while (lex_.getType() == Token::Type::LvalueBinaryOperator) {
        getLex();
        exp12();
    }
}

void SyntacticAnalyzer::ifStatement() {
    if (lex_.getContent() != "if") {
        throw lex_;
    }
    getLex();
    if (lex_.getType() != Token::Type::OpenParenthesis) {
        throw lex_;
    }
    getLex();
    exp14();
    if (lex_.getType() != Token::Type::CloseParenthesis) {
        throw lex_;
    }
    getLex();
    statement();
    if (lex_.getContent() != "else") {
        return;
    }
    getLex();
    statement();
}

void SyntacticAnalyzer::whileStatement() {
    if (lex_.getContent() != "while") {
        throw lex_;
    }
    getLex();
    if (lex_.getType() != Token::Type::OpenParenthesis) {
        throw lex_;
    }
    getLex();
    exp14();
    if (lex_.getType() != Token::Type::CloseParenthesis) {
        throw lex_;
    }
    getLex();
    statement();
}

void SyntacticAnalyzer::forStatement() {
    if (lex_.getContent() != "for") {
        throw lex_;
    }
    getLex();
    if (lex_.getType() != Token::Type::OpenParenthesis) {
        throw lex_;
    }
    getLex();
    if (lex_.getType() != Token::Type::Semicolon) {
        if (isType()) {
            varDefinition();
        } else {
            exp14();
            if (lex_.getType() != Token::Type::Semicolon) {
                throw lex_;
            }
            getLex();
        }
    }
    if (lex_.getType() != Token::Type::Semicolon) {
        exp14();
    }
    if (lex_.getType() != Token::Type::Semicolon) {
        throw lex_;
    }
    getLex();
    if (lex_.getType() != Token::Type::CloseParenthesis) {
        exp14();
    }
    if (lex_.getType() != Token::Type::CloseParenthesis) {
        throw lex_;
    }
    getLex();
    statement();
}

void SyntacticAnalyzer::switchItem() {
    if (lex_.getContent() != "case") {
        throw lex_;
    }
    getLex();
    literal();
    if (lex_.getContent() != ":") {
        throw lex_;
    }
    getLex();
    statement();
}

void SyntacticAnalyzer::switchStatement() {
    if (lex_.getContent() != "switch") {
        throw lex_;
    }
    getLex();
    if (lex_.getType() != Token::Type::OpenParenthesis) {
        throw lex_;
    }
    getLex();
    exp14();
    if (lex_.getType() != Token::Type::CloseParenthesis) {
        throw lex_;
    }
    getLex();
    if (lex_.getType() != Token::Type::OpenCurlyBrace) {
        throw lex_;
    }
    getLex();
    while (lex_.getContent() != "default" &&
           lex_.getType() != Token::Type::CloseCurlyBrace) {
        switchItem();
    }
    if (lex_.getContent() == "default") {
        getLex();
        if (lex_.getContent() != ":") {
            throw lex_;
        }
        getLex();
        statement();
    }
    if (lex_.getType() != Token::Type::CloseCurlyBrace) {
        throw lex_;
    }
    getLex();
}

void SyntacticAnalyzer::returnStatement() {
    if (lex_.getContent() != "return") {
        throw lex_;
    }
    getLex();
    if (lex_.getType() != Token::Type::Semicolon) {
        exp14();
    }
    if (lex_.getType() != Token::Type::Semicolon) {
        throw lex_;
    }
    getLex();
}

void SyntacticAnalyzer::block() {
    if (lex_.getType() != Token::Type::OpenCurlyBrace) {
        throw lex_;
    }
    getLex();
    while (lex_.getType() != Token::Type::CloseCurlyBrace) {
        statement();
    }
    getLex();
}

void SyntacticAnalyzer::varDefinition() {
    type();
    var();
    while (lex_.getType() == Token::Type::Comma) {
        getLex();
        var();
    }
    if (lex_.getType() != Token::Type::Semicolon) {
        throw lex_;
    }
    getLex();
}

void SyntacticAnalyzer::statement() {
    if (lex_.getContent() == "break" || lex_.getContent() == "continue") {
        getLex();
        if (lex_.getType() != Token::Type::Semicolon) {
            throw lex_;
        }
        getLex();
        return;
    }
    if (lex_.getContent() == "if") {
        ifStatement();
        return;
    }
    if (lex_.getContent() == "while") {
        whileStatement();
        return;
    }
    if (lex_.getContent() == "for") {
        forStatement();
        return;
    }
    if (lex_.getContent() == "switch") {
        switchStatement();
        return;
    }
    if (lex_.getContent() == "return") {
        returnStatement();
        return;
    }
    if (lex_.getType() == Token::Type::Semicolon) {
        getLex();
        return;
    }
    if (lex_.getType() == Token::Type::OpenCurlyBrace) {
        block();
        return;
    }
    if (isType()) {
        varDefinition();
        return;
    }
    exp14();
    if (lex_.getType() != Token::Type::Semicolon) {
        throw lex_;
    }
    getLex();
}

void SyntacticAnalyzer::type() {
    if (lex_.getContent() == "int" ||
        lex_.getContent() == "float" ||
        lex_.getContent() == "bool" ||
        lex_.getContent() == "char" ||
        lex_.getContent() == "string") {
        getLex();
        return;
    }
    if (lex_.getContent() == "array") {
        array();
        return;
    }
    if (lex_.getContent() != "typename") {
        throw lex_;
    }
    getLex();
    if (lex_.getContent() == "int" ||
        lex_.getContent() == "float" ||
        lex_.getContent() == "bool" ||
        lex_.getContent() == "char" ||
        lex_.getContent() == "string") {
        getLex();
        return;
    }
    if (lex_.getContent() == "array") {
        array();
        return;
    }
    if (lex_.getType() != Token::Type::Identifier) {
        throw lex_;
    }
    getLex();
    while (lex_.getContent() == "::") {
        getLex();
        if (lex_.getType() != Token::Type::Identifier &&
            lex_.getType() != Token::Type::ReservedWord) {
            throw lex_;
        }
        getLex();
    }
}

void SyntacticAnalyzer::var() {
    if (lex_.getType() != Token::Type::Identifier) {
        throw lex_;
    }
    getLex();
    if (lex_.getContent() != "=") {
        return;
    }
    getLex();
    exp12();
}

void SyntacticAnalyzer::function() {
    if (lex_.getContent() != "func") {
        throw lex_;
    }
    getLex();
    type();
    if (lex_.getType() != Token::Type::Identifier) {
        throw lex_;
    }
    getLex();
    if (lex_.getType() != Token::Type::OpenParenthesis) {
        throw lex_;
    }
    getLex();
    if (lex_.getType() == Token::Type::CloseParenthesis) {
        getLex();
        block();
        return;
    }
    funcVarDefinition();
    if (lex_.getType() != Token::Type::CloseParenthesis) {
        throw lex_;
    }
    getLex();
    block();
}

void SyntacticAnalyzer::funcVarDefinition() {
    type();
    if (lex_.getType() != Token::Type::Identifier) {
        throw lex_;
    }
    getLex();
    while (lex_.getType() == Token::Type::Comma) {
        getLex();
        type();
        if (lex_.getType() != Token::Type::Identifier) {
            throw lex_;
        }
        getLex();
    }
}

void SyntacticAnalyzer::structure() {
    if (lex_.getContent() != "struct") {
        throw lex_;
    }
    getLex();
    if (lex_.getType() != Token::Type::Identifier) {
        throw lex_;
    }
    getLex();
    if (lex_.getType() != Token::Type::OpenCurlyBrace) {
        throw lex_;
    }
    getLex();
    while (lex_.getType() != Token::Type::CloseCurlyBrace) {
        programBody();
    }
    getLex();
    if (lex_.getType() != Token::Type::Semicolon) {
        throw lex_;
    }
    getLex();
}

void SyntacticAnalyzer::programBody() {
    if (lex_.getContent() == "func") {
        function();
        return;
    }
    if (lex_.getContent() == "struct") {
        structure();
        return;
    }
    varDefinition();
}

void SyntacticAnalyzer::program() {
    includes();
    namespaces();
    while (lex_.getType() != Token::Type::EndOfFile) {
        programBody();
    }
}

void SyntacticAnalyzer::includes() {
    while (lex_.getContent() == "#include") {
        getLex();
        if (lex_.getType() != Token::Type::StringLiteral) {
            throw lex_;
        }
        getLex();
    }
}

void SyntacticAnalyzer::namespaces() {
    if (lex_.getContent() != "using") return;
    while (lex_.getContent() == "using") {
        getLex();
        if (lex_.getContent() != "namespace") {
            throw lex_;
        }
        getLex();
        identifierNamespace();
    }
    if (lex_.getType() != Token::Type::Semicolon) {
        throw lex_;
    }
    getLex();
}

void SyntacticAnalyzer::identifierNamespace() {
    if (lex_.getType() != Token::Type::Identifier) {
        throw lex_;
    }
    getLex();
    while (lex_.getType() == Token::Type::Dot) {
        getLex();
        if (lex_.getType() != Token::Type::Identifier) {
            throw lex_;
        }
        getLex();
    }
}

void SyntacticAnalyzer::startAnalysis() {
    getLex();
    program();
}

void SyntacticAnalyzer::array() {
    if (lex_.getContent() != "array") {
        throw lex_;
    }
    getLex();
    if (lex_.getContent() != "<") {
        throw lex_;
    }
    getLex();
    type();
    if (lex_.getType() != Token::Type::Comma) {
        throw lex_;
    }
    getLex();
    if (lex_.getType() != Token::Type::NumericLiteral) {
        throw lex_;
    }
    getLex();
    if (lex_.getContent() != ">") {
        throw lex_;
    }
    getLex();
}

bool SyntacticAnalyzer::isType() {
    if (lex_.getContent() == "int" ||
        lex_.getContent() == "float" ||
        lex_.getContent() == "bool" ||
        lex_.getContent() == "char" ||
        lex_.getContent() == "string" ||
        lex_.getContent() == "array" ||
        lex_.getContent() == "typename") {
        return true;
    }
    return false;
}

void SyntacticAnalyzer::vars() {
    exp13();
    while (lex_.getType() == Token::Type::Comma) {
        getLex();
        exp13();
    }
}


