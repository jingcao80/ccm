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

set(OBJ_DIR ${CMAKE_BINARY_DIR}/src/libcore/src/)

add_subdirectory(ccm)
add_subdirectory(ccmrt)
add_subdirectory(libcore)
add_subdirectory(pisces)

include_directories(
    ./
    ../inc
    ../inc/ccm/core
    ../inc/ccm/io
    ../inc/ccm/security
    ../inc/ccm/security/action
    ../inc/ccm/util
    ../inc/ccm/util/calendar
    ../inc/ccm/util/concurrent/atomic
    ../inc/ccm/util/regex
    ../inc/ccmrt/system
    ${INC_DIR}
    ${OBJ_DIR})

set(GENERATED_SOURCES
    ${OBJ_DIR}/_ccm_core_CArray.cpp
    ${OBJ_DIR}/_ccm_core_CBoolean.cpp
    ${OBJ_DIR}/_ccm_core_CByte.cpp
    ${OBJ_DIR}/_ccm_core_CChar.cpp
    ${OBJ_DIR}/_ccm_core_CDouble.cpp
    ${OBJ_DIR}/_ccm_core_CFloat.cpp
    ${OBJ_DIR}/_ccm_core_CInteger.cpp
    ${OBJ_DIR}/_ccm_core_CLong.cpp
    ${OBJ_DIR}/_ccm_core_CRuntimeFactory.cpp
    ${OBJ_DIR}/_ccm_core_CRuntimePermission.cpp
    ${OBJ_DIR}/_ccm_core_CShort.cpp
    ${OBJ_DIR}/_ccm_core_CStackTrace.cpp
    ${OBJ_DIR}/_ccm_core_CStackTraceElement.cpp
    ${OBJ_DIR}/_ccm_core_CString.cpp
    ${OBJ_DIR}/_ccm_core_CStringBuffer.cpp
    ${OBJ_DIR}/_ccm_core_CStringBuilder.cpp
    ${OBJ_DIR}/_ccm_core_CSystem.cpp
    ${OBJ_DIR}/_ccm_core_CThread.cpp
    ${OBJ_DIR}/_ccm_core_CThreadGroup.cpp
    ${OBJ_DIR}/_ccm_core_CThreadLocal.cpp
    ${OBJ_DIR}/_ccm_io_CBufferedWriter.cpp
    ${OBJ_DIR}/_ccm_io_CFile.cpp
    ${OBJ_DIR}/_ccm_io_CFileDescriptor.cpp
    ${OBJ_DIR}/_ccm_io_CFileInputStream.cpp
    ${OBJ_DIR}/_ccm_io_CFileOutputStream.cpp
    ${OBJ_DIR}/_ccm_io_COutputStreamWriter.cpp
    ${OBJ_DIR}/_ccm_io_CPrintWriter.cpp
    ${OBJ_DIR}/_ccm_io_CStringWriter.cpp
    ${OBJ_DIR}/_ccm_security_CPermissions.cpp
    ${OBJ_DIR}/_ccm_security_action_CGetPropertyAction.cpp
    ${OBJ_DIR}/_ccm_util_CArrayList.cpp
    ${OBJ_DIR}/_ccm_util_CDate.cpp
    ${OBJ_DIR}/_ccm_util_CFormatter.cpp
    ${OBJ_DIR}/_ccm_util_CHashMap.cpp
    ${OBJ_DIR}/_ccm_util_CHashSet.cpp
    ${OBJ_DIR}/_ccm_util_CHashtable.cpp
    ${OBJ_DIR}/_ccm_util_CLinkedHashMap.cpp
    ${OBJ_DIR}/_ccm_util_CLinkedHashSet.cpp
    ${OBJ_DIR}/_ccm_util_CLocale.cpp
    ${OBJ_DIR}/_ccm_util_CLocaleBuilder.cpp
    ${OBJ_DIR}/_ccm_util_CProperties.cpp
    ${OBJ_DIR}/_ccm_util_CPropertyPermission.cpp
    ${OBJ_DIR}/_ccm_util_CRandom.cpp
    ${OBJ_DIR}/_ccm_util_CStringTokenizer.cpp
    ${OBJ_DIR}/_ccm_util_CTreeMap.cpp
    ${OBJ_DIR}/_ccm_util_CTreeSet.cpp
    ${OBJ_DIR}/_ccm_util_calendar_CCalendarSystemFactory.cpp
    ${OBJ_DIR}/_ccm_util_calendar_CEra.cpp
    ${OBJ_DIR}/_ccm_util_calendar_CGregorian.cpp
    ${OBJ_DIR}/_ccm_util_calendar_CJulianCalendar.cpp
    ${OBJ_DIR}/_ccm_util_calendar_CLocalGregorianCalendar.cpp
    ${OBJ_DIR}/_ccm_util_concurrent_atomic_CAtomicInteger.cpp
    ${OBJ_DIR}/_ccm_util_concurrent_atomic_CAtomicLong.cpp
    ${OBJ_DIR}/_ccm_util_regex_CPatternFactory.cpp
    ${OBJ_DIR}/_ccmrt_system_CPathClassLoader.cpp
    ${OBJ_DIR}/libcorePub.cpp
    ${OBJ_DIR}/MetadataWrapper.cpp)

add_custom_target(build_libcore
    DEPENDS
        deploy_libcore_headers
        ${GENERATED_SOURCES})

set(LIBCORE_LINK_LIBS
    -Wl,--whole-archive
    ccm_core
    ccm_io
    ccm_io_ch
    ccm_io_charset
    ccm_misc
    ccm_security
    ccm_util
    ccm_util_calendar
    ccm_util_concurrent
    ccm_util_locale
    ccm_util_regex
    ccmrt_system
    libcore_icu
    libcore_io
    pisces_system
    -Wl,--no-whole-archive)

IMPORT_LIBRARY(ccmrt.so)

add_library(libcore
    SHARED
    ${GENERATED_SOURCES})
target_link_libraries(libcore
    ${LIBCORE_LINK_LIBS}
    ccmrt.so
    dl
    pthread
    unwind
    icuuc
    icui18n
    ${EXTERNAL_DIR}/fdlibm53/lib/libm.a)

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
        -i ${LIBCORE_DIR}/cdl/
        "${LIBCORE_DIR}/src/libcore.cdl")

MKDIR(libcore ${INC_DIR}/core)

COPY(libcore ${OBJ_DIR}/libcore.so ${BIN_DIR})

