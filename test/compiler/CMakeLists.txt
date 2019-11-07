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

set(COMPILER_DIR ${TEST_DIR}/compiler)
set(OBJ_DIR ${CMAKE_BINARY_DIR}/test/compiler)

include_directories(
    ./
    ${INC_DIR}
    ${OBJ_DIR})

set(SOURCES
    CFoo.cpp
    CFooBar.cpp
    main.cpp)

set(GENERATED_SOURCES
    ${OBJ_DIR}/_CFoo.h
    ${OBJ_DIR}/_CFoo.cpp
    ${OBJ_DIR}/_Namespace1_Namespace2_Namespace3_CFooBar.h
    ${OBJ_DIR}/_Namespace1_Namespace2_Namespace3_CFooBar.cpp
    ${OBJ_DIR}/TestModule.h
    ${OBJ_DIR}/TestModulePub.cpp
    ${OBJ_DIR}/MetadataWrapper.cpp)

add_custom_command(
    OUTPUT
        ${GENERATED_SOURCES}
    COMMAND
        "${BIN_DIR}/cdlc"
        -gen
        -mode-component
        -i "${COMPILER_DIR}"
        -d ${OBJ_DIR}
        -c "${COMPILER_DIR}/Module.cdl")

IMPORT_LIBRARY(comort.so)

add_executable(testCompiler
    ${SOURCES}
    ${GENERATED_SOURCES})
target_link_libraries(testCompiler comort.so)
