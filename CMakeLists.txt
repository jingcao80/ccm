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

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected (options are: Debug Release), default to Release.")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (default Release)" FORCE)
endif()

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/inc)

#set(CMAKE_VERBOSE_MAKEFILE on)
set(PROJECT_DIR ${CMAKE_BINARY_DIR}/../)
set(BIN_DIR ${PROJECT_DIR}/bin)
set(EXTERNAL_DIR ${PROJECT_DIR}/external)
set(INC_DIR ${BIN_DIR}/inc)

include(${PROJECT_DIR}/build/common.cmake)

add_custom_target(
    clobber
    COMMAND
        "${CMAKE_COMMAND}"
        -E remove_directory ${CMAKE_SOURCE_DIR}/bin
    COMMAND
        "${CMAKE_COMMAND}"
        -E make_directory ${CMAKE_SOURCE_DIR}/bin
    COMMAND
        "${CMAKE_COMMAND}"
        -E make_directory ${CMAKE_SOURCE_DIR}/bin/inc
    COMMAND
        "make" clean)

add_custom_target(
    rebuild
    DEPENDS
        clobber
    COMMAND
        "make")

add_subdirectory(src)
add_subdirectory(test)
