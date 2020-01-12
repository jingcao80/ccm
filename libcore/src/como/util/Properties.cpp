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

#include "como/core/AutoLock.h"
#include "como/core/Character.h"
#include "como/core/CoreUtils.h"
#include "como/core/CStringBuffer.h"
#include "como/io/CBufferedWriter.h"
#include "como/io/COutputStreamWriter.h"
#include "como/util/CDate.h"
#include "como/util/CHashtable.h"
#include "como/util/Properties.h"
#include "como/util/XMLUtils.h"
#include "como.core.ICharSequence.h"
#include "como.core.IInteger.h"
#include "como.core.IString.h"
#include "como.core.IStringBuffer.h"
#include "como.io.IWriter.h"
#include <comolog.h>

using como::core::AutoLock;
using como::core::Character;
using como::core::CoreUtils;
using como::core::CStringBuffer;
using como::core::ICharSequence;
using como::core::IInteger;
using como::core::IString;
using como::core::IStringBuffer;
using como::core::IID_IStringBuffer;
using como::io::CBufferedWriter;
using como::io::COutputStreamWriter;
using como::io::IBufferedWriter;
using como::io::IOutputStreamWriter;
using como::io::IID_IBufferedWriter;
using como::io::IID_IOutputStreamWriter;
using como::io::IWriter;

