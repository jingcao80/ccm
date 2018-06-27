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

#include "ccm/core/Character.h"
#include "ccm/core/CoreUtils.h"
#include "ccm/core/CStringBuffer.h"
#include "ccm/util/Properties.h"
#include "ccm.core.ICharSequence.h"
#include "ccm.core.IInteger.h"
#include "ccm.core.IStringBuffer.h"
#include <ccmlogger.h>

using ccm::core::Character;
using ccm::core::CoreUtils;
using ccm::core::CStringBuffer;
using ccm::core::ICharSequence;
using ccm::core::IInteger;
using ccm::core::IStringBuffer;
using ccm::core::IID_IStringBuffer;

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_1(Properties, Hashtable, IProperties);

ECode Properties::Constructor()
{
    return Constructor(nullptr);
}

ECode Properties::Constructor(
    /* [in] */ IProperties* defaults)
{
    mDefaults = defaults;
    return NOERROR;
}

ECode Properties::SetProperty(
    /* [in] */ const String& key,
    /* [in] */ const String& value,
    /* [out] */ String* prevValue)
{
    AutoPtr<IInterface> old;
    FAIL_RETURN(Put(CoreUtils::Box(key), CoreUtils::Box(value),
            prevValue == nullptr ? nullptr : (IInterface**)&old));
    if (prevValue != nullptr) {
        *prevValue = CoreUtils::Unbox(ICharSequence::Probe(old));
    }
    return NOERROR;
}

ECode Properties::Load(
    /* [in] */ IReader* reader)
{
    return Load0(new LineReader(reader));
}

ECode Properties::Load(
    /* [in] */ IInputStream* instream)
{
    return Load0(new LineReader(instream));
}

ECode Properties::Load0 (
    /* [in] */ AutoPtr<LineReader> lr)
{
    Array<Char> convtBuf(1024);
    Integer limit;
    Integer keyLen;
    Integer valueStart;
    Char c;
    Boolean hasSep;
    Boolean precedingBackslash;

    FAIL_RETURN(lr->ReadLine(&limit));
    while (limit >= 0) {
        c = 0;
        keyLen = 0;
        valueStart = limit;
        hasSep = false;

        precedingBackslash = false;
        while (keyLen < limit) {
            c = lr->mLineBuf[keyLen];
            //need check if escaped.
            if ((c == '=' ||  c == ':') && !precedingBackslash) {
                valueStart = keyLen + 1;
                hasSep = true;
                break;
            }
            else if (Character::IsWhitespace(c) && !precedingBackslash) {
                valueStart = keyLen + 1;
                break;
            }
            if (c == '\\') {
                precedingBackslash = !precedingBackslash;
            }
            else {
                precedingBackslash = false;
            }
            keyLen++;
        }
        while (valueStart < limit) {
            c = lr->mLineBuf[valueStart];
            if (!Character::IsWhitespace(c)) {
                if (!hasSep && (c == '=' ||  c == ':')) {
                    hasSep = true;
                }
                else {
                    break;
                }
            }
            valueStart++;
        }
        String key, value;
        FAIL_RETURN(LoadConvert(lr->mLineBuf, 0, keyLen, convtBuf, &key));
        FAIL_RETURN(LoadConvert(lr->mLineBuf, valueStart, limit - valueStart, convtBuf, &value));
        Put(CoreUtils::Box(key), CoreUtils::Box(value));

        FAIL_RETURN(lr->ReadLine(&limit));
    }
    return NOERROR;
}

