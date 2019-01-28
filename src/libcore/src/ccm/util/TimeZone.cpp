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

#include "ccm/core/AutoLock.h"
#include "ccm/core/CoreUtils.h"
#include "ccm/core/CStringBuilder.h"
#include "ccm/core/StringUtils.h"
#include "ccm/util/CDate.h"
#include "ccm/util/CSimpleTimeZone.h"
#include "ccm/util/Locale.h"
#include "ccm/util/TimeZone.h"
#include "ccm/util/regex/Pattern.h"
#include "ccm.core.ICloneable.h"
#include "ccm.util.regex.IMatcher.h"
#include "libcore/icu/TimeZoneNames.h"
#include "libcore/util/ZoneInfoDB.h"
#include "libcore.util.IZoneInfo.h"
#include <ccmlogger.h>

using ccm::core::AutoLock;
using ccm::core::CoreUtils;
using ccm::core::CStringBuilder;
using ccm::core::E_ASSERTION_ERROR;
using ccm::core::E_NUMBER_FORMAT_EXCEPTION;
using ccm::core::ICloneable;
using ccm::core::IStringBuilder;
using ccm::core::IID_ICloneable;
using ccm::core::IID_IStringBuilder;
using ccm::core::StringUtils;
using ccm::io::IID_ISerializable;
using ccm::util::regex::IMatcher;
using ccm::util::regex::Pattern;
using libcore::icu::TimeZoneNames;
using libcore::util::IZoneInfo;
using libcore::util::ZoneInfoDB;

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_3(TimeZone, SyncObject, ITimeZone, ISerializable, ICloneable);

static AutoPtr<IPattern> CreateCUSTOM_ZONE_ID_PATTERN()
{
    AutoPtr<IPattern> pattern;
    Pattern::Compile(String("^GMT[-+](\\d{1,2})(:?(\\d\\d))?$"), &pattern);
    return pattern;
}

static AutoPtr<ITimeZone> CreateGMT()
{
    AutoPtr<ITimeZone> zone;
    CSimpleTimeZone::New(0, String("GMT"), IID_ITimeZone, (IInterface**)&zone);
    return zone;
}

static AutoPtr<ITimeZone> CreateUTC()
{
    AutoPtr<ITimeZone> zone;
    CSimpleTimeZone::New(0, String("UTC"), IID_ITimeZone, (IInterface**)&zone);
    return zone;
}

AutoPtr<ITimeZone> TimeZone::NO_TIMEZONE;

AutoPtr<IPattern> TimeZone::GetCUSTOM_ZONE_ID_PATTERN()
{
    static const AutoPtr<IPattern> CUSTOM_ZONE_ID_PATTERN = CreateCUSTOM_ZONE_ID_PATTERN();
    return CUSTOM_ZONE_ID_PATTERN;
}

AutoPtr<ITimeZone> TimeZone::GetGMT()
{
    static const AutoPtr<ITimeZone> GMT = CreateGMT();
    return GMT;
}

AutoPtr<ITimeZone> TimeZone::GetUTC()
{
    static const AutoPtr<ITimeZone> UTC = CreateUTC();
    return UTC;
}

SyncObject& TimeZone::GetClassLock()
{
    static SyncObject sLock;
    return sLock;
}

ECode TimeZone::GetOffset(
    /* [in] */ Long date,
    /* [out] */ Integer* offset)
{
    VALIDATE_NOT_NULL(offset);

    AutoPtr<IDate> dateObj;
    CDate::New(date, IID_IDate, (IInterface**)&dateObj);
    Boolean daylightTime;
    if (InDaylightTime(dateObj, &daylightTime), daylightTime) {
        Integer ro, st;
        GetRawOffset(&ro);
        GetDSTSavings(&st);
        *offset = ro + st;
        return NOERROR;
    }
    return GetRawOffset(offset);
}

Integer TimeZone::GetOffsets(
    /* [in] */ Long date,
    /* [out] */ Array<Integer>& offsets)
{
    Integer rawoffset;
    GetRawOffset(&rawoffset);
    Integer dstoffset = 0;
    AutoPtr<IDate> dateObj;
    CDate::New(date, IID_IDate, (IInterface**)&dateObj);
    Boolean daylightTime;
    if (InDaylightTime(dateObj, &daylightTime), daylightTime) {
        GetDSTSavings(&dstoffset);
    }
    if (!offsets.IsNull()) {
        offsets[0] = rawoffset;
        offsets[1] = dstoffset;
    }
    return rawoffset + dstoffset;
}

