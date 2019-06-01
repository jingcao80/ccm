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

#include "ccm/io/CharBuffer.h"
#include "ccm/io/charset/CharsetDecoder.h"
#include "ccm/io/charset/CoderResult.h"
#include "ccm.io.IBuffer.h"
#include <ccmlogger.h>

using ccm::core::E_ILLEGAL_STATE_EXCEPTION;

namespace ccm {
namespace io {
namespace charset {

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
    /* [out] */ ICharset** cs)
{
    VALIDATE_NOT_NULL(cs);

    *cs = mCharset;
    REFCOUNT_ADD(*cs);
    return NOERROR;
}

ECode CharsetDecoder::GetReplacement(
    /* [out] */ String* replacement)
{
    VALIDATE_NOT_NULL(replacement);

    *replacement = mReplacement;
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
    /* [out] */ ICodingErrorAction** action)
{
    VALIDATE_NOT_NULL(action);

    *action = mMalformedInputAction;
    REFCOUNT_ADD(*action);
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
    /* [out] */ ICodingErrorAction** action)
{
    VALIDATE_NOT_NULL(action);

    *action = mUnmappableCharacterAction;
    REFCOUNT_ADD(*action);
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
    /* [out] */ Float* averageCharsPerByte)
{
    VALIDATE_NOT_NULL(averageCharsPerByte);

    *averageCharsPerByte = mAverageCharsPerByte;
    return NOERROR;
}

ECode CharsetDecoder::GetMaxCharsPerByte(
    /* [out] */ Float* maxCharsPerByte)
{
    VALIDATE_NOT_NULL(maxCharsPerByte);

    *maxCharsPerByte = mMaxCharsPerByte;
    return NOERROR;
}

ECode CharsetDecoder::Decode(
    /* [in] */ IByteBuffer* bb,
    /* [in] */ ICharBuffer* cb,
    /* [in] */ Boolean endOfInput,
    /* [out] */ ICoderResult** result)
{
    VALIDATE_NOT_NULL(result);

    Integer newState = endOfInput ? ST_END : ST_CODING;
    if ((mState != ST_RESET) && (mState != ST_CODING) &&
            !(endOfInput && (mState == ST_END))) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mState = newState;

    for (;;) {
        AutoPtr<ICoderResult> cr;
        ECode ec = DecodeLoop(bb, cb, &cr);
        if (FAILED(ec)) {
            return E_CODER_MALFUNCTION_ERROR;
        }

        Boolean overflow;
        if (cr->IsOverflow(&overflow), overflow) {
            cr.MoveTo(result);
            return NOERROR;
        }

        Boolean underflow;
        if (cr->IsUnderflow(&underflow), underflow) {
            Boolean hasRemaining;
            if (endOfInput && (IBuffer::Probe(bb)->HasRemaining(&hasRemaining), hasRemaining)) {
                Integer remaining;
                IBuffer::Probe(bb)->Remaining(&remaining);
                cr = CoderResult::MalformedForLength(remaining);
            }
            else {
                cr.MoveTo(result);
                return NOERROR;
            }
        }

        AutoPtr<ICodingErrorAction> action;
        Boolean malformed, unmappable;
        if (cr->IsMalformed(&malformed), malformed) {
            action = mMalformedInputAction;
        }
        else if (cr->IsUnmappable(&unmappable), unmappable) {
            action = mUnmappableCharacterAction;
        }
        else {
            CHECK(false);
        }

        if (action == CodingErrorAction::GetREPORT()) {
            cr.MoveTo(result);
            return NOERROR;
        }

        if (action == CodingErrorAction::GetREPLACE()) {
            Integer remaining;
            if (IBuffer::Probe(cb)->Remaining(&remaining), remaining < mReplacement.GetLength()) {
                AutoPtr<ICoderResult> ret = CoderResult::GetOVERFLOW();
                ret.MoveTo(result);
                return NOERROR;
            }
            cb->Put(mReplacement);
        }

        if ((action == CodingErrorAction::GetIGNORE()) ||
                (action == CodingErrorAction::GetREPLACE())) {
            // Skip erroneous input either way
            Integer pos, len;
            IBuffer::Probe(bb)->GetPosition(&pos);
            cr->GetLength(&len);
            IBuffer::Probe(bb)->SetPosition(pos + len);
            continue;
        }

        CHECK(false);
    }
}

ECode CharsetDecoder::Flush(
    /* [out] */ ICharBuffer* cb,
    /* [out] */ ICoderResult** result)
{
    VALIDATE_NOT_NULL(result);

    if (mState == ST_END) {
        AutoPtr<ICoderResult> cr = ImplFlush(cb);
        Boolean underflow;
        if (cr->IsUnderflow(&underflow), underflow) {
            mState = ST_FLUSHED;
        }
        cr.MoveTo(result);
        return NOERROR;
    }

    if (mState != ST_FLUSHED) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<ICoderResult> cr = CoderResult::GetUNDERFLOW();
    cr.MoveTo(result);
    return NOERROR;
}

AutoPtr<ICoderResult> CharsetDecoder::ImplFlush(
    /* [out] */ ICharBuffer* cb)
{
    return CoderResult::GetUNDERFLOW();
}

ECode CharsetDecoder::Reset()
{
    ImplReset();
    mState = ST_RESET;
    return NOERROR;
}

ECode CharsetDecoder::Decode(
    /* [in] */ IByteBuffer* bb,
    /* [out] */ ICharBuffer** cb)
{
    VALIDATE_NOT_NULL(cb);

    Integer remaining;
    Float averageCharsPerByte;
    IBuffer::Probe(bb)->Remaining(&remaining);
    GetAverageCharsPerByte(&averageCharsPerByte);
    Integer n = remaining * averageCharsPerByte;
    AutoPtr<ICharBuffer> out;
    CharBuffer::Allocate(n, &out);

    if ((n == 0) && (remaining == 0)) {
        out.MoveTo(cb);
        return NOERROR;
    }
    Reset();
    for (;;) {
        AutoPtr<ICoderResult> cr;
        Boolean result;
        if (IBuffer::Probe(bb)->HasRemaining(&result), result) {
            FAIL_RETURN(Decode(bb, out, true, &cr));
        }
        else {
            cr = CoderResult::GetUNDERFLOW();
        }
        if (cr->IsUnderflow(&result), result) {
            cr = nullptr;
            Flush(out, &cr);
        }

        if (cr->IsUnderflow(&result), result) {
            break;
        }
        if (cr->IsOverflow(&result), result) {
            n = 2 * n + 1;
            AutoPtr<ICharBuffer> o;
            CharBuffer::Allocate(n, &o);
            IBuffer::Probe(out)->Flip();
            o->Put(out);
            out = o;
            continue;
        }
        return cr->ThrowException();
    }
    IBuffer::Probe(out)->Flip();
    out.MoveTo(cb);
    return NOERROR;
}

ECode CharsetDecoder::IsAutoDetecting(
    /* [out] */ Boolean* autoDetecting)
{
    VALIDATE_NOT_NULL(autoDetecting);

    *autoDetecting = false;
    return NOERROR;
}

ECode CharsetDecoder::IsCharsetDetected(
    /* [out] */ Boolean* charsetDetected)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CharsetDecoder::GetDetectedCharset(
    /* [out] */ ICharset** cs)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

}
}
}
