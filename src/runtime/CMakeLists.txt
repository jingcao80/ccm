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

project(runtime CXX)

set(RUNTIME_DIR ${SRC_DIR}/runtime)
set(OBJ_DIR ${CMAKE_BINARY_DIR}/src/runtime)

add_subdirectory(cdl)
add_subdirectory(component)
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
    ccmids.cpp
    runtime.cpp)

set(CCMRT_LINK_LIBS
    -Wl,--whole-archive
    cdl
    component
    metadata
    reflection
    rpc
    rpc-dbus
    type
    util
    -Wl,--no-whole-archive)

add_library(ccmrt
    SHARED
    ${SOURCES})
target_link_libraries(ccmrt
    ${CCMRT_LINK_LIBS}
    dl
    pthread
    dbus-1)

COPY(ccmrt ${RUNTIME_DIR}/ccmapi.h ${INC_DIR})
COPY(ccmrt ${RUNTIME_DIR}/ccmdef.h ${INC_DIR})
COPY(ccmrt ${RUNTIME_DIR}/ccmerror.h ${INC_DIR})
COPY(ccmrt ${RUNTIME_DIR}/ccmtypes.h ${INC_DIR})
COPY(ccmrt ${OBJ_DIR}/ccmrt.so ${BIN_DIR})

set(DEPLOY_HEADERS
    ${RUNTIME_DIR}/ccmapi.h
    ${RUNTIME_DIR}/ccmdef.h
    ${RUNTIME_DIR}/ccmerror.h
    ${RUNTIME_DIR}/ccmtypes.h)

set(DEPLOY_LIBS
    ${OBJ_DIR}/ccmrt.so)

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
