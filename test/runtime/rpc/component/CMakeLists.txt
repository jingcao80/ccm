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

project(RPCTestUnit CXX)

set(COMPONENT_DIR ${RPC_DIR}/component)
set(OBJ_DIR ${UNIT_TEST_OBJ_DIR}/rpc/component)

include_directories(
    ./
    ${INC_DIR}
    ${OBJ_DIR})

set(SOURCES
    CService.cpp)

set(GENERATED_SOURCES
    ${OBJ_DIR}/_ccm_test_rpc_CService.cpp
    ${OBJ_DIR}/RPCTestUnitPub.cpp
    ${OBJ_DIR}/MetadataWrapper.cpp)

IMPORT_LIBRARY(ccmrt.so)

add_library(RPCTestUnit
    SHARED
    ${SOURCES}
    ${GENERATED_SOURCES})
target_link_libraries(RPCTestUnit ccmrt.so)
add_dependencies(RPCTestUnit libtest)

add_custom_command(
    OUTPUT
        ${GENERATED_SOURCES}
    COMMAND
        "${BIN_DIR}/ccdl"
        -c
        -g
        -k
        -d ${OBJ_DIR}
        "${COMPONENT_DIR}/RPCTestUnit.cdl")

COPY(RPCTestUnit ${OBJ_DIR}/RPCTestUnit.so ${BIN_DIR})

install(FILES
    ${OBJ_DIR}/RPCTestUnit.so
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