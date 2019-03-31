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
#include "ccm/core/CStringBuilder.h"
#include "ccm/core/Math.h"
#include "ccm/core/StringUtils.h"
#include "ccm/core/System.h"
#include "ccm/io/CBufferedWriter.h"
#include "ccm/io/CFile.h"
#include "ccm/io/CFileOutputStream.h"
#include "ccm/io/COutputStreamWriter.h"
#include "ccm/io/charset/Charset.h"
#include "ccm/math/CBigDecimal.h"
#include "ccm/math/CMathContext.h"
#include "ccm/misc/DoubleConsts.h"
#include "ccm/misc/FormattedFloatingDecimal.h"
#include "ccm/text/DateFormatSymbols.h"
#include "ccm/text/DecimalFormatSymbols.h"
#include "ccm/text/NumberFormat.h"
#include "ccm/util/CArrayList.h"
#include "ccm/util/Calendar.h"
#include "ccm/util/CFormatter.h"
#include "ccm/util/Formatter.h"
#include "ccm/util/Locale.h"
#include "libcore/icu/LocaleData.h"
#include "ccm.core.IBoolean.h"
#include "ccm.core.IByte.h"
#include "ccm.core.IChar.h"
#include "ccm.core.IDouble.h"
#include "ccm.core.IFloat.h"
#include "ccm.core.IInteger.h"
#include "ccm.core.ILong.h"
#include "ccm.core.IShort.h"
#include "ccm.io.IFile.h"
#include "ccm.io.IOutputStream.h"
#include "ccm.io.IWriter.h"
#include "ccm.misc.IFormattedFloatingDecimal.h"
#include "ccm.text.IDecimalFormat.h"
#include "ccm.util.IDate.h"
#include "ccm.util.IFormattable.h"
#include "libcore.icu.ILocaleData.h"
#include <ccmlogger.h>

using ccm::core::Character;
using ccm::core::CoreUtils;
using ccm::core::CStringBuilder;
using ccm::core::IID_IAppendable;
using ccm::core::IID_IAutoCloseable;
using ccm::core::IID_IStringBuilder;
using ccm::core::IBoolean;
using ccm::core::IByte;
using ccm::core::IChar;
using ccm::core::IDouble;
using ccm::core::IFloat;
using ccm::core::IInteger;
using ccm::core::ILong;
using ccm::core::IShort;
using ccm::core::Math;
using ccm::core::StringUtils;
using ccm::core::System;
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
using ccm::math::CBigDecimal;
using ccm::math::CMathContext;
using ccm::math::IID_IBigDecimal;
using ccm::math::IID_IMathContext;
using ccm::math::IMathContext;
using ccm::math::RoundingMode;
using ccm::misc::DoubleConsts;
using ccm::misc::FormattedFloatingDecimal;
using ccm::misc::FormattedFloatingDecimalForm;
using ccm::misc::IFormattedFloatingDecimal;
using ccm::text::DateFormatSymbols;
using ccm::text::DecimalFormatSymbols;
using ccm::text::IDecimalFormat;
using ccm::text::IDecimalFormatSymbols;
using ccm::text::INumberFormat;
using ccm::text::NumberFormat;
using ccm::util::Calendar;
using ccm::util::IDate;
using libcore::icu::LocaleData;
using libcore::icu::ILocaleData;

namespace ccm {
namespace util {

Double Formatter::sScaleUp = 0.0;

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
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::Category::GetFORMAT());
    AutoPtr<IAppendable> a;
    CStringBuilder::New(IID_IAppendable, (IInterface**)&a);
    return Constructor(l, a);
}

ECode Formatter::Constructor(
    /* [in] */ IAppendable* a)
{
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::Category::GetFORMAT());
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
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::Category::GetFORMAT());
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
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::Category::GetFORMAT());
    return Constructor(fileName, csn, l);
}

ECode Formatter::Constructor(
    /* [in] */ const String& fileName,
    /* [in] */ const String& csn,
    /* [in] */ ILocale* l)
{
    AutoPtr<ICharset> cs;
    FAIL_RETURN(ToCharset(csn, &cs));
    AutoPtr<IFile> f;
    FAIL_RETURN(CFile::New(fileName, IID_IFile, (IInterface**)&f));
    return Constructor(cs, l, f);
}

ECode Formatter::Constructor(
    /* [in] */ IFile* file)
{
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::Category::GetFORMAT());
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
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::Category::GetFORMAT());
    return Constructor(file, csn, l);
}

ECode Formatter::Constructor(
    /* [in] */ IFile* file,
    /* [in] */ const String& csn,
    /* [in] */ ILocale* l)
{
    AutoPtr<ICharset> cs;
    FAIL_RETURN(ToCharset(csn, &cs));
    return Constructor(cs, l, file);
}

ECode Formatter::Constructor(
    /* [in] */ IPrintStream* ps)
{
    if (ps == nullptr) {
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::Category::GetFORMAT());
    return Constructor(l, IAppendable::Probe(ps));
}

ECode Formatter::Constructor(
    /* [in] */ IOutputStream* os)
{
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::Category::GetFORMAT());
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
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::Category::GetFORMAT());
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
    if ((l != nullptr) && (!Object::Equals(l, Locale::GetUS()))) {
        AutoPtr<IDecimalFormatSymbols> dfs = DecimalFormatSymbols::GetInstance(l);
        Char c;
        dfs->GetZeroDigit(&c);
        return c;
    }
    else {
        return U'0';
    }
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
        Integer nextPercent = s.IndexOf(U'%', i);
        if (s.GetChar(i) != U'%') {
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
            FAIL_RETURN(fsp->Constructor(this, s, i + 1));
            al->Add(fsp->GetFormatSpecifier());
            i = fsp->GetEndIdx();
        }
    }
    return al->ToArray(IID_IFormatString, (Array<IInterface*>*)formats);
}

//-------------------------------------------------------------------------

AutoPtr<Formatter::Flags> Formatter::Flags::GetNONE()
{
    static AutoPtr<Flags> NONE = new Flags(0); // ''
    return NONE;
}

AutoPtr<Formatter::Flags> Formatter::Flags::GetLEFT_JUSTIFY()
{
    static AutoPtr<Flags> LEFT_JUSTIFY = new Flags(1 << 0); // '-'
    return LEFT_JUSTIFY;
}

AutoPtr<Formatter::Flags> Formatter::Flags::GetUPPERCASE()
{
    static AutoPtr<Flags> UPPERCASE = new Flags(1 << 1); // '^'
    return UPPERCASE;
}

AutoPtr<Formatter::Flags> Formatter::Flags::GetALTERNATE()
{
    static AutoPtr<Flags> ALTERNATE = new Flags(1 << 2); // '#'
    return ALTERNATE;
}

AutoPtr<Formatter::Flags> Formatter::Flags::GetPLUS()
{
    static AutoPtr<Flags> PLUS = new Flags(1 << 3); // '+'
    return PLUS;
}

AutoPtr<Formatter::Flags> Formatter::Flags::GetLEADING_SPACE()
{
    static AutoPtr<Flags> LEADING_SPACE = new Flags(1 << 4); // ' '
    return LEADING_SPACE;
}

AutoPtr<Formatter::Flags> Formatter::Flags::GetZERO_PAD()
{
    static AutoPtr<Flags> ZERO_PAD = new Flags(1 << 5); // '0'
    return ZERO_PAD;
}

AutoPtr<Formatter::Flags> Formatter::Flags::GetGROUP()
{
    static AutoPtr<Flags> GROUP = new Flags(1 << 6); // ','
    return GROUP;
}

AutoPtr<Formatter::Flags> Formatter::Flags::GetPARENTHESES()
{
    static AutoPtr<Flags> PARENTHESES = new Flags(1 << 7); // '('
    return PARENTHESES;
}

AutoPtr<Formatter::Flags> Formatter::Flags::GetPREVIOUS()
{
    static AutoPtr<Flags> PREVIOUS = new Flags(1 << 8); // '<'
    return PREVIOUS;
}

ECode Formatter::Flags::Parse(
    /* [in] */ const String& s,
    /* [out] */ Flags** retf)
{
    VALIDATE_NOT_NULL(retf);

    Array<Char> ca = s.GetChars();
    AutoPtr<Flags> f = new Flags(0);
    for (Integer i = 0; i < ca.GetLength(); i++) {
        AutoPtr<Flags> v;
        FAIL_RETURN(Parse(ca[i], &v));
        if (f->Contains(v)) {
            return E_DUPLICATE_FORMAT_FLAGS_EXCEPTION;
        }
        f->Add(v);
    }
    f.MoveTo(retf);
    return NOERROR;
}

ECode Formatter::Flags::Parse(
    /* [in] */ Char c,
    /* [out] */ Flags** retf)
{
    AutoPtr<Flags> f;
    switch (c) {
        case U'-': f = GetLEFT_JUSTIFY(); break;
        case U'#': f = GetALTERNATE(); break;
        case U'+': f = GetPLUS(); break;
        case U' ': f = GetLEADING_SPACE(); break;
        case U'0': f = GetZERO_PAD(); break;
        case U',': f = GetGROUP(); break;
        case U'(': f = GetPARENTHESES(); break;
        case U'<': f = GetPREVIOUS(); break;
        default:
            return E_UNKNOWN_FORMAT_FLAGS_EXCEPTION;
    }
    f.MoveTo(retf);
    return NOERROR;
}

String Formatter::Flags::ToString()
{
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    if (Contains(GetLEFT_JUSTIFY()))  sb->Append(U'-');
    if (Contains(GetUPPERCASE()))     sb->Append(U'^');
    if (Contains(GetALTERNATE()))     sb->Append(U'#');
    if (Contains(GetPLUS()))          sb->Append(U'+');
    if (Contains(GetLEADING_SPACE())) sb->Append(U' ');
    if (Contains(GetZERO_PAD()))      sb->Append(U'0');
    if (Contains(GetGROUP()))         sb->Append(U',');
    if (Contains(GetPARENTHESES()))   sb->Append(U'(');
    if (Contains(GetPREVIOUS()))      sb->Append(U'<');
    String str;
    sb->ToString(&str);
    return str;
}

//-------------------------------------------------------------------------

