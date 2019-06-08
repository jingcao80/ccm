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

#include "ccm/core/StringFactory.h"
#include "ccm/io/ByteBuffer.h"
#include "ccm/util/EmptyArray.h"
#include "ccm.core.IArrayHolder.h"
#include "ccm.core.ICharSequence.h"
#include "ccm.io.IBuffer.h"
#include "ccm.io.ICharBuffer.h"
#include "libcore/util/CharsetUtils.h"

using ccm::io::ByteBuffer;
using ccm::io::IBuffer;
using ccm::io::IByteBuffer;
using ccm::io::ICharBuffer;
using ccm::util::EmptyArray;
using libcore::util::CharsetUtils;

namespace ccm {
namespace core {

ECode StringFactory::NewStringFromBytes(
    /* [in] */ const Array<Byte>& data,
    /* [in] */ Integer offset,
    /* [in] */ Integer byteCount,
    /* [in] */ ICharset* cs,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    if (offset < 0 || byteCount < 0 || byteCount > (data.GetLength() - offset)) {
        return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Array<Char> value;
    Integer length;

    // We inline UTF-8, ISO-8859-1, and US-ASCII decoders for speed.
    String canonicalCharsetName;
    cs->GetName(&canonicalCharsetName);
    if (canonicalCharsetName.Equals("UTF-8")) {
        Array<Byte> d = data;
        Array<Char> v(byteCount);

        Integer idx = offset;
        Integer last = offset + byteCount;
        Integer s = 0;
outer:
        while (idx < last) {
            Byte b0 = d[idx++];
            if ((b0 & 0x80) == 0) {
                // 0xxxxxxx
                // Range:  U-00000000 - U-0000007F
                Integer val = b0 & 0xff;
                v[s++] = val;
            }
            else if (((b0 & 0xe0) == 0xc0) || ((b0 & 0xf0) == 0xe0) ||
                    ((b0 & 0xf8) == 0xf0) || ((b0 & 0xfc) == 0xf8) || ((b0 & 0xfe) == 0xfc)) {
                Integer utfCount = 1;
                if ((b0 & 0xf0) == 0xe0) utfCount = 2;
                else if ((b0 & 0xf8) == 0xf0) utfCount = 3;
                else if ((b0 & 0xfc) == 0xf8) utfCount = 4;
                else if ((b0 & 0xfe) == 0xfc) utfCount = 5;

                // 110xxxxx (10xxxxxx)+
                // Range:  U-00000080 - U-000007FF (count == 1)
                // Range:  U-00000800 - U-0000FFFF (count == 2)
                // Range:  U-00010000 - U-001FFFFF (count == 3)
                // Range:  U-00200000 - U-03FFFFFF (count == 4)
                // Range:  U-04000000 - U-7FFFFFFF (count == 5)

                if (idx + utfCount > last) {
                    v[s++] = REPLACEMENT_CHAR;
                    continue;
                }

                // Extract usable bits from b0
                Integer val = b0 & (0x1f >> (utfCount - 1));
                for (Integer i = 0; i < utfCount; ++i) {
                    Byte b = d[idx++];
                    if ((b & 0xc0) != 0x80) {
                        v[s++] = REPLACEMENT_CHAR;
                        idx--;
                        goto outer;
                    }
                    val <<= 6;
                    val |= b & 0x3f;
                }

                // Allow surrogate values (0xD800 - 0xDFFF) to
                // be specified using 3-byte UTF values only
                if ((utfCount != 2) && (val >= 0xD800) && (val <= 0xDFFF)) {
                    v[s++] = REPLACEMENT_CHAR;
                    continue;
                }

                // Reject chars greater than the Unicode maximum of U+10FFFF.
                if (val > 0x10FFFF) {
                    v[s++] = REPLACEMENT_CHAR;
                    continue;
                }

                v[s++] = val;
            }
            else {
                // Illegal values 0x8*, 0x9*, 0xa*, 0xb*, 0xfd-0xff
                v[s++] = REPLACEMENT_CHAR;
            }
        }

        if (s == byteCount) {
            // We guessed right, so we can use our temporary array as-is.
            value = v;
            length = s;
        }
        else {
            // Our temporary array was too big, so reallocate and copy.
            value = Array<Char>(s);
            length = s;
            value.Copy(0, v, 0, s);
        }
    }
    else if (canonicalCharsetName.Equals("ISO-8859-1")) {
        value = Array<Char>(byteCount);
        length = byteCount;
        CharsetUtils::IsoLatin1BytesToChars(data, offset, byteCount, value);
    }
    else if (canonicalCharsetName.Equals("US-ASCII")) {
        value = Array<Char>(byteCount);
        length = byteCount;
        CharsetUtils::AsciiBytesToChars(data, offset, byteCount, value);
    }
    else {
        AutoPtr<IByteBuffer> bb;
        ByteBuffer::Wrap(data, offset, byteCount, &bb);
        AutoPtr<ICharBuffer> cb;
        cs->Decode(bb, &cb);
        ICharSequence::Probe(cb)->GetLength(&length);
        if (length > 0) {
            value = Array<Char>(length);
            AutoPtr<IArrayHolder> holder;
            IBuffer::Probe(cb)->GetArray((IInterface**)&holder);
            Array<Char> cbArray;
            holder->GetArray(&cbArray);
            value.Copy(0, cbArray, 0, length);
        }
        else {
            value = EmptyArray::CHAR;
        }
    }
    *str = String(value, 0, length);
    return NOERROR;
}

}
}
