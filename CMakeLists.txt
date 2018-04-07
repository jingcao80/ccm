cmake_minimum_required(VERSION 2.8)

project(CCDL)

add_compile_options(-std=c++11)

set(TOOL_SOURCES
    src/runtime/metadata/MetaSerializer.cpp
    src/tool/ast/AdditiveExpression.cpp
    src/tool/ast/AndExpression.cpp
    src/tool/ast/ArrayType.cpp
    src/tool/ast/BooleanType.cpp
    src/tool/ast/ByteType.cpp
    src/tool/ast/CharType.cpp
    src/tool/ast/Coclass.cpp
    src/tool/ast/CoclassIDType.cpp
    src/tool/ast/Constant.cpp
    src/tool/ast/DoubleType.cpp
    src/tool/ast/Enumeration.cpp
    src/tool/ast/ExclusiveOrExpression.cpp
    src/tool/ast/Expression.cpp
    src/tool/ast/FloatType.cpp
    src/tool/ast/HANDLEType.cpp
    src/tool/ast/InclusiveOrExpression.cpp
    src/tool/ast/IntegerType.cpp
    src/tool/ast/Interface.cpp
    src/tool/ast/InterfaceIDType.cpp
    src/tool/ast/LongType.cpp
    src/tool/ast/Method.cpp
    src/tool/ast/Module.cpp
    src/tool/ast/MultiplicativeExpression.cpp
    src/tool/ast/Namespace.cpp
    src/tool/ast/Parameter.cpp
    src/tool/ast/PointerType.cpp
    src/tool/ast/PostfixExpression.cpp
    src/tool/ast/Pool.cpp
    src/tool/ast/ShiftExpression.cpp
    src/tool/ast/ShortType.cpp
    src/tool/ast/StringType.cpp
    src/tool/ast/Type.cpp
    src/tool/ast/UnaryExpression.cpp
    src/tool/codegen/CodeGenerator.cpp
    src/tool/metadata/MetaBuilder.cpp
    src/tool/metadata/MetaDumper.cpp
    src/tool/parser/Environment.cpp
    src/tool/parser/Parser.cpp
    src/tool/parser/Tokenizer.cpp
    src/tool/util/File.cpp
    src/tool/util/Logger.cpp
    src/tool/util/String.cpp
    src/tool/util/StringBuilder.cpp
    src/tool/util/StringPool.cpp
    src/tool/util/Uuid.cpp
    src/tool/Options.cpp
    src/tool/main.cpp)

add_executable(ccdl ${TOOL_SOURCES})

set(RT_SOURCES
    src/runtime/metadata/MetaSerializer.cpp
    src/runtime/component/ccmcomponent.cpp
    src/runtime/component/ccmobjectapi.cpp
    src/runtime/type/ccmarray.cpp
    src/runtime/type/ccmsharedbuffer.cpp
    src/runtime/type/ccmstring.cpp
    src/runtime/util/logger.cpp
    src/runtime/ccmids.cpp
    src/runtime/runtime.cpp)

add_library(ccmrt SHARED
    ${RT_SOURCES})

set(TEST_SOURCES
    build/tmp/SolarSystemPub.cpp)

#add_library(ss SHARED
#    ${TEST_SOURCES})
