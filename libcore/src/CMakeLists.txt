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

set(LIBCORE_SRC_DIR ${LIBCORE_DIR}/src)
set(LIBCORE_OBJ_DIR ${OBJ_DIR}/libcore/src)

add_subdirectory(como)
add_subdirectory(comort)
add_subdirectory(libcore)
add_subdirectory(jing)

include_directories(
    ./
    ../inc
    ../inc/como/core
    ../inc/como/io
    ../inc/como/io/charset
    ../inc/como/math
    ../inc/como/misc
    ../inc/como/security
    ../inc/como/security/action
    ../inc/como/text
    ../inc/como/util
    ../inc/como/util/calendar
    ../inc/como/util/concurrent
    ../inc/como/util/concurrent/atomic
    ../inc/como/util/regex
    ../inc/libcore/io
    ../inc/libcore/util
    ../inc/jing/system
    ../inc/comort/system
    ${INC_DIR}
    ${LIBCORE_OBJ_DIR})

set(GENERATED_SOURCES
    ${LIBCORE_OBJ_DIR}/_como_core_CArray.cpp
    ${LIBCORE_OBJ_DIR}/_como_core_CArrayHolder.cpp
    ${LIBCORE_OBJ_DIR}/_como_core_CBoolean.cpp
    ${LIBCORE_OBJ_DIR}/_como_core_CByte.cpp
    ${LIBCORE_OBJ_DIR}/_como_core_CChar.cpp
    ${LIBCORE_OBJ_DIR}/_como_core_CDouble.cpp
    ${LIBCORE_OBJ_DIR}/_como_core_CFloat.cpp
    ${LIBCORE_OBJ_DIR}/_como_core_CInteger.cpp
    ${LIBCORE_OBJ_DIR}/_como_core_CLong.cpp
    ${LIBCORE_OBJ_DIR}/_como_core_CRuntimePermission.cpp
    ${LIBCORE_OBJ_DIR}/_como_core_CShort.cpp
    ${LIBCORE_OBJ_DIR}/_como_core_CStackTrace.cpp
    ${LIBCORE_OBJ_DIR}/_como_core_CStackTraceElement.cpp
    ${LIBCORE_OBJ_DIR}/_como_core_CString.cpp
    ${LIBCORE_OBJ_DIR}/_como_core_CStringBuffer.cpp
    ${LIBCORE_OBJ_DIR}/_como_core_CStringBuilder.cpp
    ${LIBCORE_OBJ_DIR}/_como_core_CSystem.cpp
    ${LIBCORE_OBJ_DIR}/_como_core_CThread.cpp
    ${LIBCORE_OBJ_DIR}/_como_core_CThreadGroup.cpp
    ${LIBCORE_OBJ_DIR}/_como_core_CThreadLocal.cpp
    ${LIBCORE_OBJ_DIR}/_como_io_CBufferedWriter.cpp
    ${LIBCORE_OBJ_DIR}/_como_io_CByteArrayOutputStream.cpp
    ${LIBCORE_OBJ_DIR}/_como_io_CDirectByteBuffer.cpp
    ${LIBCORE_OBJ_DIR}/_como_io_CFile.cpp
    ${LIBCORE_OBJ_DIR}/_como_io_CFileDescriptor.cpp
    ${LIBCORE_OBJ_DIR}/_como_io_CFileInputStream.cpp
    ${LIBCORE_OBJ_DIR}/_como_io_CFileOutputStream.cpp
    ${LIBCORE_OBJ_DIR}/_como_io_COutputStreamWriter.cpp
    ${LIBCORE_OBJ_DIR}/_como_io_CPrintWriter.cpp
    ${LIBCORE_OBJ_DIR}/_como_io_CStringWriter.cpp
    ${LIBCORE_OBJ_DIR}/_como_math_CBigDecimal.cpp
    ${LIBCORE_OBJ_DIR}/_como_math_CBigInteger.cpp
    ${LIBCORE_OBJ_DIR}/_como_math_CMathContext.cpp
    ${LIBCORE_OBJ_DIR}/_como_misc_CFDBigInteger.cpp
    ${LIBCORE_OBJ_DIR}/_como_security_CPermissions.cpp
    ${LIBCORE_OBJ_DIR}/_como_security_CSecureRandom.cpp
    ${LIBCORE_OBJ_DIR}/_como_security_action_CGetPropertyAction.cpp
    ${LIBCORE_OBJ_DIR}/_como_text_CAttributedCharacterIteratorAttribute.cpp
    ${LIBCORE_OBJ_DIR}/_como_text_CAttributedString.cpp
    ${LIBCORE_OBJ_DIR}/_como_text_CChoiceFormat.cpp
    ${LIBCORE_OBJ_DIR}/_como_text_CDateFormatField.cpp
    ${LIBCORE_OBJ_DIR}/_como_text_CDateFormatSymbols.cpp
    ${LIBCORE_OBJ_DIR}/_como_text_CDecimalFormat.cpp
    ${LIBCORE_OBJ_DIR}/_como_text_CDecimalFormatSymbols.cpp
    ${LIBCORE_OBJ_DIR}/_como_text_CFieldPosition.cpp
    ${LIBCORE_OBJ_DIR}/_como_text_CMessageFormat.cpp
    ${LIBCORE_OBJ_DIR}/_como_text_CParsePosition.cpp
    ${LIBCORE_OBJ_DIR}/_como_text_CSimpleDateFormat.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_CArrayList.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_CDate.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_CFormatter.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_CGregorianCalendar.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_CHashMap.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_CHashSet.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_CHashtable.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_CLinkedHashMap.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_CLinkedHashSet.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_CLinkedList.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_CLocale.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_CLocaleBuilder.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_CProperties.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_CPropertyPermission.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_CRandom.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_CSimpleEntry.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_CSimpleImmutableEntry.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_CSimpleTimeZone.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_CStringTokenizer.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_CTreeMap.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_CTreeSet.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_CVector.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_calendar_CEra.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_calendar_CGregorian.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_calendar_CJulianCalendar.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_calendar_CLocalGregorianCalendar.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_concurrent_CConcurrentLinkedQueue.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_concurrent_CConcurrentHashMap.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_concurrent_atomic_CAtomicBoolean.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_concurrent_atomic_CAtomicInteger.cpp
    ${LIBCORE_OBJ_DIR}/_como_util_concurrent_atomic_CAtomicLong.cpp
    ${LIBCORE_OBJ_DIR}/_libcore_io_CLibcore.cpp
    ${LIBCORE_OBJ_DIR}/_libcore_io_CMemoryMappedFile.cpp
    ${LIBCORE_OBJ_DIR}/_libcore_util_CZoneInfoWallTime.cpp
    ${LIBCORE_OBJ_DIR}/_jing_system_CStructStat.cpp
    ${LIBCORE_OBJ_DIR}/_comort_system_CPathClassLoader.cpp
    ${LIBCORE_OBJ_DIR}/libcorePub.cpp
    ${LIBCORE_OBJ_DIR}/MetadataWrapper.cpp)

