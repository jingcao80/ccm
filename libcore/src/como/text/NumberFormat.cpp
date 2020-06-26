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

#include "como/core/CStringBuffer.h"
#include "como/core/Math.h"
#include "como/text/CDecimalFormat.h"
#include "como/text/CParsePosition.h"
#include "como/text/DecimalFormatSymbols.h"
#include "como/text/DontCareFieldPosition.h"
#include "como/text/NumberFormat.h"
#include "como/text/StringArrayWrapper.h"
#include "como/util/CHashtable.h"
#include "como/util/CLocale.h"
#include "como.core.IByte.h"
#include "como.core.ILong.h"
#include "como.core.INumber.h"
#include "como.core.IShort.h"
#include "como.math.IBigInteger.h"
#include "como.util.concurrent.atomic.IAtomicInteger.h"
#include "como.util.concurrent.atomic.IAtomicLong.h"
#include "libcore/icu/ICU.h"
#include "libcore/icu/LocaleData.h"

using como::core::CStringBuffer;
using como::core::IByte;
using como::core::IID_IStringBuffer;
using como::core::ILong;
using como::core::INumber;
using como::core::IShort;
using como::core::Math;
using como::math::IBigInteger;
using como::util::CHashtable;
using como::util::CLocale;
using como::util::IID_IHashtable;
using como::util::concurrent::atomic::IAtomicInteger;
using como::util::concurrent::atomic::IAtomicLong;
using libcore::icu::ICU;
using libcore::icu::ILocaleData;
using libcore::icu::LocaleData;

namespace como {
namespace text {

static AutoPtr<IHashtable> CreateHashtable()
{
    AutoPtr<IHashtable> ht;
    CHashtable::New(3, IID_IHashtable, (IInterface**)&ht);
    return ht;
}

AutoPtr<IHashtable> NumberFormat::GetCachedLocaleData()
{
    static AutoPtr<IHashtable> sCachedLocaleData = CreateHashtable();
    return sCachedLocaleData;
}

COMO_INTERFACE_IMPL_1(NumberFormat, BaseFormat, INumberFormat);

ECode NumberFormat::Format(
    /* [in] */ IInterface* number,
    /* [in, out] */ IStringBuffer* toAppendTo,
    /* [in] */ IFieldPosition* pos)
{
    Integer bitLength;
    if (ILong::Probe(number) != nullptr ||
        IInteger::Probe(number) != nullptr ||
        IShort::Probe(number) != nullptr ||
        IByte::Probe(number) != nullptr ||
        IAtomicInteger::Probe(number) != nullptr ||
        IAtomicLong::Probe(number) != nullptr ||
        (IBigInteger::Probe(number) != nullptr &&
            (IBigInteger::Probe(number)->BitLength(bitLength), bitLength< 64))) {
        Long value;
        INumber::Probe(number)->LongValue(value);
        return Format(value, toAppendTo, pos);
    }
    return NOERROR;
}

ECode NumberFormat::ParseObject(
    /* [in] */ const String& source,
    /* [in] */ IParsePosition* pos,
    /* [out] */ AutoPtr<IInterface>& object)
{
    AutoPtr<INumber> number;
    FAIL_RETURN(Parse(source, pos, number));
    object = std::move(number);
    return NOERROR;
}

ECode NumberFormat::Format(
    /* [in] */ Double number,
    /* [out] */ String& str)
{
    AutoPtr<IStringBuffer> sb;
    CStringBuffer::New(IID_IStringBuffer, (IInterface**)&sb);
    FAIL_RETURN(Format(number, sb, DontCareFieldPosition::GetInstance()));
    return sb->ToString(str);
}

ECode NumberFormat::Format(
    /* [in] */ Long number,
    /* [out] */ String& str)
{
    AutoPtr<IStringBuffer> sb;
    CStringBuffer::New(IID_IStringBuffer, (IInterface**)&sb);
    FAIL_RETURN(Format(number, sb, DontCareFieldPosition::GetInstance()));
    return sb->ToString(str);
}

ECode NumberFormat::Parse(
    /* [in] */ const String& source,
    /* [out] */ AutoPtr<INumber>& number)
{
    AutoPtr<IParsePosition> parsePosition;
    CParsePosition::New(0, IID_IParsePosition, (IInterface**)&parsePosition);
    ECode ec = Parse(source, parsePosition, number);
    Integer index;
    if (parsePosition->GetIndex(index), index == 0) {
        Logger::E("NumberFormat", "Unparseable number: \"%s\"", source.string());
        return E_PARSE_EXCEPTION;
    }
    return ec;
}

ECode NumberFormat::IsParseIntegerOnly(
    /* [out] */ Boolean& value)
{
    value = mParseIntegerOnly;
    return NOERROR;
}

ECode NumberFormat::SetParseIntegerOnly(
    /* [in] */ Boolean value)
{
    mParseIntegerOnly = value;
    return NOERROR;
}

ECode NumberFormat::GetInstance(
    /* [out] */ AutoPtr<INumberFormat>& instance)
{
    return GetInstance(CLocale::GetDefault(CLocale::Category::GetFORMAT()), NUMBERSTYLE, instance);
}

ECode NumberFormat::GetInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ AutoPtr<INumberFormat>& instance)
{
    return GetInstance(locale, NUMBERSTYLE, instance);
}

ECode NumberFormat::GetNumberInstance(
    /* [out] */ AutoPtr<INumberFormat>& instance)
{
    return GetInstance(CLocale::GetDefault(CLocale::Category::GetFORMAT()), NUMBERSTYLE, instance);
}

ECode NumberFormat::GetNumberInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ AutoPtr<INumberFormat>& instance)
{
    return GetInstance(locale, NUMBERSTYLE, instance);
}

