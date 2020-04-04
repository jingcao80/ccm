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

#include "como/io/charset/CharsetDecoderICU.h"
#include "como/io/charset/CoderResult.h"
#include "como/util/EmptyArray.h"
#include "como.core.IArrayHolder.h"
#include "como.io.IBuffer.h"
#include "libcore/icu/ICU.h"
#include "libcore/icu/NativeConverter.h"

using como::core::E_ASSERTION_ERROR;
using como::core::IArrayHolder;
using como::util::EmptyArray;
using libcore::icu::ICU;
using libcore::icu::NativeConverter;

namespace como {
namespace io {
namespace charset {

CharsetDecoderICU::~CharsetDecoderICU()
{
    NativeConverter::CloseConverter(mConverterHandle);
}

ECode CharsetDecoderICU::NewInstance(
    /* [in] */ ICharset* cs,
    /* [in] */ const String& icuCanonicalName,
    /* [out] */ ICharsetDecoder** decoder)
{
    VALIDATE_NOT_NULL(decoder);

    HANDLE address;
    FAIL_RETURN(NativeConverter::OpenConverter(icuCanonicalName, &address));
    Float averageCharsPerByte = NativeConverter::GetAveCharsPerByte(address);
    AutoPtr<CharsetDecoderICU> result = new CharsetDecoderICU();
    result->Constructor(cs, averageCharsPerByte, address);
    result->UpdateCallback();
    *decoder = (ICharsetDecoder*)result.Get();
    REFCOUNT_ADD(*decoder);
    return NOERROR;
}

ECode CharsetDecoderICU::Constructor(
    /* [in] */ ICharset* cs,
    /* [in] */ Float averageCharsPerByte,
    /* [in] */ HANDLE address)
{
    FAIL_RETURN(CharsetDecoder::Constructor(cs, averageCharsPerByte, MAX_CHARS_PER_BYTE));
    mConverterHandle = address;
    return NOERROR;
}

void CharsetDecoderICU::ImplReplaceWith(
    /* [in] */ const String& newReplacement)
{
    UpdateCallback();
}

void CharsetDecoderICU::ImplOnMalformedInput(
    /* [in] */ ICodingErrorAction* newAction)
{
    UpdateCallback();
}

void CharsetDecoderICU::ImplOnUnmappableCharacter(
    /* [in] */ ICodingErrorAction* newAction)
{
    UpdateCallback();
}

void CharsetDecoderICU::UpdateCallback()
{
    NativeConverter::SetCallbackDecode(mConverterHandle, this);
}

void CharsetDecoderICU::ImplReset()
{
    NativeConverter::ResetByteToChar(mConverterHandle);
    mData[INPUT_OFFSET] = 0;
    mData[OUTPUT_OFFSET] = 0;
    mData[INVALID_BYTE_COUNT] = 0;
    mOutput.Clear();
    mInput.Clear();
    mAllocatedInput.Clear();
    mAllocatedOutput.Clear();
    mInEnd = 0;
    mOutEnd = 0;
}

ECode CharsetDecoderICU::ImplFlush(
    /* [out] */ ICharBuffer* cb,
    /* [out] */ AutoPtr<ICoderResult>& result)
{
    mInput = EmptyArray::BYTE;
    mInEnd = 0;
    mData[INPUT_OFFSET] = 0;

    mData[OUTPUT_OFFSET] = GetArray(cb);
    mData[INVALID_BYTE_COUNT] = 0;

    Integer error;
    ECode ec = NativeConverter::Decode(mConverterHandle,
            mInput, mInEnd, mOutput, mOutEnd, mData, true, &error);
    if (FAILED(ec)) {
        SetPosition(cb);
        ImplReset();
        return ec;
    }
    if (ICU::U_FAILURE(error)) {
        if (error == ICU::U_BUFFER_OVERFLOW_ERROR_) {
            result = CoderResult::GetOVERFLOW();
            return NOERROR;
        }
        else if (error == ICU::U_TRUNCATED_CHAR_FOUND_) {
            if (mData[INVALID_BYTE_COUNT] > 0) {
                result = CoderResult::MalformedForLength(mData[INVALID_BYTE_COUNT]);
                return NOERROR;
            }
        }
    }
    result = CoderResult::GetUNDERFLOW();
    return NOERROR;
}

ECode CharsetDecoderICU::DecodeLoop(
    /* [in] */ IByteBuffer* bb,
    /* [out] */ ICharBuffer* cb,
    /* [out] */ AutoPtr<ICoderResult>& result)
{
    Boolean hasRemaining;
    if (IBuffer::Probe(bb)->HasRemaining(hasRemaining), !hasRemaining) {
        result = CoderResult::GetUNDERFLOW();
        return NOERROR;
    }

    mData[INPUT_OFFSET] = GetArray(bb);
    mData[OUTPUT_OFFSET] = GetArray(cb);

    Integer error;
    ECode ec = NativeConverter::Decode(mConverterHandle,
            mInput, mInEnd, mOutput, mOutEnd, mData, false, &error);
    if (FAILED(ec)) {
        SetPosition(bb);
        SetPosition(cb);
        return ec;
    }
    if (ICU::U_FAILURE(error)) {
        if (error == ICU::U_BUFFER_OVERFLOW_ERROR_) {
            result = CoderResult::GetOVERFLOW();
            return NOERROR;
        }
        else if (error == ICU::U_INVALID_CHAR_FOUND_) {
            result = CoderResult::UnmappableForLength(mData[INVALID_BYTE_COUNT]);
            return NOERROR;
        }
        else if (error == ICU::U_ILLEGAL_CHAR_FOUND_) {
            result = CoderResult::MalformedForLength(mData[INVALID_BYTE_COUNT]);
            return NOERROR;
        }
        else {
            return E_ASSERTION_ERROR;
        }
    }
    result = CoderResult::GetUNDERFLOW();
    return NOERROR;
}

Integer CharsetDecoderICU::GetArray(
    /* [in] */ ICharBuffer* cb)
{
    IBuffer* b = IBuffer::Probe(cb);
    Boolean hasArray;
    if (cb->HasArray(hasArray), hasArray) {
        AutoPtr<IArrayHolder> holder;
        b->GetArray(holder);
        holder->GetArray(&mOutput);
        Integer offset, lim, pos;
        b->GetArrayOffset(offset);
        b->GetLimit(lim);
        b->GetPosition(pos);
        mOutEnd = offset + lim;
        return offset + pos;
    }
    else {
        b->Remaining(mOutEnd);
        if (mAllocatedOutput.IsNull() || mOutEnd > mAllocatedOutput.GetLength()) {
            mAllocatedOutput = Array<Char>(mOutEnd);
        }
        mOutput = mAllocatedOutput;
        return 0;
    }
}

Integer CharsetDecoderICU::GetArray(
    /* [in] */ IByteBuffer* bb)
{
    IBuffer* b = IBuffer::Probe(bb);
    Boolean hasArray;
    if (bb->HasArray(hasArray), hasArray) {
        AutoPtr<IArrayHolder> holder;
        b->GetArray(holder);
        holder->GetArray(&mInput);
        Integer offset, lim, pos;
        b->GetArrayOffset(offset);
        b->GetLimit(lim);
        b->GetPosition(pos);
        mInEnd = offset + lim;
        return offset + pos;
    }
    else {
        b->Remaining(mInEnd);
        if (mAllocatedInput.IsNull() || mInEnd > mAllocatedInput.GetLength()) {
            mAllocatedInput = Array<Byte>(mInEnd);
        }
        Integer pos;
        b->GetPosition(pos);
        bb->Get(mAllocatedInput, 0, mInEnd);
        b->SetPosition(pos);
        mInput = mAllocatedInput;
        return 0;
    }
}

void CharsetDecoderICU::SetPosition(
    /* [in] */ ICharBuffer* cb)
{
    IBuffer* b = IBuffer::Probe(cb);
    Boolean hasArray;
    if (cb->HasArray(hasArray), hasArray) {
        Integer pos;
        b->GetPosition(pos);
        b->SetPosition(pos + mData[OUTPUT_OFFSET]);
    }
    else {
        cb->Put(mOutput, 0, mData[OUTPUT_OFFSET]);
    }
    mOutput.Clear();
}

void CharsetDecoderICU::SetPosition(
    /* [in] */ IByteBuffer* bb)
{
    IBuffer* b = IBuffer::Probe(bb);
    Integer pos;
    b->GetPosition(pos);
    b->SetPosition(pos + mData[INPUT_OFFSET]);
    mInput.Clear();
}

}
}
}
