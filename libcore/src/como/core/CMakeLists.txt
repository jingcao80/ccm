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

set(CORE_DIR ${LIBCORE_COMO_SRC_DIR}/core)

if($ENV{PLATFORM} STREQUAL "linux")
include_directories(
    ./
    ../
    ../../../inc
    ${EXTERNAL_DIR}/fdlibm53/inc
    ${INC_DIR}
    ${LIBCORE_OBJ_DIR})
elseif($ENV{PLATFORM} STREQUAL "android")
include_directories(
    ./
    ../
    ../../../inc
    ${INC_DIR}
    ${LIBCORE_OBJ_DIR})
endif()

set(SOURCES
    AbstractStringBuilder.cpp
    ArrayHolder.cpp
    CArray.cpp
    CBoolean.cpp
    CByte.cpp
    CChar.cpp
    CDouble.cpp
    CFloat.cpp
    Character.cpp
    CInteger.cpp
    ClassLoader.cpp
    CLong.cpp
    CoreUtils.cpp
    CShort.cpp
    CString.cpp
    CStringBuffer.cpp
    CStringBuilder.cpp
    CSystem.cpp
    HardcodedSystemProperties.cpp
    HexStringParser.cpp
    Math.cpp
    NativeBacktrace.cpp
    NativeFaultHandler.cpp
    NativeLockWord.cpp
    NativeMapData.cpp
    NativeMonitor.cpp
    NativeMonitorPool.cpp
    NativeMutex.cpp
    NativeObject.cpp
    NativeRuntime.cpp
    NativeRuntimeCallbacks.cpp
    NativeScopedThreadStateChange.cpp
    NativeThread.cpp
    NativeThreadList.cpp
    NativeTimeUtils.cpp
    RealToString.cpp
    Runnable.cpp
    Runtime.cpp
    RuntimeFactory.cpp
    StackTrace.cpp
    StackTraceElement.cpp
    StrictMath.cpp
    StringFactory.cpp
    StringToReal.cpp
    StringUtils.cpp
    SyncObject.cpp
    System.cpp
    Thread.cpp
    ThreadGroup.cpp
    ThreadLocal.cpp
    cbigint.cpp
    core.cpp
    init.cpp
    nativeapi.cpp)

add_library(como_core STATIC
    ${SOURCES})
add_dependencies(como_core build_libcore)