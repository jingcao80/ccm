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

project(demo-client CXX)

set(CLIENT_DIR ${SAMPLE_DIR}/client)
set(OBJ_DIR ${CMAKE_BINARY_DIR}/src/sample/client)

include_directories(
    ./
    ${INC_DIR}
    ${OBJ_DIR})

set(SOURCES
    main.cpp)

set(GENERATED_SOURCES
    ${OBJ_DIR}/FooBarDemo.cpp)

add_library(ccmrt.so UNKNOWN IMPORTED)
set_property(TARGET ccmrt.so PROPERTY IMPORTED_LOCATION ${BIN_DIR}/ccmrt.so)

add_executable(demo
    ${SOURCES}
    ${GENERATED_SOURCES})
target_link_libraries(demo ccmrt.so)

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
        "${BIN_DIR}/FooBarDemo.so")

COPY(demo ${OBJ_DIR}/demo ${BIN_DIR})

install(FILES
    ${OBJ_DIR}/demo
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
