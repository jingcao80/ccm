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
#include "como/core/CoreUtils.h"
#include "como/text/CDateFormatSymbols.h"
#include "como/text/DateFormatSymbols.h"
#include "como/util/Arrays.h"
#include "como/util/CLocale.h"
#include "como/util/TimeZone.h"
#include "como/util/concurrent/CConcurrentHashMap.h"
#include "como.util.IMap.h"
#include "como.util.ITimeZone.h"
#include "libcore/icu/ICU.h"
#include "libcore/icu/LocaleData.h"
#include "libcore/icu/TimeZoneNames.h"

using como::core::AutoLock;
using como::core::CoreUtils;
using como::core::IID_ICloneable;
using como::io::IID_ISerializable;
using como::util::Arrays;
using como::util::CLocale;
using como::util::IMap;
using como::util::ITimeZone;
using como::util::TimeZone;
using como::util::concurrent::CConcurrentHashMap;
using como::util::concurrent::IID_IConcurrentMap;
using libcore::icu::ICU;
using libcore::icu::LocaleData;
using libcore::icu::TimeZoneNames;

namespace como {
namespace text {

const String DateFormatSymbols::sPatternChars("GyMdkHmsSEDFwWahKzZYuXLc");

static AutoPtr<IConcurrentMap> CreateCachedInstances()
{
    AutoPtr<IConcurrentMap> map;
    CConcurrentHashMap::New(3, IID_IConcurrentMap, (IInterface**)&map);
    return map;
}

AutoPtr<IConcurrentMap> DateFormatSymbols::GetCachedInstances()
{
    static AutoPtr<IConcurrentMap> sCachedInstances = CreateCachedInstances();
    return sCachedInstances;
}

COMO_INTERFACE_IMPL_3(DateFormatSymbols, SyncObject, IDateFormatSymbols, ISerializable, ICloneable);

ECode DateFormatSymbols::Constructor()
{
    InitializeData(CLocale::GetDefault(CLocale::Category::GetFORMAT()));
    return NOERROR;
}

ECode DateFormatSymbols::Constructor(
    /* [in] */ ILocale* locale)
{
    InitializeData(locale);
    return NOERROR;
}

Array<ILocale*> DateFormatSymbols::GetAvailableLocales()
{
    return ICU::GetAvailableLocales();
}

AutoPtr<IDateFormatSymbols> DateFormatSymbols::GetInstance()
{
    return GetInstance(CLocale::GetDefault(CLocale::Category::GetFORMAT()));
}

AutoPtr<IDateFormatSymbols> DateFormatSymbols::GetInstance(
    /* [in] */ ILocale* locale)
{
    AutoPtr<IDateFormatSymbols> clone = (IDateFormatSymbols*)
            CoreUtils::Clone(GetCachedInstance(locale), IID_IDateFormatSymbols).Get();
    return clone;
}

AutoPtr<IDateFormatSymbols> DateFormatSymbols::GetInstanceRef(
    /* [in] */ ILocale* locale)
{
    return GetCachedInstance(locale);
}

AutoPtr<IDateFormatSymbols> DateFormatSymbols::GetCachedInstance(
    /* [in] */ ILocale* locale)
{
    AutoPtr<IConcurrentMap> cachedInstances = GetCachedInstances();
    AutoPtr<IInterface> v;
    IMap::Probe(cachedInstances)->Get(locale, v);
    AutoPtr<IDateFormatSymbols> ref = std::move(v);
    if (ref == nullptr) {
        CDateFormatSymbols::New(locale, IID_IDateFormatSymbols, (IInterface**)&ref);
        AutoPtr<IDateFormatSymbols> x;
        cachedInstances->PutIfAbsent(locale, ref, (IInterface**)&x);
        if (x != nullptr) {
            ref = x;
        }
    }
    return ref;
}

ECode DateFormatSymbols::GetEras(
    /* [out, callee] */ Array<String>* eras)
{
    VALIDATE_NOT_NULL(eras);

    return Arrays::CopyOf(mEras, mEras.GetLength(), eras);
}

ECode DateFormatSymbols::SetEras(
    /* [in] */ const Array<String>& newEras)
{
    Arrays::CopyOf(newEras, newEras.GetLength(), &mEras);
    VOLATILE_SET(mCachedHashCode, 0);
    return NOERROR;
}

ECode DateFormatSymbols::GetMonths(
    /* [out, callee] */ Array<String>* months)
{
    VALIDATE_NOT_NULL(months);

    return Arrays::CopyOf(mMonths, mMonths.GetLength(), months);
}

ECode DateFormatSymbols::SetMonths(
    /* [in] */ const Array<String>& newMonths)
{
    Arrays::CopyOf(newMonths, newMonths.GetLength(), &mMonths);
    VOLATILE_SET(mCachedHashCode, 0);
    return NOERROR;
}

ECode DateFormatSymbols::GetShortMonths(
    /* [out, callee] */ Array<String>* months)
{
    VALIDATE_NOT_NULL(months);

    return Arrays::CopyOf(mShortMonths, mShortMonths.GetLength(), months);
}

ECode DateFormatSymbols::SetShortMonths(
    /* [in] */ const Array<String>& newShortMonths)
{
    Arrays::CopyOf(newShortMonths, newShortMonths.GetLength(), &mShortMonths);
    VOLATILE_SET(mCachedHashCode, 0);
    return NOERROR;
}

ECode DateFormatSymbols::GetWeekdays(
    /* [out, callee] */ Array<String>* weekdays)
{
    VALIDATE_NOT_NULL(weekdays);

    return Arrays::CopyOf(mWeekdays, mWeekdays.GetLength(), weekdays);
}

ECode DateFormatSymbols::SetWeekdays(
    /* [in] */ const Array<String>& newWeekdays)
{
    Arrays::CopyOf(newWeekdays, newWeekdays.GetLength(), &mWeekdays);
    VOLATILE_SET(mCachedHashCode, 0);
    return NOERROR;
}

ECode DateFormatSymbols::GetShortWeekdays(
    /* [out, callee] */ Array<String>* weekdays)
{
    VALIDATE_NOT_NULL(weekdays);

    return Arrays::CopyOf(mShortWeekdays, mShortWeekdays.GetLength(), weekdays);
}

ECode DateFormatSymbols::SetShortWeekdays(
    /* [in] */ const Array<String>& newShortWeekdays)
{
    Arrays::CopyOf(newShortWeekdays, newShortWeekdays.GetLength(), &mShortWeekdays);
    VOLATILE_SET(mCachedHashCode, 0);
    return NOERROR;
}

ECode DateFormatSymbols::GetAmPmStrings(
    /* [out, callee] */ Array<String>* ampm)
{
    VALIDATE_NOT_NULL(ampm);

    return Arrays::CopyOf(mAmpms, mAmpms.GetLength(), ampm);
}

ECode DateFormatSymbols::SetAmPmStrings(
    /* [in] */ const Array<String>& newAmpms)
{
    Arrays::CopyOf(newAmpms, newAmpms.GetLength(), &mAmpms);
    VOLATILE_SET(mCachedHashCode, 0);
    return NOERROR;
}

ECode DateFormatSymbols::GetZoneStrings(
    /* [out, callee] */ Array<Array<String>>* zoneStrings)
{
    VALIDATE_NOT_NULL(zoneStrings);

    *zoneStrings = GetZoneStringsImpl(true);
    return NOERROR;
}

ECode DateFormatSymbols::SetZoneStrings(
    /* [in] */ const Array<Array<String>>& newZoneStrings)
{
    Array<Array<String>> aCopy(newZoneStrings.GetLength());
    for (Integer i = 0; i < newZoneStrings.GetLength(); ++i) {
        Integer len = newZoneStrings[i].GetLength();
        if (len < 5) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        Arrays::CopyOf(newZoneStrings[i], len, &aCopy[i]);
    }
    mZoneStrings = aCopy;
    mIsZoneStringsSet = true;
    return NOERROR;
}

ECode DateFormatSymbols::GetLocalPatternChars(
    /* [out] */ String& localPatternChars)
{
    localPatternChars = mLocalPatternChars;
    return NOERROR;
}

ECode DateFormatSymbols::SetLocalPatternChars(
    /* [in] */ const String& newLocalPatternChars)
{
    mLocalPatternChars = newLocalPatternChars;
    VOLATILE_SET(mCachedHashCode, 0);
    return NOERROR;
}

Array<String> DateFormatSymbols::GetTinyMonths()
{
    return mTinyMonths;
}

Array<String> DateFormatSymbols::GetStandAloneMonths()
{
    return mStandAloneMonths;
}

Array<String> DateFormatSymbols::GetShortStandAloneMonths()
{
    return mShortStandAloneMonths;
}

Array<String> DateFormatSymbols::GetTinyStandAloneMonths()
{
    return mTinyStandAloneMonths;
}

Array<String> DateFormatSymbols::GetTinyWeekdays()
{
    return mTinyWeekdays;
}

Array<String> DateFormatSymbols::GetStandAloneWeekdays()
{
    return mStandAloneWeekdays;
}

Array<String> DateFormatSymbols::GetShortStandAloneWeekdays()
{
    return mShortStandAloneWeekdays;
}

Array<String> DateFormatSymbols::GetTinyStandAloneWeekdays()
{
    return mTinyStandAloneWeekdays;
}

ECode DateFormatSymbols::CloneImpl(
    /* [in] */ IDateFormatSymbols* newObj)
{
    DateFormatSymbols* other = (DateFormatSymbols*)newObj;
    CopyMembers(this, other);
    return NOERROR;
}

ECode DateFormatSymbols::GetHashCode(
    /* [out] */ Integer& hash)
{
    VOLATILE_GET(hash, mCachedHashCode);
    if (hash == 0) {
        hash = 5;
        hash = 11 * hash + Arrays::GetHashCode(mEras);
        hash = 11 * hash + Arrays::GetHashCode(mMonths);
        hash = 11 * hash + Arrays::GetHashCode(mShortMonths);
        hash = 11 * hash + Arrays::GetHashCode(mWeekdays);
        hash = 11 * hash + Arrays::GetHashCode(mShortWeekdays);
        hash = 11 * hash + Arrays::GetHashCode(mAmpms);
        hash = 11 * hash + mLocalPatternChars.GetHashCode();
        VOLATILE_SET(mCachedHashCode, hash);
    }

    return NOERROR;
}

ECode DateFormatSymbols::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& same)
{
    DateFormatSymbols* other = (DateFormatSymbols*)IDateFormatSymbols::Probe(obj);
    if (this == other) {
        same = true;
        return NOERROR;
    }
    if (other == nullptr) {
        same = false;
        return NOERROR;
    }
    if (!(Arrays::Equals(mEras, other->mEras) &&
            Arrays::Equals(mMonths, other->mMonths) &&
            Arrays::Equals(mShortMonths, other->mShortMonths) &&
            Arrays::Equals(mTinyMonths, other->mTinyMonths) &&
            Arrays::Equals(mWeekdays, other->mWeekdays) &&
            Arrays::Equals(mShortWeekdays, other->mShortWeekdays) &&
            Arrays::Equals(mTinyWeekdays, other->mTinyWeekdays) &&
            Arrays::Equals(mStandAloneMonths, other->mStandAloneMonths) &&
            Arrays::Equals(mShortStandAloneMonths, other->mShortStandAloneMonths) &&
            Arrays::Equals(mTinyStandAloneMonths, other->mTinyStandAloneMonths) &&
            Arrays::Equals(mStandAloneWeekdays, other->mStandAloneWeekdays) &&
            Arrays::Equals(mShortStandAloneWeekdays, other->mShortStandAloneWeekdays) &&
            Arrays::Equals(mTinyStandAloneWeekdays, other->mTinyStandAloneWeekdays) &&
            Arrays::Equals(mAmpms, other->mAmpms) &&
            mLocalPatternChars.Equals(other->mLocalPatternChars))) {
        same = false;
        return NOERROR;
    }
    if (!mIsZoneStringsSet && !other->mIsZoneStringsSet && Object::Equals(mLocale, other->mLocale)) {
        same = true;
        return NOERROR;
    }
    same = Arrays::DeepEquals(GetZoneStringsWrapper(), other->GetZoneStringsWrapper());
    return NOERROR;
}

void DateFormatSymbols::InitializeData(
    /* [in] */ ILocale* desiredLocale)
{
    mLocale = desiredLocale;

    AutoPtr<IInterface> v;
    IMap::Probe(GetCachedInstances())->Get(mLocale, v);
    AutoPtr<IDateFormatSymbols> ref = std::move(v);
    if (ref != nullptr) {
        CopyMembers((DateFormatSymbols*)ref.Get(), this);
        return;
    }
    mLocale = LocaleData::MapInvalidAndNullLocales(mLocale);
    AutoPtr<ILocaleData> localeData;
    LocaleData::Get(mLocale, &localeData);

    localeData->GetEras(&mEras);

    localeData->GetLongMonthNames(&mMonths);
    localeData->GetShortMonthNames(&mShortMonths);

    localeData->GetAmPm(&mAmpms);
    mLocalPatternChars = sPatternChars;

    localeData->GetLongWeekdayNames(&mWeekdays);
    localeData->GetShortWeekdayNames(&mShortWeekdays);

    InitializeSupplementaryData(localeData);
}

void DateFormatSymbols::InitializeSupplementaryData(
    /* [in] */ ILocaleData* localeData)
{
    localeData->GetTinyMonthNames(&mTinyMonths);
    localeData->GetTinyWeekdayNames(&mTinyWeekdays);

    localeData->GetLongStandAloneMonthNames(&mStandAloneMonths);
    localeData->GetShortStandAloneMonthNames(&mShortStandAloneMonths);
    localeData->GetTinyStandAloneMonthNames(&mTinyStandAloneMonths);

    localeData->GetLongStandAloneWeekdayNames(&mStandAloneWeekdays);
    localeData->GetShortStandAloneWeekdayNames(&mShortStandAloneWeekdays);
    localeData->GetTinyStandAloneWeekdayNames(&mTinyStandAloneWeekdays);
}

Integer DateFormatSymbols::GetZoneIndex(
    /* [in] */ const String& ID)
{
    Array<Array<String>> zoneStrings = GetZoneStringsWrapper();

    if (mLastZoneIndex < zoneStrings.GetLength() && ID.Equals(zoneStrings[mLastZoneIndex][0])) {
        return mLastZoneIndex;
    }

    for (Integer index = 0; index < zoneStrings.GetLength(); index++) {
        if (ID.Equals(zoneStrings[index][0])) {
            mLastZoneIndex = index;
            return index;
        }
    }

    return -1;
}

Array<Array<String>> DateFormatSymbols::GetZoneStringsWrapper()
{
    CoclassID cid;
    GetCoclassID(cid);
    if (cid != CID_CDateFormatSymbols) {
        Array<Array<String>> zoneStrings;
        GetZoneStrings(&zoneStrings);
        return zoneStrings;
    }
    else {
        return GetZoneStringsImpl(false);
    }
}

Array<Array<String>> DateFormatSymbols::InternalZoneStrings()
{
    AutoLock lock(this);
    if (mZoneStrings.IsNull()) {
        TimeZoneNames::GetZoneStrings(mLocale, &mZoneStrings);
        for (Array<String> zone : mZoneStrings) {
            String id = zone[0];
            if (zone[1].IsNull()) {
                AutoPtr<ITimeZone> tz;
                TimeZone::GetTimeZone(id, &tz);
                tz->GetDisplayName(false, ITimeZone::LONG, mLocale, zone[1]);
            }
            if (zone[2].IsNull()) {
                AutoPtr<ITimeZone> tz;
                TimeZone::GetTimeZone(id, &tz);
                tz->GetDisplayName(false, ITimeZone::SHORT, mLocale, zone[2]);
            }
            if (zone[3].IsNull()) {
                AutoPtr<ITimeZone> tz;
                TimeZone::GetTimeZone(id, &tz);
                tz->GetDisplayName(true, ITimeZone::LONG, mLocale, zone[3]);
            }
            if (zone[4].IsNull()) {
                AutoPtr<ITimeZone> tz;
                TimeZone::GetTimeZone(id, &tz);
                tz->GetDisplayName(true, ITimeZone::SHORT, mLocale, zone[4]);
            }
        }
    }
    return mZoneStrings;
}

Array<Array<String>> DateFormatSymbols::GetZoneStringsImpl(
    /* [in] */ Boolean needsCopy)
{
    Array<Array<String>> zoneStrings = InternalZoneStrings();

    if (!needsCopy) {
        return zoneStrings;
    }

    Integer len = zoneStrings.GetLength();
    Array<Array<String>> aCopy(len);
    for (Integer i = 0; i < len; i++) {
        Arrays::CopyOf(zoneStrings[i], zoneStrings[i].GetLength(), &aCopy[i]);
    }
    return aCopy;
}

void DateFormatSymbols::CopyMembers(
    /* [in] */ DateFormatSymbols* src,
    /* [in] */ DateFormatSymbols* dst)
{
    Arrays::CopyOf(src->mEras, src->mEras.GetLength(), &dst->mEras);
    Arrays::CopyOf(src->mMonths, src->mMonths.GetLength(), &dst->mMonths);
    Arrays::CopyOf(src->mShortMonths, src->mShortMonths.GetLength(), &dst->mShortMonths);
    Arrays::CopyOf(src->mWeekdays, src->mWeekdays.GetLength(), &dst->mWeekdays);
    Arrays::CopyOf(src->mShortWeekdays, src->mShortWeekdays.GetLength(), &dst->mShortWeekdays);
    Arrays::CopyOf(src->mAmpms, src->mAmpms.GetLength(), &dst->mAmpms);
    if (!src->mZoneStrings.IsNull()) {
        dst->mZoneStrings = src->GetZoneStringsImpl(true);
    }
    dst->mLocalPatternChars = src->mLocalPatternChars;

    dst->mTinyMonths = src->mTinyMonths;
    dst->mTinyWeekdays = src->mTinyWeekdays;

    dst->mStandAloneMonths = src->mStandAloneMonths;
    dst->mShortStandAloneMonths = src->mShortStandAloneMonths;
    dst->mTinyStandAloneMonths = src->mTinyStandAloneMonths;

    dst->mStandAloneWeekdays = src->mStandAloneWeekdays;
    dst->mShortStandAloneWeekdays = src->mShortStandAloneWeekdays;
    dst->mTinyStandAloneWeekdays = src->mTinyStandAloneWeekdays;
}

}
}
