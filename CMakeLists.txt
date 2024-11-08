cmake_minimum_required(VERSION 3.23)
project(Yuhan)

set(CMAKE_CXX_STANDARD 23)

add_executable(Yuhan main.cpp
        source/Trie.cpp
        include/Trie.h
        include/includes.h
        include/Token.h
        source/LexicalAnalyzer.cpp
        include/LexicalAnalyzer.h
        source/Token.cpp
        source/Starter.cpp
        include/Starter.h
        include/SyntacticAnalyzer.h
        source/SyntacticAnalyzer.cpp
)

