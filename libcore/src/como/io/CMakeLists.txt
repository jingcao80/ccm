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

set(IO_DIR ${LIBCORE_COMO_SRC_DIR}/io)

add_subdirectory(ch)
add_subdirectory(charset)

include_directories(
    ./
    ../
    ../../../inc
    ${INC_DIR}
    ${LIBCORE_OBJ_DIR})

set(SOURCES
    Bits.cpp
    Buffer.cpp
    BufferedWriter.cpp
    ByteArrayOutputStream.cpp
    ByteBuffer.cpp
    ByteBufferAsCharBuffer.cpp
    ByteBufferAsDoubleBuffer.cpp
    ByteBufferAsFloatBuffer.cpp
    ByteBufferAsIntegerBuffer.cpp
    ByteBufferAsLongBuffer.cpp
    ByteBufferAsShortBuffer.cpp
    ByteBufferFactory.cpp
    ByteOrder.cpp
    CharBuffer.cpp
    DeleteOnExitHook.cpp
    DirectByteBuffer.cpp
    DoubleBuffer.cpp
    ExpiringCache.cpp
    File.cpp
    FileDescriptor.cpp
    FileInputStream.cpp
    FileOutputStream.cpp
    FileSystem.cpp
    FloatBuffer.cpp
    HeapByteBuffer.cpp
    HeapCharBuffer.cpp
    HeapDoubleBuffer.cpp
    HeapFloatBuffer.cpp
    HeapIntegerBuffer.cpp
    HeapLongBuffer.cpp
    HeapShortBuffer.cpp
    InputStream.cpp
    IntegerBuffer.cpp
    LongBuffer.cpp
    MappedByteBuffer.cpp
    OutputStream.cpp
    OutputStreamWriter.cpp
    PrintWriter.cpp
    ShortBuffer.cpp
    StringCharBuffer.cpp
    StringWriter.cpp
    UnixFileSystem.cpp
    Writer.cpp
    canonicalize_md.cpp
    io.cpp
    io_util_md.cpp)

add_library(como_io STATIC
    ${SOURCES})
add_dependencies(como_io build_libcore)