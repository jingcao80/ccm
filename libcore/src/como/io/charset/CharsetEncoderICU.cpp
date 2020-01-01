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

#include "como/core/CArrayHolder.h"
#include "como/core/CoreUtils.h"
#include "como/io/charset/CharsetEncoderICU.h"
#include "como/io/charset/CoderResult.h"
#include "como/util/CHashMap.h"
#include "como/util/EmptyArray.h"
#include "como.core.IArrayHolder.h"
#include "como.io.IBuffer.h"
#include "libcore/icu/ICU.h"
#include "libcore/icu/NativeConverter.h"

using como::core::CArrayHolder;
using como::core::CoreUtils;
using como::core::E_ASSERTION_ERROR;
using como::core::IArrayHolder;
using como::core::IID_IArrayHolder;
using como::util::CHashMap;
using como::util::EmptyArray;
using como::util::IID_IMap;
using libcore::icu::ICU;
using libcore::icu::NativeConverter;

namespace como {
namespace io {
namespace charset {

static AutoPtr<IMap> CreateDEFAULT_REPLACEMENTS()
{
    AutoPtr<IMap> replacements;
    CHashMap::New(IID_IMap, (IInterface**)&replacements);
    Array<Byte> questionMark{ '?' };
    AutoPtr<IArrayHolder> holder;
    CArrayHolder::New(questionMark, IID_IArrayHolder, (IInterface**)&holder);
    replacements->Put(CoreUtils::Box(String("UTF-8")), holder);
    replacements->Put(CoreUtils::Box(String("ISO-8859-1")), holder);
    replacements->Put(CoreUtils::Box(String("US-ASCII")), holder);
    return replacements;
}

AutoPtr<IMap> CharsetEncoderICU::GetDEFAULT_REPLACEMENTS()
{
    static AutoPtr<IMap> DEFAULT_REPLACEMENTS = CreateDEFAULT_REPLACEMENTS();
    return DEFAULT_REPLACEMENTS;
}

CharsetEncoderICU::~CharsetEncoderICU()
{
    NativeConverter::CloseConverter(mConverterHandle);
}

ECode CharsetEncoderICU::NewInstance(
    /* [in] */ ICharset* cs,
    /* [in] */ const String& icuCanonicalName,
    /* [out] */ ICharsetEncoder** encoder)
{
    VALIDATE_NOT_NULL(encoder);

    HANDLE address;
    FAIL_RETURN(NativeConverter::OpenConverter(icuCanonicalName, &address));
    Float averageBytesPerChar = NativeConverter::GetAveBytesPerChar(address);
    Float maxBytesPerChar = NativeConverter::GetMaxBytesPerChar(address);
    Array<Byte> replacement = MakeReplacement(icuCanonicalName, address);
    AutoPtr<CharsetEncoderICU> result = new CharsetEncoderICU();
    result->Constructor(cs, averageBytesPerChar, maxBytesPerChar, replacement, address);
    *encoder = (ICharsetEncoder*)result.Get();
    REFCOUNT_ADD(*encoder);
    return NOERROR;
}

Array<Byte> CharsetEncoderICU::MakeReplacement(
    /* [in] */ const String& icuCanonicalName,
    /* [in] */ HANDLE address)
{
    AutoPtr<IArrayHolder> holder;
    GetDEFAULT_REPLACEMENTS()->Get(CoreUtils::Box(icuCanonicalName), (IInterface**)&holder);
    if (holder != nullptr) {
        Array<Byte> replacement;
        holder->GetArray(&replacement);
        return replacement.Clone();
    }
    return NativeConverter::GetSubstitutionBytes(address);
}

ECode CharsetEncoderICU::Constructor(
    /* [in] */ ICharset* cs,
    /* [in] */ Float averageBytesPerChar,
    /* [in] */ Float maxBytesPerChar,
    /* [in] */ const Array<Byte>& replacement,
    /* [in] */ HANDLE address)
{
    FAIL_RETURN(CharsetEncoder::Constructor(cs, averageBytesPerChar, maxBytesPerChar, replacement, true));
    mConverterHandle = address;
    UpdateCallback();
    return NOERROR;
}

void CharsetEncoderICU::ImplReplaceWith(
    /* [in] */ const Array<Byte>& newReplacement)
{
    UpdateCallback();
}

void CharsetEncoderICU::ImplOnMalformedInput(
    /* [in] */ ICodingErrorAction* newAction)
{
    UpdateCallback();
}

void CharsetEncoderICU::ImplOnUnmappableCharacter(
    /* [in] */ ICodingErrorAction* newAction)
{
    UpdateCallback();
}

void CharsetEncoderICU::UpdateCallback()
{
    NativeConverter::SetCallbackEncode(mConverterHandle, this);
}

void CharsetEncoderICU::ImplReset()
{
    NativeConverter::ResetCharToByte(mConverterHandle);
    mData[INPUT_OFFSET] = 0;
    mData[OUTPUT_OFFSET] = 0;
    mData[INVALID_CHAR_COUNT] = 0;
    mOutput.Clear();
    mInput.Clear();
    mAllocatedInput.Clear();
    mAllocatedOutput.Clear();
    mInEnd = 0;
    mOutEnd = 0;
}

ECode CharsetEncoderICU::ImplFlush(
    /* [out] */ IByteBuffer* bb,
    /* [out] */ ICoderResult** result)
{
    mInput = EmptyArray::CHAR;
    mInEnd = 0;
    mData[INPUT_OFFSET] = 0;

    mData[OUTPUT_OFFSET] = GetArray(bb);
    mData[INVALID_CHAR_COUNT] = 0;

    Integer error;
    ECode ec = NativeConverter::Encode(mConverterHandle,
            mInput, mInEnd, mOutput, mOutEnd, mData, true, &error);
    if (FAILED(ec)) {
        SetPosition(bb);
        ImplReset();
        return ec;
    }
    if (ICU::U_FAILURE(error)) {
        if (error == ICU::U_BUFFER_OVERFLOW_ERROR_) {
            CoderResult::GetOVERFLOW().MoveTo(result);
            return NOERROR;
        }
        else if (error == ICU::U_TRUNCATED_CHAR_FOUND_) {
            if (mData[INVALID_CHAR_COUNT] > 0) {
                CoderResult::MalformedForLength(mData[INVALID_CHAR_COUNT]).MoveTo(result);
                return NOERROR;
            }
        }
    }
    CoderResult::GetUNDERFLOW().MoveTo(result);
    return NOERROR;
}

ECode CharsetEncoderICU::EncodeLoop(
    /* [in] */ ICharBuffer* cb,
    /* [out] */ IByteBuffer* bb,
    /* [out] */ ICoderResult** result)
{
    Boolean hasRemaining;
    if (IBuffer::Probe(cb)->HasRemaining(&hasRemaining), !hasRemaining) {
        CoderResult::GetUNDERFLOW().MoveTo(result);
        return NOERROR;
    }

    mData[INPUT_OFFSET] = GetArray(cb);
    mData[OUTPUT_OFFSET] = GetArray(bb);
    mData[INVALID_CHAR_COUNT] = 0;

    Integer error;
    ECode ec = NativeConverter::Encode(mConverterHandle,
            mInput, mInEnd, mOutput, mOutEnd, mData, false, &error);
    if (FAILED(ec)) {
        SetPosition(cb);
        SetPosition(bb);
        return ec;
    }
    if (ICU::U_FAILURE(error)) {
        if (error == ICU::U_BUFFER_OVERFLOW_ERROR_) {
            CoderResult::GetOVERFLOW().MoveTo(result);
            return NOERROR;
        }
        else if (error == ICU::U_INVALID_CHAR_FOUND_) {
            CoderResult::UnmappableForLength(mData[INVALID_CHAR_COUNT]).MoveTo(result);
            return NOERROR;
        }
        else if (error == ICU::U_ILLEGAL_CHAR_FOUND_) {
            CoderResult::MalformedForLength(mData[INVALID_CHAR_COUNT]).MoveTo(result);
            return NOERROR;
        }
        else {
            return E_ASSERTION_ERROR;
        }
    }
    CoderResult::GetUNDERFLOW().MoveTo(result);
    return NOERROR;
}

Integer CharsetEncoderICU::GetArray(
    /* [in] */ IByteBuffer* bb)
{
    IBuffer* b = IBuffer::Probe(bb);
    Boolean hasArray;
    if (bb->HasArray(&hasArray), hasArray) {
        AutoPtr<IArrayHolder> holder;
        b->GetArray((IInterface**)&holder);
        holder->GetArray(&mOutput);
        Integer offset, lim, pos;
        b->GetArrayOffset(&offset);
        b->GetLimit(&lim);
        b->GetPosition(&pos);
        mOutEnd = offset + lim;
        return offset + pos;
    }
    else {
        b->Remaining(&mOutEnd);
        if (mAllocatedOutput.IsNull() || mOutEnd > mAllocatedOutput.GetLength()) {
            mAllocatedOutput = Array<Byte>(mOutEnd);
        }
        mOutput = mAllocatedOutput;
        return 0;
    }
}

Integer CharsetEncoderICU::GetArray(
    /* [in] */ ICharBuffer* cb)
{
    IBuffer* b = IBuffer::Probe(cb);
    Boolean hasArray;
    if (cb->HasArray(&hasArray), hasArray) {
        AutoPtr<IArrayHolder> holder;
        b->GetArray((IInterface**)&holder);
        holder->GetArray(&mInput);
        Integer offset, lim, pos;
        b->GetArrayOffset(&offset);
        b->GetLimit(&lim);
        b->GetPosition(&pos);
        mInEnd = offset + lim;
        return offset + pos;
    }
    else {
        b->Remaining(&mInEnd);
        if (mAllocatedInput.IsNull() || mInEnd > mAllocatedInput.GetLength()) {
            mAllocatedInput = Array<Char>(mInEnd);
        }
        Integer pos;
        b->GetPosition(&pos);
        cb->Get(mAllocatedInput, 0, mInEnd);
        b->SetPosition(pos);
        mInput = mAllocatedInput;
        return 0;
    }
}

void CharsetEncoderICU::SetPosition(
    /* [in] */ IByteBuffer* bb)
{
    IBuffer* b = IBuffer::Probe(bb);
    Boolean hasArray;
    if (bb->HasArray(&hasArray), hasArray) {
        Integer offset;
        b->GetArrayOffset(&offset);
        b->SetPosition(mData[OUTPUT_OFFSET] - offset);
    }
    else {
        bb->Put(mOutput, 0, mData[OUTPUT_OFFSET]);
    }
    mOutput.Clear();
}

void CharsetEncoderICU::SetPosition(
    /* [in] */ ICharBuffer* cb)
{
    IBuffer* b = IBuffer::Probe(cb);
    Integer pos;
    b->GetPosition(&pos);
    pos = pos + mData[INPUT_OFFSET] - mData[INVALID_CHAR_COUNT];
    if (pos < 0) {
        // The calculated position might be negative if we encountered an
        // invalid char that spanned input buffers. We adjust it to 0 in this case.
        pos = 0;
    }

    b->SetPosition(pos);
    mInput.Clear();
}

}
}
}
