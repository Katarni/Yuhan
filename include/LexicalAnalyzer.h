//
// Created by Тимур Ахметзянов on 23.10.2024.
//

#pragma once

#include "Token.h"
#include "Trie.h"


class LexicalAnalyzer {
 public:
    explicit LexicalAnalyzer(Trie* reserved_words_trie) : reserved_words_(reserved_words_trie) {}

 private:
    Trie* reserved_words_;
};
