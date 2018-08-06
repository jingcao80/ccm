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

#include "ccm/core/CoreUtils.h"
#include "ccm/core/CStringBuilder.h"
#include "ccm/io/CBufferedWriter.h"
#include "ccm/io/CFile.h"
#include "ccm/io/CFileOutputStream.h"
#include "ccm/io/COutputStreamWriter.h"
#include "ccm/io/charset/Charset.h"
#include "ccm/util/CArrayList.h"
#include "ccm/util/Formatter.h"
#include "ccm/util/Locale.h"
#include "ccm.io.IFile.h"
#include "ccm.io.IOutputStream.h"
#include "ccm.io.IWriter.h"

using ccm::core::CoreUtils;
using ccm::core::CStringBuilder;
using ccm::core::IID_IAppendable;
using ccm::core::IID_IAutoCloseable;
using ccm::io::CBufferedWriter;
using ccm::io::CFile;
using ccm::io::CFileOutputStream;
using ccm::io::COutputStreamWriter;
using ccm::io::IFile;
using ccm::io::IID_ICloseable;
using ccm::io::IID_IFile;
using ccm::io::IID_IFlushable;
using ccm::io::IID_IOutputStream;
using ccm::io::IID_IWriter;
using ccm::io::IOutputStream;
using ccm::io::IWriter;
using ccm::io::charset::Charset;

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_4(Formatter, SyncObject, IFormatter, ICloseable, IFlushable, IAutoCloseable);

ECode Formatter::ToCharset(
    /* [in] */ const String& csn,
    /* [out] */ ICharset** cs)
{
    if (csn.IsNull()) {
        Logger::E("Formatter", "charsetName is null");
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    ECode ec = Charset::ForName(csn, cs);
    return SUCCEEDED(ec) ? NOERROR : ccm::io::E_UNSUPPORTED_ENCODING_EXCEPTION;
}

AutoPtr<IAppendable> Formatter::NonNullAppendable(
    /* [in] */ IAppendable* a)
{
    if (a == nullptr) {
        AutoPtr<IAppendable> sb;
        CStringBuilder::New(IID_IAppendable, (IInterface**)&sb);
        return sb;
    }
    return a;
}

ECode Formatter::Constructor(
    /* [in] */ ILocale* l,
    /* [in] */ IAppendable* a)
{
    mA = a;
    mL = l;
    mZero = GetZero(l);
    return NOERROR;
}

ECode Formatter::Constructor(
    /* [in] */ ICharset* charset,
    /* [in] */ ILocale* l,
    /* [in] */ IFile* file)
{
    AutoPtr<IOutputStream> fos;
    FAIL_RETURN(CFileOutputStream::New(file, IID_IOutputStream, (IInterface**)&fos));
    AutoPtr<IWriter> osw;
    FAIL_RETURN(COutputStreamWriter::New(fos, charset, IID_IWriter, (IInterface**)&osw));
    AutoPtr<IAppendable> a;
    CBufferedWriter::New(osw, IID_IAppendable, (IInterface**)&a);
    return Constructor(l, a);
}

ECode Formatter::Constructor()
{
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::GetFormatCategory());
    AutoPtr<IAppendable> a;
    CStringBuilder::New(IID_IAppendable, (IInterface**)&a);
    return Constructor(l, a);
}

ECode Formatter::Constructor(
    /* [in] */ IAppendable* a)
{
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::GetFormatCategory());
    AutoPtr<IAppendable> aa = NonNullAppendable(a);
    return Constructor(l, aa);
}

ECode Formatter::Constructor(
    /* [in] */ ILocale* l)
{
    AutoPtr<IAppendable> a;
    CStringBuilder::New(IID_IAppendable, (IInterface**)&a);
    return Constructor(l, a);
}

ECode Formatter::Constructor(
    /* [in] */ IAppendable* a,
    /* [in] */ ILocale* l)
{
    AutoPtr<IAppendable> aa = NonNullAppendable(a);
    return Constructor(l, aa);
}