ECode Properties::LoadConvert(
    /* [in] */ const Array<Char>& in,
    /* [in] */ Integer off,
    /* [in] */ Integer len,
    /* [in] */ Array<Char>& convtBuf,
    /* [out] */ String* str)
{
    if (convtBuf.GetLength() < len) {
        Integer newLen = len * 2;
        if (newLen < 0) {
            newLen = IInteger::MAX_VALUE;
        }
        convtBuf = Array<Char>(newLen);
    }
    Char aChar;
    Array<Char>& out = convtBuf;
    Integer outLen = 0;
    Integer end = off + len;

    while (off < end) {
        aChar = in[off++];
        if (aChar == '\\') {
            aChar = in[off++];
            if(aChar == 'u') {
                // Read the xxxx
                Integer value = 0;
                for (Integer i = 0; i < 4; i++) {
                    aChar = in[off++];
                    switch (aChar) {
                        case '0': case '1': case '2': case '3': case '4':
                        case '5': case '6': case '7': case '8': case '9':
                            value = (value << 4) + aChar - '0';
                            break;
                        case 'a': case 'b': case 'c':
                        case 'd': case 'e': case 'f':
                            value = (value << 4) + 10 + aChar - 'a';
                            break;
                        case 'A': case 'B': case 'C':
                        case 'D': case 'E': case 'F':
                            value = (value << 4) + 10 + aChar - 'A';
                            break;
                        default:
                            Logger::E("Properties", "Malformed \\uxxxx encoding.");
                            return E_ILLEGAL_ARGUMENT_EXCEPTION;
                    }
                 }
                out[outLen++] = (char)value;
            }
            else {
                if (aChar == 't') aChar = '\t';
                else if (aChar == 'r') aChar = '\r';
                else if (aChar == 'n') aChar = '\n';
                else if (aChar == 'f') aChar = '\f';
                out[outLen++] = aChar;
            }
        }
        else {
            out[outLen++] = aChar;
        }
    }
    *str = String(out, 0, outLen);
    return NOERROR;
}

String Properties::SaveConvert(
    /* [in] */ const String& theString,
    /* [in] */ Boolean escapeSpace,
    /* [in] */ Boolean escapeUnicode)
{
    Integer len = theString.GetLength();
    Integer bufLen = len * 2;
    if (bufLen < 0) {
        bufLen = IInteger::MAX_VALUE;
    }
    AutoPtr<IStringBuffer> outBuffer;
    CStringBuffer::New(bufLen, IID_IStringBuffer, (IInterface**)&outBuffer);

    for(Integer x = 0; x < len; x++) {
        Char aChar = theString.GetChar(x);
        // Handle common case first, selecting largest block that
        // avoids the specials below
        if ((aChar > 61) && (aChar < 127)) {
            if (aChar == '\\') {
                outBuffer->AppendChar('\\');
                outBuffer->AppendChar('\\');
                continue;
            }
            outBuffer->AppendChar(aChar);
            continue;
        }
        switch(aChar) {
            case ' ':
                if (x == 0 || escapeSpace) {
                    outBuffer->AppendChar('\\');
                }
                outBuffer->AppendChar(' ');
                break;
            case '\t':
                outBuffer->AppendChar('\\');
                outBuffer->AppendChar('t');
                break;
            case '\n':
                outBuffer->AppendChar('\\');
                outBuffer->AppendChar('n');
                break;
            case '\r':
                outBuffer->AppendChar('\\');
                outBuffer->AppendChar('r');
                break;
            case '\f':
                outBuffer->AppendChar('\\');
                outBuffer->AppendChar('f');
                break;
            case '=': // Fall through
            case ':': // Fall through
            case '#': // Fall through
            case '!':
                outBuffer->AppendChar('\\');
                outBuffer->AppendChar(aChar);
                break;
            default:
                if (((aChar < 0x0020) || (aChar > 0x007e)) & escapeUnicode ) {
                    outBuffer->AppendChar('\\');
                    outBuffer->AppendChar('u');
                    outBuffer->AppendChar(ToHex((aChar >> 12) & 0xF));
                    outBuffer->AppendChar(ToHex((aChar >>  8) & 0xF));
                    outBuffer->AppendChar(ToHex((aChar >>  4) & 0xF));
                    outBuffer->AppendChar(ToHex( aChar        & 0xF));
                }
                else {
                    outBuffer->AppendChar(aChar);
                }
        }
    }
    String str;
    outBuffer->ToString(&str);
    return str;
}

