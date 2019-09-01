#=========================================================================
# Copyright (C) 2018 The C++ Component Model(COMO) Open Source Project
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

set(AST_DIR ${CDLC_DIR}/ast)

include_directories(
    ${CDLC_DIR})

set(SOURCES
    AdditiveExpression.cpp
    AndExpression.cpp
    BooleanType.cpp
    ByteType.cpp
    CharType.cpp
    CoclassIDType.cpp
    ComponentIDType.cpp
    Constant.cpp
    DoubleType.cpp
    ECodeType.cpp
    EnumerationType.cpp
    Expression.cpp
    FloatType.cpp
    HANDLEType.cpp
    InclusiveOrExpression.cpp
    IntegerType.cpp
    InterfaceIDType.cpp
    InterfaceType.cpp
    LongType.cpp
    Method.cpp
    Module.cpp
    MultiplicativeExpression.cpp
    Namespace.cpp
    Node.cpp
    Parameter.cpp
    PointerType.cpp
    PostfixExpression.cpp
    ReferenceType.cpp
    ShiftExpression.cpp
    ShortType.cpp
    StringType.cpp
    TripleType.cpp
    Type.cpp
    UnaryExpression.cpp)

add_library(ast STATIC
    ${SOURCES})
