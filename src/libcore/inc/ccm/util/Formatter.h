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

#ifndef __CCM_UTIL_FORMATTER_H__
#define __CCM_UTIL_FORMATTER_H__

#include "ccm/core/SyncObject.h"
#include "ccm.core.IAppendable.h"
#include "ccm.core.IAutoCloseable.h"
#include "ccm.core.IStringBuilder.h"
#include "ccm.io.ICloseable.h"
#include "ccm.io.IFile.h"
#include "ccm.io.IFlushable.h"
#include "ccm.io.IOutputStream.h"
#include "ccm.io.IPrintStream.h"
#include "ccm.io.charset.ICharset.h"
#include "ccm.math.IBigDecimal.h"
#include "ccm.math.IBigInteger.h"
#include "ccm.util.ICalendar.h"
#include "ccm.util.IFormatter.h"
#include "ccm.util.ILocale.h"
#include <ccmautoptr.h>
#include <ccmrefbase.h>

using ccm::core::IAppendable;
using ccm::core::IAutoCloseable;
using ccm::core::IStringBuilder;
using ccm::core::SyncObject;
using ccm::io::ICloseable;
using ccm::io::IFile;
using ccm::io::IFlushable;
using ccm::io::IOutputStream;
using ccm::io::IPrintStream;
using ccm::io::charset::ICharset;
using ccm::math::IBigDecimal;
using ccm::math::IBigInteger;

