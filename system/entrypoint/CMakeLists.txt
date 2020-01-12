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

project(libentry CXX)

set(ENTRYPOINT_SRC_DIR ${SYSTEM_SRC_DIR}/entrypoint)
set(ENTRYPOINT_OBJ_DIR ${SYSTEM_OBJ_DIR}/entrypoint)

include_directories(
    ./
    ${INC_DIR}
    ${ENTRYPOINT_OBJ_DIR})

set(SOURCES
    entry.cpp)

set(GENERATED_SOURCES
    ${ENTRYPOINT_OBJ_DIR}/libcore.cpp)

IMPORT_LIBRARY(comort.so)
IMPORT_LIBRARY(libcore.so)

add_library(libentry
    STATIC
    ${SOURCES}
    ${GENERATED_SOURCES})
target_link_libraries(libentry
    comort.so
    libcore.so)
add_dependencies(libentry libcore)

add_custom_command(
    OUTPUT
        ${GENERATED_SOURCES}
    COMMAND
        "$ENV{CDLC}"
        -gen
        -mode-client
        -split
        -d ${ENTRYPOINT_OBJ_DIR}
        -metadata-so "${BIN_DIR}/libcore.so")

COPY(libentry ${ENTRYPOINT_OBJ_DIR}/libentry.a ${BIN_DIR})

install(FILES
    ${ENTRYPOINT_OBJ_DIR}/libentry.a
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
