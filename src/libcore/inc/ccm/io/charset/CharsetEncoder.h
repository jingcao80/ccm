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

#ifndef __CCM_IO_CHARSET_CHARSETENCODER_H__
#define __CCM_IO_CHARSET_CHARSETENCODER_H__

#include "ccm/core/SyncObject.h"
#include "ccm/io/charset/CodingErrorAction.h"
#include "ccm.core.ICharSequence.h"
#include "ccm.io.charset.ICharset.h"
#include "ccm.io.charset.ICharsetEncoder.h"
#include "ccm.io.charset.ICoderResult.h"

using ccm::core::ICharSequence;
using ccm::core::SyncObject;

namespace ccm {
namespace io {
namespace charset {

class CharsetEncoder
    : public SyncObject
    , public ICharsetEncoder
{
public:
    ECode GetCharset(
        /* [out] */ ICharset** cs) override;

    ECode GetReplacement(
        /* [out, callee] */ Array<Byte>* replacement) override;

    ECode ReplaceWith(
        /* [in] */ const Array<Byte>& newReplacement) override;

    ECode IsLegalReplacement(
        /* [in] */ const Array<Byte>& repl,
        /* [out] */ Boolean* isLegal) override;

    ECode GetMalformedInputAction(
        /* [out] */ ICodingErrorAction** action) override;

    ECode OnMalformedInput(
        /* [in] */ ICodingErrorAction* newAction) override;

    ECode GetUnmappableCharacterAction(
        /* [out] */ ICodingErrorAction** action) override;

    ECode OnUnmappableCharacter(
        /* [in] */ ICodingErrorAction* newAction) override;

    ECode GetAverageBytesPerChar(
        /* [out] */ Float* averageBytesPerChar) override;

    ECode GetMaxBytesPerChar(
        /* [out] */ Float* maxBytesPerChar) override;

    ECode Encode(
        /* [in] */ ICharBuffer* cb,
        /* [out] */ IByteBuffer* bb,
        /* [in] */ Boolean endOfInput,
        /* [out] */ ICoderResult** result) override;

    ECode Flush(
        /* [out] */ IByteBuffer* bb,
        /* [out] */ ICoderResult** result) override;

    ECode Reset() override;

    ECode Encode(
        /* [in] */ ICharBuffer* cb,
        /* [out] */ IByteBuffer** bb) override;

    ECode CanEncode(
        /* [in] */ Char c,
        /* [out] */ Boolean* result) override;

    ECode CanEncode(
        /* [in] */ ICharSequence* cs,
        /* [out] */ Boolean* result) override;

protected:
    ECode Constructor(
        /* [in] */ ICharset* cs,
        /* [in] */ Float averageBytesPerChar,
        /* [in] */ Float maxBytesPerChar,
        /* [in] */ const Array<Byte>& replacement);

    ECode Constructor(
        /* [in] */ ICharset* cs,
        /* [in] */ Float averageBytesPerChar,
        /* [in] */ Float maxBytesPerChar);

    virtual void ImplReplaceWith(
        /* [in] */ const Array<Byte>& newReplacement)
    {}

    virtual void ImplOnMalformedInput(
        /* [in] */ ICodingErrorAction* newAction)
    {}

    virtual void ImplOnUnmappableCharacter(
        /* [in] */ ICodingErrorAction* newAction)
    {}

    virtual AutoPtr<ICoderResult> ImplFlush(
        /* [out] */ IByteBuffer* bb);

    virtual void ImplReset()
    {}

    virtual ECode EncodeLoop(
        /* [in] */ ICharBuffer* cb,
        /* [out] */ IByteBuffer* bb,
        /* [out] */ ICoderResult** result) = 0;

private:
    ECode Constructor(
        /* [in] */ ICharset* cs,
        /* [in] */ Float averageBytesPerChar,
        /* [in] */ Float maxBytesPerChar,
        /* [in] */ const Array<Byte>& replacement,
        /* [in] */ Boolean trusted);

    Boolean CanEncode(
        /* [in] */ ICharBuffer* cb);

private:
    AutoPtr<ICharset> mCharset;
    Float mAverageBytesPerChar = 0;
    Float mMaxBytesPerChar = 0;

    Array<Byte> mReplacement;
    AutoPtr<ICodingErrorAction> mMalformedInputAction =
            CodingErrorAction::GetREPORT();
    AutoPtr<ICodingErrorAction> mUnmappableCharacterAction =
            CodingErrorAction::GetREPORT();

    static constexpr Integer ST_RESET = 0;
    static constexpr Integer ST_CODING = 1;
    static constexpr Integer ST_END = 2;
    static constexpr Integer ST_FLUSHED = 3;

    Integer mState = ST_RESET;

    AutoPtr<IWeakReference> mCachedDecoder;
};

}
}
}

#endif // __CCM_IO_CHARSET_CHARSETENCODER_H__
