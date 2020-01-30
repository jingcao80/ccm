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

project(FileInputStreamTest CXX)

set(LIBCORE_COMO_IO_FILEINPUTSTREAM_SRC_DIR ${LIBCORE_COMO_IO_SRC_DIR}/fileinputstream)
set(LIBCORE_COMO_IO_FILEINPUTSTREAM_OBJ_DIR ${LIBCORE_COMO_IO_OBJ_DIR}/fileinputstream)

include_directories(
    ./
    ${INC_DIR}
    ${LIBCORE_COMO_IO_FILEINPUTSTREAM_OBJ_DIR})

set(SOURCES
    main.cpp)

set(GENERATED_SOURCES
    ${LIBCORE_COMO_IO_FILEINPUTSTREAM_OBJ_DIR}/libcore.cpp)

IMPORT_LIBRARY(comort.so)
IMPORT_LIBRARY(libcore.so)
IMPORT_LIBRARY(libentry.a)
IMPORT_LIBRARY(libtest.a)
IMPORT_GTEST()

add_executable(testFileInputStream
    ${SOURCES}
    ${GENERATED_SOURCES})
target_link_libraries(testFileInputStream libentry.a libtest.a comort.so libcore.so ${GTEST_LIBS})
add_dependencies(testFileInputStream libtest)

add_custom_command(
    OUTPUT
        ${GENERATED_SOURCES}
    COMMAND
        "$ENV{CDLC}"
        -gen
        -mode-client
        -split
        -d ${LIBCORE_COMO_IO_FILEINPUTSTREAM_OBJ_DIR}
        -metadata-so "${BIN_DIR}/libcore.so")

COPY(testFileInputStream ${LIBCORE_COMO_IO_FILEINPUTSTREAM_OBJ_DIR}/testFileInputStream ${BIN_DIR})
COPY(testFileInputStream ${LIBCORE_COMO_IO_FILEINPUTSTREAM_SRC_DIR}/testfile ${BIN_DIR})

install(FILES
    ${LIBCORE_COMO_IO_FILEINPUTSTREAM_OBJ_DIR}/testFileInputStream
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
