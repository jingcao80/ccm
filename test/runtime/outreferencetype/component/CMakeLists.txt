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

project(ReferenceTypeTestComponent CXX)

set(COMPONENT_SRC_DIR ${REFERENCETYPE_SRC_DIR}/component)
set(COMPONENT_OBJ_DIR ${REFERENCETYPE_OBJ_DIR}/component)

include_directories(
    ./
    ${INC_DIR}
    ${COMPONENT_OBJ_DIR})

set(SOURCES
    CFooBar.cpp)

set(GENERATED_SOURCES
    ${COMPONENT_OBJ_DIR}/_como_test_CFooBar.cpp
    ${COMPONENT_OBJ_DIR}/ReferenceTypeTestPub.cpp
    ${COMPONENT_OBJ_DIR}/MetadataWrapper.cpp)

IMPORT_LIBRARY(comort.so)

add_library(ReferenceTypeTest
    SHARED
    ${SOURCES}
    ${GENERATED_SOURCES})
target_link_libraries(ReferenceTypeTest comort.so)

add_custom_command(
    OUTPUT
        ${GENERATED_SOURCES}
    COMMAND
        "$ENV{CDLC}"
        -gen
        -mode-component
        -split
        -d ${COMPONENT_OBJ_DIR}
        -c "${COMPONENT_SRC_DIR}/outreferencetype.cdl")

COPY(ReferenceTypeTest ${COMPONENT_OBJ_DIR}/ReferenceTypeTest.so ${BIN_DIR})

install(FILES
    ${OBJ_DIR}/ReferenceTypeTest.so
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
