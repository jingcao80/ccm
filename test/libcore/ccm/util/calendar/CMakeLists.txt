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

project(CalendarTest CXX)

set(calendar_DIR ${UNIT_TEST_SRC_DIR}/calendar)
set(OBJ_DIR ${UNIT_TEST_OBJ_DIR}/calendar)

include_directories(
    ./
    ${INC_DIR}
    ${OBJ_DIR})

set(SOURCES
    main.cpp)

set(GENERATED_SOURCES
    ${OBJ_DIR}/libcore.cpp)

IMPORT_LIBRARY(comort.so)
IMPORT_LIBRARY(libcore.so)

add_executable(testCalendar
    ${SOURCES}
    ${GENERATED_SOURCES})
target_link_libraries(testCalendar comort.so libcore.so)
add_dependencies(testCalendar libtest)

add_custom_command(
    OUTPUT
        ${GENERATED_SOURCES}
    COMMAND
        "${BIN_DIR}/ccdl"
        -g
        -u
        -p
        -s
        -d ${OBJ_DIR}
        "${BIN_DIR}/libcore.so")

COPY(testCalendar ${OBJ_DIR}/testCalendar ${BIN_DIR})

install(FILES
    ${OBJ_DIR}/testCalendar
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
