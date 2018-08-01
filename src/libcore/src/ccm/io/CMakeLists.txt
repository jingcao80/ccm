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

set(IO_DIR ${LIBCORE_CCM_SRC_DIR}/io)

add_subdirectory(ch)
add_subdirectory(charset)

include_directories(
    ./
    ../
    ../../../inc
    ${INC_DIR}
    ${OBJ_DIR})

set(SOURCES
    BufferedWriter.cpp
    DeleteOnExitHook.cpp
    ExpiringCache.cpp
    File.cpp
    FileDescriptor.cpp
    FileInputStream.cpp
    FileOutputStream.cpp
    FileSystem.cpp
    InputStream.cpp
    OutputStream.cpp
    OutputStreamWriter.cpp
    PrintWriter.cpp
    StringWriter.cpp
    UnixFileSystem.cpp
    Writer.cpp
    canonicalize_md.cpp
    io.cpp
    io_util_md.cpp)

add_library(ccm_io STATIC
    ${SOURCES})
add_dependencies(ccm_io build_libcore)