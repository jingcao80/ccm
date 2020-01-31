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

project(libtest CXX)

set(LIBTEST_SRC_DIR ${TEST_SRC_DIR}/libtest)
set(LIBTEST_OBJ_DIR ${TEST_OBJ_DIR}/libtest)

include_directories(
    ./
    ${INC_DIR}
    ${LIBTEST_OBJ_DIR})

set(SOURCES
    wrapper.cpp)

set(GENERATED_SOURCES
    ${LIBTEST_OBJ_DIR}/libcore.cpp)

IMPORT_LIBRARY(comort.so)
IMPORT_LIBRARY(libcore.so)
IMPORT_GTEST()

add_library(libtest
    STATIC
    ${SOURCES}
    ${GENERATED_SOURCES})
target_link_libraries(libtest comort.so libcore.so ${GTEST_LIBS})
add_dependencies(libtest libcore gtest gtest_main)

add_custom_command(
    OUTPUT
        ${GENERATED_SOURCES}
    COMMAND
        "$ENV{CDLC}"
        -gen
        -mode-client
        -split
        -d ${LIBTEST_OBJ_DIR}
        -metadata-so "${BIN_DIR}/libcore.so")

MKDIR(libtest ${INC_DIR}/test)

COPY(libtest ${LIBTEST_SRC_DIR}/gtest.h ${INC_DIR}/test)
COPY(libtest ${LIBTEST_OBJ_DIR}/libtest.a ${BIN_DIR})

install(FILES
    ${LIBTEST_OBJ_DIR}/libtest.a
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