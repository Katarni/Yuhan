#include "../include/SyntacticAnalyzer.h"

SyntacticAnalyzer::SyntacticAnalyzer(LexicalAnalyzer *lexer) : lexer_(lexer), tid_tree_() {}


void SyntacticAnalyzer::getLex() {
    lex_ = lexer_->getToken();
}

void SyntacticAnalyzer::exp14() {
    exp13();
    while (lex_.getType() == Token::Type::Comma) {
        sem_stack_.clear();
        getLex();
        exp13();
    }
}

Type SyntacticAnalyzer::literal() {
    if (lex_.getType() == Token::Type::CharLiteral) {
        getLex();
        return Type("char", false);
    }
    if (lex_.getType() == Token::Type::ExponentialLiteral ||
        lex_.getType() == Token::Type::FloatLiteral) {
        getLex();
        return Type("float", false);
    }
    if (lex_.getType() == Token::Type::NumericLiteral) {
        getLex();
        return Type("int", false);
    }
    if (lex_.getType() == Token::Type::StringLiteral) {
        getLex();
        return Type("string", false);
    }
    throw lex_;
}

void SyntacticAnalyzer::B() {
    for (;;) {
        if (lex_.getType() == Token::Type::OpenSquareBracket) {
            sem_stack_.push(lex_);
            getLex();
            exp14();
            sem_stack_.checkBinary();
            if (lex_.getType() != Token::Type::CloseSquareBracket) {
                throw lex_;
            }
            getLex();
        } else if (lex_.getType() == Token::Type::Dot) {
            getLex();
            if (lex_.getType() != Token::Type::Identifier) {
                throw lex_;
            }
            Type type_last = sem_stack_.popOperand();
            auto field = tid_tree_.checkField(type_last.getName(), lex_.getContent());
            field.setLvalue(true);
//            field.setName(type_last.getName() + "::" + field.getName());
            sem_stack_.push(field);
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
    if (lex_.getType() == Token::Type::Identifier ||
        lex_.getType() == Token::Type::NamespaceIdentifier) {
        auto name = lex_.getContent();
        getLex();
        if (lex_.getType() == Token::Type::OpenParenthesis) {
            getLex();
            std::vector<Type> args;
            if (lex_.getType() != Token::Type::CloseParenthesis) {
                vars(args);
            }
            if (lex_.getType() != Token::Type::CloseParenthesis) {
                throw lex_;
            }
            sem_stack_.push(tid_tree_.checkFunction(name, args));
            getLex();
        } else {
            Type type_val = tid_tree_.checkVariable(name);
            type_val.setLvalue(true);
            sem_stack_.push(type_val);
        }
        B();
        return;
    }
    sem_stack_.push(literal());
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
    int cnt_operations = 0;
    while (isOp2()) {
        sem_stack_.push(lex_);
        ++cnt_operations;
        getLex();
    }
    exp1();
    for (int i = 0; i < cnt_operations; ++i) {
        sem_stack_.checkUno();
    }
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
        sem_stack_.push(lex_);
        getLex();
        exp2();
        sem_stack_.checkBinary();
    }
}

void SyntacticAnalyzer::exp4() {
    exp3();
    while (lex_.getType() == Token::Type::PlusOrMinusOperator) {
        sem_stack_.push(lex_);
        getLex();
        exp3();
        sem_stack_.checkBinary();
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
        sem_stack_.push(lex_);
        getLex();
        exp4();
        sem_stack_.checkBinary();
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
        sem_stack_.push(lex_);
        getLex();
        exp5();
        sem_stack_.checkBinary();
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
        sem_stack_.push(lex_);
        getLex();
        exp6();
        sem_stack_.checkBinary();
    }
}

void SyntacticAnalyzer::exp8() {
    exp7();
    while (lex_.getContent() == "&") {
        sem_stack_.push(lex_);
        getLex();
        exp7();
        sem_stack_.checkBinary();
    }
}

void SyntacticAnalyzer::exp9() {
    exp8();
    while (lex_.getContent() == "^") {
        sem_stack_.push(lex_);
        getLex();
        exp8();
        sem_stack_.checkBinary();
    }
}

void SyntacticAnalyzer::exp10() {
    exp9();
    while (lex_.getContent() == "|") {
        sem_stack_.push(lex_);
        getLex();
        exp9();
        sem_stack_.checkBinary();
    }
}

void SyntacticAnalyzer::exp11() {
    exp10();
    while (lex_.getContent() == "and") {
        sem_stack_.push(lex_);
        getLex();
        exp10();
        sem_stack_.checkBinary();
    }
}

void SyntacticAnalyzer::exp12() {
    exp11();
    while (lex_.getContent() == "or") {
        sem_stack_.push(lex_);
        getLex();
        exp11();
        sem_stack_.checkBinary();
    }
}

void SyntacticAnalyzer::exp13() {
    int cnt_operations = 0;
    exp12();
    while (lex_.getType() == Token::Type::LvalueBinaryOperator) {
        sem_stack_.push(lex_);
        ++cnt_operations;
        getLex();
        exp12();
    }
    for (int i = 0; i < cnt_operations; ++i) {
        sem_stack_.checkBinary();
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
    sem_stack_.checkBool();
    if (lex_.getType() != Token::Type::CloseParenthesis) {
        throw lex_;
    }
    getLex();
    tid_tree_.createScope();
    statement();
    tid_tree_.closeScope();
    if (lex_.getContent() != "else") {
        return;
    }
    getLex();
    tid_tree_.createScope();
    statement();
    tid_tree_.closeScope();
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
    sem_stack_.checkBool();
    if (lex_.getType() != Token::Type::CloseParenthesis) {
        throw lex_;
    }
    getLex();
    tid_tree_.createScope(TypeScope::Loop);
    statement();
    tid_tree_.closeScope();
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
    tid_tree_.createScope(TypeScope::Loop);
    if (lex_.getType() != Token::Type::Semicolon) {
        if (isType()) {
            varDefinition();
        } else {
            exp14();
            sem_stack_.popOperand();
            if (lex_.getType() != Token::Type::Semicolon) {
                throw lex_;
            }
            getLex();
        }
    } else {
        getLex();
    }
    if (lex_.getType() != Token::Type::Semicolon) {
        exp14();
        sem_stack_.checkBool();
    }
    if (lex_.getType() != Token::Type::Semicolon) {
        throw lex_;
    }
    getLex();
    if (lex_.getType() != Token::Type::CloseParenthesis) {
        exp14();
        sem_stack_.popOperand();
    }
    if (lex_.getType() != Token::Type::CloseParenthesis) {
        throw lex_;
    }
    getLex();
    statement();
    tid_tree_.closeScope();
}

void SyntacticAnalyzer::switchItem(Type &type_exp) {
    if (lex_.getContent() != "case") {
        throw lex_;
    }
    getLex();
    if (!type_exp.compareWithCast(literal())) {
        throw std::runtime_error("Wrong type in case");
    }
    tid_tree_.createScope(TypeScope::SwitchItem);
    block();
    tid_tree_.closeScope();
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
    Type type_exp = sem_stack_.popOperand();
    if (lex_.getType() != Token::Type::CloseParenthesis) {
        throw lex_;
    }
    getLex();
    if (lex_.getType() != Token::Type::OpenCurlyBrace) {
        throw lex_;
    }
    tid_tree_.createScope(TypeScope::Switch);
    getLex();
    while (lex_.getContent() != "default" &&
           lex_.getType() != Token::Type::CloseCurlyBrace) {
        switchItem(type_exp);
    }
    if (lex_.getContent() == "default") {
        getLex();
        tid_tree_.createScope(TypeScope::SwitchItem);
        block();
        tid_tree_.closeScope();
    }
    if (lex_.getType() != Token::Type::CloseCurlyBrace) {
        throw lex_;
    }
    getLex();
    tid_tree_.closeScope();
}

void SyntacticAnalyzer::returnStatement() {
    if (lex_.getContent() != "return") {
        throw lex_;
    }
    getLex();
    exp14();
    Type type_exp = sem_stack_.popOperand();
    tid_tree_.checkReturn(type_exp);
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
    Type type_var = type();
    var(type_var);
    while (lex_.getType() == Token::Type::Comma) {
        getLex();
        var(type_var);
    }
    if (lex_.getType() != Token::Type::Semicolon) {
        throw lex_;
    }
    getLex();
}

void SyntacticAnalyzer::statement() {
    if (lex_.getContent() == "break" || lex_.getContent() == "continue") {
        if (lex_.getContent() == "break") {
            tid_tree_.checkBreak();
        } else {
            tid_tree_.checkContinue();
        }
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
        tid_tree_.createScope();
        block();
        tid_tree_.closeScope();
        return;
    }
    if (isType()) {
        varDefinition();
        return;
    }
    exp14();
    sem_stack_.popOperand();
    if (lex_.getType() != Token::Type::Semicolon) {
        throw lex_;
    }
    getLex();
}

Type SyntacticAnalyzer::type() {
    Type type;
    if (lex_.getContent() == "int" ||
        lex_.getContent() == "float" ||
        lex_.getContent() == "bool" ||
        lex_.getContent() == "char" ||
        lex_.getContent() == "string") {
        type.setName(lex_.getContent());
        getLex();
        return type;
    }
    if (lex_.getContent() == "array") {
        return array();
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
        type.setName(lex_.getContent());
        getLex();
        return type;
    }
    if (lex_.getContent() == "array") {
        return array();
    }
    if (lex_.getType() != Token::Type::Identifier &&
        lex_.getType() != Token::Type::NamespaceIdentifier) {
        throw lex_;
    }
    if (!tid_tree_.checkStruct(lex_.getContent())) {
        throw lex_;
    }
    type.setName(lex_.getContent());
    getLex();
    return type;
}

void SyntacticAnalyzer::var(Type type_var) {
    if (lex_.getType() != Token::Type::Identifier) {
        throw lex_;
    }
    Variable vari(lex_.getContent(), type_var);
    getLex();
    if (lex_.getContent() != "=") {
        tid_tree_.pushVariable(vari.getName(), type_var);
        return;
    }
    getLex();
    exp12();
    type_var.setLvalue(false);
    sem_stack_.checkType(type_var);
    tid_tree_.pushVariable(vari.getName(), type_var);
    return;
}

void SyntacticAnalyzer::function() {
    if (lex_.getContent() != "func") {
        throw lex_;
    }
    getLex();
    Type type_func = type();
    std::string name_func;
    if (lex_.getType() != Token::Type::Identifier) {
        throw lex_;
    }
    name_func = lex_.getContent();
    getLex();
    if (lex_.getType() != Token::Type::OpenParenthesis) {
        throw lex_;
    }
    getLex();
    std::vector<Variable> args;
    if (lex_.getType() != Token::Type::CloseParenthesis) {
        funcVarDefinition(args);
    }
    if (lex_.getType() != Token::Type::CloseParenthesis) {
        throw lex_;
    }
    tid_tree_.pushFunction(name_func, type_func, args);
    tid_tree_.createScope(TypeScope::Function, name_func);
    for (auto &arg: args) {
        tid_tree_.pushVariable(arg.getName(), arg.getType());
    }
    getLex();
    block();
    tid_tree_.closeScope();
}

void SyntacticAnalyzer::funcVarDefinition(std::vector<Variable> &args) {
    Type type_arg = type();
    if (lex_.getType() == Token::Type::Ampersand) {
        type_arg.setLvalue(true);
        getLex();
    }
    if (lex_.getType() != Token::Type::Identifier &&
        lex_.getType() != Token::Type::NamespaceIdentifier) {
        throw lex_;
    }
    args.emplace_back(lex_.getContent(), type_arg);
    getLex();
    while (lex_.getType() == Token::Type::Comma) {
        getLex();
        type_arg = type();
        if (lex_.getType() == Token::Type::Ampersand) {
            getLex();
        }
        if (lex_.getType() != Token::Type::Identifier &&
            lex_.getType() != Token::Type::NamespaceIdentifier) {
            throw lex_;
        }
        args.emplace_back(lex_.getContent(), type_arg);
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
    tid_tree_.pushStruct(lex_.getContent());
    tid_tree_.createScope(TypeScope::Struct, lex_.getContent());
    getLex();
    if (lex_.getType() != Token::Type::OpenCurlyBrace) {
        throw lex_;
    }
    getLex();
    while (lex_.getType() != Token::Type::CloseCurlyBrace) {
        structBody();
    }
    getLex();
    if (lex_.getType() != Token::Type::Semicolon) {
        throw lex_;
    }
    getLex();
    tid_tree_.closeScope();
}

void SyntacticAnalyzer::programBody() {
    if (lex_.getContent() == "namespace") {
        identifierNamespace();
        return;
    }
    if (lex_.getContent() == "func") {
        function();
        return;
    }
    structBody();
}

void SyntacticAnalyzer::program() {
    includes();
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

void SyntacticAnalyzer::identifierNamespace() {
    if (lex_.getContent() != "namespace") {
        throw lex_;
    }
    getLex();
    if (lex_.getType() != Token::Type::Identifier) {
        throw lex_;
    }
    tid_tree_.createScope(TypeScope::Namespace, lex_.getContent());
    getLex();
    if (lex_.getType() != Token::Type::OpenCurlyBrace) {
        throw lex_;
    }
    getLex();
    while (lex_.getType() != Token::Type::CloseCurlyBrace) {
        programBody();
    }
    if (lex_.getType() != Token::Type::CloseCurlyBrace) {
        throw lex_;
    }
    getLex();
    tid_tree_.closeScope();
}

void SyntacticAnalyzer::startAnalysis() {
    getLex();
    program();
}

Type SyntacticAnalyzer::array() {
    Type type_arr;
    type_arr.setName("array");
    if (lex_.getContent() != "array") {
        throw lex_;
    }
    getLex();
    if (lex_.getContent() != "<") {
        throw lex_;
    }
    getLex();
    type_arr.setArrayType(type());
    if (lex_.getType() != Token::Type::Comma) {
        throw lex_;
    }
    getLex();
    if (lex_.getType() != Token::Type::NumericLiteral) {
        throw lex_;
    }
    type_arr.setArraySize(stoi(lex_.getContent()));
    getLex();
    if (lex_.getContent() != ">") {
        throw lex_;
    }
    getLex();
    return type_arr;
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

void SyntacticAnalyzer::vars(std::vector<Type> &args) {
    exp13();
    args.push_back(sem_stack_.popOperand());
    while (lex_.getType() == Token::Type::Comma) {
        getLex();
        exp13();
        args.push_back(sem_stack_.popOperand());
    }
}

void SyntacticAnalyzer::structBody() {
    if (lex_.getContent() == "struct") {
        structure();
        return;
    }
    varDefinition();
}


