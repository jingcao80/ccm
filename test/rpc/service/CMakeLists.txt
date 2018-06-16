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

project(RPCTest CXX)

set(CLIENT_DIR ${RPC_DIR}/service)
set(OBJ_DIR ${CMAKE_BINARY_DIR}/test/rpc/service)

include_directories(
    ./
    ${INC_DIR}
    ${OBJ_DIR})

set(SOURCES
    main.cpp)

set(GENERATED_SOURCES
    ${OBJ_DIR}/RPCTestUnit.cpp)

IMPORT_LIBRARY(ccmrt.so)
IMPORT_LIBRARY(libservicemanager.so)

add_executable(testRPCSrv
    ${SOURCES}
    ${GENERATED_SOURCES})
target_link_libraries(testRPCSrv ccmrt.so libservicemanager.so)
add_dependencies(testRPCSrv RPCTestUnit)

add_custom_command(
    OUTPUT
        ${GENERATED_SOURCES}
    COMMAND
        "${BIN_DIR}/ccdl"
        -g
        -u
        -s
        -d ${OBJ_DIR}
        "${BIN_DIR}/RPCTestUnit.so")

COPY(testRPCSrv ${OBJ_DIR}/testRPCSrv ${BIN_DIR})

install(FILES
    ${OBJ_DIR}/testRPCSrv
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