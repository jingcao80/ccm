#=========================================================================
# Copyright (C) 2018 The C++ Component Model(CCM) Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#=========================================================================

project(ccdl CXX)

set(SOURCES
    ast/AdditiveExpression.cpp
    ast/AndExpression.cpp
    ast/ArrayType.cpp
    ast/BooleanType.cpp
    ast/ByteType.cpp
    ast/CharType.cpp
    ast/Coclass.cpp
    ast/CoclassIDType.cpp
    ast/ComponentIDType.cpp
    ast/Constant.cpp
    ast/DoubleType.cpp
    ast/ECodeType.cpp
    ast/Enumeration.cpp
    ast/ExclusiveOrExpression.cpp
    ast/Expression.cpp
    ast/FloatType.cpp
    ast/HANDLEType.cpp
    ast/InclusiveOrExpression.cpp
    ast/IntegerType.cpp
    ast/Interface.cpp
    ast/InterfaceIDType.cpp
    ast/LongType.cpp
    ast/Method.cpp
    ast/Module.cpp
    ast/MultiplicativeExpression.cpp
    ast/Namespace.cpp
    ast/Node.cpp
    ast/Parameter.cpp
    ast/PointerType.cpp
    ast/PostfixExpression.cpp
    ast/Pool.cpp
    ast/ShiftExpression.cpp
    ast/ShortType.cpp
    ast/StringType.cpp
    ast/Type.cpp
    ast/UnaryExpression.cpp
    codegen/CodeGenerator.cpp
    metadata/MetaBuilder.cpp
    metadata/MetaDumper.cpp
    metadata/MetaResolver.cpp
    parser/Parser.cpp
    parser/Tokenizer.cpp
    parser/World.cpp
    util/File.cpp
    util/Logger.cpp
    util/MetadataUtils.cpp
    util/String.cpp
    util/StringBuilder.cpp
    util/StringPool.cpp
    util/Uuid.cpp
    Options.cpp
    main.cpp)

add_executable(ccdl ${SOURCES})
target_link_libraries(ccdl metadata)
add_dependencies(ccdl metadata)

COPY(ccdl ${CMAKE_BINARY_DIR}/src/ccdl/ccdl ${BIN_DIR})

install(FILES
    ${CMAKE_BINARY_DIR}/src/ccdl/ccdl
    DESTINATION ${BIN_DIR}
    PERMISSIONS
        OWNER_READ
        OWNER_WRITE
        OWNER_EXECUTE
        GROUP_READ
        GROUP_WRITE
        GROUP_EXECUTE
        WORLD_READ
        WORLD_EXECUTE)
