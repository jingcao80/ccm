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

project(ReflectionTestUnit CXX)

set(COMPONENT_DIR ${REFLECTION_DIR}/component)
set(OBJ_DIR ${CMAKE_BINARY_DIR}/test/reflection/component)

include_directories(
    ./
    ${INC_DIR}
    ${OBJ_DIR})

set(SOURCES
    CMethodTester.cpp)

set(GENERATED_SOURCES
    ${OBJ_DIR}/_ccm_test_reflection_CMethodTester.cpp
    ${OBJ_DIR}/ReflectionTestUnitPub.cpp
    ${OBJ_DIR}/MetadataWrapper.cpp)

add_library(ccmrt.so UNKNOWN IMPORTED)
set_property(TARGET ccmrt.so PROPERTY IMPORTED_LOCATION ${BIN_DIR}/ccmrt.so)

add_library(ReflectionTestUnit
    SHARED
    ${SOURCES}
    ${GENERATED_SOURCES})
target_link_libraries(ReflectionTestUnit ccmrt.so)

add_custom_command(
    OUTPUT
        ${GENERATED_SOURCES}
    COMMAND
        "${BIN_DIR}/ccdl"
        -c
        -g
        -k
        -d ${OBJ_DIR}
        "${COMPONENT_DIR}/ReflectionTestUnit.cdl")

COPY(ReflectionTestUnit ${OBJ_DIR}/ReflectionTestUnit.so ${BIN_DIR})

install(FILES
    ${OBJ_DIR}/ReflectionTestUnit.so
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