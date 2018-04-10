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

cmake_minimum_required(VERSION 2.8)

project(demo-client CXX)

set(PROJECT_DIR ${CMAKE_SOURCE_DIR}/../../../)
set(BIN_DIR ${PROJECT_DIR}/bin)
set(INC_DIR ${BIN_DIR}/inc)
set(OBJ_DIR ${CMAKE_BINARY_DIR}/obj)

include(${PROJECT_DIR}/build/common.cmake)

include_directories(
    ./
    ${INC_DIR}
    ${OBJ_DIR})

set(SOURCES
    main.cpp)

set(GENERATED_SOURCES
    ${OBJ_DIR}/FooBarDemo.cpp)

LINK_DIRECTORIES(${BIN_DIR})

add_executable(demo
    ${SOURCES}
    ${GENERATED_SOURCES})
target_link_libraries(demo ccmrt)

add_custom_command(
    OUTPUT
        ${GENERATED_SOURCES}
    COMMAND
        "${BIN_DIR}/ccdl"
        -g
        -u
        -d "obj"
        "${BIN_DIR}/FooBarDemo.so")

add_custom_target(
    generate_cpp ALL
    DEPENDS ${GENERATED_SOURCES})

install(FILES
    ${CMAKE_BINARY_DIR}/demo
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