namespace ccm {
namespace util {

class Formatter
    : public SyncObject
    , public IFormatter
    , public ICloseable
    , public IFlushable
    , public IAutoCloseable
{
private:
    class Flags
        : public LightRefBase
    {
    public:
        static AutoPtr<Flags> GetNONE();
        static AutoPtr<Flags> GetLEFT_JUSTIFY();
        static AutoPtr<Flags> GetUPPERCASE();
        static AutoPtr<Flags> GetALTERNATE();
        static AutoPtr<Flags> GetPLUS();
        static AutoPtr<Flags> GetLEADING_SPACE();
        static AutoPtr<Flags> GetZERO_PAD();
        static AutoPtr<Flags> GetGROUP();
        static AutoPtr<Flags> GetPARENTHESES();
        static AutoPtr<Flags> GetPREVIOUS();

        inline Integer ValueOf()
        {
            return mFlags;
        }

        inline Boolean Contains(
            /* [in] */ Flags* f)
        {
            return (mFlags & f->ValueOf()) == f->ValueOf();
        }

        inline AutoPtr<Flags> Dup()
        {
            return new Flags(mFlags);
        }

        inline void Add(
            /* [in] */ Flags* f)
        {
            mFlags |= f->ValueOf();
        }

        inline void Remove(
            /* [in] */ Flags* f)
        {
            mFlags &= ~f->ValueOf();
        }

        static ECode Parse(
            /* [in] */ const String& s,
            /* [out] */ Flags** f);

        static ECode Parse(
            /* [in] */ Char c,
            /* [out] */ Flags** f);

        inline static String ToString(
            /* [in] */ Flags* f)
        {
            return f->ToString();
        }

        String ToString();

    private:
        inline Flags(
            /* [in] */ Integer f)
            : mFlags(f)
        {}

    private:
        Integer mFlags;
    };

    static const InterfaceID IID_IFormatString;
    INTERFACE_ID(d6dc3cb6-3ca2-4066-a773-e89e69b00a6d)
    interface IFormatString
        : public IInterface
    {
        inline static IFormatString* Probe(
            /* [in] */ IInterface* object)
        {
            if (object == nullptr) return nullptr;
            return (IFormatString*)object->Probe(IID_IFormatString);
        }

        virtual ECode GetIndex(
            /* [out] */ Integer* idx) = 0;

        virtual ECode Print(
            /* [in] */ IInterface* arg,
            /* [in] */ ILocale* l) = 0;

        virtual ECode ToString(
            /* [out] */ String* str) = 0;
    };

    class FixedString
        : public LightRefBase
        , public IFormatString
    {
    public:
        CCM_INTERFACE_DECL();

        FixedString(
            /* [in] */ Formatter* owner,
            /* [in] */ const String& s)
            : mOwner(owner)
            , mS(s)
        {}

        ECode GetIndex(
            /* [out] */ Integer* idx) override;

        ECode Print(
            /* [in] */ IInterface* arg,
            /* [in] */ ILocale* l) override;

        ECode ToString(
            /* [out] */ String* str) override;

    private:
        Formatter* mOwner;
        String mS;
    };

    class FormatSpecifier
        : public LightRefBase
        , public IFormatString
    {
    public:
        CCM_INTERFACE_DECL();

        ECode Constructor(
            /* [in] */ Formatter* owner,
            /* [in] */ const String& indexStr,
            /* [in] */ const String& flagsStr,
            /* [in] */ const String& widthStr,
            /* [in] */ const String& precisionStr,
            /* [in] */ const String& tTStr,
            /* [in] */ const String& convStr);

        ECode GetIndex(
            /* [out] */ Integer* idx) override;

        inline AutoPtr<Flags> GetFlags()
        {
            return mF;
        }

        inline Integer Width()
        {
            return mWidth;
        }

        inline Integer Precision()
        {
            return mPrecision;
        }

        ECode Print(
            /* [in] */ IInterface* arg,
            /* [in] */ ILocale* l) override;

        ECode ToString(
            /* [out] */ String* str) override;

    private:
        Integer Index(
            /* [in] */ const String& s);

        ECode ToFlags(
            /* [in] */ const String& s,
            /* [out] */ Flags** f = nullptr);

        ECode Width(
            /* [in] */ const String& s,
            /* [out] */ Integer* w = nullptr);

        ECode Precision(
            /* [in] */ const String& s,
            /* [out] */ Integer* p = nullptr);

        ECode Conversion(
            /* [in] */ const String& s,
            /* [out] */ Char* c = nullptr);

        inline Char Conversion()
        {
            return mC;
        }

        ECode PrintInteger(
            /* [in] */ IInterface* arg,
            /* [in] */ ILocale* l);

        ECode PrintFloat(
            /* [in] */ IInterface* arg,
            /* [in] */ ILocale* l);

        ECode PrintDateTime(
            /* [in] */ IInterface* arg,
            /* [in] */ ILocale* l);

        ECode PrintCharacter(
            /* [in] */ IInterface* arg);

        ECode PrintString(
            /* [in] */ IInterface* arg,
            /* [in] */ ILocale* l);

        ECode PrintBoolean(
            /* [in] */ IInterface* arg);

        ECode PrintHashCode(
            /* [in] */ IInterface* arg);

        ECode Print(
            /* [in] */ const String& s);

        String Justify(
            /* [in] */ const String& s);

        ECode CheckGeneral();

        ECode CheckDateTime();

        ECode CheckCharacter();

        ECode CheckInteger();

        ECode CheckBadFlags(
            /* [in] */ const Array<Flags*>& badFlags);

        ECode CheckFloat();

        ECode CheckNumeric();

        ECode CheckText();

        ECode Print(
            /* [in] */ Byte value,
            /* [in] */ ILocale* l);

        ECode Print(
            /* [in] */ Short value,
            /* [in] */ ILocale* l);

        ECode Print(
            /* [in] */ Integer value,
            /* [in] */ ILocale* l);

        ECode Print(
            /* [in] */ Long value,
            /* [in] */ ILocale* l);

        void LeadingSign(
            /* [in] */ IStringBuilder* sb,
            /* [in] */ Boolean neg);

        void TrailingSign(
            /* [in] */ IStringBuilder* sb,
            /* [in] */ Boolean neg);

        ECode Print(
            /* [in] */ IBigInteger* value,
            /* [in] */ ILocale* l);

        ECode Print(
            /* [in] */ Float value,
            /* [in] */ ILocale* l);

        ECode Print(
            /* [in] */ Double value,
            /* [in] */ ILocale* l);

        ECode Print(
            /* [in] */ IStringBuilder* sb,
            /* [in] */ Double value,
            /* [in] */ ILocale* l,
            /* [in] */ Flags* f,
            /* [in] */ Char c,
            /* [in] */ Integer precision,
            /* [in] */ Boolean neg);

        Array<Char> AddZeros(
            /* [in] */ const Array<Char>& v,
            /* [in] */ Integer prec);

        String HexDouble(
            /* [in] */ Double d,
            /* [in] */ Integer prec);

        ECode Print(
            /* [in] */ IBigDecimal* value,
            /* [in] */ ILocale* l);

        ECode Print(
            /* [in] */ IStringBuilder* sb,
            /* [in] */ IBigDecimal* value,
            /* [in] */ ILocale* l,
            /* [in] */ Flags* f,
            /* [in] */ Char c,
            /* [in] */ Integer precision,
            /* [in] */ Boolean neg);

        Integer AdjustWidth(
            /* [in] */ Integer width,
            /* [in] */ Flags* f,
            /* [in] */ Boolean neg);

        Array<Char> AddDot(
            /* [in] */ const Array<Char>& mant);

        Array<Char> TrailingZeros(
            /* [in] */ const Array<Char>& mant,
            /* [in] */ Integer nzeros);

        ECode Print(
            /* [in] */ ICalendar* t,
            /* [in] */ Char c,
            /* [in] */ ILocale* l);

        ECode Print(
            /* [in] */ IStringBuilder* sb,
            /* [in] */ ICalendar* t,
            /* [in] */ Char c,
            /* [in] */ ILocale* l);

        inline ECode FailMismatch()
        {
            return E_FORMAT_FLAGS_CONVERSION_MISMATCH_EXCEPTION;
        }

        inline ECode FailConversion()
        {
            return E_ILLEGAL_FORMAT_CONVERSION_EXCEPTION;
        }

        Char GetZero(
            /* [in] */ ILocale* l);

        void LocalizedMagnitude(
            /* [in] */ IStringBuilder* sb,
            /* [in] */ Long value,
            /* [in] */ Flags* f,
            /* [in] */ Integer width,
            /* [in] */ ILocale* l);

        void LocalizedMagnitude(
            /* [in] */ IStringBuilder* sb,
            /* [in] */ const Array<Char>& value,
            /* [in] */ Flags* f,
            /* [in] */ Integer width,
            /* [in] */ ILocale* l);

    private:
        Formatter* mOwner;
        Integer mIndex = -1;
        AutoPtr<Flags> mF = Flags::GetNONE();
        Integer mWidth = 0;
        Integer mPrecision = 0;
        Boolean mDt = false;
        Char mC = 0;
    };

    class FormatSpecifierParser
        : public LightRefBase
    {
    public:
        ECode Constructor(
            /* [in] */ Formatter* owner,
            /* [in] */ const String& format,
            /* [in] */ Integer startIdx);

        String NextInt();

        Boolean NextIsInt();

        inline AutoPtr<FormatSpecifier> GetFormatSpecifier()
        {
            return mFs;
        }

        inline Integer GetEndIdx()
        {
            return mCursor;
        }

    private:
        static String GetFLAGS();

        ECode Peek(
            /* [out] */ Char* c);

        ECode Advance(
            /* [out] */ Char* c = nullptr);

        inline void Back(
            /* [in] */ Integer len)
        {
            mCursor -= len;
        }

        inline Boolean IsEnd()
        {
            return mCursor == mFormat.GetLength();
        }

    private:
        String mFormat;
        Integer mCursor;
        AutoPtr<FormatSpecifier> mFs;

        String mIndex;
        String mFlags;
        String mWidth;
        String mPrecision;
        String mTT;
        String mConv;
    };

    class Conversion
    {
    public:
        static Boolean IsValid(
            /* [in] */ Char c);

        static Boolean IsGeneral(
            /* [in] */ Char c);

        static Boolean IsCharacter(
            /* [in] */ Char c);

        static Boolean IsInteger(
            /* [in] */ Char c);

        static Boolean IsFloat(
            /* [in] */ Char c);

        static Boolean IsText(
            /* [in] */ Char c);

    private:
        Conversion();

    public:
        // Byte, Short, Integer, Long, BigInteger
        static constexpr Char DECIMAL_INTEGER     = U'd';
        static constexpr Char OCTAL_INTEGER       = U'o';
        static constexpr Char HEXADECIMAL_INTEGER = U'x';
        static constexpr Char HEXADECIMAL_INTEGER_UPPER = U'X';

        // Float, Double, BigDecimal
        static constexpr Char SCIENTIFIC          = U'e';
        static constexpr Char SCIENTIFIC_UPPER    = U'E';
        static constexpr Char GENERAL             = U'g';
        static constexpr Char GENERAL_UPPER       = U'G';
        static constexpr Char DECIMAL_FLOAT       = U'f';
        static constexpr Char HEXADECIMAL_FLOAT   = U'a';
        static constexpr Char HEXADECIMAL_FLOAT_UPPER = U'A';

        // Character, Byte, Short, Integer
        static constexpr Char CHARACTER           = U'c';
        static constexpr Char CHARACTER_UPPER     = U'C';

        // ccm.util.CDate, ccm.util.CCalendar, long
        static constexpr Char DATE_TIME           = U't';
        static constexpr Char DATE_TIME_UPPER     = U'T';

        // if (arg.TYPE != boolean) return boolean
        // if (arg != null) return true; else return false;
        static constexpr Char BOOLEAN             = U'b';
        static constexpr Char BOOLEAN_UPPER       = U'B';
        // if (arg instanceof Formattable) arg.formatTo()
        // else arg.toString();
        static constexpr Char STRING              = U's';
        static constexpr Char STRING_UPPER        = U'S';
        // arg.hashCode()
        static constexpr Char HASHCODE            = U'h';
        static constexpr Char HASHCODE_UPPER      = U'H';

        static constexpr Char LINE_SEPARATOR      = U'n';
        static constexpr Char PERCENT_SIGN        = U'%';
    };

    class DateTime
    {
    public:
        static Boolean IsValid(
            /* [in] */ Char c);

    private:
        DateTime();

    public:
        static constexpr Char HOUR_OF_DAY_0 = U'H'; // (00 - 23)
        static constexpr Char HOUR_0        = U'I'; // (01 - 12)
        static constexpr Char HOUR_OF_DAY   = U'k'; // (0 - 23) -- like H
        static constexpr Char HOUR          = U'l'; // (1 - 12) -- like I
        static constexpr Char MINUTE        = U'M'; // (00 - 59)
        static constexpr Char NANOSECOND    = U'N'; // (000000000 - 999999999)
        static constexpr Char MILLISECOND   = U'L'; // (000 - 999)
        static constexpr Char MILLISECOND_SINCE_EPOCH = U'Q'; // (0 - 99...?)
        static constexpr Char AM_PM         = U'p'; // (am or pm)
        static constexpr Char SECONDS_SINCE_EPOCH = U's'; // (0 - 99...?)
        static constexpr Char SECOND        = U'S'; // (00 - 60 - leap second)
        static constexpr Char TIME          = U'T'; // (24 hour hh:mm:ss)
        static constexpr Char ZONE_NUMERIC  = U'z'; // (-1200 - +1200) - ls minus?
        static constexpr Char ZONE          = U'Z'; // (symbol)

        // Date
        static constexpr Char NAME_OF_DAY_ABBREV    = U'a'; // 'a'
        static constexpr Char NAME_OF_DAY           = U'A'; // 'A'
        static constexpr Char NAME_OF_MONTH_ABBREV  = U'b'; // 'b'
        static constexpr Char NAME_OF_MONTH         = U'B'; // 'B'
        static constexpr Char CENTURY               = U'C'; // (00 - 99)
        static constexpr Char DAY_OF_MONTH_0        = U'd'; // (01 - 31)
        static constexpr Char DAY_OF_MONTH          = U'e'; // (1 - 31) -- like d
        static constexpr Char NAME_OF_MONTH_ABBREV_X  = U'h'; // -- same b
        static constexpr Char DAY_OF_YEAR           = U'j'; // (001 - 366)
        static constexpr Char MONTH                 = U'm'; // (01 - 12)
        static constexpr Char YEAR_2                = U'y'; // (00 - 99)
        static constexpr Char YEAR_4                = U'Y'; // (0000 - 9999)

        // Composites
        static constexpr Char TIME_12_HOUR  = U'r'; // (hh:mm:ss [AP]M)
        static constexpr Char TIME_24_HOUR  = U'R'; // (hh:mm same as %H:%M)
        static constexpr Char DATE_TIME             = U'c';
                                            // (Sat Nov 04 12:02:33 EST 1999)
        static constexpr Char DATE                  = U'D'; // (mm/dd/yy)
        static constexpr Char ISO_STANDARD_DATE     = U'F'; // (%Y-%m-%d)
    };

public:
    CCM_INTERFACE_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ IAppendable* a);