extern const InterfaceID Formatter::IID_IFormatString =
    {{0xd6dc3cb6,0x3ca2,0x4066,0xa773,{0xe,0x8,0x9,0xe,0x6,0x9,0xb,0x0,0x0,0xa,0x6,0xd}}, &CID_libcore};

CCM_INTERFACE_IMPL_LIGHT_1(Formatter::FixedString, LightRefBase, IFormatString);

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

CCM_INTERFACE_IMPL_LIGHT_1(Formatter::FormatSpecifier, LightRefBase, IFormatString);

ECode Formatter::FormatSpecifier::Constructor(
    /* [in] */ Formatter* owner,
    /* [in] */ const String& indexStr,
    /* [in] */ const String& flagsStr,
    /* [in] */ const String& widthStr,
    /* [in] */ const String& precisionStr,
    /* [in] */ const String& tTStr,
    /* [in] */ const String& convStr)
{
    mOwner = owner;
    Integer idx = 1;

    Index(indexStr);
    ToFlags(flagsStr);
    Width(widthStr);
    Precision(precisionStr);

    if (!tTStr.IsNull()) {
        mDt = true;
        if (tTStr.Equals("T")) {
            mF->Add(Flags::GetUPPERCASE());
        }
    }

    Conversion(convStr);

    if (mDt) {
        FAIL_RETURN(CheckDateTime());
    }
    else if (Conversion::IsGeneral(mC)) {
        FAIL_RETURN(CheckGeneral())
    }
    else if (Conversion::IsCharacter(mC)) {
        FAIL_RETURN(CheckCharacter());
    }
    else if (Conversion::IsInteger(mC)) {
        FAIL_RETURN(CheckInteger());
    }
    else if (Conversion::IsFloat(mC)) {
        FAIL_RETURN(CheckFloat());
    }
    else if (Conversion::IsText(mC)) {
        FAIL_RETURN(CheckText());
    }
    else {
        return E_UNKNOWN_FORMAT_CONVERSION_EXCEPTION;
    }
}

Integer Formatter::FormatSpecifier::Index(
    /* [in] */ const String& s)
{
    if (!s.IsNull()) {
        ECode ec = StringUtils::ParseInteger(s, &mIndex);
        CHECK(SUCCEEDED(ec));
    }
    else {
        mIndex = 0;
    }
    return mIndex;
}

ECode Formatter::FormatSpecifier::GetIndex(
    /* [out] */ Integer* idx)
{
    VALIDATE_NOT_NULL(idx);

    *idx = mIndex;
    return NOERROR;
}

ECode Formatter::FormatSpecifier::ToFlags(
    /* [in] */ const String& s)
{
    mF = nullptr;
    FAIL_RETURN(Flags::Parse(s, &mF));
    if (mF->Contains(Flags::GetPREVIOUS())) {
        mIndex = -1;
    }
    return NOERROR;
}

