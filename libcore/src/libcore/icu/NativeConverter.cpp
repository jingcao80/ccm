//=========================================================================
// Copyright (C) 2018 The C++ Component Model(COMO) Open Source Project
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

#include "como/core/CoreUtils.h"
#include "libcore/icu/NativeConverter.h"
#include "como/io/charset/CharsetICU.h"
#include "como/io/charset/CodingErrorAction.h"
#include "libcore/icu/UStringEnumeration.h"
#include <memory>
#include <vector>
#include <unicode/ucnv.h>
#include <unicode/ucnv_cb.h>
#include <unicode/uniset.h>
#include <unicode/ustring.h>

using como::core::CoreUtils;
using como::io::charset::CharsetICU;
using como::io::charset::CodingErrorAction;

namespace libcore {
namespace icu {

extern ECode MaybeThrowIcuException(
    /* [in] */ const char* provider,
    /* [in] */ UErrorCode errorCode);

extern String ToUTF8String(
    /* [in] */ const ::icu::UnicodeString& value);

#define MAX_REPLACEMENT_LENGTH 32

struct DecoderCallbackContext
{
    UChar replacementChars[MAX_REPLACEMENT_LENGTH];
    size_t replacementCharCount;
    UConverterToUCallback onUnmappableInput;
    UConverterToUCallback onMalformedInput;
};

struct EncoderCallbackContext
{
    char replacementBytes[MAX_REPLACEMENT_LENGTH];
    size_t replacementByteCount;
    UConverterFromUCallback onUnmappableInput;
    UConverterFromUCallback onMalformedInput;
};

static UConverter* ToUConverter(
    /* [in] */ HANDLE address)
{
    return reinterpret_cast<UConverter*>(address);
}

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

static bool ShouldCodecThrow(
    /* [in] */ Boolean flush,
    /* [in] */ UErrorCode error)
{
    if (flush) {
        return (error != U_BUFFER_OVERFLOW_ERROR && error != U_TRUNCATED_CHAR_FOUND);
    }
    else {
        return (error != U_BUFFER_OVERFLOW_ERROR && error != U_INVALID_CHAR_FOUND && error != U_ILLEGAL_CHAR_FOUND);
    }
}

ECode NativeConverter::Decode(
    /* [in] */ HANDLE handle,
    /* [in] */ const Array<Byte>& input,
    /* [in] */ Integer inEnd,
    /* [out] */ Array<Char>& output,
    /* [in] */ Integer outEnd,
    /* [out] */ Array<Integer>& data,
    /* [in] */ Boolean flush,
    /* [out] */ Integer* error)
{
    UConverter* cnv = ToUConverter(handle);

    Array<Short> utf16chars = CoreUtils::ToUTF16Chars(output);
    int* sourceOffset = &data[0];
    int* targetOffset = &data[1];
    const char* mySource = reinterpret_cast<const char*>(input.GetPayload() + *sourceOffset);
    const char* mySourceLimit = reinterpret_cast<const char*>(input.GetPayload() + inEnd);
    UChar* cTarget = reinterpret_cast<UChar*>(utf16chars.GetPayload()) + *targetOffset;
    const UChar* cTargetLimit = reinterpret_cast<UChar*>(utf16chars.GetPayload()) + outEnd;
    UErrorCode errorCode = U_ZERO_ERROR;
    ucnv_toUnicode(cnv, &cTarget, cTargetLimit, &mySource, mySourceLimit, nullptr, flush, &errorCode);
    *sourceOffset = mySource - reinterpret_cast<const char*>(input.GetPayload()) - *sourceOffset;
    *targetOffset = cTarget - reinterpret_cast<UChar*>(utf16chars.GetPayload()) - *targetOffset;

    // If there was an error, count the problematic bytes.
    if (errorCode == U_ILLEGAL_CHAR_FOUND || errorCode == U_INVALID_CHAR_FOUND ||
        errorCode == U_TRUNCATED_CHAR_FOUND) {
        int8_t invalidByteCount = 32;
        char invalidBytes[32] = {'\0'};
        UErrorCode minorErrorCode = U_ZERO_ERROR;
        ucnv_getInvalidChars(cnv, invalidBytes, &invalidByteCount, &minorErrorCode);
        if (U_SUCCESS(minorErrorCode)) {
            data[2] = invalidByteCount;
        }
    }

    ECode ec = NOERROR;
    if (ShouldCodecThrow(flush, errorCode)) {
        ec = MaybeThrowIcuException("ucnv_toUnicode", errorCode);
    }
    *error = errorCode;
    CoreUtils::ToChars(utf16chars, output);
    return ec;
}

ECode NativeConverter::Encode(
    /* [in] */ HANDLE handle,
    /* [in] */ const Array<Char>& input,
    /* [in] */ Integer inEnd,
    /* [out] */ Array<Byte>& output,
    /* [in] */ Integer outEnd,
    /* [out] */ Array<Integer>& data,
    /* [in] */ Boolean flush,
    /* [out] */ Integer* error)
{
    UConverter* cnv = ToUConverter(handle);

    Array<Short> utf16chars = CoreUtils::ToUTF16Chars(input);
    int* sourceOffset = &data[0];
    int* targetOffset = &data[1];
    const UChar* mySource = reinterpret_cast<UChar*>(utf16chars.GetPayload()) + *sourceOffset;
    const UChar* mySourceLimit= reinterpret_cast<UChar*>(utf16chars.GetPayload()) + inEnd;
    char* cTarget = reinterpret_cast<char*>(output.GetPayload() + *targetOffset);
    const char* cTargetLimit = reinterpret_cast<const char*>(output.GetPayload() + outEnd);
    UErrorCode errorCode = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv , &cTarget, cTargetLimit, &mySource, mySourceLimit, nullptr, (UBool)flush, &errorCode);
    *sourceOffset = (mySource - reinterpret_cast<UChar*>(utf16chars.GetPayload())) - *sourceOffset;
    *targetOffset = (reinterpret_cast<Byte*>(cTarget) - output.GetPayload());

