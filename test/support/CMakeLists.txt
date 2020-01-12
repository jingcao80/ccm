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

project(support CXX)

set(SUPPORT_SRC_DIR ${TEST_SRC_DIR}/support)
set(SUPPORT_OBJ_DIR ${TEST_OBJ_DIR}/support)

include_directories(
    ./
    ${INC_DIR}
    ${SUPPORT_OBJ_DIR})

set(SOURCES
    Support_CollectionTest.cpp
    Support_ListTest.cpp
    Support_UnmodifiableCollectionTest.cpp)

set(GENERATED_SOURCES
    ${SUPPORT_OBJ_DIR}/como.util.IList.h)

IMPORT_LIBRARY(comort.so)
IMPORT_LIBRARY(libcore.so)
IMPORT_GTEST()

add_library(libsupport
    STATIC
    ${SOURCES}
    ${GENERATED_SOURCES})
target_link_libraries(libsupport comort.so libcore.so ${GTEST_LIBS})
add_dependencies(libsupport libtest)

add_custom_command(
    OUTPUT
        ${GENERATED_SOURCES}
    COMMAND
        "$ENV{CDLC}"
        -gen
        -mode-client
        -split
        -d ${SUPPORT_OBJ_DIR}
        -metadata-so "${BIN_DIR}/libcore.so")

MKDIR(libsupport ${INC_DIR}/test/support)

COPY(libsupport ${SUPPORT_SRC_DIR}/Support_ListTest.h ${INC_DIR}/test/support)
COPY(libsupport ${SUPPORT_OBJ_DIR}/libsupport.a ${BIN_DIR})

install(FILES
    ${SUPPORT_OBJ_DIR}/libsupport.a
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