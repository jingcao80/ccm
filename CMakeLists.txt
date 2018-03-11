cmake_minimum_required(VERSION 2.8)

project(CCDL)

add_compile_options(-std=c++11)

add_executable(ccdl src/File.cpp src/Logger.cpp src/Options.cpp src/Parser.cpp src/String.cpp src/StringBuilder.cpp src/Tokenizer.cpp src/main.cpp)