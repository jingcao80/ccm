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

project(runtime CXX ASM)

set(RUNTIME_SRC_DIR ${COMO_SRC_DIR}/runtime)
set(RUNTIME_OBJ_DIR ${COMO_OBJ_DIR}/runtime)

add_subdirectory(cdl)
add_subdirectory(component)
add_subdirectory(metadata)
add_subdirectory(reflection)
add_subdirectory(rpc)
add_subdirectory(type)
add_subdirectory(util)

include_directories(
    ./
    component
    reflection
    rpc
    type
    util
    ${INC_DIR})

set(SOURCES
    comoids.cpp
    runtime.cpp)

set(COMORT_LINK_LIBS
    -Wl,--whole-archive
    cdl
    component
    metadata
    reflection
    rpc
    type
    util
    -Wl,--no-whole-archive)

if($ENV{PLATFORM} STREQUAL "linux")
    set(COMORT_LINK_LIBS
        ${COMORT_LINK_LIBS}
        -Wl,--whole-archive
        rpc-dbus
        -Wl,--no-whole-archive)
elseif($ENV{PLATFORM} STREQUAL "android")
    set(COMORT_LINK_LIBS
        ${COMORT_LINK_LIBS})
endif()

add_library(comort
    SHARED
    ${SOURCES})
target_link_libraries(comort
    ${COMORT_LINK_LIBS}
    dl)
if($ENV{PLATFORM} STREQUAL "linux")
    target_link_libraries(comort
        pthread
        dbus-1)
endif()

COPY(comort ${RUNTIME_SRC_DIR}/comoapi.h ${INC_DIR})
COPY(comort ${RUNTIME_SRC_DIR}/comodef.h ${INC_DIR})
COPY(comort ${RUNTIME_SRC_DIR}/comoerror.h ${INC_DIR})
COPY(comort ${RUNTIME_SRC_DIR}/comotypes.h ${INC_DIR})
COPY(comort ${RUNTIME_OBJ_DIR}/comort.so ${BIN_DIR})

set(DEPLOY_HEADERS
    ${RUNTIME_SRC_DIR}/comoapi.h
    ${RUNTIME_SRC_DIR}/comodef.h
    ${RUNTIME_SRC_DIR}/comoerror.h
    ${RUNTIME_SRC_DIR}/comotypes.h)

set(DEPLOY_LIBS
    ${RUNTIME_OBJ_DIR}/comort.so)

install(FILES
    ${DEPLOY_HEADERS}
    DESTINATION ${INC_DIR})
install(FILES
    ${DEPLOY_LIBS}
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
