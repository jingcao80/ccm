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

project(cdlc CXX)

set(CDLC_DIR ${SRC_DIR}/cdlc)
set(OBJ_DIR ${CMAKE_BINARY_DIR}/src/cdlc)

add_subdirectory(ast)
add_subdirectory(metadata)
add_subdirectory(parser)
add_subdirectory(phase)
add_subdirectory(util)

include_directories(
    ${CDLC_DIR}
    ${SRC_DIR})

set(SOURCES
    main.cpp)

set(CDLC_LINK_LIBS
    -Wl,--whole-archive
    ast
    metadata2
    parser
    phase
    util
    -Wl,--no-whole-archive)

add_executable(cdlc
    ${SOURCES})
target_link_libraries(cdlc
    ${CDLC_LINK_LIBS}
    metadata)
add_dependencies(cdlc
    metadata)

COPY(cdlc ${CMAKE_BINARY_DIR}/src/cdlc/cdlc ${BIN_DIR})

install(FILES
    ${CMAKE_BINARY_DIR}/src/cdlc/cdlc
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
