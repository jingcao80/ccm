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

set(LIBCORE_CCM_SRC_DIR ${LIBCORE_DIR}/src/ccm)
set(OBJ_DIR ${CMAKE_BINARY_DIR}/src/libcore/src/ccm)

set(GENERATED_SOURCES
    ${OBJ_DIR}/_ccm_core_CArray.cpp
    ${OBJ_DIR}/_ccm_core_CBoolean.cpp
    ${OBJ_DIR}/_ccm_core_CByte.cpp
    ${OBJ_DIR}/_ccm_core_CChar.cpp
    ${OBJ_DIR}/_ccm_core_CDouble.cpp
    ${OBJ_DIR}/_ccm_core_CFloat.cpp
    ${OBJ_DIR}/_ccm_core_CInteger.cpp
    ${OBJ_DIR}/_ccm_core_CLong.cpp
    ${OBJ_DIR}/_ccm_core_CShort.cpp
    ${OBJ_DIR}/_ccm_core_CString.cpp
    ${OBJ_DIR}/_ccm_core_CStringBuffer.cpp
    ${OBJ_DIR}/_ccm_core_CStringBuilder.cpp
    ${OBJ_DIR}/_ccm_core_CThread.cpp
    ${OBJ_DIR}/_ccm_core_CThreadGroup.cpp
    ${OBJ_DIR}/_ccm_util_CArrayList.cpp
    ${OBJ_DIR}/_ccm_util_CHashtable.cpp
    ${OBJ_DIR}/_ccm_util_regex_CPatternFactory.cpp
    ${OBJ_DIR}/libcorePub.cpp
    ${OBJ_DIR}/MetadataWrapper.cpp)

add_custom_target(build_libcore
    DEPENDS
        deploy_libcore_headers
        ${GENERATED_SOURCES})

add_subdirectory(core)
add_subdirectory(util)

include_directories(
    ./
    ../../inc
    ../../inc/ccm/core
    ../../inc/ccm/util
    ../../inc/ccm/util/regex
    ${INC_DIR}
    ${OBJ_DIR})

set(LIBCORE_LINK_LIBS
    -Wl,--whole-archive
    core
    regex
    libcore_util
    -Wl,--no-whole-archive)

IMPORT_LIBRARY(ccmrt.so)

add_library(libcore
    SHARED
    ${GENERATED_SOURCES})
target_link_libraries(libcore
    ${LIBCORE_LINK_LIBS}
    ccmrt.so
    pthread
    unwind
    icuuc
    icui18n)

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
        -i ${LIBCORE_DIR}/cdl/ccm
        "${LIBCORE_CCM_SRC_DIR}/libcore.cdl")

MKDIR(libcore ${INC_DIR}/core)

COPY(libcore ${OBJ_DIR}/libcore.so ${BIN_DIR})
