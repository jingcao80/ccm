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

project(StringTokenizerTest CXX)

set(STRINGTOKENIZER_DIR ${UNIT_TEST_SRC_DIR}/stringtokenizer)
set(OBJ_DIR ${UNIT_TEST_OBJ_DIR}/stringtokenizer)

include_directories(
    ./
    ${INC_DIR}
    ${OBJ_DIR})

set(SOURCES
    main.cpp)

set(GENERATED_SOURCES
    ${OBJ_DIR}/libcore.cpp)

IMPORT_LIBRARY(comort.so)
IMPORT_GTEST()

add_executable(testStringTokenizer
    ${SOURCES}
    ${GENERATED_SOURCES})
target_link_libraries(testStringTokenizer comort.so ${GTEST_LIBS})
add_dependencies(testStringTokenizer libtest)

add_custom_command(
    OUTPUT
        ${GENERATED_SOURCES}
    COMMAND
        "${BIN_DIR}/ccdl"
        -g
        -u
        -p
        -s
        -d ${OBJ_DIR}
        "${BIN_DIR}/libcore.so")

COPY(testStringTokenizer ${OBJ_DIR}/testStringTokenizer ${BIN_DIR})

install(FILES
    ${OBJ_DIR}/testStringTokenizer
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