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

#include "como/core/CArrayHolder.h"
#include "como/core/System.h"
#include "como/util/Arrays.h"
#include "como/util/Locale.h"
#include "como/util/TimeZone.h"
#include "como/util/concurrent/TimeUnit.h"
#include "como.core.IArrayHolder.h"
#include "como.util.ITimeZone.h"
#include "libcore/icu/StringByteSink.h"
#include "libcore/icu/TimeZoneNames.h"
#include <unicode/calendar.h>
#include <unicode/locid.h>
#include <unicode/tznames.h>
#include <memory>

using como::core::CArrayHolder;
using como::core::IArrayHolder;
using como::core::IID_IArrayHolder;
using como::core::System;
using como::util::Arrays;
using como::util::IID_IComparator;
using como::util::ITimeZone;
using como::util::Locale;
using como::util::TimeZone;
using como::util::concurrent::TimeUnit;

namespace libcore {
namespace icu {

extern ECode MaybeThrowIcuException(
    /* [in] */ const char* provider,
    /* [in] */ UErrorCode errorCode);

Array<String> TimeZoneNames::GetAvailableTimeZoneIds()
{
    static Array<String> sAvailableTimeZoneIds = como::util::TimeZone::GetAvailableIDs();
    return sAvailableTimeZoneIds;
}

static AutoPtr<TimeZoneNames::ZoneStringsCache> CreateCachedZoneStrings()
{
    AutoPtr<TimeZoneNames::ZoneStringsCache> cache = new TimeZoneNames::ZoneStringsCache();
    cache->Constructor();
    AutoPtr<IInterface> zoneStrings;
    cache->Get(como::util::Locale::GetROOT(), &zoneStrings);
    zoneStrings = nullptr;
    cache->Get(como::util::Locale::GetUS(), &zoneStrings);
    zoneStrings = nullptr;
    cache->Get(como::util::Locale::GetDefault(), &zoneStrings);
    return cache;
}

AutoPtr<TimeZoneNames::ZoneStringsCache> TimeZoneNames::GetCachedZoneStrings()
{
    static AutoPtr<ZoneStringsCache> sCachedZoneStrings = CreateCachedZoneStrings();
    return sCachedZoneStrings;
}

AutoPtr<IComparator> TimeZoneNames::GetZONE_STRINGS_COMPARATOR()
{
    class _Comparator
        : public Object
        , public IComparator
    {
    public:
        Integer AddRef(
            /* [in] */ HANDLE id)
        {
            return Object::AddRef(id);
        }

        Integer Release(
            /* [in] */ HANDLE id)
        {
            return Object::Release(id);
        }

        IInterface* Probe(
            /* [in] */ const InterfaceID& iid)
        {
            if (iid == IID_IInterface) {
                return (IInterface*)(IComparator*)this;
            }
            else if (iid == IID_IComparator) {
                return (IComparator*)this;
            }
            return nullptr;
        }

        ECode GetInterfaceID(
            /* [in] */ IInterface* object,
            /* [out] */ InterfaceID* iid)
        {
            VALIDATE_NOT_NULL(iid);

            if (object == (IInterface*)(IComparator*)this) {
                *iid = IID_IComparator;
                return NOERROR;
            }
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        ECode Compare(
            /* [in] */ IInterface* o1,
            /* [in] */ IInterface* o2,
            /* [out] */ Integer* result) override
        {
            Array<String> lhs, rhs;
            ((IArrayHolder*)o1)->GetArray(&lhs);
            ((IArrayHolder*)o2)->GetArray(&rhs);
            *result = lhs[OLSON_NAME].Compare(rhs[OLSON_NAME]);
            return NOERROR;
        }

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* isEqual) override
        {
            return Object::Equals(obj, isEqual);
        }
    };

    return new _Comparator();
}

ECode TimeZoneNames::GetDisplayName(
    /* [in] */ const Array<Array<String>>& zoneStrings,
    /* [in] */ const String& id,
    /* [in] */ Boolean daylight,
    /* [in] */ Integer style,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    Array<String> needle{ id };
    Array<IInterface*> zoneStringsArray(zoneStrings.GetLength());
    for (Long i = 0; i < zoneStrings.GetLength(); i++) {
        AutoPtr<IArrayHolder> zoneStringHolder;
        CArrayHolder::New(zoneStrings[i], IID_IArrayHolder, (IInterface**)&zoneStringHolder);
        zoneStringsArray.Set(i, zoneStringHolder);
    }
    AutoPtr<IArrayHolder> needleHolder;
    CArrayHolder::New(needle, IID_IArrayHolder, (IInterface**)&needleHolder);
    Integer index = Arrays::BinarySearch(zoneStringsArray,
            needleHolder, GetZONE_STRINGS_COMPARATOR());
    if (index >= 0) {
        Array<String> row = zoneStrings[index];
        if (daylight) {
            if (style == ITimeZone::LONG) {
                *name = row[LONG_NAME_DST];
            }
            else {
                *name = row[SHORT_NAME_DST];
            }
            return NOERROR;
        }
        else {
            if (style == ITimeZone::LONG) {
                *name = row[LONG_NAME];
            }
            else {
                *name = row[SHORT_NAME];
            }
            return NOERROR;
        }
    }
    *name = nullptr;
    return NOERROR;
}

ECode TimeZoneNames::GetZoneStrings(
    /* [in] */ ILocale* locale,
    /* [out, callee] */ Array<Array<String>>* zoneStrings)
{
    VALIDATE_NOT_NULL(zoneStrings);

    AutoPtr<ILocale> aLocale = locale;
    if (aLocale == nullptr) {
        aLocale = como::util::Locale::GetDefault();
    }
    AutoPtr<IArrayHolder> zoneStringsHolder;
    GetCachedZoneStrings()->Get(aLocale, (IInterface**)&zoneStringsHolder);
    return zoneStringsHolder->GetArray(zoneStrings);
}

static bool IsUtc(
    /* [in] */ const ::icu::UnicodeString& id)
{
    static const ::icu::UnicodeString kEtcUct("Etc/UCT", 7, ::US_INV);
    static const ::icu::UnicodeString kEtcUtc("Etc/UTC", 7, ::US_INV);
    static const ::icu::UnicodeString kEtcUniversal("Etc/Universal", 13, ::US_INV);
    static const ::icu::UnicodeString kEtcZulu("Etc/Zulu", 8, ::US_INV);

    static const ::icu::UnicodeString kUct("UCT", 3, ::US_INV);
    static const ::icu::UnicodeString kUtc("UTC", 3, ::US_INV);
    static const ::icu::UnicodeString kUniversal("Universal", 9, ::US_INV);
    static const ::icu::UnicodeString kZulu("Zulu", 4, ::US_INV);

    return id == kEtcUct || id == kEtcUtc || id == kEtcUniversal || id == kEtcZulu ||
            id == kUct || id == kUtc || id == kUniversal || id == kZulu;
}

inline String ToUTF8String(
    /* [in] */ const ::icu::UnicodeString& value)
{
    StringByteSink sink;
    value.toUTF8(sink);
    return sink.ToString();
}

static bool SetStringArrayElement(
    /* [out] */ Array<String>& array,
    /* [in] */ Integer i,
    /* [in] */ const ::icu::UnicodeString& s)
{
    // Fill in whatever we got. We don't use the display names if they're "GMT[+-]xx:xx"
    // because icu4c doesn't use the up-to-date time zone transition data, so it gets these
    // wrong. TimeZone.getDisplayName creates accurate names on demand.
    // TODO: investigate whether it's worth doing that work once in the Java wrapper instead of on-demand.
    static const ::icu::UnicodeString kGmt("GMT", 3, ::US_INV);
    if (!s.isBogus() && !s.startsWith(kGmt)) {
        String str = ToUTF8String(s);
        if (str.IsNull()) {
            return false;
        }
        array[i] = str;
    }
    return true;
}

void TimeZoneNames::FillZoneStrings(
    /* [in] */ const String& languageTag,
    /* [out] */ Array<Array<String>>& result)
{
    ::icu::Locale icuLocale;
    icuLocale.setToBogus();

    icuLocale = ::icu::Locale::createFromName(languageTag.string());
    if (icuLocale.isBogus()) {
        return;
    }

    UErrorCode status = U_ZERO_ERROR;
    std::unique_ptr<::icu::TimeZoneNames> names(::icu::TimeZoneNames::createInstance(icuLocale, status));
    if (FAILED(MaybeThrowIcuException("TimeZoneNames::createInstance", status))) {
        return;
    }

    const UDate now(::icu::Calendar::getNow());

    static const ::icu::UnicodeString kUtc("UTC", 3, ::US_INV);

    Long id_count = result.GetLength();
    for (Long i = 0; i < id_count; ++i) {
        Array<String> row = result[i];
        String zoneId = row[0];
        if (zoneId.IsNull()) {
            return;
        }
        ::icu::UnicodeString zone_id(zoneId.string(), zoneId.GetByteLength());

        ::icu::UnicodeString long_std;
        names->getDisplayName(zone_id, UTZNM_LONG_STANDARD, now, long_std);
        ::icu::UnicodeString short_std;
        names->getDisplayName(zone_id, UTZNM_SHORT_STANDARD, now, short_std);
        ::icu::UnicodeString long_dst;
        names->getDisplayName(zone_id, UTZNM_LONG_DAYLIGHT, now, long_dst);
        ::icu::UnicodeString short_dst;
        names->getDisplayName(zone_id, UTZNM_SHORT_DAYLIGHT, now, short_dst);

        if (IsUtc(zone_id)) {
            // ICU doesn't have names for the UTC zones; it just says "GMT+00:00" for both
            // long and short names. We don't want this. The best we can do is use "UTC"
            // for everything (since we don't know how to say "Universal Coordinated Time" in
            // every language).
            // TODO: check CLDR doesn't actually have this somewhere.
            long_std = short_std = long_dst = short_dst = kUtc;
        }

        bool okay = SetStringArrayElement(row, 1, long_std) &&
                SetStringArrayElement(row, 2, short_std) &&
                SetStringArrayElement(row, 3, long_dst) &&
                SetStringArrayElement(row, 4, short_dst);
        if (!okay) {
            return;
        }
    }
}

//-------------------------------------------------------------------

ECode TimeZoneNames::ZoneStringsCache::Create(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** value)
{
    Long start = System::GetNanoTime();

    Array<String> availableTimeZoneIds = GetAvailableTimeZoneIds();
    Array<Array<String>> result(availableTimeZoneIds.GetLength());
    for (Long i = 0; i < availableTimeZoneIds.GetLength(); ++i) {
        result[i] = Array<String>(5);
        result[i][0] = availableTimeZoneIds[i];
    }

    String languageTag;
    ((ILocale*)key)->ToLanguageTag(&languageTag);
    FillZoneStrings(languageTag, result);

    Long end = System::GetNanoTime();
    Long duration = TimeUnit::NANOSECONDS::ToMillis(end - start);
    Logger::V("TimeZoneNames", "Loaded time zone names for \"%s\" in %lld ms",
            Object::ToString(key).string(), duration);

    return CArrayHolder::New(result, IID_IArrayHolder, value);
}

}
}
