cmake_minimum_required(VERSION 2.23)
project(Yuhan-sintex-tests)

set(CMAKE_CXX_STANDARD 23)

add_executable(Yuhan-sintex-tests sintex.cpp
        ../../source/Trie.cpp
        ../../include/Trie.h
        ../../include/includes.h
        ../../include/Token.h
        ../../source/LexicalAnalyzer.cpp
        ../../include/LexicalAnalyzer.h
        ../../source/Token.cpp
        ../../include/SyntacticAnalyzer.h
        ../../source/SyntacticAnalyzer.cpp
        ../../include/TIDTree.h
        ../../source/TIDTree.cpp
        ../../include/SemanticStack.h
        ../../source/SemanticStack.cpp
        ../../include/ReservedMemory.h
        ../../source/ReservedMemory.cpp
        ../../include/PRNGenerator.h
        ../../source/PRNGenerator.cpp
)

