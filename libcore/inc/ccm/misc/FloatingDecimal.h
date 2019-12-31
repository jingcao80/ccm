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

#ifndef __CCM_MISC_FLOATINGDECIMAL_H__
#define __CCM_MISC_FLOATINGDECIMAL_H__

#include "ccm/misc/DoubleConsts.h"
#include "ccm/misc/FloatConsts.h"
#include "ccm.core.IAppendable.h"
#include "ccm.core.IThreadLocal.h"
#include "ccm.misc.IFloatingDecimalASCIIToBinaryConverter.h"
#include "ccm.misc.IFloatingDecimalBinaryToASCIIConverter.h"
#include "ccm.util.regex.IPattern.h"
#include <ccmautoptr.h>
#include <ccmobject.h>

using ccm::core::IAppendable;
using ccm::core::IThreadLocal;
using ccm::util::regex::IPattern;

namespace ccm {
namespace misc {

class FloatingDecimal
{
private:
    class ExceptionalBinaryToASCIIBuffer
        : public Object
        , public IFloatingDecimalBinaryToASCIIConverter
    {
    public:
        ExceptionalBinaryToASCIIBuffer(
            /* [in] */ const String& image,
            /* [in] */ Boolean isNegative);

        CCM_INTERFACE_DECL();

        ECode ToFormatString(
            /* [out] */ String* str) override;

        ECode AppendTo(
            /* [in] */ IAppendable* buf) override;

        ECode GetDecimalExponent(
            /* [out] */ Integer* exponent) override;

        ECode GetDigits(
            /* [out] */ Array<Char>& digits,
            /* [out] */ Integer* number) override;

        ECode IsNegative(
            /* [out] */ Boolean* neg) override;

        ECode IsExceptional(
            /* [out] */ Boolean* exceptional) override;

        ECode DigitsRoundedUp(
            /* [out] */ Boolean* roundedUp) override;

        ECode DecimalDigitsExact(
            /* [out] */ Boolean* exact) override;

    private:
        String mImage;
        Boolean mIsNegative;
    };

    class BinaryToASCIIBuffer
        : public Object
        , public IFloatingDecimalBinaryToASCIIConverter
    {
    public:
        BinaryToASCIIBuffer();

        BinaryToASCIIBuffer(
            /* [in] */ Boolean isNegative,
            /* [in] */ Array<Char>& digits);

        CCM_INTERFACE_DECL();

        ECode ToFormatString(
            /* [out] */ String* str) override;

        ECode AppendTo(
            /* [in] */ IAppendable* buf) override;

        ECode GetDecimalExponent(
            /* [out] */ Integer* exponent) override;

        ECode GetDigits(
            /* [out] */ Array<Char>& digits,
            /* [out] */ Integer* number) override;

        ECode IsNegative(
            /* [out] */ Boolean* neg) override;

        ECode IsExceptional(
            /* [out] */ Boolean* exceptional) override;

        ECode DigitsRoundedUp(
            /* [out] */ Boolean* roundedUp) override;

        ECode DecimalDigitsExact(
            /* [out] */ Boolean* exact) override;

        inline void SetSign(
            /* [in] */ Boolean isNegative)
        {
            mIsNegative = isNegative;
        }

        void Dtoa(
            /* [in] */ Integer binExp,
            /* [in] */ Long fractBits,
            /* [in] */ Integer nSignificantBits,
            /* [in] */ Boolean isCompatibleFormat);

        /**
         * Estimate decimal exponent. (If it is small-ish,
         * we could double-check.)
         *
         * First, scale the mantissa bits such that 1 <= d2 < 2.
         * We are then going to estimate
         *          log10(d2) ~=~  (d2-1.5)/1.5 + log(1.5)
         * and so we can estimate
         *      log10(d) ~=~ log10(d2) + binExp * log10(2)
         * take the floor and call it decExp.
         */
        static Integer EstimateDecExp(
            /* [in] */ Long fractBits,
            /* [in] */ Integer binExp);

    private:
        void DevelopLongDigits(
            /* [in] */ Integer decExponent,
            /* [in] */ Long lvalue,
            /* [in] */ Integer insignificantDigits);

        void Roundup();

        static Integer InsignificantDigits(
            /* [in] */ Integer insignificant);

        static Integer InsignificantDigitsForPow2(
            /* [in] */ Integer p2);

        Integer GetChars(
            /* [out]*/ Array<Char>& result);

    private:
        Boolean mIsNegative = false;
        Integer mDecExponent = 0;
        Integer mFirstDigitIndex = 0;
        Integer mNDigits = 0;
        Array<Char> mDigits;
        Array<Char> mBuffer;

        // True if the dtoa() binary to decimal conversion was exact.
        Boolean mExactDecimalConversion = false;

        // True if the result of the binary to decimal conversion was rounded-up
        // at the end of the conversion process, i.e. roundUp() method was called.
        Boolean mDecimalDigitsRoundedUp = false;

        /**
         *  If insignificant==(1L << ixd)
         *  i = insignificantDigitsNumber[idx] is the same as:
         *  int i;
         *  for ( i = 0; insignificant >= 10L; i++ )
         *         insignificant /= 10L;
         */
        static constexpr Integer sInsignificantDigitsNumber[] = {
            0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3,
            4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7,
            8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 11, 11, 11,
            12, 12, 12, 12, 13, 13, 13, 14, 14, 14,
            15, 15, 15, 15, 16, 16, 16, 17, 17, 17,
            18, 18, 18, 19
        };

        // approximately ceil( log2( long5pow[i] ) )
        static constexpr Integer N_5_BITS[] = {
            0,
            3,
            5,
            7,
            10,
            12,
            14,
            17,
            19,
            21,
            24,
            26,
            28,
            31,
            33,
            35,
            38,
            40,
            42,
            45,
            47,
            49,
            52,
            54,
            56,
            59,
            61,
        };
    };

