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

project(TreeMapTest CXX)

set(LIBCORE_COMO_UTIL_TREEMAP_SRC_DIR ${LIBCORE_COMO_UTIL_SRC_DIR}/treemap)
set(LIBCORE_COMO_UTIL_TREEMAP_OBJ_DIR ${LIBCORE_COMO_UTIL_OBJ_DIR}/treemap)

include_directories(
    ./
    ${INC_DIR}
    ${LIBCORE_COMO_UTIL_TREEMAP_OBJ_DIR})

set(SOURCES
    main.cpp)

set(GENERATED_SOURCES
    ${LIBCORE_COMO_UTIL_TREEMAP_OBJ_DIR}/libcore.cpp)

IMPORT_LIBRARY(comort.so)
IMPORT_LIBRARY(libcore.so)
IMPORT_GTEST()

add_executable(testTreeMap
    ${SOURCES}
    ${GENERATED_SOURCES})
target_link_libraries(testTreeMap comort.so libcore.so ${GTEST_LIBS})
add_dependencies(testTreeMap libtest)

add_custom_command(
    OUTPUT
        ${GENERATED_SOURCES}
    COMMAND
        "$ENV{CDLC}"
        -gen
        -mode-client
        -split
        -d ${LIBCORE_COMO_UTIL_TREEMAP_OBJ_DIR}
        -metadata-so "${BIN_DIR}/libcore.so")

COPY(testTreeMap ${LIBCORE_COMO_UTIL_TREEMAP_OBJ_DIR}/testTreeMap ${BIN_DIR})

install(FILES
    ${LIBCORE_COMO_UTIL_TREEMAP_OBJ_DIR}/testTreeMap
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
