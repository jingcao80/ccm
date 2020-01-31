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

project(servicemanager CXX)

set(SM_EXE_SRC_DIR ${SM_SRC_DIR}/exe/linux)
set(SM_EXE_OBJ_DIR ${SM_OBJ_DIR}/exe/linux)

include_directories(
    ./
    ${INC_DIR}
    ${SM_EXE_OBJ_DIR}
    /usr/include/dbus-1.0
    /usr/lib/x86_64-linux-gnu/dbus-1.0/include)

set(SOURCES
    ServiceManager.cpp
    main.cpp)

IMPORT_LIBRARY(comort.so)

add_executable(servicemanager
    ${SOURCES})
target_link_libraries(servicemanager
    comort.so
    dbus-1)
add_dependencies(servicemanager comort)

COPY(servicemanager ${SM_EXE_OBJ_DIR}/servicemanager ${BIN_DIR})

install(FILES
    ${SM_EXE_OBJ_DIR}/servicemanager
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