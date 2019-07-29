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

project(support CXX)

set(support_DIR ${TEST_DIR}/support)
set(OBJ_DIR ${CMAKE_BINARY_DIR}/test/support)

include_directories(
    ./
    ${INC_DIR}
    ${OBJ_DIR})

set(SOURCES
    Support_CollectionTest.cpp
    Support_ListTest.cpp)

set(GENERATED_SOURCES
    ${OBJ_DIR}/ccm.util.IList.h)

IMPORT_LIBRARY(ccmrt.so)
IMPORT_LIBRARY(libcore.so)
IMPORT_GTEST()

add_library(libsupport
    STATIC
    ${SOURCES}
    ${GENERATED_SOURCES})
target_link_libraries(libsupport ccmrt.so libcore.so ${GTEST_LIBS})
add_dependencies(libsupport libtest)

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

MKDIR(libsupport ${INC_DIR}/test/support)

COPY(libsupport ${support_DIR}/Support_ListTest.h ${INC_DIR}/test/support)
COPY(libsupport ${OBJ_DIR}/libsupport.a ${BIN_DIR})

install(FILES
    ${OBJ_DIR}/libsupport.a
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