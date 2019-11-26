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

set(RPC_SRC_DIR ${RUNTIME_SRC_DIR}/rpc)

if($ENV{PLATFORM} STREQUAL "linux")
    add_subdirectory(dbus)
elseif($ENV{PLATFORM} STREQUAL "android")
endif()

include_directories(
    ./
    ../
    ../type
    ../util
    ${INC_DIR})

set(SOURCES
    CProxy.cpp
    CStub.cpp
    comorpc.cpp
    registry.cpp)

add_library(rpc STATIC
    ${SOURCES})
add_dependencies(rpc cdl)

COPY(rpc ${RPC_SRC_DIR}/comorpc.h ${INC_DIR})

set(DEPLOY_HEADERS
    ${RPC_SRC_DIR}/comorpc.h)

install(FILES
    ${DEPLOY_HEADERS}
    DESTINATION ${INC_DIR})