ECode Formatter::Constructor(
    /* [in] */  const String& fileName)
{
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::GetFormatCategory());
    AutoPtr<IOutputStream> fos;
    FAIL_RETURN(CFileOutputStream::New(fileName, IID_IOutputStream, (IInterface**)&fos));
    AutoPtr<IWriter> osw;
    FAIL_RETURN(COutputStreamWriter::New(fos, IID_IWriter, (IInterface**)&osw));
    AutoPtr<IAppendable> a;
    CBufferedWriter::New(osw, IID_IAppendable, (IInterface**)&a);
    return Constructor(l, a);
}

ECode Formatter::Constructor(
    /* [in] */ const String& fileName,
    /* [in] */ const String& csn)
{
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::GetFormatCategory());
    return Constructor(fileName, csn, l);
}

ECode Formatter::Constructor(
    /* [in] */ const String& fileName,
    /* [in] */ const String& csn,
    /* [in] */ ILocale* l)
{
    AutoPtr<ICharset> cs;
    FAIL_RETURN(ToCharset(csn, (ICharset**)&cs));
    AutoPtr<IFile> f;
    FAIL_RETURN(CFile::New(fileName, IID_IFile, (IInterface**)&f));
    return Constructor(cs, l, f);
}

ECode Formatter::Constructor(
    /* [in] */ IFile* file)
{
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::GetFormatCategory());
    AutoPtr<IOutputStream> fos;
    FAIL_RETURN(CFileOutputStream::New(file, IID_IOutputStream, (IInterface**)&fos));
    AutoPtr<IWriter> osw;
    FAIL_RETURN(COutputStreamWriter::New(fos, IID_IWriter, (IInterface**)&osw));
    AutoPtr<IAppendable> a;
    CBufferedWriter::New(osw, IID_IAppendable, (IInterface**)&a);
    return Constructor(l, a);
}

ECode Formatter::Constructor(
    /* [in] */ IFile* file,
    /* [in] */ const String& csn)
{
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::GetFormatCategory());
    return Constructor(file, csn, l);
}

ECode Formatter::Constructor(
    /* [in] */ IFile* file,
    /* [in] */ const String& csn,
    /* [in] */ ILocale* l)
{
    AutoPtr<ICharset> cs;
    FAIL_RETURN(ToCharset(csn, (ICharset**)&cs));
    return Constructor(cs, l, file);
}

ECode Formatter::Constructor(
    /* [in] */ IPrintStream* ps)
{
    if (ps == nullptr) {
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::GetFormatCategory());
    return Constructor(l, IAppendable::Probe(ps));
}

ECode Formatter::Constructor(
    /* [in] */ IOutputStream* os)
{
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::GetFormatCategory());
    AutoPtr<IWriter> osw;
    FAIL_RETURN(COutputStreamWriter::New(os, IID_IWriter, (IInterface**)&osw));
    AutoPtr<IAppendable> a;
    CBufferedWriter::New(osw, IID_IAppendable, (IInterface**)&a);
    return Constructor(l, a);
}

ECode Formatter::Constructor(
    /* [in] */ IOutputStream* os,
    /* [in] */ const String& csn)
{
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::GetFormatCategory());
    return Constructor(os, csn, l);
}

ECode Formatter::Constructor(
    /* [in] */ IOutputStream* os,
    /* [in] */ const String& csn,
    /* [in] */ ILocale* l)
{
    AutoPtr<IWriter> osw;
    FAIL_RETURN(COutputStreamWriter::New(os, csn, IID_IWriter, (IInterface**)&osw));
    AutoPtr<IAppendable> a;
    CBufferedWriter::New(osw, IID_IAppendable, (IInterface**)&a);
    return Constructor(l, a);
}

Char Formatter::GetZero(
    /* [in] */ ILocale* l)
{
    // not implemented;
    return '0';
}

