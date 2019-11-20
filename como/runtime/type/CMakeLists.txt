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

set(TYPE_SRC_DIR ${RUNTIME_SRC_DIR}/type)

include_directories(
    ./
    ../
    ../util
    ${INC_DIR})

set(SOURCES
    comoarray.cpp
    comoshbuf.cpp
    comostring.cpp
    comouuid.cpp
    ucase.cpp)

add_library(type STATIC
    ${SOURCES})
add_dependencies(type cdl)

COPY(type ${TYPE_SRC_DIR}/comoarray.h ${INC_DIR})
COPY(type ${TYPE_SRC_DIR}/comointfs.h ${INC_DIR})
COPY(type ${TYPE_SRC_DIR}/comoshbuf.h ${INC_DIR})
COPY(type ${TYPE_SRC_DIR}/comostring.h ${INC_DIR})
COPY(type ${TYPE_SRC_DIR}/comotypekind.h ${INC_DIR})
COPY(type ${TYPE_SRC_DIR}/comouuid.h ${INC_DIR})

set(DEPLOY_HEADERS
    ${TYPE_SRC_DIR}/comoarray.h
    ${TYPE_SRC_DIR}/comointfs.h
    ${TYPE_SRC_DIR}/comoshbuf.h
    ${TYPE_SRC_DIR}/comostring.h
    ${TYPE_SRC_DIR}/comotypekind.h
    ${TYPE_SRC_DIR}/comouuid.h)

install(FILES
    ${DEPLOY_HEADERS}
    DESTINATION ${INC_DIR})
