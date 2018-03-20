cmake_minimum_required(VERSION 2.8)

project(CCDL)

add_compile_options(-std=c++11)

set(SOURCES
    src/ccdl/AdditiveExpression.cpp
    src/ccdl/AndExpression.cpp
    src/ccdl/ArrayType.cpp
    src/ccdl/Component.cpp
    src/ccdl/Enumeration.cpp
    src/ccdl/ExclusiveOrExpression.cpp
    src/ccdl/Expression.cpp
    src/ccdl/InclusiveOrExpression.cpp
    src/ccdl/Interface.cpp
    src/ccdl/Method.cpp
    src/ccdl/MultiplicativeExpression.cpp
    src/ccdl/Namespace.cpp
    src/ccdl/PointerType.cpp
    src/ccdl/ShiftExpression.cpp
    src/ccdl/Type.cpp
    src/ccdl/UnaryExpression.cpp
    src/util/File.cpp
    src/util/Logger.cpp
    src/util/String.cpp
    src/util/StringBuilder.cpp
    src/util/Uuid.cpp
    src/Options.cpp
    src/Parser.cpp
    src/Tokenizer.cpp
    src/main.cpp)

add_executable(ccdl ${SOURCES})