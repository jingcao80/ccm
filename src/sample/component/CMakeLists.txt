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

project(demo-component CXX)

add_compile_options(-std=c++11)

set(PROJECT_DIR ${CMAKE_SOURCE_DIR}/../../../)
set(BIN_DIR ${PROJECT_DIR}/bin)
set(INC_DIR ${BIN_DIR}/inc)
set(OBJ_DIR ${CMAKE_SOURCE_DIR}/build/obj)

include_directories(
    ./
    ${INC_DIR}
    ${OBJ_DIR})

set(SOURCES
    CFooBar.cpp
    ${OBJ_DIR}/_ccm_demo_CFooBar.cpp
    ${OBJ_DIR}/FooBarDemoPub.cpp
    ${OBJ_DIR}/MetadataWrapper.cpp)

LINK_DIRECTORIES(${BIN_DIR})

add_library(FooBarDemo
    SHARED
    ${SOURCES})
target_link_libraries(FooBarDemo ccmrt)

add_custom_command(
    OUTPUT comp_cdl_cpp
    PRE_BUILD
    COMMAND
        "${BIN_DIR}/ccdl"
        -d "obj"
        "${CMAKE_SOURCE_DIR}/FooBarDemo.cdl")

add_custom_target(
    gen_cpp ALL
    DEPENDS comp_cdl_cpp)