add_custom_target(build_libcore
    DEPENDS
        deploy_libcore_headers
        ${GENERATED_SOURCES})

set(LIBCORE_LINK_LIBS
    -Wl,--whole-archive
    como_core
    como_io
    como_io_ch
    como_io_charset
    como_math
    como_misc
    como_security
    como_text
    como_util
    como_util_calendar
    como_util_concurrent
    como_util_locale
    como_util_regex
    comort_system
    libcore_icu
    libcore_io
    libcore_math
    libcore_util
    jing_system
    -Wl,--no-whole-archive)

IMPORT_LIBRARY(comort.so)
IMPORT_LIBRARY(libcrypto.so)

add_library(libcore
    SHARED
    ${GENERATED_SOURCES})
if($ENV{PLATFORM} STREQUAL "linux")
target_link_libraries(libcore
    ${LIBCORE_LINK_LIBS}
    comort.so
    libcrypto.so
    dl
    pthread
    unwind
    icuuc
    icui18n
    ${EXTERNAL_DIR}/fdlibm53/lib/libm.a)
add_dependencies(libcore deploy_boringssl_libraries)
elseif($ENV{PLATFORM} STREQUAL "android")
target_link_libraries(libcore
    ${LIBCORE_LINK_LIBS}
    comort.so
    m
    dl
    cutils
    crypto
    icuuc
    icui18n
    fdlibm.a)
endif()

add_custom_command(
    OUTPUT
        ${GENERATED_SOURCES}
    COMMAND
        "$ENV{CDLC}"
        -gen
        -mode-component
        -split
        -d ${LIBCORE_OBJ_DIR}
        -i "${LIBCORE_DIR}/cdl/"
        -c "${LIBCORE_DIR}/src/libcore.cdl")

MKDIR(libcore ${INC_DIR}/core)

COPY(libcore ${LIBCORE_OBJ_DIR}/libcore.so ${BIN_DIR})

