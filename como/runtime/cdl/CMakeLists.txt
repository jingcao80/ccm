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

set(CDL_OBJ_DIR ${RUNTIME_OBJ_DIR}/cdl)
set(CDL_SRC_DIR ${RUNTIME_SRC_DIR}/cdl)

include_directories(
    ../
    ../type
    ../util
    ${INC_DIR})

set(GENERATED_SOURCES
    ${CDL_OBJ_DIR}/comouuids.cpp
    ${CDL_OBJ_DIR}/MetadataWrapper.cpp)

add_library(cdl STATIC
    ${GENERATED_SOURCES})

add_custom_command(
    OUTPUT
        ${GENERATED_SOURCES}
    COMMAND
        "$ENV{CDLC}"
        -gen
        -mode-runtime
        -d "${CDL_OBJ_DIR}"
        -i "${CDL_SRC_DIR}"
        -c "${CDL_SRC_DIR}/comort.cdl"
    COMMAND
        "${CMAKE_COMMAND}"
        -E copy ${CDL_OBJ_DIR}/_comointfs.h ${INC_DIR})
