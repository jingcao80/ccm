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

cmake_minimum_required(VERSION 2.8)

if (NOT EXISTS "${CMAKE_SOURCE_DIR}/bin/inc")
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/inc)
endif()

set(CMAKE_VERBOSE_MAKEFILE off)
set(PROJECT_DIR ${CMAKE_SOURCE_DIR})
set(EXTERNAL_DIR ${PROJECT_DIR}/external)
set(OBJ_DIR ${PROJECT_DIR}/out)
set(BIN_DIR ${PROJECT_DIR}/bin)
set(INC_DIR ${BIN_DIR}/inc)

include(${PROJECT_DIR}/build/common.cmake)

add_subdirectory(external)
add_subdirectory(como)
add_subdirectory(servicemanager)
#add_subdirectory(src)
add_subdirectory(samples)
add_subdirectory(test)
