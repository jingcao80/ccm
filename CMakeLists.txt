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

include($ENV{ROOT}/build/common.cmake)

set(CMAKE_VERBOSE_MAKEFILE off)
set(PROJECT_DIR ${CMAKE_SOURCE_DIR})
set(EXTERNAL_DIR ${PROJECT_DIR}/external)
set(TOOLS_DIR ${PROJECT_DIR}/tools)

set( THREADS_PTHREAD_ARG
     "PLEASE_FILL_OUT-FAILED_TO_RUN"
     CACHE STRING "Result from TRY_RUN" FORCE)

if($ENV{PRODUCT} STREQUAL comotools)
    set(OBJ_DIR $ENV{HOST_OUT_PATH})
    set(BIN_DIR $ENV{HOST_BIN_PATH})

    add_subdirectory(como)
else()
    if (NOT EXISTS "$ENV{BIN_PATH}/inc")
        file(MAKE_DIRECTORY "$ENV{BIN_PATH}/inc")
    endif()

    set(OBJ_DIR $ENV{OUT_PATH})
    set(BIN_DIR $ENV{BIN_PATH})
    set(INC_DIR ${BIN_DIR}/inc)

    add_subdirectory(external)
    add_subdirectory(como)
    add_subdirectory(servicemanager)
    add_subdirectory(libcore)
    #add_subdirectory(src)
    add_subdirectory(samples)
    add_subdirectory(test)
endif()
