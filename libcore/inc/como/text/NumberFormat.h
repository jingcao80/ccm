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

#ifndef __COMO_TEXT_NUMBERFORMAT_H__
#define __COMO_TEXT_NUMBERFORMAT_H__

#include "como/core/SyncObject.h"
#include "como/text/Format.h"
#include "como.core.INumber.h"
#include "como.core.IStringBuffer.h"
#include "como.text.INumberFormat.h"
#include "como.text.INumberFormatField.h"
#include "como.util.ICurrency.h"
#include "como.util.IHashtable.h"
#include "como.util.ILocale.h"

using como::core::INumber;
using como::core::IStringBuffer;
using como::core::SyncObject;
using como::math::RoundingMode;
using como::util::ICurrency;
using como::util::IHashtable;
using como::util::ILocale;

namespace como {
namespace text {

class NumberFormat
    : public BaseFormat
    , public INumberFormat
{
public:
    class Field
        : public BaseFormat::Field
        , public INumberFormatField
    {
    public:
        COMO_INTERFACE_DECL();

        static AutoPtr<INumberFormatField> GetINTEGER();

        static AutoPtr<INumberFormatField> GetFRACTION();

        static AutoPtr<INumberFormatField> GetEXPONENT();

        static AutoPtr<INumberFormatField> GetDECIMAL_SEPARATOR();

        static AutoPtr<INumberFormatField> GetSIGN();

        static AutoPtr<INumberFormatField> GetGROUPING_SEPARATOR();

        static AutoPtr<INumberFormatField> GetEXPONENT_SYMBOL();

        static AutoPtr<INumberFormatField> GetPERCENT();

        static AutoPtr<INumberFormatField> GetPERMILLE();

        static AutoPtr<INumberFormatField> GetCURRENCY();

        static AutoPtr<INumberFormatField> GetEXPONENT_SIGN();

    protected:
        ECode Constructor(
            /* [in] */ const String& name);

    private:
        static AutoPtr<INumberFormatField> Create(
            /* [in] */ const String& name);
    };

public:
    COMO_INTERFACE_DECL();

    ECode Format(
        /* [in] */ IInterface* number,
        /* [in, out] */ IStringBuffer* toAppendTo,
        /* [in] */ IFieldPosition* pos) override;

    ECode ParseObject(
        /* [in] */ const String& source,
        /* [in] */ IParsePosition* pos,
        /* [out] */ IInterface** object) override;

    ECode Format(
        /* [in] */ Double number,
        /* [out] */ String* str) override;

    ECode Format(
        /* [in] */ Long number,
        /* [out] */ String* str) override;

    ECode Parse(
        /* [in] */ const String& source,
        /* [out] */ INumber** number) override;

    ECode IsParseIntegerOnly(
        /* [out] */ Boolean* value) override;

    ECode SetParseIntegerOnly(
        /* [in] */ Boolean value) override;

    static ECode GetInstance(
        /* [out] */ INumberFormat** instance);

    static ECode GetInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance);

    static ECode GetNumberInstance(
        /* [out] */ INumberFormat** instance);

    static ECode GetNumberInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance);

    static ECode GetIntegerInstance(
        /* [out] */ INumberFormat** instance);

    static ECode GetIntegerInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance);

    static ECode GetCurrencyInstance(
        /* [out] */ INumberFormat** instance);

    static ECode GetCurrencyInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance);

    static ECode GetPercentInstance(
        /* [out] */ INumberFormat** instance);

    static ECode GetPercentInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance);

    static Array<ILocale*> GetAvailableLocales();

    ECode GetHashCode(
        /* [out] */ Integer* hash) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* same) override;

    ECode IsGroupingUsed(
        /* [out] */ Boolean* value) override;

    ECode SetGroupingUsed(
        /* [in] */ Boolean value) override;

    ECode GetMaximumIntegerDigits(
        /* [out] */ Integer* value) override;

    ECode SetMaximumIntegerDigits(
        /* [in] */ Integer newValue) override;

    ECode GetMinimumIntegerDigits(
        /* [out] */ Integer* value) override;

    ECode SetMinimumIntegerDigits(
        /* [in] */ Integer newValue) override;

    ECode GetMaximumFractionDigits(
        /* [out] */ Integer* value) override;

    ECode SetMaximumFractionDigits(
        /* [in] */ Integer newValue) override;

    ECode GetMinimumFractionDigits(
        /* [out] */ Integer* value) override;

    ECode SetMinimumFractionDigits(
        /* [in] */ Integer newValue) override;

    ECode GetCurrency(
        /* [out] */ ICurrency** currency) override;

    ECode SetCurrency(
        /* [in] */ ICurrency* currency) override;

    ECode GetRoundingMode(
        /* [out] */ RoundingMode* mode) override;

    ECode SetRoundingMode(
        /* [in] */ RoundingMode mode) override;

    using INumberFormat::Format;

    using INumberFormat::Parse;

protected:
    NumberFormat();

    ECode CloneImpl(
        /* [in] */ INumberFormat* newObj);

private:
    static ECode GetInstance(
        /* [in] */ ILocale* desiredLocale,
        /* [in] */ Integer choice,
        /* [out] */ INumberFormat** instance);

    static AutoPtr<IHashtable> GetCachedLocaleData();

private:
    // Constants used by factory methods to specify a style of format.
    static constexpr Integer NUMBERSTYLE = 0;
    static constexpr Integer CURRENCYSTYLE = 1;
    static constexpr Integer PERCENTSTYLE = 2;
    static constexpr Integer INTEGERSTYLE = 3;

    /**
     * True if the grouping (i.e. thousands) separator is used when
     * formatting and parsing numbers.
     */
    Boolean mGroupingUsed = true;

    /**
     * The maximum number of digits allowed in the integer portion of a
     * number.
     */
    Byte mMaxIntegerDigits = 40;

    /**
     * The minimum number of digits allowed in the integer portion of a
     * number.
     */
    Byte mMinIntegerDigits = 1;

    /**
     * The maximum number of digits allowed in the fractional portion of a
     * number.
     */
    Byte mMaxFractionDigits = 3;

    /**
     * The minimum number of digits allowed in the fractional portion of a
     * number.
     */
    Byte mMinFractionDigits = 0;

    /**
     * True if this format will parse numbers as integers only.
     */
    Boolean mParseIntegerOnly = false;

    /**
     * The maximum number of digits allowed in the integer portion of a
     * number.
     */
    Integer mMaximumIntegerDigits = 40;

    /**
     * The minimum number of digits allowed in the integer portion of a
     * number.
     */
    Integer mMinimumIntegerDigits = 1;

    /**
     * The maximum number of digits allowed in the fractional portion of a
     * number.
     */
    Integer mMaximumFractionDigits = 3;

    /**
     * The minimum number of digits allowed in the fractional portion of a
     * number.
     */
    Integer mMinimumFractionDigits = 0;

};

inline NumberFormat::NumberFormat()
{}

}
}

#endif // __COMO_TEXT_NUMBERFORMAT_H__
