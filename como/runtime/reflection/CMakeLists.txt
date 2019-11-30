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

set(REFLECTION_SRC_DIR ${RUNTIME_SRC_DIR}/reflection)

include_directories(
    ${RUNTIME_SRC_DIR}
    ${RUNTIME_SRC_DIR}/type
    ${RUNTIME_SRC_DIR}/util
    ${INC_DIR})

set(SOURCES
    CMetaCoclass.cpp
    CMetaComponent.cpp
    CMetaConstant.cpp
    CMetaConstructor.cpp
    CMetaEnumeration.cpp
    CMetaEnumerator.cpp
    CMetaInterface.cpp
    CMetaMethod.cpp
    CMetaParameter.cpp
    CMetaType.cpp
    CMetaValue.cpp
    reflection.cpp)

if($ENV{PLATFORM} STREQUAL "linux")
    set(SOURCES
        ${SOURCES}
        CArgumentList_x64.cpp
        invoke_x64.cpp)
elseif($ENV{PLATFORM} STREQUAL "android")
    set(SOURCES
        ${SOURCES}
        CArgumentList_aarch64.cpp
        invoke_aarch64.s)
    set_source_files_properties(invoke_aarch64.s PROPERTIES COMPILE_FLAGS ${CMAKE_C_FLAGS})
endif()

add_library(reflection STATIC
    ${SOURCES})
add_dependencies(reflection cdl)

COPY(reflection ${REFLECTION_SRC_DIR}/comoreflapi.h ${INC_DIR})

set(DEPLOY_HEADERS
    ${REFLECTION_SRC_DIR}/comoreflapi.h)

install(FILES
    ${DEPLOY_HEADERS}
    DESTINATION ${INC_DIR})
