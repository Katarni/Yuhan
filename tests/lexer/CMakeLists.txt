cmake_minimum_required(VERSION 2.23)
project(Yuhan-lexer-tests)

set(CMAKE_CXX_STANDARD 23)

add_executable(Yuhan-lexer-tests lexer.cpp
        ../../source/Trie.cpp
        ../../include/Trie.h
        ../../include/includes.h
        ../../include/Token.h
        ../../source/LexicalAnalyzer.cpp
        ../../include/LexicalAnalyzer.h
        ../../source/Token.cpp
        ../../include/Starter.h
        ../../source/Starter.cpp
)