ECode NumberFormat::GetIntegerInstance(
    /* [out] */ AutoPtr<INumberFormat>& instance)
{
    return GetInstance(CLocale::GetDefault(CLocale::Category::GetFORMAT()), INTEGERSTYLE, instance);
}

ECode NumberFormat::GetIntegerInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ AutoPtr<INumberFormat>& instance)
{
    return GetInstance(locale, INTEGERSTYLE, instance);
}

ECode NumberFormat::GetCurrencyInstance(
    /* [out] */ AutoPtr<INumberFormat>& instance)
{
    return GetInstance(CLocale::GetDefault(CLocale::Category::GetFORMAT()), CURRENCYSTYLE, instance);
}

ECode NumberFormat::GetCurrencyInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ AutoPtr<INumberFormat>& instance)
{
    return GetInstance(locale, CURRENCYSTYLE, instance);
}

ECode NumberFormat::GetPercentInstance(
    /* [out] */ AutoPtr<INumberFormat>& instance)
{
    return GetInstance(CLocale::GetDefault(CLocale::Category::GetFORMAT()), PERCENTSTYLE, instance);
}

ECode NumberFormat::GetPercentInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ AutoPtr<INumberFormat>& instance)
{
    return GetInstance(locale, PERCENTSTYLE, instance);
}

Array<ILocale*> NumberFormat::GetAvailableLocales()
{
    return ICU::GetAvailableLocales();
}

ECode NumberFormat::GetHashCode(
    /* [out] */ Integer& hash)
{
    hash = mMaximumIntegerDigits * 37 + mMaxFractionDigits;
    return NOERROR;
}

ECode NumberFormat::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& same)
{
    NumberFormat* other = (NumberFormat*)INumberFormat::Probe(obj);
    if (other == nullptr) {
        same = false;
        return NOERROR;
    }
    if (this == other) {
        same = true;
        return NOERROR;
    }
    CoclassID thisCID, otherCID;
    GetCoclassID(thisCID);
    other->GetCoclassID(otherCID);
    if (thisCID != otherCID) {
        same = false;
        return NOERROR;
    }
    same = (mMaximumIntegerDigits == other->mMaximumIntegerDigits
            && mMinimumIntegerDigits == other->mMinimumIntegerDigits
            && mMaximumFractionDigits == other->mMaximumFractionDigits
            && mMinimumFractionDigits == other->mMinimumFractionDigits
            && mGroupingUsed == other->mGroupingUsed
            && mParseIntegerOnly == other->mParseIntegerOnly);
    return NOERROR;
}

