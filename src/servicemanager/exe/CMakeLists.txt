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

project(servicemanager CXX)

set(SM_EXE_DIR ${SM_DIR}/exe)
set(OBJ_DIR ${CMAKE_BINARY_DIR}/src/servicemanager/exe)

include_directories(
    ./
    ${INC_DIR}
    ${OBJ_DIR}
    /usr/include/dbus-1.0
    /usr/lib/x86_64-linux-gnu/dbus-1.0/include)

set(SOURCES
    ServiceManager.cpp
    main.cpp)

IMPORT_LIBRARY(ccmrt.so)

add_executable(servicemanager
    ${SOURCES})
target_link_libraries(servicemanager 
    ccmrt.so
    dbus-1)
add_dependencies(servicemanager libcore)

COPY(servicemanager ${OBJ_DIR}/servicemanager ${BIN_DIR})

install(FILES
    ${OBJ_DIR}/servicemanager
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