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

project(StringTest CXX)

set(STRING_DIR ${TEST_DIR}/client)
set(OBJ_DIR ${CMAKE_BINARY_DIR}/test/string)

include_directories(
    ./
    ${INC_DIR}
    ${OBJ_DIR})

set(SOURCES
    main.cpp)

IMPORT_LIBRARY(ccmrt.so)
IMPORT_LIBRARY(libcore.so)
IMPORT_GTEST()

add_executable(testString
    ${SOURCES})
target_link_libraries(testString ccmrt.so libcore.so ${GTEST_LIBS})
add_dependencies(testString libservicemanager)

COPY(testString ${OBJ_DIR}/testString ${BIN_DIR})

install(FILES
    ${OBJ_DIR}/testString
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