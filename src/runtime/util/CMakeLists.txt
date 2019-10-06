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

set(UTIL_DIR ${RUNTIME_DIR}/util)

include_directories(
    ./
    ../
    ../type
    ${INC_DIR})

set(SOURCES
    comoclsobj.cpp
    comolog.cpp
    comoobj.cpp
    comoref.cpp
    comoref.cpp
    mutex.cpp)

add_library(util STATIC
    ${SOURCES})
add_dependencies(util cdl)

COPY(util ${UTIL_DIR}/comoptr.h ${INC_DIR})
COPY(util ${UTIL_DIR}/comoclsobj.h ${INC_DIR})
COPY(util ${UTIL_DIR}/comolog.h ${INC_DIR})
COPY(util ${UTIL_DIR}/comoobj.h ${INC_DIR})
COPY(util ${UTIL_DIR}/comoref.h ${INC_DIR})
COPY(util ${UTIL_DIR}/comolock.h ${INC_DIR})

set(DEPLOY_HEADERS
    ${UTIL_DIR}/comoptr.h
    ${UTIL_DIR}/comoclsobj.h
    ${UTIL_DIR}/comolog.h
    ${UTIL_DIR}/comoobj.h
    ${UTIL_DIR}/comoref.h
    ${UTIL_DIR}/comolock.h)

install(FILES
    ${DEPLOY_HEADERS}
    DESTINATION ${INC_DIR})
