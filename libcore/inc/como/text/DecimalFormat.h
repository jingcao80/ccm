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

#ifndef __COMO_TEXT_DECIMALFORMAT_H__
#define __COMO_TEXT_DECIMALFORMAT_H__

#include "como/text/NumberFormat.h"
#include "como.core.IInteger.h"
#include "como.math.IBigDecimal.h"
#include "como.math.IBigInteger.h"
#include "como.text.IDecimalFormat.h"
#include "como.text.IDecimalFormatSymbols.h"
#include "como.text.IParsePosition.h"
#include "como.util.concurrent.IConcurrentMap.h"

using como::core::IInteger;
using como::math::IBigDecimal;
using como::math::IBigInteger;
using como::math::RoundingMode;
using como::util::concurrent::IConcurrentMap;

namespace como {
namespace text {

class DecimalFormat
    : public NumberFormat
    , public IDecimalFormat
{
public:
    ~DecimalFormat();

    COMO_INTERFACE_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ const String& pattern);

    ECode Constructor(
        /* [in] */ const String& pattern,
        /* [in] */ IDecimalFormatSymbols* symbols);

    ECode Format(
        /* [in] */ IInterface* number,
        /* [in, out] */ IStringBuffer* toAppendTo,
        /* [in] */ IFieldPosition* pos) override;

    ECode Format(
        /* [in] */ Double number,
        /* [in, out] */ IStringBuffer* toAppendTo,
        /* [in] */ IFieldPosition* pos) override;

    ECode Format(
        /* [in] */ Long number,
        /* [in, out] */ IStringBuffer* toAppendTo,
        /* [in] */ IFieldPosition* pos) override;

    ECode FormatToCharacterIterator(
        /* [in] */ IInterface* obj,
        /* [out] */ IAttributedCharacterIterator** it) override;

    ECode Parse(
        /* [in] */ const String& source,
        /* [in] */ IParsePosition* pos,
        /* [out] */ INumber** number) override;

    ECode GetDecimalFormatSymbols(
        /* [out] */ IDecimalFormatSymbols** symbols) override;

    ECode SetDecimalFormatSymbols(
        /* [in] */ IDecimalFormatSymbols* symbols) override;

    ECode GetPositivePrefix(
        /* [out] */ String* prefix) override;

    ECode SetPositivePrefix(
        /* [in] */ const String& prefix) override;

    ECode GetNegativePrefix(
        /* [out] */ String* prefix) override;

    ECode SetNegativePrefix(
        /* [in] */ const String& prefix) override;

    ECode GetPositiveSuffix(
        /* [out] */ String* suffix) override;

    ECode SetPositiveSuffix(
        /* [in] */ const String& suffix) override;

    ECode GetNegativeSuffix(
        /* [out] */ String* suffix) override;

    ECode SetNegativeSuffix(
        /* [in] */ const String& suffix) override;

    ECode GetMultiplier(
        /* [out] */ Integer* multiplier) override;

    ECode SetMultiplier(
        /* [in] */ Integer multiplier) override;

    ECode GetGroupingSize(
        /* [out] */ Integer* size) override;

    ECode SetGroupingSize(
        /* [in] */ Integer size) override;

    ECode IsGroupingUsed(
        /* [out] */ Boolean* used) override;

    ECode SetGroupingUsed(
        /* [in] */ Boolean used) override;

    ECode IsDecimalSeparatorAlwaysShown(
        /* [in] */ Boolean* shown) override;

    ECode SetDecimalSeparatorAlwaysShown(
        /* [in] */ Boolean shown) override;

    ECode IsParseBigDecimal(
        /* [out] */ Boolean* value) override;

    ECode SetParseBigDecimal(
        /* [in] */ Boolean value) override;

    ECode SetParseIntegerOnly(
        /* [in] */ Boolean value) override;

    ECode IsParseIntegerOnly(
        /* [out] */ Boolean* value) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& same) override;

    ECode GetHashCode(
        /* [out] */ Integer& hash) override;

    ECode ToPattern(
        /* [out] */ String* pattern) override;

    ECode ToLocalizedPattern(
        /* [out] */ String* pattern) override;

    ECode ApplyPattern(
        /* [in] */ const String& pattern) override;

    ECode ApplyLocalizedPattern(
        /* [in] */ const String& pattern) override;

    ECode SetMaximumIntegerDigits(
        /* [in] */ Integer newValue) override;

    ECode SetMinimumIntegerDigits(
        /* [in] */ Integer newValue) override;

    ECode SetMaximumFractionDigits(
        /* [in] */ Integer newValue) override;

    ECode SetMinimumFractionDigits(
        /* [in] */ Integer newValue) override;

    ECode GetMaximumIntegerDigits(
        /* [out] */ Integer* value) override;

    ECode GetMinimumIntegerDigits(
        /* [out] */ Integer* value) override;

    ECode GetMaximumFractionDigits(
        /* [out] */ Integer* value) override;

    ECode GetMinimumFractionDigits(
        /* [out] */ Integer* value) override;

    ECode GetCurrency(
        /* [out] */ ICurrency** currency) override;

    ECode SetCurrency(
        /* [in] */ ICurrency* currency) override;

    ECode GetRoundingMode(
        /* [out] */ RoundingMode* mode) override;

    ECode SetRoundingMode(
        /* [in] */ RoundingMode mode) override;

    void AdjustForCurrencyDefaultFractionDigits();

    inline static DecimalFormat* From(
        /* [in] */ IDecimalFormat* obj);

protected:
    ECode CloneImpl(
        /* [in] */ IDecimalFormat* newObj);

private:
    ECode Init(
        /* [in] */ const String& pattern);

    ECode Format(
        /* [in] */ IBigDecimal* number,
        /* [in, out] */ IStringBuffer* toAppendTo,
        /* [in] */ IFieldPosition* pos);

    ECode Format(
        /* [in] */ IBigInteger* number,
        /* [in, out] */ IStringBuffer* toAppendTo,
        /* [in] */ IFieldPosition* pos);

    void UpdateFieldsFromNative();

    static AutoPtr<IConcurrentMap> GetCachedLocaleData();

private:
    AutoPtr<IDecimalFormatSymbols> mSymbols;

    Integer mMaximumIntegerDigits;

    Integer mMinimumIntegerDigits;

    Integer mMaximumFractionDigits;

    Integer mMinimumFractionDigits;

    RoundingMode mRoundingMode = RoundingMode::HALF_EVEN;

    // Upper limit on integer and fraction digits for a Java double
    static constexpr Integer DOUBLE_INTEGER_DIGITS = 309;
    static constexpr Integer DOUBLE_FRACTION_DIGITS = 340;

    // Upper limit on integer and fraction digits for BigDecimal and BigInteger
    static constexpr Integer MAXIMUM_INTEGER_DIGITS  = IInteger::MAX_VALUE;
    static constexpr Integer MAXIMUM_FRACTION_DIGITS = IInteger::MAX_VALUE;

    HANDLE mNativeDF = 0;
};

DecimalFormat* DecimalFormat::From(
    /* [in] */ IDecimalFormat* obj)
{
    return (DecimalFormat*)obj;
}

}
}

#endif // __COMO_TEXT_DECIMALFORMAT_H__
