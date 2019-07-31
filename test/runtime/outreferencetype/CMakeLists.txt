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

project(ReferenceTypeTest CXX)

set(REFERENCETYPE_DIR ${UNIT_TEST_SRC_DIR}/outreferencetype)
set(OBJ_DIR ${UNIT_TEST_OBJ_DIR}/outreferencetype)

include_directories(
    ./
    ${INC_DIR}
    ${OBJ_DIR})

set(SOURCES
    CFooBar.cpp
    main.cpp)

set(GENERATED_SOURCES
    ${OBJ_DIR}/_ccm_test_CFooBar.cpp
    ${OBJ_DIR}/ReferenceTypeTestPub.cpp
    ${OBJ_DIR}/MetadataWrapper.cpp)

IMPORT_LIBRARY(ccmrt.so)

add_executable(testReferenceType
    ${SOURCES}
    ${GENERATED_SOURCES})
target_link_libraries(testReferenceType ccmrt.so)
add_dependencies(testReferenceType libtest)

add_custom_command(
    OUTPUT
        ${GENERATED_SOURCES}
    COMMAND
        "${BIN_DIR}/ccdl"
        -c
        -g
        -k
        -p
        -d ${OBJ_DIR}
        "${REFERENCETYPE_DIR}/outreferencetype.cdl")

COPY(testReferenceType ${OBJ_DIR}/testReferenceType ${BIN_DIR})

install(FILES
    ${OBJ_DIR}/testReferenceType
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