    class PreparedASCIIToBinaryBuffer
        : public Object
        , public IFloatingDecimalASCIIToBinaryConverter
    {
    public:
        PreparedASCIIToBinaryBuffer(
            /* [in] */ Double doubleVal,
            /* [in] */ Float floatVal);

        CCM_INTERFACE_DECL();

        ECode DoubleValue(
            /* [out] */ Double* value) override;

        ECode FloatValue(
            /* [out] */ Float* value) override;

    private:
        Double mDoubleVal;
        Float mFloatVal;
    };

    class ASCIIToBinaryBuffer
        : public Object
        , public IFloatingDecimalASCIIToBinaryConverter
    {
    public:
        ASCIIToBinaryBuffer(
            /* [in] */ Boolean negSign,
            /* [in] */ Integer decExponent,
            /* [in] */ Array<Char>& digits,
            /* [in] */ Integer n);

        CCM_INTERFACE_DECL();

        ECode DoubleValue(
            /* [out] */ Double* value) override;

        ECode FloatValue(
            /* [out] */ Float* value) override;

    private:
        Boolean mIsNegative;
        Integer mDecExponent;
        Array<Char> mDigits;
        Integer mNDigits;

        /**
         * All the positive powers of 10 that can be
         * represented exactly in double/float.
         */
        static constexpr Double SMALL_10_POW[] = {
            1.0e0,
            1.0e1, 1.0e2, 1.0e3, 1.0e4, 1.0e5,
            1.0e6, 1.0e7, 1.0e8, 1.0e9, 1.0e10,
            1.0e11, 1.0e12, 1.0e13, 1.0e14, 1.0e15,
            1.0e16, 1.0e17, 1.0e18, 1.0e19, 1.0e20,
            1.0e21, 1.0e22
        };

        static constexpr Float SINGLE_SMALL_10_POW[] = {
            1.0e0f,
            1.0e1f, 1.0e2f, 1.0e3f, 1.0e4f, 1.0e5f,
            1.0e6f, 1.0e7f, 1.0e8f, 1.0e9f, 1.0e10f
        };

        static constexpr Double BIG_10_POW[] = {
            1e16, 1e32, 1e64, 1e128, 1e256 };
        static constexpr Double TINY_10_POW[] = {
            1e-16, 1e-32, 1e-64, 1e-128, 1e-256 };

        static const Integer MAX_SMALL_TEN;
        static const Integer SINGLE_MAX_SMALL_TEN;
    };

public:
    static String ToFormatString(
        /* [in]*/ Double d);

    static String ToFormatString(
        /* [in] */ Float f);

    static ECode AppendTo(
        /* [in] */ Double d,
        /* [in] */ IAppendable* buf);

    static ECode AppendTo(
        /* [in] */ Float f,
        /* [in] */ IAppendable* buf);

    static ECode ParseDouble(
        /* [in] */ const String& s,
        /* [out] */ Double* value);

    static ECode ParseFloat(
        /* [in] */ const String& s,
        /* [out] */ Float* value);

    inline static AutoPtr<IFloatingDecimalBinaryToASCIIConverter> GetBinaryToASCIIConverter(
        /* [in] */ Double d);

    static AutoPtr<IFloatingDecimalBinaryToASCIIConverter> GetBinaryToASCIIConverter(
        /* [in] */ Double d,
        /* [in] */ Boolean isCompatibleFormat);

private:
    static AutoPtr<BinaryToASCIIBuffer> GetBinaryToASCIIBuffer();