ECode Properties::WriteComments(
    /* [in] */ IBufferedWriter* bw,
    /* [in] */ const String& comments)
{

}

//-------------------------------------------------------------------

Properties::LineReader::LineReader(
    /* [in] */ IInputStream* inStream)
    : mInStream(inStream)
    , mInByteBuf(8192)
{}

Properties::LineReader::LineReader(
    /* [in] */ IReader* reader)
    : mReader(reader)
    , mInCharBuf(8192)
{}

ECode Properties::LineReader::ReadLine(
    /* [in] */ Integer* number)
{
    Integer len = 0;
    Char c = 0;

    Boolean skipWhiteSpace = true;
    Boolean isCommentLine = false;
    Boolean isNewLine = true;
    Boolean appendedLineBegin = false;
    Boolean precedingBackslash = false;
    Boolean skipLF = false;

    while (true) {
        if (mInOff >= mInLimit) {
            if (mInStream == nullptr) {
                FAIL_RETURN(mReader->Read(mInCharBuf, &mInLimit));
            }
            else {
                FAIL_RETURN(mInStream->Read(mInByteBuf, &mInLimit));
            }
            mInOff = 0;
            if (mInLimit <= 0) {
                if (len == 0 || isCommentLine) {
                    return -1;
                }
                if (precedingBackslash) {
                    len--;
                }
                return len;
            }
        }
        if (mInStream != nullptr) {
            //The line below is equivalent to calling a
            //ISO8859-1 decoder.
            c = (Char) (0xff & mInByteBuf[mInOff++]);
        }
        else {
            c = mInCharBuf[mInOff++];
        }
        if (skipLF) {
            skipLF = false;
            if (c == '\n') {
                continue;
            }
        }
        if (skipWhiteSpace) {
            if (Character::IsWhitespace(c)) {
                continue;
            }
            if (!appendedLineBegin && (c == '\r' || c == '\n')) {
                continue;
            }
            skipWhiteSpace = false;
            appendedLineBegin = false;
        }
        if (isNewLine) {
            isNewLine = false;
            if (c == '#' || c == '!') {
                isCommentLine = true;
                continue;
            }
        }

        if (c != '\n' && c != '\r') {
            mLineBuf[len++] = c;
            if (len == mLineBuf.GetLength()) {
                Integer newLength = mLineBuf.GetLength() * 2;
                if (newLength < 0) {
                    newLength = IInteger::MAX_VALUE;
                }
                Array<Char> buf(newLength);
                buf.Copy(0, mLineBuf, 0, mLineBuf.GetLength());
                mLineBuf = buf;
            }
            //flip the preceding backslash flag
            if (c == '\\') {
                precedingBackslash = !precedingBackslash;
            }
            else {
                precedingBackslash = false;
            }
        }
        else {
            // reached EOL
            if (isCommentLine || len == 0) {
                isCommentLine = false;
                isNewLine = true;
                skipWhiteSpace = true;
                len = 0;
                continue;
            }
            if (mInOff >= mInLimit) {
                if (mInStream == nullptr) {
                    FAIL_RETURN(mReader->Read(mInCharBuf, &mInLimit));
                }
                else {
                    FAIL_RETURN(mInStream->Read(mInByteBuf, &mInLimit));
                }
                mInOff = 0;
                if (mInLimit <= 0) {
                    if (precedingBackslash) {
                        len--;
                    }
                    return len;
                }
            }
            if (precedingBackslash) {
                len -= 1;
                //skip the leading whitespace characters in following line
                skipWhiteSpace = true;
                appendedLineBegin = true;
                precedingBackslash = false;
                if (c == '\r') {
                    skipLF = true;
                }
            }
            else {
                return len;
            }
        }
    }
}

}
}
