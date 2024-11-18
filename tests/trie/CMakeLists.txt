cmake_minimum_required(VERSION 2.23)
project(Yuhan-trie-tests)

set(CMAKE_CXX_STANDARD 23)

add_executable(Yuhan-trie-tests trie.cpp
        ../../source/Trie.cpp
        ../../include/Trie.h
        ../../include/includes.h
        ../../include/Token.h
        ../../source/Token.cpp
        ../../include/ReservedMemory.h
        ../../source/ReservedMemory.cpp
)

