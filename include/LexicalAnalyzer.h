//
// Created by Тимур Ахметзянов on 23.10.2024.
//

#pragma once

#include "Token.h"
#include "Trie.h"


class LexicalAnalyzer {
 public:
    explicit LexicalAnalyzer(Trie *reserved_words_trie,
                             char *text, size_t text_size) : reserved_words_(reserved_words_trie),
                                                            text_(text), cur_symbol_(text),
                                                            text_size_(text_size), cur_line_(0),
                                                            cur_col_(0) {}

    ~LexicalAnalyzer();

    [[nodiscard]]
    Token getToken();

 private:
    Trie *reserved_words_;
    char *text_, *cur_symbol_;
    size_t text_size_, cur_line_, cur_col_;
};
