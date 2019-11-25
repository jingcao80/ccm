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

set(COMPILER_SRC_DIR ${TEST_SRC_DIR}/compiler)
set(COMPILER_OBJ_DIR ${TEST_OBJ_DIR}/compiler)

include_directories(
    ./
    ${INC_DIR}
    ${COMPILER_OBJ_DIR})

set(SOURCES
    CFoo.cpp
    CFooBar.cpp
    main.cpp)

set(GENERATED_SOURCES
    ${COMPILER_OBJ_DIR}/_CFoo.h
    ${COMPILER_OBJ_DIR}/_CFoo.cpp
    ${COMPILER_OBJ_DIR}/_Namespace1_Namespace2_Namespace3_CFooBar.h
    ${COMPILER_OBJ_DIR}/_Namespace1_Namespace2_Namespace3_CFooBar.cpp
    ${COMPILER_OBJ_DIR}/TestModule.h
    ${COMPILER_OBJ_DIR}/TestModulePub.cpp
    ${COMPILER_OBJ_DIR}/MetadataWrapper.cpp)

add_custom_command(
    OUTPUT
        ${GENERATED_SOURCES}
    COMMAND
        "$ENV{CDLC}"
        -gen
        -mode-component
        -i "${COMPILER_SRC_DIR}"
        -d ${COMPILER_OBJ_DIR}
        -c "${COMPILER_SRC_DIR}/Module.cdl")

IMPORT_LIBRARY(comort.so)

add_executable(testCompiler
    ${SOURCES}
    ${GENERATED_SOURCES})
target_link_libraries(testCompiler comort.so)