ECode NumberFormat::CloneImpl(
    /* [in] */ INumberFormat* newObj)
{
    NumberFormat* nf = (NumberFormat*)newObj;

    nf->mGroupingUsed = mGroupingUsed;
    nf->mMaxIntegerDigits = mMaxIntegerDigits;
    nf->mMinIntegerDigits = mMinIntegerDigits;
    nf->mMaxFractionDigits = mMaxFractionDigits;
    nf->mMinFractionDigits = mMinFractionDigits;
    nf->mParseIntegerOnly = mParseIntegerOnly;
    nf->mMaximumIntegerDigits = mMaximumIntegerDigits;
    nf->mMinimumIntegerDigits = mMinimumIntegerDigits;
    nf->mMaximumFractionDigits = mMaximumFractionDigits;
    nf->mMinimumFractionDigits = mMinimumFractionDigits;
    return NOERROR;
}

ECode NumberFormat::IsGroupingUsed(
    /* [out] */ Boolean& value)
{
    value = mGroupingUsed;
    return NOERROR;
}

ECode NumberFormat::SetGroupingUsed(
    /* [in] */ Boolean value)
{
    mGroupingUsed = value;
    return NOERROR;
}

ECode NumberFormat::GetMaximumIntegerDigits(
    /* [out] */ Integer& value)
{
    value = mMaximumIntegerDigits;
    return NOERROR;
}

ECode NumberFormat::SetMaximumIntegerDigits(
    /* [in] */ Integer newValue)
{
    mMaximumIntegerDigits = Math::Max(0, newValue);
    if (mMinimumIntegerDigits > mMaximumIntegerDigits) {
        mMinimumIntegerDigits = mMaximumIntegerDigits;
    }
    return NOERROR;
}

ECode NumberFormat::GetMinimumIntegerDigits(
    /* [out] */ Integer& value)
{
    value = mMinimumIntegerDigits;
    return NOERROR;
}

ECode NumberFormat::SetMinimumIntegerDigits(
    /* [in] */ Integer newValue)
{
    mMinimumIntegerDigits = Math::Max(0, newValue);
    if (mMinimumIntegerDigits > mMaximumIntegerDigits) {
        mMaximumIntegerDigits = mMinimumIntegerDigits;
    }
    return NOERROR;
}

ECode NumberFormat::GetMaximumFractionDigits(
    /* [out] */ Integer& value)
{
    value = mMaximumFractionDigits;
    return NOERROR;
}

ECode NumberFormat::SetMaximumFractionDigits(
    /* [in] */ Integer newValue)
{
    mMaximumFractionDigits = Math::Max(0, newValue);
    if (mMaximumFractionDigits < mMinimumFractionDigits) {
        mMinimumFractionDigits = mMaximumFractionDigits;
    }
    return NOERROR;
}

ECode NumberFormat::GetMinimumFractionDigits(
    /* [out] */ Integer& value)
{
    value = mMinimumFractionDigits;
    return NOERROR;
}

ECode NumberFormat::SetMinimumFractionDigits(
    /* [in] */ Integer newValue)
{
    mMinimumFractionDigits = Math::Max(0, newValue);
    if (mMaximumFractionDigits < mMinimumFractionDigits) {
        mMaximumFractionDigits = mMinimumFractionDigits;
    }
    return NOERROR;
}

ECode NumberFormat::GetCurrency(
    /* [out] */ AutoPtr<ICurrency>& currency)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode NumberFormat::SetCurrency(
    /* [in] */ ICurrency* currency)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode NumberFormat::GetRoundingMode(
    /* [out] */ RoundingMode& mode)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode NumberFormat::SetRoundingMode(
    /* [in] */ RoundingMode mode)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode NumberFormat::GetInstance(
    /* [in] */ ILocale* desiredLocale,
    /* [in] */ Integer choice,
    /* [out] */ AutoPtr<INumberFormat>& instance)
{
    AutoPtr<IInterface> data;
    GetCachedLocaleData()->Get(desiredLocale, data);
    Array<String> numberPatterns;
    if (data != nullptr) {
        numberPatterns = ((StringArrayWrapper*)data.Get())->mValues;
    }
    if (numberPatterns.IsNull()) {
        AutoPtr<ILocaleData> data;
        LocaleData::Get(desiredLocale, &data);
        numberPatterns = Array<String>(4);
        String pattern;
        data->GetNumberPattern(&pattern);
        numberPatterns[NUMBERSTYLE] = pattern;
        data->GetCurrencyPattern(&pattern);
        numberPatterns[CURRENCYSTYLE] = pattern;
        data->GetPercentPattern(&pattern);
        numberPatterns[PERCENTSTYLE] = pattern;
        data->GetIntegerPattern(&pattern);
        numberPatterns[INTEGERSTYLE] = pattern;
        GetCachedLocaleData()->Put(desiredLocale, new StringArrayWrapper(numberPatterns));
    }

    AutoPtr<IDecimalFormatSymbols> symbols = DecimalFormatSymbols::GetInstance(desiredLocale);
    Integer entry = (choice == INTEGERSTYLE) ? NUMBERSTYLE : choice;
    AutoPtr<IDecimalFormat> format;
    CDecimalFormat::New(numberPatterns[entry], symbols, IID_IDecimalFormat, (IInterface**)&format);

    if (choice == INTEGERSTYLE) {
        INumberFormat::Probe(format)->SetMaximumFractionDigits(0);
        format->SetDecimalSeparatorAlwaysShown(false);
        format->SetParseIntegerOnly(true);
    }
    else if (choice == CURRENCYSTYLE) {
        CDecimalFormat::From(format)->AdjustForCurrencyDefaultFractionDigits();
    }

    instance = INumberFormat::Probe(format);
    return NOERROR;
}