ECode TimeZone::GetID(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);

    *id = mID;
    return NOERROR;
}

ECode TimeZone::SetID(
    /* [in] */ const String& ID)
{
    if (ID.IsNull()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mID = ID;
    return NOERROR;
}

ECode TimeZone::GetDisplayName(
    /* [out] */ String* name)
{
    return GetDisplayName(false, LONG,
            Locale::GetDefault(Locale::Category::GetDISPLAY()), name);
}

ECode TimeZone::GetDisplayName(
    /* [in] */ ILocale* locale,
    /* [out] */ String* name)
{
    return GetDisplayName(false, LONG, locale, name);
}

ECode TimeZone::GetDisplayName(
    /* [in] */ Boolean daylight,
    /* [in] */ Integer style,
    /* [out] */ String* name)
{
    return GetDisplayName(daylight, style,
            Locale::GetDefault(Locale::Category::GetDISPLAY()), name);
}

ECode TimeZone::GetDisplayName(
    /* [in] */ Boolean daylightTime,
    /* [in] */ Integer style,
    /* [in] */ ILocale* locale,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    if (style != SHORT && style != LONG) {
        Logger::E("TimeZone", "Illegal style: %d", style);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Array<Array<String>> zoneStrings;
    TimeZoneNames::GetZoneStrings(locale, &zoneStrings);
    String id;
    GetID(&id);
    String result;
    TimeZoneNames::GetDisplayName(zoneStrings, id, daylightTime, style, &result);
    if (!result.IsNull()) {
        *name = result;
        return NOERROR;
    }

    // If we get here, it's because icu4c has nothing for us. Most commonly, this is in the
    // case of short names. For Pacific/Fiji, for example, icu4c has nothing better to offer
    // than "GMT+12:00". Why do we re-do this work ourselves? Because we have up-to-date
    // time zone transition data, which icu4c _doesn't_ use --- it uses its own baked-in copy,
    // which only gets updated when we update icu4c. http://b/7955614 and http://b/8026776.

    // TODO: should we generate these once, in TimeZoneNames.getDisplayName? Revisit when we
    // upgrade to icu4c 50 and rewrite the underlying native code. See also the
    // "element[j] != null" check in SimpleDateFormat.parseTimeZone, and the extra work in
    // DateFormatSymbols.getZoneStrings.
    Integer offsetMillis;
    GetRawOffset(&offsetMillis);
    if (daylightTime) {
        Integer savingTime;
        GetDSTSavings(&savingTime);
        offsetMillis += savingTime;
    }
    *name = CreateGmtOffsetString(true /* includeGmt */, true /* includeMinuteSeparator */,
            offsetMillis);
    return NOERROR;
}

String TimeZone::CreateGmtOffsetString(
    /* [in] */ Boolean includeGmt,
    /* [in] */ Boolean includeMinuteSeparator,
    /* [in] */ Integer offsetMillis)
{
    Integer offsetMinutes = offsetMillis / 60000;
    Char sign = '+';
    if (offsetMinutes < 0) {
        sign = '-';
        offsetMinutes = -offsetMinutes;
    }
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(9, IID_IStringBuilder, (IInterface**)&sb);
    if (includeGmt) {
        sb->Append(String("GMT"));
    }
    sb->AppendChar(sign);
    AppendNumber(sb, 2, offsetMinutes / 60);
    if (includeMinuteSeparator) {
        sb->AppendChar(':');
    }
    AppendNumber(sb, 2, offsetMinutes % 60);
    String offsetStr;
    sb->ToString(&offsetStr);
    return offsetStr;
}

void TimeZone::AppendNumber(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ Integer count,
    /* [in] */ Integer value)
{
    String string = StringUtils::ToString(value);
    for (Integer i = 0; i < count - string.GetLength(); i++) {
        sb->AppendChar('0');
    }
    sb->Append(string);
}

ECode TimeZone::GetDSTSavings(
    /* [out] */ Integer* savingTime)
{
    VALIDATE_NOT_NULL(savingTime);

    Boolean daylightTime;
    if (UseDaylightTime(&daylightTime), daylightTime) {
        *savingTime = 3600000;
        return NOERROR;
    }
    *savingTime = 0;
    return NOERROR;
}

ECode TimeZone::ObservesDaylightTime(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean daylightTime;
    UseDaylightTime(&daylightTime);
    if (daylightTime) {
        *result = true;
        return NOERROR;
    }

    AutoPtr<IDate> date;
    CDate::New(IID_IDate, (IInterface**)&date);
    return InDaylightTime(date, result);
}

ECode TimeZone::GetTimeZone(
    /* [in] */ const String& id,
    /* [out] */ ITimeZone** zone)
{
    VALIDATE_NOT_NULL(zone);

    AutoLock lock(GetClassLock());

    if (id.IsNull()) {
        Logger::E("TimeZone", "id == null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (id.GetLength() == 3) {
        if (id.Equals("GMT")) {
            return ICloneable::Probe(GetGMT())->Clone(
                    IID_ITimeZone, (IInterface**)zone);
        }
        if (id.Equals("UTC")) {
            return ICloneable::Probe(GetUTC())->Clone(
                    IID_ITimeZone, (IInterface**)zone);
        }
    }

    AutoPtr<IZoneInfo> zoneInfo;
    ZoneInfoDB::GetInstance()->MakeTimeZone(id, &zoneInfo);
    AutoPtr<ITimeZone> tz = ITimeZone::Probe(zoneInfo);

    if (tz == nullptr && id.GetLength() > 3 && id.StartsWith("GMT")) {
        FAIL_RETURN(GetCustomTimeZone(id, &tz));
    }

    if (tz != nullptr) {
        *zone = tz;
        REFCOUNT_ADD(*zone);
        return NOERROR;
    }
    else {
        return ICloneable::Probe(GetGMT())->Clone(
                IID_ITimeZone, (IInterface**)zone);
    }
}

ECode TimeZone::GetCustomTimeZone(
    /* [in] */ const String& id,
    /* [out] */ ITimeZone** zone)
{
    VALIDATE_NOT_NULL(zone);

    AutoPtr<IMatcher> m;
    GetCUSTOM_ZONE_ID_PATTERN()->Matcher(CoreUtils::Box(id), &m);
    Boolean matched;
    if (m->Matches(&matched), !matched) {
        *zone = nullptr;
        return NOERROR;
    }

    Integer hour;
    Integer minute = 0;
    String part;
    m->Group(1, &part);
    ECode ec = StringUtils::ParseInteger(part, &hour);
    if (ec == E_NUMBER_FORMAT_EXCEPTION) {
        return E_ASSERTION_ERROR;
    }
    m->Group(3, &part);
    if (!part.IsNull()) {
        ec = StringUtils::ParseInteger(part, &minute);
        if (ec == E_NUMBER_FORMAT_EXCEPTION) {
            return E_ASSERTION_ERROR;
        }
    }

    if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
        *zone = nullptr;
        return NOERROR;
    }

    Char sign = id.GetChar(3);
    Integer raw = (hour * 3600000) + (minute * 60000);
    if (sign == '-') {
        raw = -raw;
    }

    String cleanId = String::Format("GMT%c%02d:%02d", sign, hour, minute);

    return CSimpleTimeZone::New(raw, cleanId, IID_ITimeZone, (IInterface**)zone);
}

Array<String> TimeZone::GetAvailableIDs(
    /* [in] */ Integer rawOffset)
{
    return Array<String>::Null();
}

Array<String> TimeZone::GetAvailableIDs()
{
    return Array<String>::Null();
}

AutoPtr<ITimeZone> TimeZone::GetDefault()
{
    AutoPtr<ITimeZone> zone;
    ICloneable::Probe(GetDefaultRef())->Clone(IID_ITimeZone, (IInterface**)&zone);
    return zone;
}

AutoPtr<ITimeZone> TimeZone::GetDefaultRef()
{
    return nullptr;
}

ECode TimeZone::SetDefault(
    /* [in] */ ITimeZone* timeZone)
{
    return NOERROR;
}

ECode TimeZone::HasSameRules(
    /* [in] */ ITimeZone* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (other == nullptr) {
        *result = false;
        return NOERROR;
    }

    Integer thisOffset, otherOffset;
    GetRawOffset(&thisOffset);
    other->GetRawOffset(&otherOffset);
    if (thisOffset != otherOffset) {
        *result = false;
        return NOERROR;
    }

    Boolean thisDaylight, otherDaylight;
    UseDaylightTime(&thisDaylight);
    other->UseDaylightTime(&otherDaylight);
    *result = thisDaylight == otherDaylight;
    return NOERROR;
}

ECode TimeZone::CloneImpl(
    /* [in] */ ITimeZone* newObj)
{
    TimeZone* tz = (TimeZone*)newObj;
    tz->mID = mID;
    return NOERROR;
}

}
}
