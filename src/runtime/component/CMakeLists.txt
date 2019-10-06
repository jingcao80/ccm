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

set(COMPONENT_DIR ${RUNTIME_DIR}/component)

include_directories(
    ../
    ../reflection
    ../type
    ../util
    ${INC_DIR})

set(SOURCES
    comoobjapi.cpp
    CBootClassLoader.cpp)

add_library(component STATIC
    ${SOURCES})
add_dependencies(component cdl)

COPY(component ${COMPONENT_DIR}/comocomp.h ${INC_DIR})
COPY(component ${COMPONENT_DIR}/comoobjapi.h ${INC_DIR})

set(DEPLOY_HEADERS
    ${COMPONENT_DIR}/comocomp.h
    ${COMPONENT_DIR}/comoobjapi.h)

install(FILES
    ${DEPLOY_HEADERS}
    DESTINATION ${INC_DIR})