//---------------------------------------------------------------------------

AutoPtr<INumberFormatField> NumberFormat::Field::GetINTEGER()
{
    static const AutoPtr<INumberFormatField> INTEGER = Create(String("integer"));
    return INTEGER;
}

AutoPtr<INumberFormatField> NumberFormat::Field::GetFRACTION()
{
    static const AutoPtr<INumberFormatField> FRACTION = Create(String("fraction"));
    return FRACTION;
}

AutoPtr<INumberFormatField> NumberFormat::Field::GetEXPONENT()
{
    static const AutoPtr<INumberFormatField> EXPONENT = Create(String("exponent"));
    return EXPONENT;
}

AutoPtr<INumberFormatField> NumberFormat::Field::GetDECIMAL_SEPARATOR()
{
    static const AutoPtr<INumberFormatField> DECIMAL_SEPARATOR = Create(String("decimal separator"));
    return DECIMAL_SEPARATOR;
}

AutoPtr<INumberFormatField> NumberFormat::Field::GetSIGN()
{
    static const AutoPtr<INumberFormatField> SIGN = Create(String("sign"));
    return SIGN;
}

AutoPtr<INumberFormatField> NumberFormat::Field::GetGROUPING_SEPARATOR()
{
    static const AutoPtr<INumberFormatField> GROUPING_SEPARATOR = Create(String("grouping separato"));
    return GROUPING_SEPARATOR;
}

AutoPtr<INumberFormatField> NumberFormat::Field::GetEXPONENT_SYMBOL()
{
    static const AutoPtr<INumberFormatField> EXPONENT_SYMBOL = Create(String("exponent symbol"));
    return EXPONENT_SYMBOL;
}

AutoPtr<INumberFormatField> NumberFormat::Field::GetPERCENT()
{
    static const AutoPtr<INumberFormatField> PERCENT = Create(String("percent"));
    return PERCENT;
}

AutoPtr<INumberFormatField> NumberFormat::Field::GetPERMILLE()
{
    static const AutoPtr<INumberFormatField> PERMILLE = Create(String("per mille"));
    return PERMILLE;
}

AutoPtr<INumberFormatField> NumberFormat::Field::GetCURRENCY()
{
    static const AutoPtr<INumberFormatField> CURRENCY = Create(String("currency"));
    return CURRENCY;
}

AutoPtr<INumberFormatField> NumberFormat::Field::GetEXPONENT_SIGN()
{
    static const AutoPtr<INumberFormatField> EXPONENT_SIGN = Create(String("exponent sign"));
    return EXPONENT_SIGN;
}

COMO_INTERFACE_IMPL_1(NumberFormat::Field, BaseFormat::Field, INumberFormatField);

ECode NumberFormat::Field::Constructor(
    /* [in] */ const String& name)
{
    return BaseFormat::Field::Constructor(name);
}

AutoPtr<INumberFormatField> NumberFormat::Field::Create(
    /* [in] */ const String& name)
{
    AutoPtr<NumberFormat::Field> field = new NumberFormat::Field();
    field->Constructor(name);
    return (INumberFormatField*)field.Get();
}

}
}

