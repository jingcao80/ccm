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

project(ThreadTest CXX)

set(LIBCORE_COMO_CORE_THREAD_CLIENT_SRC_DIR ${LIBCORE_COMO_CORE_THREAD_SRC_DIR}/client)
set(LIBCORE_COMO_CORE_THREAD_CLIENT_OBJ_DIR ${LIBCORE_COMO_CORE_THREAD_OBJ_DIR}/client)

include_directories(
    ./
    ${INC_DIR}
    ${LIBCORE_COMO_CORE_THREAD_CLIENT_OBJ_DIR})

set(SOURCES
    main.cpp)

set(GENERATED_SOURCES
    ${LIBCORE_COMO_CORE_THREAD_CLIENT_OBJ_DIR}/libcore.cpp)

IMPORT_LIBRARY(comort.so)
IMPORT_LIBRARY(libcore.so)
IMPORT_GTEST()

add_executable(testThreadCli
    ${SOURCES}
    ${GENERATED_SOURCES})
target_link_libraries(testThreadCli comort.so libcore.so ${GTEST_LIBS})
add_dependencies(testThreadCli libtest)

add_custom_command(
    OUTPUT
        ${GENERATED_SOURCES}
    COMMAND
        "$ENV{CDLC}"
        -gen
        -mode-client
        -split
        -d ${LIBCORE_COMO_CORE_THREAD_CLIENT_OBJ_DIR}
        -metadata-so "${BIN_DIR}/libcore.so")

COPY(testThreadCli ${LIBCORE_COMO_CORE_THREAD_CLIENT_OBJ_DIR}/testThreadCli ${BIN_DIR})

install(FILES
    ${LIBCORE_COMO_CORE_THREAD_CLIENT_OBJ_DIR}/testThreadCli
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