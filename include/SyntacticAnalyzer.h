#pragma once

#include "LexicalAnalyzer.h"
#include "TIDTree.h"
#include "SemanticStack.h"


class SyntacticAnalyzer {
public:
    SyntacticAnalyzer(LexicalAnalyzer* lexer, PRNGenerator* generator);

    void startAnalysis();

private:
    LexicalAnalyzer *lexer_;
    Token lex_;
    TIDTree tid_tree_;
    SemanticStack sem_stack_;
    PRNGenerator* generator_;
    ReservedMemory* last_identifier_;

    void getLex();

    Type literal();

    void B();

    void exp1();
    void exp2();
    void exp3();
    void exp4();
    void exp5();
    void exp6();
    void exp7();
    void exp8();
    void exp9();
    void exp10();
    void exp11();
    void exp12();
    void exp13();
    void exp14();

    bool isOp2();
    bool isOp3();
    bool isOp5();
    bool isOp6();
    bool isOp7();

    void ifStatement();

    void whileStatement();
    void forStatement();

    void switchItem(Type &type_exp);
    void switchStatement();

    void returnStatement();

    void block();

    Type type();

    void var(Type type_var);
    void varDefinition();
    void vars(std::vector<Type>& args);

    void statement();

    void function();
    void funcVarDefinition(std::vector<Variable>& args);

    void structure();
    void structBody();

    void programBody();

    void program();

    void includes();

    void identifierNamespace();

    Type array();

    bool isType();
};
