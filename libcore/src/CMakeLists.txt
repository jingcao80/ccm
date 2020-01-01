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

set(OBJ_DIR ${CMAKE_BINARY_DIR}/src/libcore/src/)

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
    ${OBJ_DIR})

set(GENERATED_SOURCES
    ${OBJ_DIR}/_como_core_CArray.cpp
    ${OBJ_DIR}/_como_core_CArrayHolder.cpp
    ${OBJ_DIR}/_como_core_CBoolean.cpp
    ${OBJ_DIR}/_como_core_CByte.cpp
    ${OBJ_DIR}/_como_core_CChar.cpp
    ${OBJ_DIR}/_como_core_CDouble.cpp
    ${OBJ_DIR}/_como_core_CFloat.cpp
    ${OBJ_DIR}/_como_core_CInteger.cpp
    ${OBJ_DIR}/_como_core_CLong.cpp
    ${OBJ_DIR}/_como_core_CRuntimePermission.cpp
    ${OBJ_DIR}/_como_core_CShort.cpp
    ${OBJ_DIR}/_como_core_CStackTrace.cpp
    ${OBJ_DIR}/_como_core_CStackTraceElement.cpp
    ${OBJ_DIR}/_como_core_CString.cpp
    ${OBJ_DIR}/_como_core_CStringBuffer.cpp
    ${OBJ_DIR}/_como_core_CStringBuilder.cpp
    ${OBJ_DIR}/_como_core_CSystem.cpp
    ${OBJ_DIR}/_como_core_CThread.cpp
    ${OBJ_DIR}/_como_core_CThreadGroup.cpp
    ${OBJ_DIR}/_como_core_CThreadLocal.cpp
    ${OBJ_DIR}/_como_io_CBufferedWriter.cpp
    ${OBJ_DIR}/_como_io_CByteArrayOutputStream.cpp
    ${OBJ_DIR}/_como_io_CDirectByteBuffer.cpp
    ${OBJ_DIR}/_como_io_CFile.cpp
    ${OBJ_DIR}/_como_io_CFileDescriptor.cpp
    ${OBJ_DIR}/_como_io_CFileInputStream.cpp
    ${OBJ_DIR}/_como_io_CFileOutputStream.cpp
    ${OBJ_DIR}/_como_io_COutputStreamWriter.cpp
    ${OBJ_DIR}/_como_io_CPrintWriter.cpp
    ${OBJ_DIR}/_como_io_CStringWriter.cpp
    ${OBJ_DIR}/_como_math_CBigDecimal.cpp
    ${OBJ_DIR}/_como_math_CBigInteger.cpp
    ${OBJ_DIR}/_como_math_CMathContext.cpp
    ${OBJ_DIR}/_como_misc_CFDBigInteger.cpp
    ${OBJ_DIR}/_como_security_CPermissions.cpp
    ${OBJ_DIR}/_como_security_CSecureRandom.cpp
    ${OBJ_DIR}/_como_security_action_CGetPropertyAction.cpp
    ${OBJ_DIR}/_como_text_CAttributedCharacterIteratorAttribute.cpp
    ${OBJ_DIR}/_como_text_CAttributedString.cpp
    ${OBJ_DIR}/_como_text_CChoiceFormat.cpp
    ${OBJ_DIR}/_como_text_CDateFormatField.cpp
    ${OBJ_DIR}/_como_text_CDateFormatSymbols.cpp
    ${OBJ_DIR}/_como_text_CDecimalFormat.cpp
    ${OBJ_DIR}/_como_text_CDecimalFormatSymbols.cpp
    ${OBJ_DIR}/_como_text_CFieldPosition.cpp
    ${OBJ_DIR}/_como_text_CMessageFormat.cpp
    ${OBJ_DIR}/_como_text_CParsePosition.cpp
    ${OBJ_DIR}/_como_text_CSimpleDateFormat.cpp
    ${OBJ_DIR}/_como_util_CArrayList.cpp
    ${OBJ_DIR}/_como_util_CDate.cpp
    ${OBJ_DIR}/_como_util_CFormatter.cpp
    ${OBJ_DIR}/_como_util_CGregorianCalendar.cpp
    ${OBJ_DIR}/_como_util_CHashMap.cpp
    ${OBJ_DIR}/_como_util_CHashSet.cpp
    ${OBJ_DIR}/_como_util_CHashtable.cpp
    ${OBJ_DIR}/_como_util_CLinkedHashMap.cpp
    ${OBJ_DIR}/_como_util_CLinkedHashSet.cpp
    ${OBJ_DIR}/_como_util_CLinkedList.cpp
    ${OBJ_DIR}/_como_util_CLocale.cpp
    ${OBJ_DIR}/_como_util_CLocaleBuilder.cpp
    ${OBJ_DIR}/_como_util_CProperties.cpp
    ${OBJ_DIR}/_como_util_CPropertyPermission.cpp
    ${OBJ_DIR}/_como_util_CRandom.cpp
    ${OBJ_DIR}/_como_util_CSimpleEntry.cpp
    ${OBJ_DIR}/_como_util_CSimpleImmutableEntry.cpp
    ${OBJ_DIR}/_como_util_CSimpleTimeZone.cpp
    ${OBJ_DIR}/_como_util_CStringTokenizer.cpp
    ${OBJ_DIR}/_como_util_CTreeMap.cpp
    ${OBJ_DIR}/_como_util_CTreeSet.cpp
    ${OBJ_DIR}/_como_util_CVector.cpp
    ${OBJ_DIR}/_como_util_calendar_CEra.cpp
    ${OBJ_DIR}/_como_util_calendar_CGregorian.cpp
    ${OBJ_DIR}/_como_util_calendar_CJulianCalendar.cpp
    ${OBJ_DIR}/_como_util_calendar_CLocalGregorianCalendar.cpp
    ${OBJ_DIR}/_como_util_concurrent_CConcurrentLinkedQueue.cpp
    ${OBJ_DIR}/_como_util_concurrent_CConcurrentHashMap.cpp
    ${OBJ_DIR}/_como_util_concurrent_atomic_CAtomicBoolean.cpp
    ${OBJ_DIR}/_como_util_concurrent_atomic_CAtomicInteger.cpp
    ${OBJ_DIR}/_como_util_concurrent_atomic_CAtomicLong.cpp
    ${OBJ_DIR}/_libcore_io_CLibcore.cpp
    ${OBJ_DIR}/_libcore_io_CMemoryMappedFile.cpp
    ${OBJ_DIR}/_libcore_util_CZoneInfoWallTime.cpp
    ${OBJ_DIR}/_pisces_system_CStructStat.cpp
    ${OBJ_DIR}/_comort_system_CPathClassLoader.cpp
    ${OBJ_DIR}/libcorePub.cpp
    ${OBJ_DIR}/MetadataWrapper.cpp)

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
    pisces_system
    -Wl,--no-whole-archive)

IMPORT_LIBRARY(comort.so)
IMPORT_LIBRARY(libcrypto.so)

add_library(libcore
    SHARED
    ${GENERATED_SOURCES})
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

