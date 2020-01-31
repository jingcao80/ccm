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

project(CharTest CXX)

set(CHAR_SRC_DIR ${RUNTIME_SRC_DIR}/char)
set(CHAR_OBJ_DIR ${RUNTIME_OBJ_DIR}/char)

include_directories(
    ./
    ${INC_DIR}
    ${CHAR_OBJ_DIR})

set(SOURCES
    main.cpp)

IMPORT_LIBRARY(comort.so)
IMPORT_GTEST()

add_executable(testChar
    ${SOURCES})
target_link_libraries(testChar comort.so ${GTEST_LIBS})
add_dependencies(testChar comort gtest gtest_main)

COPY(testChar ${CHAR_OBJ_DIR}/testChar ${BIN_DIR})

install(FILES
    ${CHAR_OBJ_DIR}/testChar
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