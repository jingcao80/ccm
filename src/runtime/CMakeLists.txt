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

add_compile_options(-std=c++11)

add_subdirectory(component)
add_subdirectory(type)

include_directories(./ util)

set(SOURCES
    ccmids.cpp
    runtime.cpp)

add_library(ccmrt SHARED
    ${SOURCES})
target_link_libraries(ccmrt component)
target_link_libraries(ccmrt metadata)
target_link_libraries(ccmrt type)
target_link_libraries(ccmrt util)

install(DIRECTORY DESTINATION ${CMAKE_BINARY_DIR}/sdk)

install(FILES
    ccmapi.h
    ccmdef.h
    ccmerror.h
    ccmtypes.h
    DESTINATION ${CMAKE_BINARY_DIR}/sdk)