    static AutoPtr<IThreadLocal> GetThreadLocalBinaryToASCIIBuffer();

    static AutoPtr<IFloatingDecimalBinaryToASCIIConverter> GetBinaryToASCIIConverter(
        /* [in] */ Float f);

    static ECode ReadFormatString(
        /* [in] */ const String& in,
        /* [out] */ IFloatingDecimalASCIIToBinaryConverter** converter);

    static ECode ParseHexString(
        /* [in] */ const String& in,
        /* [out] */ IFloatingDecimalASCIIToBinaryConverter** converter);

    static String StripLeadingZeros(
        /* [in] */ const String& s);

    static ECode GetHexDigit(
        /* [in] */ const String& s,
        /* [in] */ Integer position,
        /* [out] */ Integer* value);

    static AutoPtr<IFloatingDecimalBinaryToASCIIConverter> Get_B2AC_POSITIVE_INFINITY();

    static AutoPtr<IFloatingDecimalBinaryToASCIIConverter> Get_B2AC_NEGATIVE_INFINITY();

    static AutoPtr<IFloatingDecimalBinaryToASCIIConverter> Get_B2AC_NOT_A_NUMBER();

    static AutoPtr<IFloatingDecimalBinaryToASCIIConverter> Get_B2AC_POSITIVE_ZERO();

    static AutoPtr<IFloatingDecimalBinaryToASCIIConverter> Get_B2AC_NEGATIVE_ZERO();

    static AutoPtr<IFloatingDecimalASCIIToBinaryConverter> Get_A2BC_POSITIVE_INFINITY();

    static AutoPtr<IFloatingDecimalASCIIToBinaryConverter> Get_A2BC_NEGATIVE_INFINITY();

    static AutoPtr<IFloatingDecimalASCIIToBinaryConverter> Get_A2BC_NOT_A_NUMBER();

    static AutoPtr<IFloatingDecimalASCIIToBinaryConverter> Get_A2BC_POSITIVE_ZERO();

    static AutoPtr<IFloatingDecimalASCIIToBinaryConverter> Get_A2BC_NEGATIVE_ZERO();

    static AutoPtr<IPattern> GET_HEX_FLOAT_PATTERN();

public:
    //
    // Constants of the implementation;
    // most are IEEE-754 related.
    // (There are more really boring constants at the end.)
    //
    static constexpr Integer EXP_SHIFT = DoubleConsts::SIGNIFICAND_WIDTH - 1;
    static constexpr Long FRACT_HOB = (1LL << EXP_SHIFT); // assumed High-Order bit
    static constexpr Long EXP_ONE = ((Long)DoubleConsts::EXP_BIAS) << EXP_SHIFT; // exponent of 1.0
    static constexpr Integer MAX_SMALL_BIN_EXP = 62;
    static constexpr Integer MIN_SMALL_BIN_EXP = -( 63 / 3 );
    static constexpr Integer MAX_DECIMAL_DIGITS = 15;
    static constexpr Integer MAX_DECIMAL_EXPONENT = 308;
    static constexpr Integer MIN_DECIMAL_EXPONENT = -324;
    static constexpr Integer BIG_DECIMAL_EXPONENT = 324; // i.e. abs(MIN_DECIMAL_EXPONENT)
    static constexpr Integer MAX_NDIGITS = 1100;

    static constexpr Integer SINGLE_EXP_SHIFT = FloatConsts::SIGNIFICAND_WIDTH - 1;
    static constexpr Integer SINGLE_FRACT_HOB = (1 << SINGLE_EXP_SHIFT);
    static constexpr Integer SINGLE_MAX_DECIMAL_DIGITS = 7;
    static constexpr Integer SINGLE_MAX_DECIMAL_EXPONENT = 38;
    static constexpr Integer SINGLE_MIN_DECIMAL_EXPONENT = -45;
    static constexpr Integer SINGLE_MAX_NDIGITS = 200;

    static constexpr Integer INT_DECIMAL_DIGITS = 9;

private:
    static const String INFINITY_REP;
    static const Integer INFINITY_LENGTH;
    static const String NAN_REP;
    static const Integer NAN_LENGTH;
};

AutoPtr<IFloatingDecimalBinaryToASCIIConverter> FloatingDecimal::GetBinaryToASCIIConverter(
    /* [in] */ Double d)
{
    return GetBinaryToASCIIConverter(d, true);
}

}
}

#endif // __CCM_MISC_FLOATINGDECIMAL_H__
