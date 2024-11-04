#pragma once

#include "LexicalAnalyzer.h"


class SyntacticAnalyzer {
public:
    SyntacticAnalyzer(LexicalAnalyzer* lexer);

private:
    LexicalAnalyzer *lexer_;
    Token lex_;

    void getLex();

    void exp14();

    void literal();

    void A();

    void exp1();

    bool isOp2();

    void exp2();

    bool isOp3();

    void exp3();

    void exp4();

    bool isOp5();

    void exp5();

    bool isOp6();

    void exp6();

    bool isOp7();

    void exp7();

    void exp8();

    void exp9();

    void exp10();

    void exp11();

    void exp12();

    void exp13();

    void ifStatement();

    void whileStatement();

    void forStatement();

    void switchItem();

    void switchStatement();

    void returnStatement();

    void block();

    void type();

    void var();

    void varDefinition();

    void statement();
};