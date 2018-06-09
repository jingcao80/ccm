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

add_subdirectory(dbus)

include_directories(
    ./
    ../
    ../type
    ../util
    ${INC_DIR})

set(SOURCES
    CProxy.cpp
    CStub.cpp
    ccmrpc.cpp
    registry.cpp)

add_library(rpc STATIC
    ${SOURCES})
add_dependencies(rpc cdl)

COPY(rpc ${RPC_DIR}/ccmrpc.h ${INC_DIR})

set(DEPLOY_HEADERS
    ${RPC_DIR}/ccmrpc.h)

install(FILES
    ${DEPLOY_HEADERS}
    DESTINATION ${INC_DIR})