namespace como {
namespace util {

COMO_INTERFACE_IMPL_1(Properties, Hashtable, IProperties);

ECode Properties::Constructor()
{
    return Constructor(nullptr);
}

ECode Properties::Constructor(
    /* [in] */ IProperties* defaults)
{
    Hashtable::Constructor();
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
            prevValue == nullptr ? nullptr : &old));
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
            if ((c == U'=' ||  c == U':') && !precedingBackslash) {
                valueStart = keyLen + 1;
                hasSep = true;
                break;
            }
            else if (Character::IsWhitespace(c) && !precedingBackslash) {
                valueStart = keyLen + 1;
                break;
            }
            if (c == U'\\') {
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
                if (!hasSep && (c == U'=' ||  c == U':')) {
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
        if (aChar == U'\\') {
            aChar = in[off++];
            if(aChar == U'u') {
                // Read the xxxx
                Integer value = 0;
                for (Integer i = 0; i < 4; i++) {
                    aChar = in[off++];
                    switch (aChar) {
                        case U'0': case U'1': case U'2': case U'3': case U'4':
                        case U'5': case U'6': case U'7': case U'8': case U'9':
                            value = (value << 4) + aChar - U'0';
                            break;
                        case U'a': case U'b': case U'c':
                        case U'd': case U'e': case U'f':
                            value = (value << 4) + 10 + aChar - U'a';
                            break;
                        case U'A': case U'B': case U'C':
                        case U'D': case U'E': case U'F':
                            value = (value << 4) + 10 + aChar - U'A';
                            break;
                        default:
                            Logger::E("Properties", "Malformed \\uxxxx encoding.");
                            return E_ILLEGAL_ARGUMENT_EXCEPTION;
                    }
                 }
                out[outLen++] = (char)value;
            }
            else {
                if (aChar == U't') aChar = U'\t';
                else if (aChar == U'r') aChar = U'\r';
                else if (aChar == U'n') aChar = U'\n';
                else if (aChar == U'f') aChar = U'\f';
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
            if (aChar == U'\\') {
                outBuffer->Append(U'\\');
                outBuffer->Append(U'\\');
                continue;
            }
            outBuffer->Append(aChar);
            continue;
        }
        switch(aChar) {
            case U' ':
                if (x == 0 || escapeSpace) {
                    outBuffer->Append(U'\\');
                }
                outBuffer->Append(U' ');
                break;
            case U'\t':
                outBuffer->Append(U'\\');
                outBuffer->Append(U't');
                break;
            case U'\n':
                outBuffer->Append(U'\\');
                outBuffer->Append(U'n');
                break;
            case U'\r':
                outBuffer->Append(U'\\');
                outBuffer->Append(U'r');
                break;
            case U'\f':
                outBuffer->Append(U'\\');
                outBuffer->Append(U'f');
                break;
            case U'=': // Fall through
            case U':': // Fall through
            case U'#': // Fall through
            case U'!':
                outBuffer->Append(U'\\');
                outBuffer->Append(aChar);
                break;
            default:
                if (((aChar < 0x0020) || (aChar > 0x007e)) & escapeUnicode ) {
                    outBuffer->Append(U'\\');
                    outBuffer->Append(U'u');
                    outBuffer->Append(ToHex((aChar >> 12) & 0xF));
                    outBuffer->Append(ToHex((aChar >>  8) & 0xF));
                    outBuffer->Append(ToHex((aChar >>  4) & 0xF));
                    outBuffer->Append(ToHex( aChar        & 0xF));
                }
                else {
                    outBuffer->Append(aChar);
                }
        }
    }
    String str;
    outBuffer->ToString(str);
    return str;
}

ECode Properties::WriteComments(
    /* [in] */ IBufferedWriter* bw,
    /* [in] */ const String& comments)
{
    FAIL_RETURN(IWriter::Probe(bw)->Write(String("#")));
    Integer len = comments.GetLength();
    Integer current = 0;
    Integer last = 0;
    Array<Char> uu(6);
    uu[0] = U'\\';
    uu[1] = U'u';
    while (current < len) {
        Char c = comments.GetChar(current);
        if (c > 0x00ff || c == U'\n' || c == U'\r') {
            if (last != current) {
                FAIL_RETURN(IWriter::Probe(bw)->Write(comments.Substring(last, current)));
            }
            if (c > 0x00ff) {
                uu[2] = ToHex((c >> 12) & 0xf);
                uu[3] = ToHex((c >>  8) & 0xf);
                uu[4] = ToHex((c >>  4) & 0xf);
                uu[5] = ToHex( c        & 0xf);
                FAIL_RETURN(IWriter::Probe(bw)->Write(String(uu)));
            }
            else {
                FAIL_RETURN(bw->NewLine());
                if (c == U'\r' &&
                    current != len - 1 &&
                    comments.GetChar(current + 1) == U'\n') {
                    current++;
                }
                if (current == len - 1 ||
                    (comments.GetChar(current + 1) != U'#' &&
                    comments.GetChar(current + 1) != U'!'))
                    FAIL_RETURN(IWriter::Probe(bw)->Write(String("#")));
            }
            last = current + 1;
        }
        current++;
    }
    if (last != current) {
        FAIL_RETURN(IWriter::Probe(bw)->Write(comments.Substring(last, current)));
    }
    FAIL_RETURN(bw->NewLine());
    return NOERROR;
}

ECode Properties::Save(
    /* [in] */ IOutputStream* outstream,
    /* [in] */ const String& comments)
{
    Store(outstream, comments);
    return NOERROR;
}

ECode Properties::Store(
    /* [in] */ IWriter* writer,
    /* [in] */ const String& comments)
{
    AutoPtr<IBufferedWriter> bw =
            IBufferedWriter::Probe(writer);
    if (bw == nullptr) {
        CBufferedWriter::New(writer, IID_IBufferedWriter, (IInterface**)&bw);
    }
    return Store0(bw, comments, false);
}

ECode Properties::Store(
    /* [in] */ IOutputStream* outstream,
    /* [in] */ const String& comments)
{
    AutoPtr<IOutputStreamWriter> sw;
    COutputStreamWriter::New(outstream, String("8859_1"),
            IID_IOutputStreamWriter, (IInterface**)&sw);
    AutoPtr<IBufferedWriter> bw;
    CBufferedWriter::New(IWriter::Probe(sw),
            IID_IBufferedWriter, (IInterface**)&bw);
    return Store0(bw, comments, true);
}

ECode Properties::Store0(
    /* [in] */ IBufferedWriter* bw,
    /* [in] */ const String& comments,
    /* [in] */ Boolean escUnicode)
{
    if (!comments.IsNull()) {
        FAIL_RETURN(WriteComments(bw, comments));
    }
    AutoPtr<IDate> d;
    CDate::New(IID_IDate, (IInterface**)&d);
    IWriter::Probe(bw)->Write(String("#") + Object::ToString(d));
    bw->NewLine();
    {
        AutoLock lock(this);
        AutoPtr<IEnumeration> e;
        GetKeys(&e);
        Boolean hasMore;
        while (e->HasMoreElements(&hasMore), hasMore) {
            AutoPtr<IInterface> ko, vo;
            e->NextElement(&ko);
            String key = CoreUtils::Unbox(ICharSequence::Probe(ko));
            Get(ko, &vo);
            String val = CoreUtils::Unbox(ICharSequence::Probe(vo));
            key = SaveConvert(key, true, escUnicode);
            val = SaveConvert(val, false, escUnicode);
            IWriter::Probe(bw)->Write(key + "=" + val);
            bw->NewLine();
        }
    }
    IWriter::Probe(bw)->Flush();
    return NOERROR;
}

ECode Properties::LoadFromXML(
    /* [in] */ IInputStream* instream)
{
    FAIL_RETURN(XMLUtils::Load(this, instream));
    return instream->Close();
}

ECode Properties::StoreToXML(
    /* [in] */ IOutputStream* os,
    /* [in] */ const String& comment)
{
    return StoreToXML(os, comment, String("UTF-8"));
}

ECode Properties::StoreToXML(
    /* [in] */ IOutputStream* os,
    /* [in] */ const String& comment,
    /* [in] */ const String& encoding)
{
    return XMLUtils::Save(this, os, comment, encoding);
}

ECode Properties::GetProperty(
    /* [in] */ const String& key,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<IInterface> oval;
    Hashtable::Get(CoreUtils::Box(key), &oval);
    String sval;
    if (IString::Probe(oval) != nullptr) {
        ICharSequence::Probe(oval)->ToString(sval);
    }
    if (sval.IsNull() && mDefaults != nullptr) {
        mDefaults->GetProperty(key, &sval);
    }
    *value = sval;
    return NOERROR;
}

ECode Properties::GetProperty(
    /* [in] */ const String& key,
    /* [in] */ const String& defaultValue,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    String val;
    GetProperty(key, &val);
    *value = val.IsNull() ? defaultValue : val;
    return NOERROR;
}

ECode Properties::PropertyNames(
    /* [out] */ IEnumeration** names)
{
    VALIDATE_NOT_NULL(names);

    AutoPtr<IHashtable> h;
    CHashtable::New(IID_IHashtable, (IInterface**)&h);
    Enumerate(h);
    return h->GetKeys(names);
}

ECode Properties::StringPropertyNames(
    /* [out] */ ISet** names)
{
    VALIDATE_NOT_NULL(names);

    AutoPtr<IHashtable> h;
    CHashtable::New(IID_IHashtable, (IInterface**)&h);
    EnumerateStringProperties(h);
    return h->GetKeySet(names);
}

ECode Properties::List(
    /* [in] */ IPrintStream* outstream)
{
    outstream->Println(String("-- listing properties --"));
    AutoPtr<IHashtable> h;
    CHashtable::New(IID_IHashtable, (IInterface**)&h);
    Enumerate(h);
    AutoPtr<IEnumeration> e;
    h->GetKeys(&e);
    Boolean hasMore;
    while (e->HasMoreElements(&hasMore), hasMore) {
        AutoPtr<IInterface> okey;
        e->NextElement(&okey);
        String key = CoreUtils::Unbox(ICharSequence::Probe(okey));
        AutoPtr<IInterface> oval;
        h->Get(okey, &oval);
        String val = CoreUtils::Unbox(ICharSequence::Probe(oval));
        if (val.GetLength() > 40) {
            val = val.Substring(0, 37) + "...";
        }
        outstream->Println(key + "=" + val);
    }
    return NOERROR;
}

ECode Properties::List(
    /* [in] */ IPrintWriter* outwriter)
{
    outwriter->Println(String("-- listing properties --"));
    AutoPtr<IHashtable> h;
    CHashtable::New(IID_IHashtable, (IInterface**)&h);
    Enumerate(h);
    AutoPtr<IEnumeration> e;
    h->GetKeys(&e);
    Boolean hasMore;
    while (e->HasMoreElements(&hasMore), hasMore) {
        AutoPtr<IInterface> okey;
        e->NextElement(&okey);
        String key = CoreUtils::Unbox(ICharSequence::Probe(okey));
        AutoPtr<IInterface> oval;
        h->Get(okey, &oval);
        String val = CoreUtils::Unbox(ICharSequence::Probe(oval));
        if (val.GetLength() > 40) {
            val = val.Substring(0, 37) + "...";
        }
        outwriter->Println(key + "=" + val);
    }
    return NOERROR;
}

void Properties::Enumerate(
    /* [in] */ IHashtable* h)
{
    if (mDefaults != nullptr) {
        ((Properties*)mDefaults.Get())->Enumerate(h);
    }
    AutoPtr<IEnumeration> e;
    GetKeys(&e);
    Boolean hasMore;
    while (e->HasMoreElements(&hasMore), hasMore) {
        AutoPtr<IInterface> okey;
        e->NextElement(&okey);
        AutoPtr<IInterface> oval;
        Get(okey, &oval);
        h->Put(okey, oval);
    }
}

void Properties::EnumerateStringProperties(
    /* [in] */ IHashtable* h)
{
    if (mDefaults != nullptr) {
        ((Properties*)mDefaults.Get())->EnumerateStringProperties(h);
    }
    AutoPtr<IEnumeration> e;
    GetKeys(&e);
    Boolean hasMore;
    while (e->HasMoreElements(&hasMore), hasMore) {
        AutoPtr<IInterface> okey;
        e->NextElement(&okey);
        AutoPtr<IInterface> oval;
        Get(okey, &oval);
        if (IString::Probe(okey) != nullptr && IString::Probe(oval) != nullptr) {
            h->Put(okey, oval);
        }
    }
}

Char Properties::ToHex(
    /* [in] */ Integer nibble)
{
    static Char sHexDigit[] = {
        U'0',U'1',U'2',U'3',U'4',U'5',U'6',U'7',U'8',U'9',U'A',U'B',U'C',U'D',U'E',U'F'
    };
    return sHexDigit[nibble & 0xF];
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
            if (c == U'\n') {
                continue;
            }
        }
        if (skipWhiteSpace) {
            if (Character::IsWhitespace(c)) {
                continue;
            }
            if (!appendedLineBegin && (c == U'\r' || c == U'\n')) {
                continue;
            }
            skipWhiteSpace = false;
            appendedLineBegin = false;
        }
        if (isNewLine) {
            isNewLine = false;
            if (c == U'#' || c == U'!') {
                isCommentLine = true;
                continue;
            }
        }

        if (c != U'\n' && c != U'\r') {
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
            if (c == U'\\') {
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
                if (c == U'\r') {
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