ECode Formatter::FormatSpecifier::Width(
    /* [in] */ const String& s)
{
    mWidth = -1;
    if (!s.IsNull()) {
        ECode ec = StringUtils::ParseInteger(s, &mWidth);
        CHECK(SUCCEEDED(ec));
        if (mWidth < 0) {
            return E_ILLEGAL_FORMAT_WIDTH_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode Formatter::FormatSpecifier::Precision(
    /* [in] */ const String& s)
{
    mPrecision = -1;
    if (!s.IsNull()) {
        ECode ec = StringUtils::ParseInteger(s, &mPrecision);
        CHECK(SUCCEEDED(ec));
        if (mPrecision < 0) {
            return E_ILLEGAL_FORMAT_PRECISION_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode Formatter::FormatSpecifier::Conversion(
    /* [in] */ const String& s)
{
    mC = s.GetChar(0);
    if (!mDt) {
        if (!Conversion::IsValid(mC)) {
            return E_UNKNOWN_FORMAT_CONVERSION_EXCEPTION;
        }
        if (Character::IsUpperCase(mC)) {
            mF->Add(Flags::GetUPPERCASE());
        }
        mC = Character::ToLowerCase(mC);
        if (Conversion::IsText(mC)) {
            mIndex = -2;
        }
    }
    return NOERROR;
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ IInterface* arg,
    /* [in] */ ILocale* l)
{
    if (mDt) {
        return PrintDateTime(arg, l);
    }
    switch (mC) {
        case Conversion::DECIMAL_INTEGER:
        case Conversion::OCTAL_INTEGER:
        case Conversion::HEXADECIMAL_INTEGER:
            return PrintInteger(arg, l);
        case Conversion::SCIENTIFIC:
        case Conversion::GENERAL:
        case Conversion::DECIMAL_FLOAT:
        case Conversion::HEXADECIMAL_FLOAT:
            return PrintFloat(arg, l);
        case Conversion::CHARACTER:
        case Conversion::CHARACTER_UPPER:
            return PrintCharacter(arg);
        case Conversion::BOOLEAN:
            return PrintBoolean(arg);
        case Conversion::STRING:
            return PrintString(arg, l);
        case Conversion::HASHCODE:
            return PrintHashCode(arg);
        case Conversion::LINE_SEPARATOR:
            return mOwner->mA->Append(CoreUtils::Box(System::GetLineSeparator()));
        case Conversion::PERCENT_SIGN:
            return mOwner->mA->Append(U'%');
        default:
            CHECK(0);
    }
    return NOERROR;
}

ECode Formatter::FormatSpecifier::PrintInteger(
    /* [in] */ IInterface* arg,
    /* [in] */ ILocale* l)
{
    if (arg == nullptr) {
        return Print(String("null"));
    }
    else if (IByte::Probe(arg) != nullptr) {
        Byte value;
        IByte::Probe(arg)->ByteValue(&value);
        return Print(value, l);
    }
    else if (IShort::Probe(arg) != nullptr) {
        Short value;
        IShort::Probe(arg)->ShortValue(&value);
        return Print(value, l);
    }
    else if (IInteger::Probe(arg) != nullptr) {
        Integer value;
        IInteger::Probe(arg)->IntegerValue(&value);
        return Print(value, l);
    }
    else if (ILong::Probe(arg) != nullptr) {
        Long value;
        ILong::Probe(arg)->LongValue(&value);
        return Print(value, l);
    }
    else if (IBigInteger::Probe(arg) != nullptr) {
        return Print(IBigInteger::Probe(arg), l);
    }
    else {
        return FailConversion();
    }
}

ECode Formatter::FormatSpecifier::PrintFloat(
    /* [in] */ IInterface* arg,
    /* [in] */ ILocale* l)
{
    if (arg == nullptr) {
        return Print(String("null"));
    }
    else if (IFloat::Probe(arg) != nullptr) {
        Float value;
        IFloat::Probe(arg)->FloatValue(&value);
        return Print(value, l);
    }
    else if (IDouble::Probe(arg) != nullptr) {
        Double value;
        IDouble::Probe(arg)->DoubleValue(&value);
        return Print(value, l);
    }
    else if (IBigDecimal::Probe(arg) != nullptr) {
        return Print(IBigDecimal::Probe(arg), l);
    }
    else {
        return FailConversion();
    }
}

ECode Formatter::FormatSpecifier::PrintDateTime(
    /* [in] */ IInterface* arg,
    /* [in] */ ILocale* l)
{
    if (arg == nullptr) {
        return Print(String("null"));
    }
    AutoPtr<ICalendar> cal;

    if (ILong::Probe(arg) != nullptr) {
        cal = Calendar::GetInstance(l == nullptr ? Locale::GetUS().Get() : l);
        cal->SetTimeInMillis(CoreUtils::Unbox(ILong::Probe(arg)));
    }
    else if (IDate::Probe(arg) != nullptr) {
        cal = Calendar::GetInstance(l == nullptr ? Locale::GetUS().Get() : l);
        cal->SetTime(IDate::Probe(arg));
    }
    else if (ICalendar::Probe(arg) != nullptr) {
        cal = (ICalendar*)CoreUtils::Clone(arg, IID_ICalendar).Get();
        cal->SetLenient(true);
    }
    else {
        return FailConversion();
    }
    return Print(cal, mC, l);
}

ECode Formatter::FormatSpecifier::PrintCharacter(
    /* [in] */ IInterface* arg)
{
    if (arg == nullptr) {
        return Print(String("null"));
    }
    String s;
    if (IChar::Probe(arg) != nullptr) {
        Char c;
        IChar::Probe(arg)->GetValue(&c);
        s = String::ValueOf(c);
    }
    else if (IByte::Probe(arg) != nullptr) {
        Byte i;
        IByte::Probe(arg)->ByteValue(&i);
        if (Character::IsValidCodePoint(i)) {
            s = String::ValueOf((Char)i);
        }
        else {
            return E_ILLEGAL_FORMAT_CODE_POINT_EXCEPTION;
        }
    }
    else if (IShort::Probe(arg) != nullptr) {
        Short i;
        IShort::Probe(arg)->ShortValue(&i);
        if (Character::IsValidCodePoint(i)) {
            s = String::ValueOf((Char)i);
        }
        else {
            return E_ILLEGAL_FORMAT_CODE_POINT_EXCEPTION;
        }
    }
    else if (IInteger::Probe(arg) != nullptr) {
        Integer i;
        IInteger::Probe(arg)->IntegerValue(&i);
        if (Character::IsValidCodePoint(i)) {
            s = String::ValueOf((Char)i);
        }
        else {
            return E_ILLEGAL_FORMAT_CODE_POINT_EXCEPTION;
        }
    }
    else {
        return FailConversion();
    }
    return Print(s);
}

ECode Formatter::FormatSpecifier::PrintString(
    /* [in] */ IInterface* arg,
    /* [in] */ ILocale* l)
{
    if (IFormattable::Probe(arg) != nullptr) {
        AutoPtr<IFormatter> fmt = mOwner;
        AutoPtr<ILocale> ll;
        FAIL_RETURN(fmt->GetLocale(&ll));
        if (ll != l) {
            AutoPtr<IAppendable> out;
            fmt->GetOut(&out);
            CFormatter::New(out, l, IID_IFormatter, (IInterface**)&fmt);
        }
        IFormattable::Probe(arg)->FormatTo(fmt, mF->ValueOf(), mWidth, mPrecision);
    }
    else {
        if (mF->Contains(Flags::GetALTERNATE())) {
            return FailMismatch(Flags::GetALTERNATE(), U's');
        }
        if (arg == nullptr) {
            return Print(String("null"));
        }
        else {
            return Print(Object::ToString(arg));
        }
    }
}

ECode Formatter::FormatSpecifier::PrintBoolean(
    /* [in] */ IInterface* arg)
{
    String s;
    if (arg != nullptr) {
        s = (IBoolean::Probe(arg) != nullptr) ?
                Object::ToString(arg) :
                StringUtils::ToString(true);
    }
    else {
        s = StringUtils::ToString(false);
    }
    return Print(s);
}

ECode Formatter::FormatSpecifier::PrintHashCode(
    /* [in] */ IInterface* arg)
{
    String s;
    if (arg == nullptr) {
        s = "null";
    }
    else {
        s = StringUtils::ToString(Object::GetHashCode(arg), 16);
    }
    return Print(s);
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ const String& s_)
{
    String s = s_;
    if (mPrecision != -1 && mPrecision < s.GetLength()) {
        s = s.Substring(0, mPrecision);
    }
    if (mF->Contains(Flags::GetUPPERCASE())) {
        s = s.ToUpperCase();
    }
    return mOwner->mA->Append(CoreUtils::Box(Justify(s)));
}

String Formatter::FormatSpecifier::Justify(
    /* [in] */ const String& s)
{
    if (mWidth == -1) {
        return s;
    }
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    Boolean pad = mF->Contains(Flags::GetLEFT_JUSTIFY());
    Integer sp = mWidth - s.GetLength();
    if (!pad) {
        for (Integer i = 0; i < sp; i++) {
            sb->Append(U' ');
        }
    }
    sb->Append(s);
    if (pad) {
        for (Integer i = 0; i < sp; i++) {
            sb->Append(U' ');
        }
    }
    String str;
    sb->ToString(&str);
    return str;
}

ECode Formatter::FormatSpecifier::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(String("%"), IID_IStringBuilder, (IInterface**)&sb);
    AutoPtr<Flags> dupf = mF->Dup();
    dupf->Remove(Flags::GetUPPERCASE());
    sb->Append(dupf->ToString());
    if (mIndex > 0) {
        sb->Append(mIndex);
        sb->Append(U'$');
    }
    if (mWidth != -1) {
        sb->Append(mWidth);
    }
    if (mPrecision != -1) {
        sb->Append(U'.');
        sb->Append(mPrecision);
    }
    if (mDt) {
        sb->Append(mF->Contains(Flags::GetUPPERCASE()) ? U'T' : U't');
    }
    sb->Append(mF->Contains(Flags::GetUPPERCASE()) ?
            Character::ToUpperCase(mC) : mC);
    return sb->ToString(str);
}

ECode Formatter::FormatSpecifier::CheckGeneral()
{
    if ((mC == Conversion::BOOLEAN || mC == Conversion::HASHCODE) &&
            mF->Contains(Flags::GetALTERNATE())) {
        return FailMismatch(Flags::GetALTERNATE(), mC);
    }
    // '-' requires a width
    if (mWidth == -1 && mF->Contains(Flags::GetLEFT_JUSTIFY())) {
        return E_MISSING_FORMAT_WIDTH_EXCEPTION;
    }
    Array<Flags*> badFlags(5);
    badFlags.Set(0, Flags::GetPLUS());
    badFlags.Set(1, Flags::GetLEADING_SPACE());
    badFlags.Set(2, Flags::GetZERO_PAD());
    badFlags.Set(3, Flags::GetGROUP());
    badFlags.Set(4, Flags::GetPARENTHESES());
    return CheckBadFlags(badFlags);
}

ECode Formatter::FormatSpecifier::CheckDateTime()
{
    if (mPrecision != -1) {
        return E_ILLEGAL_FORMAT_PRECISION_EXCEPTION;
    }
    if (!DateTime::IsValid(mC)) {
        return E_UNKNOWN_FORMAT_CONVERSION_EXCEPTION;
    }
    Array<Flags*> badFlags(6);
    badFlags.Set(0, Flags::GetALTERNATE());
    badFlags.Set(1, Flags::GetPLUS());
    badFlags.Set(2, Flags::GetLEADING_SPACE());
    badFlags.Set(3, Flags::GetZERO_PAD());
    badFlags.Set(4, Flags::GetGROUP());
    badFlags.Set(5, Flags::GetPARENTHESES());
    FAIL_RETURN(CheckBadFlags(badFlags));
    // '-' requires a width
    if (mWidth == -1 && mF->Contains(Flags::GetLEFT_JUSTIFY())) {
        return E_MISSING_FORMAT_WIDTH_EXCEPTION;
    }
    return NOERROR;
}

ECode Formatter::FormatSpecifier::CheckCharacter()
{
    if (mPrecision != -1) {
        return E_ILLEGAL_FORMAT_PRECISION_EXCEPTION;
    }
    Array<Flags*> badFlags(6);
    badFlags.Set(0, Flags::GetALTERNATE());
    badFlags.Set(1, Flags::GetPLUS());
    badFlags.Set(2, Flags::GetLEADING_SPACE());
    badFlags.Set(3, Flags::GetZERO_PAD());
    badFlags.Set(4, Flags::GetGROUP());
    badFlags.Set(5, Flags::GetPARENTHESES());
    FAIL_RETURN(CheckBadFlags(badFlags));
    // '-' requires a width
    if (mWidth == -1 && mF->Contains(Flags::GetLEFT_JUSTIFY())) {
        return E_MISSING_FORMAT_WIDTH_EXCEPTION;
    }
    return NOERROR;
}

ECode Formatter::FormatSpecifier::CheckInteger()
{
    FAIL_RETURN(CheckNumeric());
    if (mPrecision != -1) {
        return E_ILLEGAL_FORMAT_PRECISION_EXCEPTION;
    }

    if (mC == Conversion::DECIMAL_INTEGER) {
        Array<Flags*> badFlags(1);
        badFlags.Set(0, Flags::GetALTERNATE());
        return CheckBadFlags(badFlags);
    }
    else if (mC == Conversion::OCTAL_INTEGER) {
        Array<Flags*> badFlags(1);
        badFlags.Set(0, Flags::GetGROUP());
        return CheckBadFlags(badFlags);
    }
    else {
        Array<Flags*> badFlags(1);
        badFlags.Set(0, Flags::GetGROUP());
        return CheckBadFlags(badFlags);
    }
}

ECode Formatter::FormatSpecifier::CheckBadFlags(
    /* [in] */ const Array<Flags*>& badFlags)
{
    for (Integer i = 0; i < badFlags.GetLength(); i++) {
        if (mF->Contains(badFlags[i])) {
            return FailMismatch(badFlags[i], mC);
        }
    }
    return NOERROR;
}

ECode Formatter::FormatSpecifier::CheckFloat()
{
    FAIL_RETURN(CheckNumeric());
    if (mC == Conversion::DECIMAL_FLOAT) {
    }
    else if (mC == Conversion::HEXADECIMAL_FLOAT) {
        Array<Flags*> badFlags(2);
        badFlags.Set(0, Flags::GetPARENTHESES());
        badFlags.Set(1, Flags::GetGROUP());
        return CheckBadFlags(badFlags);
    }
    else if (mC == Conversion::SCIENTIFIC) {
        Array<Flags*> badFlags(1);
        badFlags.Set(0, Flags::GetGROUP());
        return CheckBadFlags(badFlags);
    }
    else if (mC == Conversion::GENERAL) {
        Array<Flags*> badFlags(1);
        badFlags.Set(0, Flags::GetALTERNATE());
        return CheckBadFlags(badFlags);
    }
    return NOERROR;
}

ECode Formatter::FormatSpecifier::CheckNumeric()
{
    if (mWidth != -1 && mWidth < 0) {
        return E_ILLEGAL_FORMAT_WIDTH_EXCEPTION;
    }

    if (mPrecision != -1 && mPrecision < 0) {
        return E_ILLEGAL_FORMAT_PRECISION_EXCEPTION;
    }

    // '-' and '0' require a width
    if (mWidth == -1 && (mF->Contains(Flags::GetLEFT_JUSTIFY()) ||
            mF->Contains(Flags::GetZERO_PAD()))) {
        return E_MISSING_FORMAT_WIDTH_EXCEPTION;
    }

    // bad combination
    if ((mF->Contains(Flags::GetPLUS()) && mF->Contains(Flags::GetLEADING_SPACE())) ||
            (mF->Contains(Flags::GetLEFT_JUSTIFY()) && mF->Contains(Flags::GetZERO_PAD()))) {
        return E_ILLEGAL_FORMAT_FLAGS_EXCEPTION;
    }
    return NOERROR;
}

ECode Formatter::FormatSpecifier::CheckText()
{
    if (mPrecision != -1) {
        return E_ILLEGAL_FORMAT_PRECISION_EXCEPTION;
    }
    switch (mC) {
        case Conversion::PERCENT_SIGN:
            if (mF->ValueOf() != Flags::GetLEFT_JUSTIFY()->ValueOf() &&
                    mF->ValueOf() != Flags::GetNONE()->ValueOf()) {
                return E_ILLEGAL_FORMAT_FLAGS_EXCEPTION;
            }
            // '-' requires a width
            if (mWidth == -1 && mF->Contains(Flags::GetLEFT_JUSTIFY())) {
                return E_MISSING_FORMAT_WIDTH_EXCEPTION;
            }
            break;
        case Conversion::LINE_SEPARATOR:
            if (mWidth != -1) {
                return E_ILLEGAL_FORMAT_WIDTH_EXCEPTION;
            }
            if (mF->ValueOf() != Flags::GetNONE()->ValueOf()) {
                return E_ILLEGAL_FORMAT_FLAGS_EXCEPTION;
            }
            break;
        default:
            CHECK(0);
    }
    return NOERROR;
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ Byte value,
    /* [in] */ ILocale* l)
{
    Long v = value;
    if (value < 0 &&
            (mC == Conversion::OCTAL_INTEGER ||
             mC == Conversion::HEXADECIMAL_INTEGER)) {
        v += (1ll << 8);
    }
    return Print(v, l);
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ Short value,
    /* [in] */ ILocale* l)
{
    Long v = value;
    if (value < 0 &&
            (mC == Conversion::OCTAL_INTEGER ||
             mC == Conversion::HEXADECIMAL_INTEGER)) {
        v += (1ll << 16);
    }
    return Print(v, l);
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ Integer value,
    /* [in] */ ILocale* l)
{
    Long v = value;
    if (value < 0 &&
            (mC == Conversion::OCTAL_INTEGER ||
             mC == Conversion::HEXADECIMAL_INTEGER)) {
        v += (1ll << 32);
    }
    return Print(v, l);
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ Long value,
    /* [in] */ ILocale* l)
{
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);

    if (mC == Conversion::DECIMAL_INTEGER) {
        Boolean neg = value < 0;
        Array<Char> va;
        if (value < 0) {
            va = StringUtils::ToString(value, 10).Substring(1).GetChars();
        }
        else {
            va = StringUtils::ToString(value, 10).GetChars();
        }

        // leading sign indicator
        LeadingSign(sb, neg);

        // the value
        LocalizedMagnitude(sb, va, mF, AdjustWidth(mWidth, mF, neg), l);

        // trailing sign indicator
        TrailingSign(sb, neg);
    }
    else if (mC == Conversion::OCTAL_INTEGER) {
        Array<Flags*> badFlags(3);
        badFlags.Set(0, Flags::GetPARENTHESES());
        badFlags.Set(1, Flags::GetLEADING_SPACE());
        badFlags.Set(2, Flags::GetPLUS());
        FAIL_RETURN(CheckBadFlags(badFlags));
        String s = StringUtils::ToOctalString(value);
        Integer len = mF->Contains(Flags::GetALTERNATE()) ?
                s.GetLength() + 1 : s.GetLength();

        // apply ALTERNATE (radix indicator for octal) before ZERO_PAD
        if (mF->Contains(Flags::GetALTERNATE())) {
            sb->Append(U'0');
        }
        if (mF->Contains(Flags::GetZERO_PAD())) {
            for (Integer i = 0; i < mWidth - len; i++) {
                sb->Append(U'0');
            }
        }
        sb->Append(s);
    }
    else if (mC == Conversion::HEXADECIMAL_INTEGER) {
        Array<Flags*> badFlags(3);
        badFlags.Set(0, Flags::GetPARENTHESES());
        badFlags.Set(1, Flags::GetLEADING_SPACE());
        badFlags.Set(2, Flags::GetPLUS());
        FAIL_RETURN(CheckBadFlags(badFlags));
        String s = StringUtils::ToHexString(value);
        Integer len = mF->Contains(Flags::GetALTERNATE()) ?
                s.GetLength() + 2 : s.GetLength();

        // apply ALTERNATE (radix indicator for hex) before ZERO_PAD
        if (mF->Contains(Flags::GetALTERNATE())) {
            sb->Append(mF->Contains(Flags::GetUPPERCASE()) ? String("0X") : String("0x"));
        }
        if (mF->Contains(Flags::GetZERO_PAD())) {
            for (Integer i = 0; i < mWidth - len; i++) {
                sb->Append(U'0');
            }
        }
        if (mF->Contains(Flags::GetUPPERCASE())) {
            s = s.ToUpperCase();
        }
        sb->Append(s);
    }

    // justify based on width
    String str;
    sb->ToString(&str);
    return mOwner->mA->Append(CoreUtils::Box(Justify(str)));
}

void Formatter::FormatSpecifier::LeadingSign(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ Boolean neg)
{
    if (!neg) {
        if (mF->Contains(Flags::GetPLUS())) {
            sb->Append(U'+');
        }
        else if (mF->Contains(Flags::GetLEADING_SPACE())) {
            sb->Append(U' ');
        }
    }
    else {
        if (mF->Contains(Flags::GetPARENTHESES())) {
            sb->Append(U'(');
        }
        else {
            sb->Append(U'-');
        }
    }
}

void Formatter::FormatSpecifier::TrailingSign(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ Boolean neg)
{
    if (neg && mF->Contains(Flags::GetPARENTHESES())) {
        sb->Append(U')');
    }
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ IBigInteger* value,
    /* [in] */ ILocale* l)
{
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    Integer sign;
    value->Signum(&sign);
    Boolean neg = sign == -1;
    AutoPtr<IBigInteger> v;
    value->Abs(&v);

    // leading sign indicator
    LeadingSign(sb, neg);

    if (mC == Conversion::DECIMAL_INTEGER) {
        Array<Char> va = Object::ToString(v).GetChars();
        LocalizedMagnitude(sb, va, mF, AdjustWidth(mWidth, mF, neg), l);
    }
    else if (mC == Conversion::OCTAL_INTEGER) {
        String s;
        v->ToString(8, &s);

        Integer len;
        sb->GetLength(&len);
        len += s.GetLength();
        if (neg && mF->Contains(Flags::GetPARENTHESES())) {
            len++;
        }

        // apply ALTERNATE (radix indicator for octal) before ZERO_PAD
        if (mF->Contains(Flags::GetALTERNATE())) {
            len++;
            sb->Append(U'0');
        }
        if (mF->Contains(Flags::GetZERO_PAD())) {
            for (Integer i = 0; i < mWidth - len; i++) {
                sb->Append(U'0');
            }
        }
        sb->Append(s);
    }
    else if (mC == Conversion::HEXADECIMAL_INTEGER) {
        String s;
        v->ToString(16, &s);

        Integer len;
        sb->GetLength(&len);
        len += s.GetLength();
        if (neg && mF->Contains(Flags::GetPARENTHESES())) {
            len++;
        }

        // apply ALTERNATE (radix indicator for hex) before ZERO_PAD
        if (mF->Contains(Flags::GetALTERNATE())) {
            len += 2;
            sb->Append(mF->Contains(Flags::GetUPPERCASE()) ? String("0X") : String("0x"));
        }
        if (mF->Contains(Flags::GetZERO_PAD())) {
            for (Integer i = 0; i < mWidth - len; i++) {
                sb->Append(U'0');
            }
        }
        if (mF->Contains(Flags::GetUPPERCASE())) {
            s = s.ToUpperCase();
        }
        sb->Append(s);
    }

    TrailingSign(sb, neg);

    String str;
    sb->ToString(&str);
    return mOwner->mA->Append(CoreUtils::Box(Justify(str)));
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ Float value,
    /* [in] */ ILocale* l)
{
    return Print((Double)value, l);
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ Double value,
    /* [in] */ ILocale* l)
{
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    Boolean neg = Math::Compare(value, 0.0) == -1;

    if (!Math::IsNaN(value)) {
        Double v = Math::Abs(value);

        // leading sign indicator
        LeadingSign(sb, neg);

        // the value
        if (!Math::IsInfinite(v)) {
            FAIL_RETURN(Print(sb, v, l, mF, mC, mPrecision, neg));
        }
        else {
            sb->Append(mF->Contains(Flags::GetUPPERCASE()) ?
                    String("INFINITY") : String("Infinity"));
        }

        // trailing sign indicator
        TrailingSign(sb, neg);
    }
    else {
        sb->Append(mF->Contains(Flags::GetUPPERCASE()) ? String("NAN") : String("NaN"));
    }

    // justify based on width
    String str;
    sb->ToString(&str);
    return mOwner->mA->Append(CoreUtils::Box(Justify(str)));
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ Double value,
    /* [in] */ ILocale* l,
    /* [in] */ Flags* f,
    /* [in] */ Char c,
    /* [in] */ Integer precision,
    /* [in] */ Boolean neg)
{
    if (mC == Conversion::SCIENTIFIC) {
        Integer prec = (precision == -1 ? 6 : precision);

        AutoPtr<IFormattedFloatingDecimal> fd;
        FormattedFloatingDecimal::ValueOf(value, prec,
                FormattedFloatingDecimalForm::SCIENTIFIC, &fd);

        Array<Char> mantissa;
        fd->GetMantissa(&mantissa);
        Array<Char> mant = AddZeros(mantissa, prec);

        // If the precision is zero and the '#' flag is set, add the
        // requested decimal point.
        if (f->Contains(Flags::GetALTERNATE()) && (prec == 0)){
            mant = AddDot(mant);
        }

        Array<Char> exp;
        if (value == 0.0) {
            exp = { U'+', U'0', U'0' };
        }
        else {
            fd->GetExponent(&exp);
        }

        Integer newW = mWidth;
        if (mWidth != -1) {
            newW = AdjustWidth(mWidth - exp.GetLength() - 1, f, neg);
        }
        LocalizedMagnitude(sb, mant, f, newW, l);

        AutoPtr<ILocale> separatorLocale = (l != nullptr) ? l : Locale::GetDefault().Get();
        AutoPtr<ILocaleData> localeData;
        LocaleData::Get(separatorLocale, &localeData);
        String exponentSeparator;
        localeData->GetExponentSeparator(&exponentSeparator);
        sb->Append(f->Contains(Flags::GetUPPERCASE()) ?
                exponentSeparator.ToUpperCase() :
                exponentSeparator.ToLowerCase());

        AutoPtr<Flags> flags = f->Dup();
        flags->Remove(Flags::GetGROUP());
        Char sign = exp[0];
        CHECK(sign == U'+' || sign == U'-');
        sb->Append(sign);

        Array<Char> tmp(exp.GetLength() - 1);
        tmp.Copy(0, exp, 1, exp.GetLength() - 1);
        sb->Append(LocalizedMagnitude(nullptr, tmp, flags, -1, l));
    }
    else if (mC == Conversion::DECIMAL_FLOAT) {
        // Create a new FormattedFloatingDecimal with the desired
        // precision.
        Integer prec = (precision == -1 ? 6 : precision);

        AutoPtr<IFormattedFloatingDecimal> fd;
        FormattedFloatingDecimal::ValueOf(value, prec,
                FormattedFloatingDecimalForm::DECIMAL_FLOAT, &fd);

        Array<Char> mantissa;
        fd->GetMantissa(&mantissa);
        Array<Char> mant = AddZeros(mantissa, prec);

        // If the precision is zero and the '#' flag is set, add the
        // requested decimal point.
        if (f->Contains(Flags::GetALTERNATE()) && (prec == 0)) {
            mant = AddDot(mant);
        }

        Integer newW = mWidth;
        if (mWidth != -1) {
            newW = AdjustWidth(mWidth, f, neg);
        }
        LocalizedMagnitude(sb, mant, f, newW, l);
    }
    else if (mC == Conversion::GENERAL) {
        Integer prec = precision;
        if (precision == -1) {
            prec = 6;
        }
        else if (precision == 0) {
            prec = 1;
        }

        Array<Char> exp;
        Array<Char> mant;
        Integer expRounded;
        if (value == 0.0) {
            exp = Array<Char>::Null();
            mant = { U'0' };
            expRounded = 0;
        }
        else {
            AutoPtr<IFormattedFloatingDecimal> fd;
            FormattedFloatingDecimal::ValueOf(value, prec,
                    FormattedFloatingDecimalForm::GENERAL, &fd);
            fd->GetExponent(&exp);
            fd->GetMantissa(&mant);
            fd->GetExponentRounded(&expRounded);
        }

        if (!exp.IsNull()) {
            prec -= 1;
        }
        else {
            prec -= expRounded + 1;
        }

        mant = AddZeros(mant, prec);
        // If the precision is zero and the '#' flag is set, add the
        // requested decimal point.
        if (f->Contains(Flags::GetALTERNATE()) && (prec == 0)) {
            mant = AddDot(mant);
        }

        Integer newW = mWidth;
        if (mWidth != -1) {
            if (!exp.IsNull()) {
                newW = AdjustWidth(mWidth - exp.GetLength() - 1, f, neg);
            }
            else {
                newW = AdjustWidth(mWidth, f, neg);
            }
        }
        LocalizedMagnitude(sb, mant, f, newW, l);

        if (!exp.IsNull()) {
            sb->Append(f->Contains(Flags::GetUPPERCASE()) ? U'E' : U'e');

            AutoPtr<Flags> flags = f->Dup();
            flags->Remove(Flags::GetGROUP());
            Char sign = exp[0];
            CHECK(sign == U'+' || sign == U'-');
            sb->Append(sign);

            Array<Char> tmp(exp.GetLength() - 1);
            tmp.Copy(0, exp, 1, exp.GetLength() - 1);
            sb->Append(LocalizedMagnitude(nullptr, tmp, flags, -1, l));
        }
    }
    else if (mC == Conversion::HEXADECIMAL_FLOAT) {
        Integer prec = precision;
        if (precision == -1) {
            // assume that we want all of the digits
            prec = 0;
        }
        else if (precision == 0) {
            prec = 1;
        }

        String s = HexDouble(value, prec);

        Array<Char> va;
        Boolean upper = f->Contains(Flags::GetUPPERCASE());
        sb->Append(upper ? String("0X") : String("0x"));

        if (f->Contains(Flags::GetZERO_PAD())) {
            for (Integer i = 0; i < mWidth - s.GetLength() - 2; i++) {
                sb->Append(U'0');
            }
        }

        Integer idx = s.IndexOf(U'p');
        va = s.Substring(0, idx).GetChars();
        if (upper) {
            String tmp(va);
            // don't localize hex
            tmp = tmp.ToUpperCase();
            va = tmp.GetChars();
        }
        sb->Append(prec != 0 ? AddZeros(va, prec) : va);
        sb->Append(upper ? U'P' : U'p');
        sb->Append(s.Substring(idx + 1));
    }
    return NOERROR;
}

Array<Char> Formatter::FormatSpecifier::AddZeros(
    /* [in] */ const Array<Char>& v,
    /* [in] */ Integer prec)
{
    // Look for the dot.  If we don't find one, the we'll need to add
    // it before we add the zeros.
    Integer i;
    for (i = 0; i < v.GetLength(); i++) {
        if (v[i] == U'.') {
            break;
        }
    }
    Boolean needDot = false;
    if (i == v.GetLength()) {
        needDot = true;
    }

    // Determine existing precision.
    Integer outPrec = v.GetLength() - i - (needDot ? 0 : 1);
    CHECK(outPrec <= prec);
    if (outPrec == prec) {
        return v;
    }

    // Create new array with existing contents.
    Array<Char> tmp(v.GetLength() + prec - outPrec + (needDot ? 1 : 0));
    tmp.Copy(v, v.GetLength());

    // Add dot if previously determined to be necessary.
    Integer start = v.GetLength();
    if (needDot) {
        tmp[v.GetLength()] = U'.';
        start++;
    }

    // Add zeros.
    for (Integer j = start; j < tmp.GetLength(); j++) {
        tmp[j] = U'0';
    }

    return tmp;
}

String Formatter::FormatSpecifier::HexDouble(
    /* [in] */ Double d,
    /* [in] */ Integer prec)
{
    if (!Math::IsFinite(d) || d == 0.0 || prec == 0 || prec >= 13) {
        // remove "0x"
        return StringUtils::ToHexString(d).Substring(2);
    }
    else {
        Integer exponent = Math::GetExponent(d);
        Boolean subnormal = (exponent == DoubleConsts::MIN_EXPONENT - 1);

        // If this is subnormal input so normalize (could be faster to
        // do as integer operation).
        if (subnormal) {
            Formatter::sScaleUp = Math::Scalb(1.0, 54);
            d *= sScaleUp;
            // Calculate the exponent.  This is not just exponent + 54
            // since the former is not the normalized exponent.
            exponent = Math::GetExponent(d);
            CHECK(exponent >= DoubleConsts::MIN_EXPONENT &&
                    exponent <= DoubleConsts::MAX_EXPONENT);
        }

        Integer precision = 1 + prec * 4;
        Integer shiftDistance = DoubleConsts::SIGNIFICAND_WIDTH - precision;
        CHECK(shiftDistance >= 1 && shiftDistance < DoubleConsts::SIGNIFICAND_WIDTH);

        Long doppel = Math::DoubleToLongBits(d);
        // Deterime the number of bits to keep.
        Long newSignif = (doppel & (DoubleConsts::EXP_BIT_MASK |
                DoubleConsts::SIGNIF_BIT_MASK)) >> shiftDistance;
        // Bits to round away.
        Long roundingBits = doppel & ~(~0LL << shiftDistance);

        // To decide how to round, look at the low-order bit of the
        // working significand, the highest order discarded bit (the
        // round bit) and whether any of the lower order discarded bits
        // are nonzero (the sticky bit).

        Boolean leastZero = (newSignif & 0x1LL) == 0LL;
        Boolean round = ((1LL << (shiftDistance - 1)) & roundingBits) != 0LL;
        Boolean sticky = shiftDistance > 1 &&
                (~(1LL << (shiftDistance - 1)) & roundingBits) != 0;
        if ((leastZero && round && sticky) || (!leastZero && round)) {
            newSignif++;
        }

        Long signBit = doppel & DoubleConsts::SIGN_BIT_MASK;
        newSignif = signBit | (newSignif << shiftDistance);
        Double result = Math::LongBitsToDouble(newSignif);

        if (Math::IsInfinite(result)) {
            // Infinite result generated by rounding
            return String("1.0p1024");
        }
        else {
            String res = StringUtils::ToHexString(result).Substring(2);
            if (!subnormal) {
                return res;
            }
            else {
                // Create a normalized subnormal string.
                Integer idx = res.IndexOf(U'p');
                if (idx == -1) {
                    // No 'p' character in hex string.
                    CHECK(0);
                    return String(nullptr);
                }
                else {
                    // Get exponent and append at the end.
                    String exp = res.Substring(idx + 1);
                    Integer iexp;
                    StringUtils::ParseInteger(exp, &iexp);
                    iexp = iexp - 54;
                    return res.Substring(0, idx) + "p" + StringUtils::ToString(iexp);
                }
            }
        }
    }
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ IBigDecimal* value,
    /* [in] */ ILocale* l)
{
    if (mC == Conversion::HEXADECIMAL_FLOAT) {
        return FailConversion();
    }
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    Integer sign;
    value->Signum(&sign);
    Boolean neg = sign == -1;
    AutoPtr<IBigDecimal> v;
    value->Abs(&v);
    // leading sign indicator
    LeadingSign(sb, neg);

    // the value
    FAIL_RETURN(Print(sb, v, l, mF, mC, mPrecision, neg));

    // trailing sign indicator
    TrailingSign(sb, neg);

    // justify based on width
    String str;
    sb->ToString(&str);
    return mOwner->mA->Append(CoreUtils::Box(Justify(str)));
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ IBigDecimal* _value,
    /* [in] */ ILocale* l,
    /* [in] */ Flags* f,
    /* [in] */ Char c,
    /* [in] */ Integer precision,
    /* [in] */ Boolean neg)
{
    AutoPtr<IBigDecimal> value = _value;
    if (c == Conversion::SCIENTIFIC) {
        // Create a new BigDecimal with the desired precision.
        Integer prec = (precision == -1 ? 6 : precision);
        Integer scale, origPrec;
        value->Scale(&scale);
        value->Precision(&origPrec);
        Integer nzeros = 0;
        Integer compPrec;

        if (prec > origPrec - 1) {
            compPrec = origPrec;
            nzeros = prec - (origPrec - 1);
        }
        else {
            compPrec = prec + 1;
        }

        AutoPtr<IMathContext> mc;
        CMathContext::New(compPrec, IID_IMathContext, (IInterface**)&mc);
        AutoPtr<IBigInteger> bi;
        value->UnscaledValue(&bi);
        AutoPtr<IBigDecimal> v;
        CBigDecimal::New(bi, scale, mc, IID_IBigDecimal, (IInterface**)&v);

        bi = nullptr;
        v->UnscaledValue(&bi);
        v->Scale(&scale);
        BigDecimalLayout bdl(bi, scale, FormatterBigDecimalLayoutForm::SCIENTIFIC);

        Array<Char> mMant = bdl.Mantissa();

        // Add a decimal point if necessary.  The mantissa may not
        // contain a decimal point if the scale is zero (the internal
        // representation has no fractional part) or the original
        // precision is one. Append a decimal point if '#' is set or if
        // we require zero padding to get to the requested precision.
        if ((origPrec == 1 || !bdl.HasDot()) && (
                nzeros > 0 || f->Contains(Flags::GetALTERNATE()))) {
            mMant = AddDot(mMant);
        }

        // Add trailing zeros in the case precision is greater than
        // the number of available digits after the decimal separator.
        mMant = TrailingZeros(mMant, nzeros);

        Array<Char> exp = bdl.Exponent();
        Integer newW = mWidth;
        if (mWidth != -1) {
            newW = AdjustWidth(mWidth - exp.GetLength() - 1, f, neg);
        }
        LocalizedMagnitude(sb, mMant, f, newW, l);

        sb->Append(f->Contains(Flags::GetUPPERCASE()) ? U'E' : U'e');

        AutoPtr<Flags> flags = f->Dup();
        flags->Remove(Flags::GetGROUP());
        Char sign = exp[0];
        CHECK(sign == U'+' || sign == U'-');
        sb->Append(exp[0]);

        Array<Char> tmp(exp.GetLength() - 1);
        tmp.Copy(0, exp, 1, exp.GetLength() - 1);
        sb->Append(LocalizedMagnitude(nullptr, tmp, flags, -1, l));
    }
    else if (c == Conversion::DECIMAL_FLOAT) {
        // Create a new BigDecimal with the desired precision.
        Integer prec = (precision == -1 ? 6 : precision);
        Integer scale;
        value->Scale(&scale);

        if (scale > prec) {
            // more "scale" digits than the requested "precision"
            Integer compPrec;
            value->Precision(&compPrec);
            if (compPrec <= scale) {
                // case of 0.xxxxxx
                AutoPtr<IBigDecimal> tempBD;
                value->SetScale(prec, RoundingMode::HALF_UP, &tempBD);
                value = std::move(tempBD);
            }
            else {
                compPrec -= (scale - prec);
                AutoPtr<IMathContext> mc;
                CMathContext::New(compPrec, IID_IMathContext, (IInterface**)&mc);
                AutoPtr<IBigInteger> tempBI;
                value->UnscaledValue(&tempBI);
                AutoPtr<IBigDecimal> tempBD;
                CBigDecimal::New(tempBI, scale, mc, IID_IBigDecimal, (IInterface**)&tempBD);
                value = std::move(tempBD);
            }
        }
        AutoPtr<IBigInteger> bi;
        value->UnscaledValue(&bi);
        value->Scale(&scale);
        BigDecimalLayout bdl(bi, scale, FormatterBigDecimalLayoutForm::DECIMAL_FLOAT);

        Array<Char> mant = bdl.Mantissa();
        Integer nzeros = (bdl.Scale() < prec ? prec - bdl.Scale() : 0);

        // Add a decimal point if necessary.  The mantissa may not
        // contain a decimal point if the scale is zero (the internal
        // representation has no fractional part).  Append a decimal
        // point if '#' is set or we require zero padding to get to the
        // requested precision.
        if (bdl.Scale() == 0 && (f->Contains(Flags::GetALTERNATE()) || nzeros > 0)) {
            mant = AddDot(bdl.Mantissa());
        }

        // Add trailing zeros if the precision is greater than the
        // number of available digits after the decimal separator.
        mant = TrailingZeros(mant, nzeros);

        LocalizedMagnitude(sb, mant, f, AdjustWidth(mWidth, f, neg), l);
    }
    else if (c == Conversion::GENERAL) {
        Integer prec = precision;
        if (precision == -1) {
            prec = 6;
        }
        else if (precision == 0) {
            prec = 1;
        }

        AutoPtr<IBigDecimal> tenToTheNegFour;
        CBigDecimal::ValueOf(1, 4, &tenToTheNegFour);
        AutoPtr<IBigDecimal> tenToThePrec;
        CBigDecimal::ValueOf(1, -prec, &tenToThePrec);
        Integer comp;
        if ((Object::Equals(value, CBigDecimal::GetZERO())) ||
                ((IComparable::Probe(value)->CompareTo(tenToTheNegFour, &comp), comp != -1) &&
                (IComparable::Probe(value)->CompareTo(tenToThePrec, &comp), comp == -1))) {
            Integer scale;
            value->Scale(&scale);
            AutoPtr<IBigInteger> bi;
            value->UnscaledValue(&bi);
            Integer e = -scale + (Object::ToString(bi).GetLength() - 1);

            // xxx.yyy
            //   g precision (# sig digits) = #x + #y
            //   f precision = #y
            //   exponent = #x - 1
            // => f precision = g precision - exponent - 1
            // 0.000zzz
            //   g precision (# sig digits) = #z
            //   f precision = #0 (after '.') + #z
            //   exponent = - #0 (after '.') - 1
            // => f precision = g precision - exponent - 1
            prec = prec - e - 1;

            return Print(sb, value, l, f, Conversion::DECIMAL_FLOAT, prec, neg);
        }
        else {
            return Print(sb, value, l, f, Conversion::SCIENTIFIC, prec - 1, neg);
        }
    }
    else if (c == Conversion::HEXADECIMAL_FLOAT) {
        // This conversion isn't supported.  The error should be
        // reported earlier.
        CHECK(0);
    }
    return NOERROR;
}

Integer Formatter::FormatSpecifier::AdjustWidth(
    /* [in] */ Integer width,
    /* [in] */ Flags* f,
    /* [in] */ Boolean neg)
{
    Integer newW = mWidth;
    if (newW != -1 && neg && mF->Contains(Flags::GetPARENTHESES())) {
        newW--;
    }
    return newW;
}

Array<Char> Formatter::FormatSpecifier::AddDot(
    /* [in] */ const Array<Char>& mant)
{
    Array<Char> tmp(mant.GetLength() + 1);
    tmp.Copy(mant, mant.GetLength());
    tmp[tmp.GetLength() - 1] = U'.';
    return tmp;
}

Array<Char> Formatter::FormatSpecifier::TrailingZeros(
    /* [in] */ const Array<Char>& mant,
    /* [in] */ Integer nzeros)
{
    Array<Char> tmp = mant;
    if (nzeros > 0) {
        tmp = Array<Char>(mant.GetLength() + nzeros);
        tmp.Copy(mant, mant.GetLength());
        for (Integer i = mant.GetLength(); i < tmp.GetLength(); i++) {
            tmp[i] = U'0';
        }
    }
    return tmp;
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ ICalendar* t,
    /* [in] */ Char c,
    /* [in] */ ILocale* l)
{
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    FAIL_RETURN(Print(sb, t, c, l));

    // justify based on width
    String str;
    sb->ToString(&str);
    String s = Justify(str);
    if (mF->Contains(Flags::GetUPPERCASE())) {
        s = s.ToUpperCase();
    }

    return mOwner->mA->Append(CoreUtils::Box(s));
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ IStringBuilder* _sb,
    /* [in] */ ICalendar* t,
    /* [in] */ Char c,
    /* [in] */ ILocale* l)
{
    AutoPtr<IStringBuilder> sb = _sb;
    if (sb == nullptr) {
        CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    }
    switch(c) {
        case DateTime::HOUR_OF_DAY_0: // 'H' (00 - 23)
        case DateTime::HOUR_0:        // 'I' (01 - 12)
        case DateTime::HOUR_OF_DAY:   // 'k' (0 - 23) -- like H
        case DateTime::HOUR: {        // 'l' (1 - 12) -- like I
            Integer i;
            t->Get(ICalendar::HOUR_OF_DAY, &i);
            if (c == DateTime::HOUR_0 || c == DateTime::HOUR) {
                i = ((i == 0 || i == 12) ? 12 : i % 12);
            }
            AutoPtr<Flags> flags = ((c == DateTime::HOUR_OF_DAY_0 ||
                    c == DateTime::HOUR_0) ? Flags::GetZERO_PAD() : Flags::GetNONE());
            sb->Append(LocalizedMagnitude(nullptr, i, flags, 2, l));
            break;
        }
        case DateTime::MINUTE: { // 'M' (00 - 59)
            Integer i;
            t->Get(ICalendar::MINUTE, &i);
            AutoPtr<Flags> flags = Flags::GetZERO_PAD();
            sb->Append(LocalizedMagnitude(nullptr, i, flags, 2, l));
            break;
        }
        case DateTime::NANOSECOND: { // 'N' (000000000 - 999999999)
            Integer i;
            t->Get(ICalendar::MILLISECOND, &i);
            i = i * 1000000;
            AutoPtr<Flags> flags = Flags::GetZERO_PAD();
            sb->Append(LocalizedMagnitude(nullptr, i, flags, 9, l));
            break;
        }
        case DateTime::MILLISECOND_SINCE_EPOCH: { // 'Q' (0 - 99...?)
            Long i;
            t->GetTimeInMillis(&i);
            AutoPtr<Flags> flags = Flags::GetNONE();
            sb->Append(LocalizedMagnitude(nullptr, i, flags, mWidth, l));
            break;
        }
        case DateTime::AM_PM: { // 'p' (am or pm)
            Array<String> ampm { String("AM"), String("PM") };
            if (l != nullptr && l != Locale::GetUS()) {
                AutoPtr<IDateFormatSymbols> dfs = DateFormatSymbols::GetInstance(l);
                dfs->GetAmPmStrings(&ampm);
            }
            Integer i;
            t->Get(ICalendar::AM_PM, &i);
            String s = ampm[i];
            sb->Append(s.ToLowerCase(/* l != nullptr ? l : Locale::GetUS() */));
            break;
        }
        case DateTime::SECONDS_SINCE_EPOCH: { // 's' (0 - 99...?)
            Long i;
            t->GetTimeInMillis(&i);
            i = i / 1000;
            AutoPtr<Flags> flags = Flags::GetNONE();
            sb->Append(LocalizedMagnitude(nullptr, i, flags, mWidth, l));
            break;
        }
        case DateTime::SECOND: { // 'S' (00 - 60 - leap second)
            Integer i;
            t->Get(ICalendar::SECOND, &i);
            AutoPtr<Flags> flags = Flags::GetZERO_PAD();
            sb->Append(LocalizedMagnitude(nullptr, i, flags, 2, l));
            break;
        }
        case DateTime::ZONE_NUMERIC: { // 'z' ({-|+}####) - ls minus?
            Integer zoff, doff;
            t->Get(ICalendar::ZONE_OFFSET, &zoff);
            t->Get(ICalendar::DST_OFFSET, &doff);
            Integer i = zoff + doff;
            Boolean neg = i < 0;
            sb->Append(neg ? U'-' : U'+');
            if (neg) {
                i = -i;
            }
            Integer min = i / 60000;
            // combine minute and hour into a single integer
            Integer offset = (min / 60) * 100 + (min % 60);
            AutoPtr<Flags> flags = Flags::GetZERO_PAD();
            sb->Append(LocalizedMagnitude(nullptr, offset, flags, 4, l));
            break;
        }
        case DateTime::ZONE: { // 'Z' (symbol)
            AutoPtr<ITimeZone> tz;
            t->GetTimeZone(&tz);
            Integer i;
            t->Get(ICalendar::DST_OFFSET, &i);
            String disName;
            tz->GetDisplayName(i != 0, ITimeZone::SHORT,
                    (l == nullptr) ? Locale::GetUS().Get() : l, &disName);
            sb->Append(disName);
            break;
        }

        // Date
        case DateTime::NAME_OF_DAY_ABBREV: // 'a'
        case DateTime::NAME_OF_DAY: {      // 'A'
            Integer i;
            t->Get(ICalendar::DAY_OF_WEEK, &i);
            AutoPtr<ILocale> lt = ((l == nullptr) ? Locale::GetUS().Get() : l);
            AutoPtr<IDateFormatSymbols> dfs = DateFormatSymbols::GetInstance(lt);
            if (c == DateTime::NAME_OF_DAY) {
                Array<String> weekdays;
                dfs->GetWeekdays(&weekdays);
                sb->Append(weekdays[i]);
            }
            else {
                Array<String> weekdays;
                dfs->GetShortWeekdays(&weekdays);
                sb->Append(weekdays[i]);
            }
            break;
        }
        case DateTime::NAME_OF_MONTH_ABBREV:     // 'b'
        case DateTime::NAME_OF_MONTH_ABBREV_X: // 'h' -- same b
        case DateTime::NAME_OF_MONTH: {        // 'B'
            Integer i;
            t->Get(ICalendar::MONTH, &i);
            AutoPtr<ILocale> lt = ((l == nullptr) ? Locale::GetUS().Get() : l);
            AutoPtr<IDateFormatSymbols> dfs = DateFormatSymbols::GetInstance(lt);
            if (c == DateTime::NAME_OF_MONTH) {
                Array<String> months;
                dfs->GetMonths(&months);
                sb->Append(months[i]);
            }
            else {
                Array<String> months;
                dfs->GetShortMonths(&months);
                sb->Append(months[i]);
            }
            break;
        }
        case DateTime::CENTURY:     // 'C' (00 - 99)
        case DateTime::YEAR_2:      // 'y' (00 - 99)
        case DateTime::YEAR_4: {    // 'Y' (0000 - 9999)
            Integer i;
            t->Get(ICalendar::YEAR, &i);
            Integer size = 2;
            switch(c) {
                case DateTime::CENTURY:
                    i /= 100;
                    break;
                case DateTime::YEAR_2:
                    i %= 100;
                    break;
                case DateTime::YEAR_4:
                    size = 4;
                    break;
            }
            AutoPtr<Flags> flags = Flags::GetZERO_PAD();
            sb->Append(LocalizedMagnitude(nullptr, i, flags, size, l));
            break;
        }
        case DateTime::DAY_OF_MONTH_0:  // 'd' (01 - 31)
        case DateTime::DAY_OF_MONTH: {  // 'e' (1 - 31) -- like d
            Integer i;
            t->Get(ICalendar::DATE, &i);
            AutoPtr<Flags> flags = (c == DateTime::DAY_OF_MONTH_0 ?
                    Flags::GetZERO_PAD() : Flags::GetNONE());
            sb->Append(LocalizedMagnitude(nullptr, i, flags, 2, l));
            break;
        }
        case DateTime::DAY_OF_YEAR: {  // 'j' (001 - 366)
            Integer i;
            t->Get(ICalendar::DAY_OF_YEAR, &i);
            AutoPtr<Flags> flags = Flags::GetZERO_PAD();
            sb->Append(LocalizedMagnitude(nullptr, i, flags, 3, l));
            break;
        }
        case DateTime::MONTH: { // 'm' (01 - 12)
            Integer i;
            t->Get(ICalendar::MONTH, &i);
            i = i + 1;
            AutoPtr<Flags> flags = Flags::GetZERO_PAD();
            sb->Append(LocalizedMagnitude(nullptr, i, flags, 2, l));
            break;
        }

        // Composites
        case DateTime::TIME:        // 'T' (24 hour hh:mm:ss - %tH:%tM:%tS)
        case DateTime::TIME_24_HOUR: {  // 'R' (hh:mm same as %H:%M)
            Char sep = U':';
            Print(sb, t, DateTime::HOUR_OF_DAY_0, l);
            sb->Append(sep);
            Print(sb, t, DateTime::MINUTE, l);
            if (c == DateTime::TIME) {
                sb->Append(sep);
                Print(sb, t, DateTime::SECOND, l);
            }
            break;
        }
        case DateTime::TIME_12_HOUR: {  // 'r' (hh:mm:ss [AP]M)
            Char sep = U':';
            Print(sb, t, DateTime::HOUR_0, l);
            sb->Append(sep);
            Print(sb, t, DateTime::MINUTE, l);
            sb->Append(sep);
            Print(sb, t, DateTime::SECOND, l);
            sb->Append(U' ');
            // this may be in wrong place for some locales
            AutoPtr<IStringBuilder> tsb;
            CStringBuilder::New(IID_IStringBuilder, (IInterface**)&tsb);
            Print(tsb, t, DateTime::AM_PM, l);
            sb->Append(Object::ToString(tsb).ToUpperCase(/*l != nullptr ? l : Locale::GetUS().Get()*/));
            break;
        }
        case DateTime::DATE_TIME: { // 'c' (Sat Nov 04 12:02:33 EST 1999)
            Char sep = U' ';
            Print(sb, t, DateTime::NAME_OF_DAY_ABBREV, l);
            sb->Append(sep);
            Print(sb, t, DateTime::NAME_OF_MONTH_ABBREV, l);
            sb->Append(sep);
            Print(sb, t, DateTime::DAY_OF_MONTH_0, l);
            sb->Append(sep);
            Print(sb, t, DateTime::TIME, l);
            sb->Append(sep);
            Print(sb, t, DateTime::ZONE, l);
            sb->Append(sep);
            Print(sb, t, DateTime::YEAR_4, l);
            break;
        }
        case DateTime::DATE: {  // 'D' (mm/dd/yy)
            Char sep = U'/';
            Print(sb, t, DateTime::MONTH, l);
            sb->Append(sep);
            Print(sb, t, DateTime::DAY_OF_MONTH_0, l);
            sb->Append(sep);
            Print(sb, t, DateTime::YEAR_2, l);
            break;
        }
        case DateTime::ISO_STANDARD_DATE: { // 'F' (%Y-%m-%d)
            Char sep = U'-';
            Print(sb, t, DateTime::YEAR_4, l);
            sb->Append(sep);
            Print(sb, t, DateTime::MONTH, l);
            sb->Append(sep);
            Print(sb, t, DateTime::DAY_OF_MONTH_0, l);
            break;
        }
        default:
            CHECK(0);
    }
    return NOERROR;
}

ECode Formatter::FormatSpecifier::FailMismatch(
    /* [in] */ Flags* f,
    /* [in] */ Char c)
{
    Logger::E("Formatter::FormatSpecifier", "f: %s, c: %c", f->ToString().string(), c);
    return E_FORMAT_FLAGS_CONVERSION_MISMATCH_EXCEPTION;
}

Char Formatter::FormatSpecifier::GetZero(
    /* [in] */ ILocale* l)
{
    AutoPtr<ILocale> ll;
    mOwner->GetLocale(&ll);
    if ((l != nullptr) && !Object::Equals(l, ll)) {
        AutoPtr<IDecimalFormatSymbols> dfs = DecimalFormatSymbols::GetInstance(l);
        Char c;
        dfs->GetZeroDigit(&c);
        return c;
    }
    return mOwner->mZero;
}

AutoPtr<IStringBuilder> Formatter::FormatSpecifier::LocalizedMagnitude(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ Long value,
    /* [in] */ Flags* f,
    /* [in] */ Integer width,
    /* [in] */ ILocale* l)
{
    Array<Char> va = StringUtils::ToString(value, 10).GetChars();
    return LocalizedMagnitude(sb, va, f, width, l);
}

AutoPtr<IStringBuilder> Formatter::FormatSpecifier::LocalizedMagnitude(
    /* [in] */ IStringBuilder* _sb,
    /* [in] */ const Array<Char>& value,
    /* [in] */ Flags* f,
    /* [in] */ Integer width,
    /* [in] */ ILocale* l)
{
    AutoPtr<IStringBuilder> sb = _sb;
    if (sb == nullptr) {
        CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    }
    Integer begin;
    sb->GetLength(&begin);

    Char zero = GetZero(l);

    // determine localized grouping separator and size
    Char grpSep = U'\0';
    Integer grpSize = -1;
    Char decSep = U'\0';

    Integer len = value.GetLength();
    Integer dot = len;
    for (Integer j = 0; j < len; j++) {
        if (value[j] == U'.') {
            dot = j;
            break;
        }
    }

    if (dot < len) {
        if (l == nullptr || Object::Equals(l, Locale::GetUS())) {
            decSep = U'.';
        }
        else {
            AutoPtr<IDecimalFormatSymbols> dfs = DecimalFormatSymbols::GetInstance(l);
            dfs->GetDecimalSeparator(&decSep);
        }
    }

    if (f->Contains(Flags::GetGROUP())) {
        if (l == nullptr || Object::Equals(l, Locale::GetUS())) {
            grpSep = U',';
            grpSize = 3;
        }
        else {
            AutoPtr<IDecimalFormatSymbols> dfs = DecimalFormatSymbols::GetInstance(l);
            dfs->GetGroupingSeparator(&grpSep);
            AutoPtr<INumberFormat> nf;
            NumberFormat::GetIntegerInstance(l, &nf);
            AutoPtr<IDecimalFormat> df = IDecimalFormat::Probe(nf);
            df->GetGroupingSize(&grpSize);

            Boolean used;
            Integer size;
            if ((df->IsGroupingUsed(&used), !used) || (df->GetGroupingSize(&size), size == 0)) {
                grpSep = U'\0';
            }
        }
    }

    // localize the digits inserting group separators as necessary
    for (Integer j = 0; j < len; j++) {
        if (j == dot) {
            sb->Append(decSep);
            // no more group separators after the decimal separator
            grpSep = U'\0';
            continue;
        }

        Char c = value[j];
        sb->Append((Char)(c - U'0' + zero));
        if (grpSep != U'\0' && j != dot - 1 && ((dot - j) % grpSize == 1)) {
            sb->Append(grpSep);
        }
    }

    // apply zero padding
    sb->GetLength(&len);
    if (width != -1 && f->Contains(Flags::GetZERO_PAD())) {
        for (Integer k = 0; k < width - len; k++) {
            sb->Insert(begin, zero);
        }
    }

    return sb;
}

//-------------------------------------------------------------------------

String Formatter::FormatSpecifierParser::GetFLAGS()
{
    static const String FLAGS(",-(+# 0<");
    return FLAGS;
}

ECode Formatter::FormatSpecifierParser::Constructor(
    /* [in] */ Formatter* owner,
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
        if (c == U'$') {
            mIndex = nint;
            Advance();
        }
        else if (nint.GetChar(0) == U'0') {
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
    if (c == U'.') {
        Advance();
        if (!NextIsInt()) {
            return E_ILLEGAL_FORMAT_PRECISION_EXCEPTION;
        }
        mPrecision = NextInt();
    }
    FAIL_RETURN(Peek(&c));
    if (c == U't' || c == U'T') {
        Advance(&c);
        mTT = String::Format("%c", c);
    }
    // Conversion
    FAIL_RETURN(Advance(&c));
    mConv = String::Format("%c", c);

    mFs = new FormatSpecifier();
    return mFs->Constructor(owner, mIndex, mFlags, mWidth, mPrecision, mTT, mConv);
}

String Formatter::FormatSpecifierParser::NextInt()
{
    Integer strBegin = mCursor;
    while (NextIsInt()) {
        Advance();
    }
    return mFormat.Substring(strBegin, mCursor);
}

Boolean Formatter::FormatSpecifierParser::NextIsInt()
{
    Char c;
    ECode ec = Peek(&c);
    if (FAILED(ec)) return false;
    return !IsEnd() && Character::IsDigit(c);
}

ECode Formatter::FormatSpecifierParser::Peek(
    /* [out] */ Char* c)
{
    if (IsEnd()) {
        Logger::E("Formatter", "End of String");
        return E_UNKNOWN_FORMAT_CONVERSION_EXCEPTION;
    }
    *c = mFormat.GetChar(mCursor);
    return NOERROR;
}

ECode Formatter::FormatSpecifierParser::Advance(
    /* [out] */ Char* c)
{
    if (IsEnd()) {
        Logger::E("Formatter", "End of String");
        return E_UNKNOWN_FORMAT_CONVERSION_EXCEPTION;
    }
    Char ch = mFormat.GetChar(mCursor++);
    if (c != nullptr) {
        *c = ch;
    }
    return NOERROR;
}

//-------------------------------------------------------------------------

Formatter::BigDecimalLayout::BigDecimalLayout(
    /* [in] */ IBigInteger* integerValue,
    /* [in] */ Integer scale,
    /* [in] */ FormatterBigDecimalLayoutForm form)
{
    Layout(integerValue, scale, form);
}

Array<Char> Formatter::BigDecimalLayout::LayoutChars()
{
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(ICharSequence::Probe(mMant), IID_IStringBuilder, (IInterface**)&sb);
    if (mExp != nullptr) {
        sb->Append(U'E');
        sb->Append(mExp);
    }
    return ToCharArray(sb);
}

Array<Char> Formatter::BigDecimalLayout::ToCharArray(
    /* [in] */ IStringBuilder* sb)
{
    if (sb == nullptr) {
        return Array<Char>::Null();
    }
    Integer size;
    sb->GetLength(&size);
    Array<Char> result(size);
    sb->GetChars(0, result.GetLength(), result, 0);
    return result;
}

void Formatter::BigDecimalLayout::Layout(
    /* [in] */ IBigInteger* integerValue,
    /* [in] */ Integer scale,
    /* [in] */ FormatterBigDecimalLayoutForm form)
{
    Array<Char> coeff = Object::ToString(integerValue).GetChars();
    mScale = scale;

    // Construct a buffer, with sufficient capacity for all cases.
    // If E-notation is needed, length will be: +1 if negative, +1
    // if '.' needed, +2 for "E+", + up to 10 for adjusted
    // exponent.  Otherwise it could have +1 if negative, plus
    // leading "0.00000"
    mMant = nullptr;
    CStringBuilder::New(coeff.GetLength() + 14, IID_IStringBuilder, (IInterface**)&mMant);

    if (scale == 0) {
        Integer len = coeff.GetLength();
        if (len > 1) {
            mMant->Append(coeff[0]);
            if (form == FormatterBigDecimalLayoutForm::SCIENTIFIC) {
                mMant->Append(U'.');
                mDot = true;
                mMant->Append(coeff, 1, len - 1);
                mExp = nullptr;
                CStringBuilder::New(String("+"), IID_IStringBuilder, (IInterface**)&mExp);
                if (len < 10) {
                    mExp->Append(String("0"));
                    mExp->Append(len - 1);
                }
                else {
                    mExp->Append(len - 1);
                }
            }
            else {
                mMant->Append(coeff, 1, len -1 );
            }
        }
        else {
            mMant->Append(coeff);
            if (form == FormatterBigDecimalLayoutForm::SCIENTIFIC) {
                mExp = nullptr;
                CStringBuilder::New(String("+00"), IID_IStringBuilder, (IInterface**)&mExp);
            }
        }
        return;
    }
    Long adjusted = -(Long)scale + (coeff.GetLength() - 1);
    if (form == FormatterBigDecimalLayoutForm::DECIMAL_FLOAT) {
        // count of padding zeros
        Integer pad = scale - coeff.GetLength();
        if (pad >= 0) {
            // 0.xxx form
            mMant->Append(String("0."));
            mDot = true;
            for (; pad > 0; pad--) {
                mMant->Append(U'0');
            }
            mMant->Append(coeff);
        }
        else {
            if (-pad < coeff.GetLength()) {
                // xx.xx form
                mMant->Append(coeff, 0, -pad);
                mMant->Append(U'.');
                mDot = true;
                mMant->Append(coeff, -pad, scale);
            }
            else {
                // xx form
                mMant->Append(coeff, 0, coeff.GetLength());
                for (Integer i = 0; i < -scale; i++) {
                    mMant->Append(U'0');
                }
                mScale = 0;
            }
        }
    }
    else {
        // x.xxx form
        mMant->Append(coeff[0]);
        if (coeff.GetLength() > 1) {
            mMant->Append(U'.');
            mDot = true;
            mMant->Append(coeff, 1, coeff.GetLength() - 1);
        }
        mExp = nullptr;
        CStringBuilder::New(IID_IStringBuilder, (IInterface**)&mExp);
        if (adjusted != 0) {
            Long abs = Math::Abs(adjusted);
            // require sign
            mExp->Append(adjusted < 0 ? U'-' : U'+');
            if (abs < 10) {
                mExp->Append(U'0');
            }
            mExp->Append(abs);
        }
        else {
            mExp->Append(String("+00"));
        }
    }
}

//-------------------------------------------------------------------------

Boolean Formatter::Conversion::IsValid(
    /* [in] */ Char c)
{
    return (IsGeneral(c) || IsInteger(c) || IsFloat(c) || IsText(c)
            || c == U't' || IsCharacter(c));
}

Boolean Formatter::Conversion::IsGeneral(
    /* [in] */ Char c)
{
    switch (c) {
        case BOOLEAN:
        case BOOLEAN_UPPER:
        case STRING:
        case STRING_UPPER:
        case HASHCODE:
        case HASHCODE_UPPER:
            return true;
        default:
            return false;
    }
}

Boolean Formatter::Conversion::IsCharacter(
    /* [in] */ Char c)
{
    switch (c) {
        case CHARACTER:
        case CHARACTER_UPPER:
            return true;
        default:
            return false;
    }
}

Boolean Formatter::Conversion::IsInteger(
    /* [in] */ Char c)
{
    switch (c) {
        case DECIMAL_INTEGER:
        case OCTAL_INTEGER:
        case HEXADECIMAL_INTEGER:
        case HEXADECIMAL_INTEGER_UPPER:
            return true;
        default:
            return false;
    }
}

Boolean Formatter::Conversion::IsFloat(
    /* [in] */ Char c)
{
    switch (c) {
        case SCIENTIFIC:
        case SCIENTIFIC_UPPER:
        case GENERAL:
        case GENERAL_UPPER:
        case DECIMAL_FLOAT:
        case HEXADECIMAL_FLOAT:
        case HEXADECIMAL_FLOAT_UPPER:
            return true;
        default:
            return false;
    }
}

Boolean Formatter::Conversion::IsText(
    /* [in] */ Char c)
{
    switch (c) {
        case LINE_SEPARATOR:
        case PERCENT_SIGN:
            return true;
        default:
            return false;
    }
}

//-------------------------------------------------------------------------

Boolean Formatter::DateTime::IsValid(
    /* [in] */ Char c)
{
    switch (c) {
        case HOUR_OF_DAY_0:
        case HOUR_0:
        case HOUR_OF_DAY:
        case HOUR:
        case MINUTE:
        case NANOSECOND:
        case MILLISECOND:
        case MILLISECOND_SINCE_EPOCH:
        case AM_PM:
        case SECONDS_SINCE_EPOCH:
        case SECOND:
        case TIME:
        case ZONE_NUMERIC:
        case ZONE:

        // Date
        case NAME_OF_DAY_ABBREV:
        case NAME_OF_DAY:
        case NAME_OF_MONTH_ABBREV:
        case NAME_OF_MONTH:
        case CENTURY:
        case DAY_OF_MONTH_0:
        case DAY_OF_MONTH:
        case NAME_OF_MONTH_ABBREV_X:
        case DAY_OF_YEAR:
        case MONTH:
        case YEAR_2:
        case YEAR_4:

        // Composites
        case TIME_12_HOUR:
        case TIME_24_HOUR:
        case DATE_TIME:
        case DATE:
        case ISO_STANDARD_DATE:
            return true;
        default:
            return false;
    }
}

}
}
