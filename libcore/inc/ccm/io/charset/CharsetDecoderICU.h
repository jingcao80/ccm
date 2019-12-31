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

#ifndef __CCM_IO_CHARSET_CHARSETDECODERICU_H__
#define __CCM_IO_CHARSET_CHARSETDECODERICU_H__

#include "ccm/io/charset/CharsetDecoder.h"
#include "ccm.io.charset.ICharset.h"
#include "ccm.io.charset.ICharsetDecoder.h"

namespace ccm {
namespace io {
namespace charset {

class CharsetDecoderICU
    : public CharsetDecoder
{
public:
    ~CharsetDecoderICU();

    static ECode NewInstance(
        /* [in] */ ICharset* cs,
        /* [in] */ const String& icuCanonicalName,
        /* [out] */ ICharsetDecoder** decoder);

protected:
    void ImplReplaceWith(
        /* [in] */ const String& newReplacement) override;

    void ImplOnMalformedInput(
        /* [in] */ ICodingErrorAction* newAction) override;

    void ImplOnUnmappableCharacter(
        /* [in] */ ICodingErrorAction* newAction) override;

    void ImplReset() override;

    ECode ImplFlush(
        /* [out] */ ICharBuffer* cb,
        /* [out] */ ICoderResult** result) override;

    ECode DecodeLoop(
        /* [in] */ IByteBuffer* bb,
        /* [out] */ ICharBuffer* cb,
        /* [out] */ ICoderResult** result) override;

private:
    ECode Constructor(
        /* [in] */ ICharset* cs,
        /* [in] */ Float averageCharsPerByte,
        /* [in] */ HANDLE address);

    void UpdateCallback();

    Integer GetArray(
        /* [in] */ ICharBuffer* cb);

    Integer GetArray(
        /* [in] */ IByteBuffer* bb);

    void SetPosition(
        /* [in] */ ICharBuffer* cb);

    void SetPosition(
        /* [in] */ IByteBuffer* bb);

private:
    static constexpr Integer MAX_CHARS_PER_BYTE = 2;

    static constexpr Integer INPUT_OFFSET = 0;
    static constexpr Integer OUTPUT_OFFSET = 1;
    static constexpr Integer INVALID_BYTE_COUNT = 2;
    /*
     * data[INPUT_OFFSET]   = on input contains the start of input and on output the number of input bytes consumed
     * data[OUTPUT_OFFSET]  = on input contains the start of output and on output the number of output chars written
     * data[INVALID_BYTE_COUNT]  = number of invalid bytes
     */
    Array<Integer> mData = Array<Integer>(3);

    HANDLE mConverterHandle = 0;

    Array<Byte> mInput;
    Array<Char> mOutput;

    Array<Byte> mAllocatedInput;
    Array<Char> mAllocatedOutput;

    // These instance variables are always assigned in the methods before being used. This class
    // is inherently thread-unsafe so we don't have to worry about synchronization.
    Integer mInEnd;
    Integer mOutEnd;
};

}
}
}

#endif // __CCM_IO_CHARSET_CHARSETDECODERICU_H__
