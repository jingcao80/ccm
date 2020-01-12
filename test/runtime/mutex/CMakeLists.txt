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

project(MutexTest CXX)

set(MUTEX_DIR ${UNIT_TEST_SRC_DIR}/mutex)
set(OBJ_DIR ${UNIT_TEST_OBJ_DIR}/mutex)

include_directories(
    ./
    ${INC_DIR}
    ${OBJ_DIR})

set(SOURCES
    main.cpp)

IMPORT_LIBRARY(comort.so)

add_executable(testMutex
    ${SOURCES})
target_link_libraries(testMutex comort.so pthread)
add_dependencies(testMutex libtest)

COPY(testMutex ${OBJ_DIR}/testMutex ${BIN_DIR})

install(FILES
    ${OBJ_DIR}/testMutex
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