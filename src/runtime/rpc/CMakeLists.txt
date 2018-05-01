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

set(RPC_DIR ${RUNTIME_DIR}/rpc)

include_directories(
    ./
    ../
    ${INC_DIR})

set(SOURCES
    CParcel.cpp
    CProxy.cpp
    CStub.cpp
    ccmrpc.cpp)

add_library(rpc STATIC
    ${SOURCES})

set(DEPLOY_HEADERS
    ${RPC_DIR}/ccmrpc.h)

add_custom_command(
    TARGET rpc
    POST_BUILD
    COMMAND
        "${CMAKE_COMMAND}"
        -E copy ${DEPLOY_HEADERS} ${INC_DIR})

install(FILES
    ${DEPLOY_HEADERS}
    DESTINATION ${INC_DIR})