    ECode Constructor(
        /* [in] */ ILocale* l);

    ECode Constructor(
        /* [in] */ IAppendable* a,
        /* [in] */ ILocale* l);

    ECode Constructor(
        /* [in] */  const String& fileName);

    ECode Constructor(
        /* [in] */ const String& fileName,
        /* [in] */ const String& csn);

    ECode Constructor(
        /* [in] */ const String& fileName,
        /* [in] */ const String& csn,
        /* [in] */ ILocale* l);

    ECode Constructor(
        /* [in] */ IFile* file);

    ECode Constructor(
        /* [in] */ IFile* file,
        /* [in] */ const String& csn);

    ECode Constructor(
        /* [in] */ IFile* file,
        /* [in] */ const String& csn,
        /* [in] */ ILocale* l);

    ECode Constructor(
        /* [in] */ IPrintStream* ps);

    ECode Constructor(
        /* [in] */ IOutputStream* os);

    ECode Constructor(
        /* [in] */ IOutputStream* os,
        /* [in] */ const String& csn);

    ECode Constructor(
        /* [in] */ IOutputStream* os,
        /* [in] */ const String& csn,
        /* [in] */ ILocale* l);

    ECode GetLocale(
        /* [out] */ ILocale** locale) override;

    ECode GetOut(
        /* [out] */ IAppendable** output) override;

