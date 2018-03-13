cmake_minimum_required(VERSION 2.8)

project(CCDL)

add_compile_options(-std=c++11)

set(SOURCES
    src/ccdl/Component.cpp
    src/ccdl/Enumeration.cpp
    src/ccdl/Namespace.cpp
    src/util/File.cpp
    src/util/Logger.cpp
    src/util/String.cpp
    src/util/StringBuilder.cpp
    src/Options.cpp
    src/Parser.cpp
    src/Tokenizer.cpp
    src/main.cpp)

add_executable(ccdl ${SOURCES})