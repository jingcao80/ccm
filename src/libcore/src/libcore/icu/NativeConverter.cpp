//=========================================================================
// Copyright (C) 2018 The C++ Component Model(CCM) Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "libcore/icu/NativeConverter.h"
#include "ccm/io/charset/CharsetICU.h"
#include "libcore/icu/UStringEnumeration.h"
#include <memory>
#include <vector>
#include <unicode/ucnv.h>
#include <unicode/uniset.h>

using ccm::io::charset::CharsetICU;

namespace libcore {
namespace icu {

extern ECode MaybeThrowIcuException(
    /* [in] */ const char* provider,
    /* [in] */ UErrorCode errorCode);

extern String ToUTF8String(
    /* [in] */ const ::icu::UnicodeString& value);

static Boolean CollectStandardNames(
    /* [in] */ const char* canonicalName,
    /* [in] */ const char* standard,
    /* [out] */ std::vector<String>& result)
{
    UErrorCode status = U_ZERO_ERROR;
    UStringEnumeration e(ucnv_openStandardNames(canonicalName, standard, &status));
    if (FAILED(MaybeThrowIcuException("ucnv_openStandardNames", status))) {
        return false;
    }

    int32_t count = e.count(status);
    if (FAILED(MaybeThrowIcuException("StringEnumeration::count", status))) {
        return false;
    }

    for (int32_t i = 0; i < count; ++i) {
        const ::icu::UnicodeString* string = e.snext(status);
        if (FAILED(MaybeThrowIcuException("StringEnumeration::snext", status))) {
            return false;
        }
        String s = ToUTF8String(*string);
        if (s.IndexOf("+,") == -1) {
            result.push_back(s);
        }
  }

  return true;
}

static const char* GetICUCanonicalName(
    /* [in] */ const char* name)
{
    UErrorCode error = U_ZERO_ERROR;
    const char* canonicalName = nullptr;
    if ((canonicalName = ucnv_getCanonicalName(name, "MIME", &error)) != nullptr) {
        return canonicalName;
    }
    else if ((canonicalName = ucnv_getCanonicalName(name, "IANA", &error)) != nullptr) {
        return canonicalName;
    }
    else if ((canonicalName = ucnv_getCanonicalName(name, "", &error)) != nullptr) {
        return canonicalName;
    }
    else if ((canonicalName = ucnv_getAlias(name, 0, &error)) != nullptr) {
        // We have some aliases in the form x-blah .. match those first.
        return canonicalName;
    }
    else if (strstr(name, "x-") == name) {
        // Check if the converter can be opened with the name given.
        error = U_ZERO_ERROR;
        ::icu::LocalUConverterPointer cnv(ucnv_open(name + 2, &error));
        if (U_SUCCESS(error)) {
            return name + 2;
        }
    }
    return nullptr;
}

String GetCcmCanonicalName(
    /* [in] */  const char* icuCanonicalName)
{
    UErrorCode status = U_ZERO_ERROR;

    // Check to see if this is a well-known MIME or IANA name.
    const char* cName = nullptr;
    if ((cName = ucnv_getStandardName(icuCanonicalName, "MIME", &status)) != nullptr) {
        return String(cName);
    }
    else if ((cName = ucnv_getStandardName(icuCanonicalName, "IANA", &status)) != nullptr) {
        return String(cName);
    }

    // Check to see if an alias already exists with "x-" prefix, if yes then
    // make that the canonical name.
    int32_t aliasCount = ucnv_countAliases(icuCanonicalName, &status);
    for (int i = 0; i < aliasCount; ++i) {
        const char* name = ucnv_getAlias(icuCanonicalName, i, &status);
        if (name != nullptr && name[0] == 'x' && name[1] == '-') {
            return String(name);
        }
    }

    // As a last resort, prepend "x-" to any alias and make that the canonical name.
    status = U_ZERO_ERROR;
    const char* name = ucnv_getStandardName(icuCanonicalName, "UTR22", &status);
    if (name == nullptr && strchr(icuCanonicalName, ',') != nullptr) {
        name = ucnv_getAlias(icuCanonicalName, 1, &status);
    }
    // If there is no UTR22 canonical name then just return the original name.
    if (name == nullptr) {
        name = icuCanonicalName;
    }
    std::unique_ptr<char[]> result(new char[2 + strlen(name) + 1]);
    strcpy(&result[0], "x-");
    strcat(&result[0], name);
    return String(&result[0]);
}

Boolean NativeConverter::Contains(
    /* [in] */ const String& converterName1,
    /* [in] */ const String& converterName2)
{
    if (converterName1.IsNull() || converterName2.IsNull()) {
        return false;
    }

    UErrorCode errorCode = U_ZERO_ERROR;
    ::icu::LocalUConverterPointer converter1(ucnv_open(converterName1.string(), &errorCode));
    ::icu::UnicodeSet set1;
    ucnv_getUnicodeSet(&*converter1, set1.toUSet(), UCNV_ROUNDTRIP_SET, &errorCode);

    ::icu::LocalUConverterPointer converter2(ucnv_open(converterName2.string(), &errorCode));
    ::icu::UnicodeSet set2;
    ucnv_getUnicodeSet(&*converter2, set2.toUSet(), UCNV_ROUNDTRIP_SET, &errorCode);

    return U_SUCCESS(errorCode) && set1.containsAll(set2);
}

Array<String> NativeConverter::GetAvailableCharsetNames()
{
    int32_t num = ucnv_countAvailable();
    Array<String> result(num);
    for (Integer i = 0; i < num; ++i) {
        String canonicalName(ucnv_getAvailableName(i));
        result[i] = canonicalName;
    }
    return result;
}

ECode NativeConverter::CharsetForName(
    /* [in] */ const String& charsetName,
    /* [out] */ ICharset** cs)
{
    VALIDATE_NOT_NULL(cs);

    if (charsetName.IsNull()) {
        *cs = nullptr;
        return NOERROR;
    }

    const char* icuCanonicalName = GetICUCanonicalName(charsetName.string());
    if (icuCanonicalName == nullptr) {
        *cs = nullptr;
        return NOERROR;
    }

    String ccmCanonicalName = GetCcmCanonicalName(icuCanonicalName);

    // Check that this charset is supported.
    {
        // ICU doesn't offer any "isSupported", so we just open and immediately close.
        UErrorCode error = U_ZERO_ERROR;
        ::icu::LocalUConverterPointer cnv(ucnv_open(icuCanonicalName, &error));
        if (!U_SUCCESS(error)) {
            *cs = nullptr;
            return NOERROR;
        }
    }

    // Get the aliases for this charset.
    std::vector<String> aliases;
    if (!CollectStandardNames(icuCanonicalName, "IANA", aliases)) {
        *cs = nullptr;
        return NOERROR;
    }
    if (!CollectStandardNames(icuCanonicalName, "MIME", aliases)) {
        *cs = nullptr;
        return NOERROR;
    }
    if (!CollectStandardNames(icuCanonicalName, "JAVA", aliases)) {
        *cs = nullptr;
        return NOERROR;
    }
    if (!CollectStandardNames(icuCanonicalName, "WINDOWS", aliases)) {
        *cs = nullptr;
        return NOERROR;
    }
    Array<String> ccmAliases(aliases.size());
    for (int i = 0; i < aliases.size(); i++) {
        ccmAliases[i] = aliases[i];
    }

    String icuCanonicalNameStr(icuCanonicalName);

    // Construct the CharsetICU object.
    AutoPtr<CharsetICU> charsetICU = new CharsetICU();
    charsetICU->Constructor(ccmCanonicalName, icuCanonicalNameStr, ccmAliases);
    *cs = (ICharset*)charsetICU.Get();
    REFCOUNT_ADD(*cs);
    return NOERROR;
}

}
}
