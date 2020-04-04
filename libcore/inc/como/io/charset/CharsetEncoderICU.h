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

#ifndef __COMO_IO_CHARSET_CHARSETENCODERICU_H__
#define __COMO_IO_CHARSET_CHARSETENCODERICU_H__

#include "como/io/charset/CharsetEncoder.h"
#include "como.io.IByteBuffer.h"
#include "como.io.ICharBuffer.h"
#include "como.util.IMap.h"

using como::io::IByteBuffer;
using como::io::ICharBuffer;
using como::util::IMap;

namespace como {
namespace io {
namespace charset {

class CharsetEncoderICU
    : public CharsetEncoder
{
public:
    ~CharsetEncoderICU();

    static ECode NewInstance(
        /* [in] */ ICharset* cs,
        /* [in] */ const String& icuCanonicalName,
        /* [out] */ ICharsetEncoder** encoder);

protected:
    void ImplReplaceWith(
        /* [in] */ const Array<Byte>& newReplacement) override;

    void ImplOnMalformedInput(
        /* [in] */ ICodingErrorAction* newAction) override;

    void ImplOnUnmappableCharacter(
        /* [in] */ ICodingErrorAction* newAction) override;

    void ImplReset() override;

    ECode ImplFlush(
        /* [out] */ IByteBuffer* bb,
        /* [out] */ AutoPtr<ICoderResult>& result) override;

    ECode EncodeLoop(
        /* [in] */ ICharBuffer* cb,
        /* [out] */ IByteBuffer* bb,
        /* [out] */ AutoPtr<ICoderResult>& result) override;

private:
    ECode Constructor(
        /* [in] */ ICharset* cs,
        /* [in] */ Float averageBytesPerChar,
        /* [in] */ Float maxBytesPerChar,
        /* [in] */ const Array<Byte>& replacement,
        /* [in] */ HANDLE address);

    static Array<Byte> MakeReplacement(
        /* [in] */ const String& icuCanonicalName,
        /* [in] */ HANDLE address);

    void UpdateCallback();

    Integer GetArray(
        /* [in] */ IByteBuffer* bb);

    Integer GetArray(
        /* [in] */ ICharBuffer* cb);

    void SetPosition(
        /* [in] */ IByteBuffer* bb);

    void SetPosition(
        /* [in] */ ICharBuffer* cb);

    static AutoPtr<IMap> GetDEFAULT_REPLACEMENTS();

private:
    static constexpr Integer INPUT_OFFSET = 0;
    static constexpr Integer OUTPUT_OFFSET = 1;
    static constexpr Integer INVALID_CHAR_COUNT = 2;
    /*
     * data[INPUT_OFFSET]   = on input contains the start of input and on output the number of input chars consumed
     * data[OUTPUT_OFFSET]  = on input contains the start of output and on output the number of output bytes written
     * data[INVALID_CHARS]  = number of invalid chars
     */
    Array<Integer> mData = Array<Integer>(3);

    HANDLE mConverterHandle = 0;

    Array<Char> mInput;
    Array<Byte> mOutput;

    Array<Char> mAllocatedInput;
    Array<Byte> mAllocatedOutput;

    Integer mInEnd = 0;
    Integer mOutEnd = 0;
};

}
}
}

#endif // __COMO_IO_CHARSET_CHARSETENCODERICU_H__
