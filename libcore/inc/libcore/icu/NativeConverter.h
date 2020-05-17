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

#ifndef __LIBCORE_ICU_NATIVECONVERTER_H__
#define __LIBCORE_ICU_NATIVECONVERTER_H__

#include "como.io.charset.ICharset.h"
#include "como.io.charset.ICharsetDecoder.h"
#include "como.io.charset.ICharsetEncoder.h"
#include "como.io.charset.ICodingErrorAction.h"

using como::io::charset::ICharset;
using como::io::charset::ICharsetDecoder;
using como::io::charset::ICharsetEncoder;
using como::io::charset::ICodingErrorAction;

namespace libcore {
namespace icu {

class NativeConverter
{
public:
    static ECode Decode(
        /* [in] */ HANDLE handle,
        /* [in] */ const Array<Byte>& input,
        /* [in] */ Integer inEnd,
        /* [out] */ Array<Char>& output,
        /* [in] */ Integer outEnd,
        /* [out] */ Array<Integer>& data,
        /* [in] */ Boolean flush,
        /* [out] */ Integer* error);

    static ECode Encode(
        /* [in] */ HANDLE handle,
        /* [in] */ const Array<Char>& input,
        /* [in] */ Integer inEnd,
        /* [out] */ Array<Byte>& output,
        /* [in] */ Integer outEnd,
        /* [out] */ Array<Integer>& data,
        /* [in] */ Boolean flush,
        /* [out] */ Integer* error);

    static ECode OpenConverter(
        /* [in] */ const String& charsetName,
        /* [out] */ HANDLE* handle);

    static void CloseConverter(
        /* [in] */ HANDLE handle);

    static void ResetByteToChar(
        /* [in] */ HANDLE handle);

    static void ResetCharToByte(
        /* [in] */ HANDLE handle);

    static Array<Byte> GetSubstitutionBytes(
        /* [in] */ HANDLE handle);

    static Integer GetMaxBytesPerChar(
        /* [in] */ HANDLE handle);

    static Integer GetMinBytesPerChar(
        /* [in] */ HANDLE handle);

    static Float GetAveBytesPerChar(
        /* [in] */ HANDLE handle);

    static Float GetAveCharsPerByte(
        /* [in] */ HANDLE handle);

    static Boolean Contains(
        /* [in] */ const String& converterName1,
        /* [in] */ const String& converterName2);

    static Array<String> GetAvailableCharsetNames();

    static ECode CharsetForName(
        /* [in] */ const String& charsetName,
        /* [out] */ AutoPtr<ICharset>& cs);

    static ECode SetCallbackDecode(
        /* [in] */ HANDLE handle,
        /* [in] */ ICharsetDecoder* decoder);

    static ECode SetCallbackEncode(
        /* [in] */ HANDLE handle,
        /* [in] */ ICharsetEncoder* encoder);

private:
    static Integer TranslateCodingErrorAction(
        /* [in] */ ICodingErrorAction* action);

    static ECode SetCallbackDecode(
        /* [in] */ HANDLE handle,
        /* [in] */ Integer onMalformedInput,
        /* [in] */ Integer onUnmappableInput,
        /* [in] */ const String& replacement);

    static ECode SetCallbackEncode(
        /* [in] */ HANDLE handle,
        /* [in] */ Integer onMalformedInput,
        /* [in] */ Integer onUnmappableInput,
        /* [in] */ const Array<Byte>& replacement);

public:
    static constexpr Integer REPORT = 0;
    static constexpr Integer IGNORE = 1;
    static constexpr Integer REPLACE = 2;
};

}
}

#endif // __LIBCORE_ICU_NATIVECONVERTER_H__