ECode Formatter::GetLocale(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale);

    FAIL_RETURN(EnsureOpen());
    *locale = mL;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}

ECode Formatter::GetOut(
    /* [out] */ IAppendable** output)
{
    VALIDATE_NOT_NULL(output);

    FAIL_RETURN(EnsureOpen());
    *output = mA;
    REFCOUNT_ADD(*output);
    return NOERROR;
}

ECode Formatter::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    FAIL_RETURN(EnsureOpen());
    *str = Object::ToString(mA);
    return NOERROR;
}

ECode Formatter::Flush()
{
    FAIL_RETURN(EnsureOpen());
    if (IFlushable::Probe(mA) != nullptr) {
        ECode ec = IFlushable::Probe(mA)->Flush();
        if (FAILED(ec)) {
            mLastException = ec;
        }
    }
    return NOERROR;
}

ECode Formatter::Close()
{
    if (mA == nullptr) {
        return NOERROR;
    }
    if (ICloseable::Probe(mA) != nullptr) {
        ECode ec = ICloseable::Probe(mA)->Close();
        if (FAILED(ec)) {
            mLastException = ec;
        }
    }
    mA = nullptr;
    return NOERROR;
}

ECode Formatter::EnsureOpen()
{
    if (mA == nullptr) {
        return E_FORMATTER_CLOSED_EXCEPTION;
    }
    return NOERROR;
}

ECode Formatter::GetIoException(
    /* [out] */ ECode* ec)
{
    VALIDATE_NOT_NULL(ec);

    *ec = mLastException;
    return NOERROR;
}

ECode Formatter::Format(
    /* [in] */ const String& format,
    /* [in] */ const Array<IInterface*>* args)
{
    return Format(mL, format, args);
}

ECode Formatter::Format(
    /* [in] */ ILocale* l,
    /* [in] */ const String& format,
    /* [in] */ const Array<IInterface*>* args)
{
    FAIL_RETURN(EnsureOpen());

    // index of last argument referenced
    Integer last = -1;
    // last ordinary index
    Integer lasto = -1;

    Array<IFormatString*> fsa;
    FAIL_RETURN(Parse(format, &fsa));
    for (Integer i = 0; i < fsa.GetLength(); i++) {
        IFormatString* fs = fsa[i];
        Integer index;
        fs->GetIndex(&index);
        switch (index) {
            case -2: { // fixed string, "%n", or "%%"
                ECode ec = fs->Print(nullptr, l);
                if (FAILED(ec)) {
                    mLastException = ec;
                }
                break;
            }
            case -1: { // relative index
                if (last < 0 || (args != nullptr && last > args->GetLength() - 1)) {
                    return E_MISSING_FORMAT_ARGUMENT_EXCEPTION;
                }
                ECode ec = fs->Print((args == nullptr ? nullptr : (*args)[last]), l);
                if (FAILED(ec)) {
                    mLastException = ec;
                }
                break;
            }
            case 0: { // ordinary index
                lasto++;
                last = lasto;
                if (args != nullptr && lasto > args->GetLength() - 1) {
                    return E_MISSING_FORMAT_ARGUMENT_EXCEPTION;
                }
                ECode ec = fs->Print((args == nullptr ? nullptr : (*args)[lasto]), l);
                if (FAILED(ec)) {
                    mLastException = ec;
                }
                break;
            }
            default: { // explicit index
                last = index - 1;
                if (args != nullptr && last > args->GetLength() - 1) {
                    return E_MISSING_FORMAT_ARGUMENT_EXCEPTION;
                }
                ECode ec = fs->Print((args == nullptr ? nullptr : (*args)[last]), l);
                if (FAILED(ec)) {
                    mLastException = ec;
                }
                break;
            }
        }
    }
    return NOERROR;
}