    // If there was an error, count the problematic characters.
    if (errorCode == U_ILLEGAL_CHAR_FOUND || errorCode == U_INVALID_CHAR_FOUND ||
        errorCode == U_TRUNCATED_CHAR_FOUND) {
        int8_t invalidUCharCount = 32;
        UChar invalidUChars[32];
        UErrorCode minorErrorCode = U_ZERO_ERROR;
        ucnv_getInvalidUChars(cnv, invalidUChars, &invalidUCharCount, &minorErrorCode);
        if (U_SUCCESS(minorErrorCode)) {
            data[2] = invalidUCharCount;
        }
    }

    ECode ec = NOERROR;
    if (ShouldCodecThrow(flush, errorCode)) {
        ec = MaybeThrowIcuException("ucnv_fromUnicode", errorCode);
    }
    *error = errorCode;
    return ec;
}

ECode NativeConverter::OpenConverter(
    /* [in] */ const String& charsetName,
    /* [out] */ HANDLE* handle)
{
    if (charsetName.IsNull()) {
        *handle = 0;
        return NOERROR;
    }
    UErrorCode status = U_ZERO_ERROR;
    UConverter* cnv = ucnv_open(charsetName.string(), &status);
    *handle = reinterpret_cast<HANDLE>(cnv);
    return MaybeThrowIcuException("ucnv_open", status);
}

void NativeConverter::CloseConverter(
    /* [in] */ HANDLE handle)
{
    ucnv_close(ToUConverter(handle));
}

void NativeConverter::ResetByteToChar(
    /* [in] */ HANDLE handle)
{
    UConverter* cnv = ToUConverter(handle);
    if (cnv) {
        ucnv_resetToUnicode(cnv);
    }
}

void NativeConverter::ResetCharToByte(
    /* [in] */ HANDLE handle)
{
    UConverter* cnv = ToUConverter(handle);
    if (cnv) {
        ucnv_resetFromUnicode(cnv);
    }
}

Array<Byte> NativeConverter::GetSubstitutionBytes(
    /* [in] */ HANDLE handle)
{
    UConverter* cnv = ToUConverter(handle);
    if (cnv == nullptr) {
        return Array<Byte>::Null();
    }
    UErrorCode status = U_ZERO_ERROR;
    char replacementBytes[MAX_REPLACEMENT_LENGTH];
    int8_t len = sizeof(replacementBytes);
    ucnv_getSubstChars(cnv, replacementBytes, &len, &status);
    if (!U_SUCCESS(status)) {
        return Array<Byte>(0);
    }
    Array<Byte> result(len);
    result.Copy(reinterpret_cast<Byte*>(replacementBytes), len);
    return result;
}

Integer NativeConverter::GetMaxBytesPerChar(
    /* [in] */ HANDLE handle)
{
    UConverter* cnv = ToUConverter(handle);
    return (cnv != nullptr) ? ucnv_getMaxCharSize(cnv) : -1;
}

Integer NativeConverter::GetMinBytesPerChar(
    /* [in] */ HANDLE handle)
{
    UConverter* cnv = ToUConverter(handle);
    return (cnv != nullptr) ? ucnv_getMinCharSize(cnv) : -1;
}

Float NativeConverter::GetAveBytesPerChar(
    /* [in] */ HANDLE handle)
{
    UConverter* cnv = ToUConverter(handle);
    return (cnv != nullptr) ? ((ucnv_getMaxCharSize(cnv) + ucnv_getMinCharSize(cnv)) / 2.0) : -1;
}

Float NativeConverter::GetAveCharsPerByte(
    /* [in] */ HANDLE handle)
{
    return (1 / (Float)GetMaxBytesPerChar(handle));
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

    String comoCanonicalName = GetCcmCanonicalName(icuCanonicalName);

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
    Array<String> comoAliases(aliases.size());
    for (int i = 0; i < aliases.size(); i++) {
        comoAliases[i] = aliases[i];
    }

    String icuCanonicalNameStr(icuCanonicalName);

    // Construct the CharsetICU object.
    AutoPtr<CharsetICU> charsetICU = new CharsetICU();
    charsetICU->Constructor(comoCanonicalName, icuCanonicalNameStr, comoAliases);
    *cs = (ICharset*)charsetICU.Get();
    REFCOUNT_ADD(*cs);
    return NOERROR;
}

Integer NativeConverter::TranslateCodingErrorAction(
    /* [in] */ ICodingErrorAction* action)
{
    if (action == CodingErrorAction::GetREPORT()) {
        return 0;
    } else if (action == CodingErrorAction::GetIGNORE()) {
        return 1;
    } else if (action == CodingErrorAction::GetREPLACE()) {
        return 2;
    } else {
        CHECK(false);
        return -1;
    }
}

ECode NativeConverter::SetCallbackDecode(
    /* [in] */ HANDLE handle,
    /* [in] */ ICharsetDecoder* decoder)
{
    AutoPtr<ICodingErrorAction> action1, action2;
    decoder->GetMalformedInputAction(&action1);
    decoder->GetUnmappableCharacterAction(&action2);
    String replacement;
    decoder->GetReplacement(&replacement);
    return SetCallbackDecode(handle, TranslateCodingErrorAction(action1),
            TranslateCodingErrorAction(action2), replacement);
}

ECode NativeConverter::SetCallbackEncode(
    /* [in] */ HANDLE handle,
    /* [in] */ ICharsetEncoder* encoder)
{
    AutoPtr<ICodingErrorAction> action1, action2;
    encoder->GetMalformedInputAction(&action1);
    encoder->GetUnmappableCharacterAction(&action2);
    Array<Byte> replacement;
    encoder->GetReplacement(&replacement);
    return SetCallbackEncode(handle, TranslateCodingErrorAction(action1),
            TranslateCodingErrorAction(action2), replacement);
}

static void decoderIgnoreCallback(const void*, UConverterToUnicodeArgs*, const char*, int32_t, UConverterCallbackReason, UErrorCode* err)
{
    // The icu4c UCNV_FROM_U_CALLBACK_SKIP callback requires that the context is NULL, which is
    // never true for us.
    *err = U_ZERO_ERROR;
}

static void decoderReplaceCallback(const void* rawContext,
        UConverterToUnicodeArgs* toArgs, const char*, int32_t, UConverterCallbackReason,
        UErrorCode* err)
{
    if (!rawContext) {
        return;
    }
    const DecoderCallbackContext* context = reinterpret_cast<const DecoderCallbackContext*>(rawContext);
    *err = U_ZERO_ERROR;
    ucnv_cbToUWriteUChars(toArgs,context->replacementChars, context->replacementCharCount, 0, err);
}

static UConverterToUCallback GetToUCallback(
    /* [in] */ int32_t mode)
{
    switch (mode) {
    case NativeConverter::IGNORE: return decoderIgnoreCallback;
    case NativeConverter::REPLACE: return decoderReplaceCallback;
    case NativeConverter::REPORT: return UCNV_TO_U_CALLBACK_STOP;
    }
    abort();
}

static void CHARSET_DECODER_CALLBACK(const void* rawContext, UConverterToUnicodeArgs* args,
        const char* codeUnits, int32_t length,
        UConverterCallbackReason reason, UErrorCode* status)
{
    if (!rawContext) {
        return;
    }
    const DecoderCallbackContext* ctx = reinterpret_cast<const DecoderCallbackContext*>(rawContext);
    switch(reason) {
    case UCNV_UNASSIGNED:
        ctx->onUnmappableInput(ctx, args, codeUnits, length, reason, status);
        return;
    case UCNV_ILLEGAL:
    case UCNV_IRREGULAR:
        ctx->onMalformedInput(ctx, args, codeUnits, length, reason, status);
        return;
    case UCNV_CLOSE:
        delete ctx;
        return;
    default:
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }
}

ECode NativeConverter::SetCallbackDecode(
    /* [in] */ HANDLE handle,
    /* [in] */ Integer onMalformedInput,
    /* [in] */ Integer onUnmappableInput,
    /* [in] */ const String& replacement)
{
    if (replacement.IsNull()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    UConverter* cnv = ToUConverter(handle);

    UConverterToUCallback oldCallback;
    const void* oldCallbackContext;
    ucnv_getToUCallBack(cnv, &oldCallback, &oldCallbackContext);

    DecoderCallbackContext* callbackContext = const_cast<DecoderCallbackContext*>(
            reinterpret_cast<const DecoderCallbackContext*>(oldCallbackContext));
    if (callbackContext == nullptr) {
        callbackContext = new DecoderCallbackContext;
    }

    callbackContext->onMalformedInput = GetToUCallback(onMalformedInput);
    callbackContext->onUnmappableInput = GetToUCallback(onUnmappableInput);

    Array<Short> replacementChars = replacement.GetUTF16Chars();
    u_strncpy(callbackContext->replacementChars, reinterpret_cast<UChar*>(replacementChars.GetPayload()), replacement.GetLength());
    callbackContext->replacementCharCount = replacement.GetLength();

    UErrorCode errorCode = U_ZERO_ERROR;
    ucnv_setToUCallBack(cnv, CHARSET_DECODER_CALLBACK, callbackContext, nullptr, nullptr, &errorCode);
    return MaybeThrowIcuException("ucnv_setToUCallBack", errorCode);
}

static void CHARSET_ENCODER_CALLBACK(const void* rawContext, UConverterFromUnicodeArgs* args,
        const UChar* codeUnits, int32_t length, UChar32 codePoint, UConverterCallbackReason reason,
        UErrorCode* status)
{
    if (!rawContext) {
        return;
    }
    const EncoderCallbackContext* ctx = reinterpret_cast<const EncoderCallbackContext*>(rawContext);
    switch(reason) {
    case UCNV_UNASSIGNED:
        ctx->onUnmappableInput(ctx, args, codeUnits, length, codePoint, reason, status);
        return;
    case UCNV_ILLEGAL:
    case UCNV_IRREGULAR:
        ctx->onMalformedInput(ctx, args, codeUnits, length, codePoint, reason, status);
        return;
    case UCNV_CLOSE:
        delete ctx;
        return;
    default:
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }
}

static void encoderReplaceCallback(const void* rawContext,
        UConverterFromUnicodeArgs* fromArgs, const UChar*, int32_t, UChar32,
        UConverterCallbackReason, UErrorCode * err)
{
    if (rawContext == nullptr) {
        return;
    }
    const EncoderCallbackContext* context = reinterpret_cast<const EncoderCallbackContext*>(rawContext);
    *err = U_ZERO_ERROR;
    ucnv_cbFromUWriteBytes(fromArgs, context->replacementBytes, context->replacementByteCount, 0, err);
}

static UConverterFromUCallback GetFromUCallback(
    /* [in] */ int32_t mode)
{
    switch(mode) {
    case NativeConverter::IGNORE: return UCNV_FROM_U_CALLBACK_SKIP;
    case NativeConverter::REPLACE: return encoderReplaceCallback;
    case NativeConverter::REPORT: return UCNV_FROM_U_CALLBACK_STOP;
    }
    abort();
}

ECode NativeConverter::SetCallbackEncode(
    /* [in] */ HANDLE handle,
    /* [in] */ Integer onMalformedInput,
    /* [in] */ Integer onUnmappableInput,
    /* [in] */ const Array<Byte>& replacement)
{
    if (replacement.IsNull()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    UConverter* cnv = ToUConverter(handle);

    UConverterFromUCallback oldCallback;
    const void* oldCallbackContext;
    ucnv_getFromUCallBack(cnv, &oldCallback, &oldCallbackContext);

    EncoderCallbackContext* callbackContext = const_cast<EncoderCallbackContext*>(
            reinterpret_cast<const EncoderCallbackContext*>(oldCallbackContext));
    if (callbackContext == nullptr) {
        callbackContext = new EncoderCallbackContext;
    }

    callbackContext->onMalformedInput = GetFromUCallback(onMalformedInput);
    callbackContext->onUnmappableInput = GetFromUCallback(onUnmappableInput);

    memcpy(callbackContext->replacementBytes, replacement.GetPayload(), replacement.GetLength());
    callbackContext->replacementByteCount = replacement.GetLength();

    UErrorCode errorCode = U_ZERO_ERROR;
    ucnv_setFromUCallBack(cnv, CHARSET_ENCODER_CALLBACK, callbackContext, nullptr, nullptr, &errorCode);
    return MaybeThrowIcuException("ucnv_setFromUCallBack", errorCode);
}

}
}
