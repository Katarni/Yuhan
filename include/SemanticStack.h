//
// Created by Timur Akhmetzianov on 11.11.2024
//

#pragma once

#include "Trie.h"
#include "Token.h"
#include "PRNGenerator.h"


/*!
    \brief Семантический стек
    \author Katarni

    Семантический стек для проверки корректности операций и операндов.
    Используется в семантическом управляемом переводе в ПОЛИЗ
*/
class SemanticStack {
 public:
    explicit SemanticStack(PRNGenerator* generator) : generator_(generator) {}

    void push(const Type& operand) noexcept;
    void push(const Token& operation) noexcept;

    /*!
        \return Тип опреанда на вершине стека
        \throw std::runtime_error Если на вершине стека операция
        \warning Удаляет операнд с вершины стека
    */
    Type popOperand();

    /*!
        \return Операцию на вершине стека
        \throw std::runtime_error Если на вершине стека операнд
        \warning Удаляет операцию с вершины стека
    */
    Token popOperation();

    /*!
        \throw SemanticStack::Error Если операция неприменина к данным операндам

        Проверка унарной операции
    */
    void checkUno();

    /*!
        \throw SemanticStack::Error Если операция неприменина к данным операндам

        Проверка бинарной операции
    */
    void checkBinary();

    void checkBool(); ///< Проверка, что выражение в стеке преобразуется к bool
    void checkType(Type type); ///< Проверка, что выражение в стеке преобразуется к type

    void clear() noexcept;

    /*!
        \brief Ошибка семантического стека
        \author Katarni

        Возвращается, если операцию, подвергшуюся проверке, невожножно произвести над данными операндами
    */
    class Error : public std::exception {
     public: 
        Error(const Type& lhs, const Token& operation, const Type& rhs);
        Error(const Type& operand, const Token& operation);

        [[nodiscard]]
        const char* what() const noexcept override;
     private:
        std::string what_;
    };

 private:
    PRNGenerator* generator_;
    std::stack<std::variant<Type, Token>> sem_stack_;

    Type getOperand();
    Token getOperation();
};
