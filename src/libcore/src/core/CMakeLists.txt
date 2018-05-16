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

set(CORE_DIR ${LIBCORE_SRC_DIR}/core)

include_directories(
    ./
    ../
    ../../inc
    ${INC_DIR})

set(SOURCES
    SyncObject.cpp)

add_library(core STATIC
    ${SOURCES})
add_dependencies(core build_libcore)

COPY_AFTER(core ${REFLECTION_DIR}/ccmreflectionapi.h ${INC_DIR})

set(DEPLOY_HEADERS
    ${REFLECTION_DIR}/ccmreflectionapi.h)

install(FILES
    ${DEPLOY_HEADERS}
    DESTINATION ${INC_DIR})