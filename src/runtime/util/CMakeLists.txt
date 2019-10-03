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
    comoclassobject.cpp
    comologger.cpp
    comoobject.cpp
    comorefbase.cpp
    comospinlock.cpp
    mutex.cpp)

add_library(util STATIC
    ${SOURCES})
add_dependencies(util cdl)

COPY(util ${UTIL_DIR}/comoautoptr.h ${INC_DIR})
COPY(util ${UTIL_DIR}/comoclassobject.h ${INC_DIR})
COPY(util ${UTIL_DIR}/comologger.h ${INC_DIR})
COPY(util ${UTIL_DIR}/comoobject.h ${INC_DIR})
COPY(util ${UTIL_DIR}/comorefbase.h ${INC_DIR})
COPY(util ${UTIL_DIR}/comospinlock.h ${INC_DIR})

set(DEPLOY_HEADERS
    ${UTIL_DIR}/comoautoptr.h
    ${UTIL_DIR}/comoclassobject.h
    ${UTIL_DIR}/comologger.h
    ${UTIL_DIR}/comoobject.h
    ${UTIL_DIR}/comorefbase.h
    ${UTIL_DIR}/comospinlock.h)

install(FILES
    ${DEPLOY_HEADERS}
    DESTINATION ${INC_DIR})