ECode Formatter::Parse(
    /* [in] */ const String& s,
    /* [out, callee] */ Array<IFormatString*>* formats)
{
    AutoPtr<IArrayList> al;
    CArrayList::New(IID_IArrayList, (IInterface**)&al);
    for (Integer i = 0, len = s.GetLength(); i < len;) {
        Integer nextPercent = s.IndexOf('%', i);
        if (s.GetChar(i) != '%') {
            // This is plain-text part, find the maximal plain-text
            // sequence and store it.
            Integer plainTextStart = i;
            Integer plainTextEnd = (nextPercent == -1) ? len : nextPercent;
            al->Add(new FixedString(this, s.Substring(plainTextStart,
                    plainTextEnd)));
            i = plainTextEnd;
        }
        else {
            // We have a format specifier
            AutoPtr<FormatSpecifierParser> fsp = new FormatSpecifierParser();
            FAIL_RETURN(fsp->Constructor(s, i + 1));
            al->Add(fsp->GetFormatSpecifier());
            i = fsp->GetEndIdx();
        }
    }
    return al->ToArray(IID_IFormatString, (Array<IInterface*>*)formats);
}

//-------------------------------------------------------------------------

extern const InterfaceID Formatter::IID_IFormatString =
    {{0xd6dc3cb6,0x3ca2,0x4066,0xa773,{0xe,0x8,0x9,0xe,0x6,0x9,0xb,0x0,0x0,0xa,0x6,0xd}}, &CID_libcore};

CCM_INTERFACE_IMPL_LIGHT_1(Formatter::FixedString, IFormatString);

ECode Formatter::FixedString::GetIndex(
    /* [out] */ Integer* idx)
{
    VALIDATE_NOT_NULL(idx);

    *idx = -2;
    return NOERROR;
}

ECode Formatter::FixedString::Print(
    /* [in] */ IInterface* arg,
    /* [in] */ ILocale* l)
{
    mOwner->mA->Append(CoreUtils::Box(mS));
    return NOERROR;
}

ECode Formatter::FixedString::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = mS;
    return NOERROR;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

String Formatter::FormatSpecifierParser::GetFLAGS()
{
    static const String FLAGS(",-(+# 0<");
    return FLAGS;
}

ECode Formatter::FormatSpecifierParser::Constructor(
    /* [in] */ const String& format,
    /* [in] */ Integer startIdx)
{
    mFormat = format;
    mCursor = startIdx;
    // Index
    if (NextIsInt()) {
        String nint = NextInt();
        Char c;
        FAIL_RETURN(Peek(&c));
        if (c == '$') {
            mIndex = nint;
            Advance();
        }
        else if (nint.GetChar(0) == '0') {
            // This is a flag, skip to parsing flags.
            Back(nint.GetLength());
        }
        else {
            // This is the width, skip to parsing precision.
            mWidth = nint;
        }
    }
    // Flags
    mFlags = "";
    if (mWidth.IsNull()) {
        Char c;
        FAIL_RETURN(Peek(&c));
        while (GetFLAGS().IndexOf(c) >= 0) {
            Advance(&c);
            mFlags = mFlags + c;
            FAIL_RETURN(Peek(&c));
        }
    }
    // Width
    if (mWidth.IsNull() && NextIsInt()) {
        mWidth = NextInt();
    }
    // Precision
    Char c;
    FAIL_RETURN(Peek(&c));
    if (c == '.') {
        Advance();
        if (!NextIsInt()) {
            return E_ILLEGAL_FORMAT_PRECISION_EXCEPTION;
        }
        mPrecision = NextInt();
    }
    FAIL_RETURN(Peek(&c));
    if (c == 't' || c == 'T') {
        Advance(&c);
        mTT = String::Format("%c", c);
    }
    // Conversion
    FAIL_RETURN(Advance(&c));
    mConv = String::Format("%c", c);

    mFs = new FormatSpecifier();
    return mFs->Constructor(mIndex, mFlags, mWidth, mPrecision, mTT, mConv);
}

}
}
