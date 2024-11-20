#include "../include/SyntacticAnalyzer.h"

SyntacticAnalyzer::SyntacticAnalyzer(LexicalAnalyzer *lexer,
                                     PRNGenerator* generator) : lexer_(lexer), tid_tree_(),
                                                                generator_(generator), sem_stack_(generator) {}

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
            try {
                sem_stack_.checkBinary();
            } catch (std::runtime_error &error) {
                throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                         std::to_string(lex_.getColumn()));
            }
            if (lex_.getType() != Token::Type::CloseSquareBracket) {
                throw lex_;
            }
            getLex();
        } else if (lex_.getType() == Token::Type::Dot) {
            auto dot = lex_;
            getLex();
            if (lex_.getType() != Token::Type::Identifier) {
                throw lex_;
            }
            Type type_last;
            try {
                type_last = sem_stack_.popOperand();
            } catch (std::runtime_error &error) {
                throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                         std::to_string(lex_.getColumn()));
            }
            Type field;
            try {
                field = tid_tree_.checkField(type_last.getName(), lex_.getContent());
            } catch (std::runtime_error &error) {
                throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                         std::to_string(lex_.getColumn()));
            }
            sem_stack_.push(field);
            generator_->push(lex_.getContent());
            generator_->push(dot);
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
            try {
                sem_stack_.push(tid_tree_.checkFunction(name, args));
            } catch (std::runtime_error &error) {
                throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                         std::to_string(lex_.getColumn()));
            }
            getLex();
        } else {
            Identifier var;
            try {
                var = tid_tree_.checkVariable(name);
            } catch (std::runtime_error &error) {
                throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                         std::to_string(lex_.getColumn()));
            }
            sem_stack_.push(var.getType());
            generator_->push(var);
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
        try {
            sem_stack_.checkUno();
        } catch (std::runtime_error &error) {
            throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                     std::to_string(lex_.getColumn()));
        }
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
        try {
            sem_stack_.checkBinary();
        } catch (std::runtime_error &error) {
            throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                     std::to_string(lex_.getColumn()));
        }
    }
}

void SyntacticAnalyzer::exp4() {
    exp3();
    while (lex_.getType() == Token::Type::PlusOrMinusOperator) {
        sem_stack_.push(lex_);
        getLex();
        exp3();
        try {
            sem_stack_.checkBinary();
        } catch (std::runtime_error &error) {
            throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                     std::to_string(lex_.getColumn()));
        }
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
        try {
            sem_stack_.checkBinary();
        } catch (std::runtime_error &error) {
            throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                     std::to_string(lex_.getColumn()));
        }
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
        try {
            sem_stack_.checkBinary();
        } catch (std::runtime_error &error) {
            throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                     std::to_string(lex_.getColumn()));
        }
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
        try {
            sem_stack_.checkBinary();
        } catch (std::runtime_error &error) {
            throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                     std::to_string(lex_.getColumn()));
        }
    }
}

void SyntacticAnalyzer::exp8() {
    exp7();
    while (lex_.getContent() == "&") {
        sem_stack_.push(lex_);
        getLex();
        exp7();
        try {
            sem_stack_.checkBinary();
        } catch (std::runtime_error &error) {
            throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                     std::to_string(lex_.getColumn()));
        }
    }
}

void SyntacticAnalyzer::exp9() {
    exp8();
    while (lex_.getContent() == "^") {
        sem_stack_.push(lex_);
        getLex();
        exp8();
        try {
            sem_stack_.checkBinary();
        } catch (std::runtime_error &error) {
            throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                     std::to_string(lex_.getColumn()));
        }
    }
}

void SyntacticAnalyzer::exp10() {
    exp9();
    while (lex_.getContent() == "|") {
        sem_stack_.push(lex_);
        getLex();
        exp9();
        try {
            sem_stack_.checkBinary();
        } catch (std::runtime_error &error) {
            throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                     std::to_string(lex_.getColumn()));
        }
    }
}

void SyntacticAnalyzer::exp11() {
    exp10();
    while (lex_.getContent() == "and") {
        sem_stack_.push(lex_);
        getLex();
        exp10();
        try {
            sem_stack_.checkBinary();
        } catch (std::runtime_error &error) {
            throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                     std::to_string(lex_.getColumn()));
        }
    }
}

