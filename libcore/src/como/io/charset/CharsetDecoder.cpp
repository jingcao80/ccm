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

#include "como/io/CharBuffer.h"
#include "como/io/charset/CharsetDecoder.h"
#include "como/io/charset/CoderResult.h"
#include "como.io.IBuffer.h"
#include <comolog.h>

using como::core::E_ILLEGAL_STATE_EXCEPTION;

namespace como {
namespace io {
namespace charset {

COMO_INTERFACE_IMPL_1(CharsetDecoder, SyncObject, ICharsetDecoder);

ECode CharsetDecoder::Constructor(
    /* [in] */ ICharset* cs,
    /* [in] */ Float averageCharsPerByte,
    /* [in] */ Float maxCharsPerByte,
    /* [in] */ const String& replacement)
{
    mCharset = cs;
    if (averageCharsPerByte <= 0.0f) {
        Logger::E("CharsetDecoder", "Non-positive averageCharsPerByte");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (maxCharsPerByte <= 0.0f) {
        Logger::E("CharsetDecoder", "Non-positive maxCharsPerByte");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mReplacement = replacement;
    mAverageCharsPerByte = averageCharsPerByte;
    mMaxCharsPerByte = maxCharsPerByte;
    return NOERROR;
}

ECode CharsetDecoder::Constructor(
    /* [in] */ ICharset* cs,
    /* [in] */ Float averageCharsPerByte,
    /* [in] */ Float maxCharsPerByte)
{
    Array<Char> replacement{ 0xFFFD };
    return Constructor(cs, averageCharsPerByte, maxCharsPerByte, String(replacement));
}

ECode CharsetDecoder::GetCharset(
    /* [out] */ AutoPtr<ICharset>& cs)
{
    cs = mCharset;
    return NOERROR;
}

ECode CharsetDecoder::GetReplacement(
    /* [out] */ String& replacement)
{
    replacement = mReplacement;
    return NOERROR;
}

ECode CharsetDecoder::ReplaceWith(
    /* [in] */ const String& newReplacement)
{
    if (newReplacement.IsNull()) {
        Logger::E("CharsetDecoder", "Null replacement");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Integer len = newReplacement.GetLength();
    if (len == 0) {
        Logger::E("CharsetDecoder", "Empty replacement");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (len > mMaxCharsPerByte) {
        Logger::E("CharsetDecoder", "Replacement too long");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mReplacement = newReplacement;

    ImplReplaceWith(mReplacement);
    return NOERROR;
}

ECode CharsetDecoder::GetMalformedInputAction(
    /* [out] */ AutoPtr<ICodingErrorAction>& action)
{
    action = mMalformedInputAction;
    return NOERROR;
}

ECode CharsetDecoder::OnMalformedInput(
    /* [in] */ ICodingErrorAction* newAction)
{
    if (newAction == nullptr) {
        Logger::E("CharsetDecoder", "Null action");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mMalformedInputAction = newAction;
    ImplOnMalformedInput(newAction);
    return NOERROR;
}

ECode CharsetDecoder::GetUnmappableCharacterAction(
    /* [out] */ AutoPtr<ICodingErrorAction>& action)
{
    action = mUnmappableCharacterAction;
    return NOERROR;
}

ECode CharsetDecoder::OnUnmappableCharacter(
    /* [in] */ ICodingErrorAction* newAction)
{
    if (newAction == nullptr) {
        Logger::E("CharsetDecoder", "Null action");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mUnmappableCharacterAction = newAction;
    ImplOnUnmappableCharacter(newAction);
    return NOERROR;
}

ECode CharsetDecoder::GetAverageCharsPerByte(
    /* [out] */ Float& averageCharsPerByte)
{
    averageCharsPerByte = mAverageCharsPerByte;
    return NOERROR;
}

ECode CharsetDecoder::GetMaxCharsPerByte(
    /* [out] */ Float& maxCharsPerByte)
{
    maxCharsPerByte = mMaxCharsPerByte;
    return NOERROR;
}

ECode CharsetDecoder::Decode(
    /* [in] */ IByteBuffer* bb,
    /* [in] */ ICharBuffer* cb,
    /* [in] */ Boolean endOfInput,
    /* [out] */ AutoPtr<ICoderResult>& result)
{
    Integer newState = endOfInput ? ST_END : ST_CODING;
    if ((mState != ST_RESET) && (mState != ST_CODING) &&
            !(endOfInput && (mState == ST_END))) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mState = newState;

    for (;;) {
        ECode ec = DecodeLoop(bb, cb, result);
        if (FAILED(ec)) {
            return E_CODER_MALFUNCTION_ERROR;
        }

        Boolean overflow;
        if (result->IsOverflow(overflow), overflow) {
            return NOERROR;
        }

        Boolean underflow;
        if (result->IsUnderflow(underflow), underflow) {
            Boolean hasRemaining;
            if (endOfInput && (IBuffer::Probe(bb)->HasRemaining(hasRemaining), hasRemaining)) {
                Integer remaining;
                IBuffer::Probe(bb)->Remaining(remaining);
                result = CoderResult::MalformedForLength(remaining);
            }
            else {
                return NOERROR;
            }
        }

        AutoPtr<ICodingErrorAction> action;
        Boolean malformed, unmappable;
        if (result->IsMalformed(malformed), malformed) {
            action = mMalformedInputAction;
        }
        else if (result->IsUnmappable(unmappable), unmappable) {
            action = mUnmappableCharacterAction;
        }
        else {
            CHECK(false);
        }

        if (action == CodingErrorAction::GetREPORT()) {
            return NOERROR;
        }

        if (action == CodingErrorAction::GetREPLACE()) {
            Integer remaining;
            if (IBuffer::Probe(cb)->Remaining(remaining), remaining < mReplacement.GetLength()) {
                result = CoderResult::GetOVERFLOW();
                return NOERROR;
            }
            cb->Put(mReplacement);
        }

        if ((action == CodingErrorAction::GetIGNORE()) ||
                (action == CodingErrorAction::GetREPLACE())) {
            // Skip erroneous input either way
            Integer pos, len;
            IBuffer::Probe(bb)->GetPosition(pos);
            result->GetLength(len);
            IBuffer::Probe(bb)->SetPosition(pos + len);
            continue;
        }

        CHECK(false);
    }
}

ECode CharsetDecoder::Flush(
    /* [out] */ ICharBuffer* cb,
    /* [out] */ AutoPtr<ICoderResult>& result)
{
    if (mState == ST_END) {
        FAIL_RETURN(ImplFlush(cb, result));
        Boolean underflow;
        if (result->IsUnderflow(underflow), underflow) {
            mState = ST_FLUSHED;
        }
        return NOERROR;
    }

    if (mState != ST_FLUSHED) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    result = CoderResult::GetUNDERFLOW();
    return NOERROR;
}

ECode CharsetDecoder::ImplFlush(
    /* [out] */ ICharBuffer* cb,
    /* [out] */ AutoPtr<ICoderResult>& result)
{
    result = CoderResult::GetUNDERFLOW();
    return NOERROR;
}

ECode CharsetDecoder::Reset()
{
    ImplReset();
    mState = ST_RESET;
    return NOERROR;
}

ECode CharsetDecoder::Decode(
    /* [in] */ IByteBuffer* bb,
    /* [out] */ AutoPtr<ICharBuffer>& cb)
{
    Integer remaining;
    Float averageCharsPerByte;
    IBuffer::Probe(bb)->Remaining(remaining);
    GetAverageCharsPerByte(averageCharsPerByte);
    Integer n = remaining * averageCharsPerByte;
    CharBuffer::Allocate(n, &cb);

    if ((n == 0) && (remaining == 0)) {
        return NOERROR;
    }
    Reset();
    for (;;) {
        AutoPtr<ICoderResult> cr;
        Boolean result;
        if (IBuffer::Probe(bb)->HasRemaining(result), result) {
            FAIL_RETURN(Decode(bb, cb, true, cr));
        }
        else {
            cr = CoderResult::GetUNDERFLOW();
        }
        if (cr->IsUnderflow(result), result) {
            Flush(cb, cr);
        }

        if (cr->IsUnderflow(result), result) {
            break;
        }
        if (cr->IsOverflow(result), result) {
            n = 2 * n + 1;
            AutoPtr<ICharBuffer> o;
            CharBuffer::Allocate(n, &o);
            IBuffer::Probe(cb)->Flip();
            o->Put(cb);
            cb = o;
            continue;
        }
        return cr->ThrowException();
    }
    IBuffer::Probe(cb)->Flip();
    return NOERROR;
}

ECode CharsetDecoder::IsAutoDetecting(
    /* [out] */ Boolean& autoDetecting)
{
    autoDetecting = false;
    return NOERROR;
}

ECode CharsetDecoder::IsCharsetDetected(
    /* [out] */ Boolean& charsetDetected)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CharsetDecoder::GetDetectedCharset(
    /* [out] */ AutoPtr<ICharset>& cs)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

}
}
}