    ECode ToString(
        /* [out] */ String* str) override;

    ECode Flush() override;

    ECode Close() override;

    ECode GetIoException(
        /* [out] */ ECode* ec) override;

    ECode Format(
        /* [in] */ const String& format,
        /* [in] */ const Array<IInterface*>* args) override;

    ECode Format(
        /* [in] */ ILocale* l,
        /* [in] */ const String& format,
        /* [in] */ const Array<IInterface*>* args) override;

private:
    static ECode ToCharset(
        /* [in] */ const String& csn,
        /* [out] */ ICharset** cs);

    static AutoPtr<IAppendable> NonNullAppendable(
        /* [in] */ IAppendable* a);

    ECode Constructor(
        /* [in] */ ILocale* l,
        /* [in] */ IAppendable* a);

    ECode Constructor(
        /* [in] */ ICharset* charset,
        /* [in] */ ILocale* l,
        /* [in] */ IFile* file);

    static Char GetZero(
        /* [in] */ ILocale* l);

    ECode EnsureOpen();

    ECode Parse(
        /* [in] */ const String& s,
        /* [out, callee] */ Array<IFormatString*>* formats);

private:
    AutoPtr<IAppendable> mA;
    AutoPtr<ILocale> mL;

    ECode mLastException;

    Char mZero;
};

}
}

#endif // __CCM_UTIL_FORMATTER_H__
