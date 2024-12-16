//
// Created by Тимур Ахметзянов on 23.10.2024.
//

#pragma once

#include "Token.h"
#include "Trie.h"


/*!
    \brief Лексический анализатор
    \author Katarni

    Реализация лексического анализатора
*/
class LexicalAnalyzer {
 public:
    explicit LexicalAnalyzer(char *text, size_t text_size, const std::string& reserved_words_file);

    ~LexicalAnalyzer();

    /*!
        \return Следующая лексема
        \warning Последняя лексема всегда имеет тип EndOfFile
    */
    [[nodiscard]]
    Token getToken();

 private:
    Trie<Node> *reserved_words_;
    char *text_, *cur_symbol_;
    size_t text_size_, cur_line_, cur_col_;
};
