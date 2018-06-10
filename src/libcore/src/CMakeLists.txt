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

set(LIBCORE_SRC_DIR ${LIBCORE_DIR}/src)
set(OBJ_DIR ${CMAKE_BINARY_DIR}/src/libcore/src)

set(GENERATED_SOURCES
    ${OBJ_DIR}/_ccm_core_CBoolean.cpp
    ${OBJ_DIR}/_ccm_core_CByte.cpp
    ${OBJ_DIR}/_ccm_core_CChar.cpp
    ${OBJ_DIR}/_ccm_core_CFloat.cpp
    ${OBJ_DIR}/_ccm_core_CInteger.cpp
    ${OBJ_DIR}/_ccm_core_CLong.cpp
    ${OBJ_DIR}/_ccm_core_CShort.cpp
    ${OBJ_DIR}/_ccm_core_CString.cpp
    ${OBJ_DIR}/_ccm_core_CStringBuffer.cpp
    ${OBJ_DIR}/_ccm_core_CStringBuilder.cpp
    ${OBJ_DIR}/_ccm_core_CThread.cpp
    ${OBJ_DIR}/_ccm_core_CThreadGroup.cpp
    ${OBJ_DIR}/libcorePub.cpp
    ${OBJ_DIR}/MetadataWrapper.cpp)

add_custom_target(build_libcore
    DEPENDS
        deploy_libcore_headers
        ${GENERATED_SOURCES})

add_subdirectory(core)

include_directories(
    ./
    ../inc
    ../inc/core
    ${INC_DIR}
    ${OBJ_DIR})

set(LIBCORE_LINK_LIBS
    -Wl,--whole-archive
    core
    -Wl,--no-whole-archive)

add_library(ccmrt.so UNKNOWN IMPORTED)
set_property(TARGET ccmrt.so PROPERTY IMPORTED_LOCATION ${BIN_DIR}/ccmrt.so)

add_library(libcore
    SHARED
    ${GENERATED_SOURCES})
target_link_libraries(libcore
    ${LIBCORE_LINK_LIBS}
    ccmrt.so
    pthread
    unwind
    icuuc)

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
        -i ${LIBCORE_DIR}/cdl
        "${LIBCORE_SRC_DIR}/libcore.cdl")

MKDIR(libcore ${INC_DIR}/core)

COPY(libcore ${OBJ_DIR}/libcore.so ${BIN_DIR})