void SyntacticAnalyzer::exp12() {
    exp11();
    while (lex_.getContent() == "or") {
        sem_stack_.push(lex_);
        getLex();
        exp11();
        try {
            sem_stack_.checkBinary();
        } catch (std::runtime_error &error) {
            throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                     std::to_string(lex_.getColumn()));
        }
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
        try {
            sem_stack_.checkBinary();
        } catch (std::runtime_error &error) {
            throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                     std::to_string(lex_.getColumn()));
        }
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
    try {
        sem_stack_.checkBool();
    } catch (std::runtime_error &error) {
        throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                 std::to_string(lex_.getColumn()));
    }
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
    try {
        sem_stack_.checkBool();
    } catch (std::runtime_error &error) {
        throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                 std::to_string(lex_.getColumn()));
    }
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
            try {
                sem_stack_.popOperand();
            } catch (std::runtime_error &error) {
                throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                         std::to_string(lex_.getColumn()));
            }
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
        try {
            sem_stack_.checkBool();
        } catch (std::runtime_error &error) {
            throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                     std::to_string(lex_.getColumn()));
        }
    }
    if (lex_.getType() != Token::Type::Semicolon) {
        throw lex_;
    }
    getLex();
    if (lex_.getType() != Token::Type::CloseParenthesis) {
        exp14();
        try {
            sem_stack_.popOperand();
        } catch (std::runtime_error &error) {
            throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                     std::to_string(lex_.getColumn()));
        }
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
    Type type_exp;
    try {
        type_exp = sem_stack_.popOperand();
    } catch (std::runtime_error &error) {
        throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                 std::to_string(lex_.getColumn()));
    }
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
    Type type_exp;
    try {
        type_exp = sem_stack_.popOperand();
    } catch (std::runtime_error &error) {
        throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                 std::to_string(lex_.getColumn()));
    }
    try {
        tid_tree_.checkReturn(type_exp);
    } catch (std::runtime_error &error) {
        throw std::runtime_error(std::string(error.what()) + " " +
                                 std::to_string(lex_.getLine()) + ":" + std::to_string(lex_.getColumn()));
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
        try {
            if (lex_.getContent() == "break") {
                tid_tree_.checkBreak();
            } else {
                tid_tree_.checkContinue();
            }
        } catch (std::runtime_error &error) {
            throw std::runtime_error(std::string(error.what()) + " " +
                    std::to_string(lex_.getLine()) + ":" + std::to_string(lex_.getColumn()));
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
    try {
        sem_stack_.popOperand();
    } catch (std::runtime_error &error) {
        throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                 std::to_string(lex_.getColumn()));
    }
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
    try {
        if (!tid_tree_.checkStruct(lex_.getContent())) {
            throw lex_;
        }
    } catch (std::runtime_error &error) {
        throw std::runtime_error(std::string(error.what()) + " " +
                std::to_string(lex_.getLine()) + ":" + std::to_string(lex_.getColumn()));
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
    auto id = genId();
    getLex();
    generator_->push({id, type_var});
    if (lex_.getContent() != "=") {
        try {
            tid_tree_.pushVariable(vari.getName(), type_var, id);
        } catch (std::runtime_error &error) {
            throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" + std::to_string(lex_.getColumn()));
        }
        return;
    }
    auto eq = lex_;
    getLex();
    exp12();
    type_var.setLvalue(false);
    generator_->push(eq);
    try {
        sem_stack_.checkType(type_var);
    } catch (std::runtime_error &error) {
        throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                 std::to_string(lex_.getColumn()));
    }
    try {
        tid_tree_.pushVariable(vari.getName(), type_var, id);
    } catch (std::runtime_error &error) {
        throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                 std::to_string(lex_.getColumn()));
    }
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
    try {
        tid_tree_.pushFunction(name_func, type_func, args);
    } catch (std::runtime_error &error) {
        throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                 std::to_string(lex_.getColumn()));
    }
    tid_tree_.createScope(TypeScope::Function, name_func);
    for (auto &arg: args) {
        try {
            // функции, это временное решение
            tid_tree_.pushVariable(arg.getName(), arg.getType(), genId());
        } catch (std::runtime_error &error) {
            throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                     std::to_string(lex_.getColumn()));
        }
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
            type_arg.setLvalue(true);
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
    try {
        tid_tree_.pushStruct(lex_.getContent());
    } catch (std::runtime_error &error) {
        throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                 std::to_string(lex_.getColumn()));
    }
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
    try {
        program();
    } catch (std::exception &error) {
        std::cout << "Yuhan og" << std::endl;
        std::cout << error.what() << std::endl;
        exit(-52);
    }
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
    try {
        args.push_back(sem_stack_.popOperand());
    } catch (std::runtime_error &error) {
        throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                 std::to_string(lex_.getColumn()));
    }
    while (lex_.getType() == Token::Type::Comma) {
        getLex();
        exp13();
        try {
            args.push_back(sem_stack_.popOperand());
        } catch (std::runtime_error &error) {
            throw std::runtime_error(std::string(error.what()) + " " + std::to_string(lex_.getLine()) + ":" +
                                     std::to_string(lex_.getColumn()));
        }
    }
}

void SyntacticAnalyzer::structBody() {
    if (lex_.getContent() == "struct") {
        structure();
        return;
    }
    varDefinition();
}

std::string SyntacticAnalyzer::genId() {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::string chars = "abcdefghijklmnopqrstuvwxyz1234567890";
    int sz = static_cast<int>(chars.size());
    std::string res;

    while (true) {
        for (int i = 0; i < 20; ++i) res += chars[rng() % sz];

        if (used_ids_.find(res) == used_ids_.end()) break;
    }
    used_ids_.insert(res);

    return res;
}
