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
    ccmclassobject.cpp
    ccmlogger.cpp
    ccmobject.cpp
    ccmrefbase.cpp
    ccmspinlock.cpp
    mutex.cpp)

add_library(util STATIC
    ${SOURCES})
add_dependencies(util cdl)

COPY(util ${UTIL_DIR}/ccmautoptr.h ${INC_DIR})
COPY(util ${UTIL_DIR}/ccmclassobject.h ${INC_DIR})
COPY(util ${UTIL_DIR}/ccmlogger.h ${INC_DIR})
COPY(util ${UTIL_DIR}/ccmobject.h ${INC_DIR})
COPY(util ${UTIL_DIR}/ccmrefbase.h ${INC_DIR})
COPY(util ${UTIL_DIR}/ccmspinlock.h ${INC_DIR})

set(DEPLOY_HEADERS
    ${UTIL_DIR}/ccmautoptr.h
    ${UTIL_DIR}/ccmclassobject.h
    ${UTIL_DIR}/ccmlogger.h
    ${UTIL_DIR}/ccmobject.h
    ${UTIL_DIR}/ccmrefbase.h
    ${UTIL_DIR}/ccmspinlock.h)

install(FILES
    ${DEPLOY_HEADERS}
    DESTINATION ${INC_DIR})
