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
#include "como/core/CStringBuilder.h"
#include "como/core/StringUtils.h"
#include "como/core/System.h"
#include "como/util/CDate.h"
#include "como/util/CPropertyPermission.h"
#include "como/util/CSimpleTimeZone.h"
#include "como/util/Locale.h"
#include "como/util/TimeZone.h"
#include "como/util/TimeZoneGetter.h"
#include "como/util/regex/Pattern.h"
#include "como.core.ICloneable.h"
#include "como.security.IPermission.h"
#include "como.util.regex.IMatcher.h"
#include "libcore/icu/TimeZoneNames.h"
#include "libcore/io/IoUtils.h"
#include "libcore/util/ZoneInfoDB.h"
#include "libcore.util.IZoneInfo.h"
#include <comolog.h>

using como::core::AutoLock;
using como::core::CoreUtils;
using como::core::CStringBuilder;
using como::core::E_ASSERTION_ERROR;
using como::core::E_NUMBER_FORMAT_EXCEPTION;
using como::core::ICloneable;
using como::core::ISecurityManager;
using como::core::IStringBuilder;
using como::core::IID_ICloneable;
using como::core::IID_IStringBuilder;
using como::core::StringUtils;
using como::core::System;
using como::io::IID_ISerializable;
using como::security::IID_IPermission;
using como::security::IPermission;
using como::util::regex::IMatcher;
using como::util::regex::Pattern;
using libcore::icu::TimeZoneNames;
using libcore::io::IoUtils;
using libcore::util::IZoneInfo;
using libcore::util::ZoneInfoDB;

namespace como {
namespace util {

VOLATILE AutoPtr<ITimeZone> TimeZone::sDefaultTimeZone;

COMO_INTERFACE_IMPL_3(TimeZone, SyncObject, ITimeZone, ISerializable, ICloneable);

static AutoPtr<IPattern> CreateCUSTOM_ZONE_ID_PATTERN()
{
    AutoPtr<IPattern> pattern;
    Pattern::Compile("^GMT[-+](\\d{1,2})(:?(\\d\\d))?$", pattern);
    return pattern;
}

static AutoPtr<ITimeZone> CreateGMT()
{
    AutoPtr<ITimeZone> zone;
    CSimpleTimeZone::New(0, "GMT", IID_ITimeZone, (IInterface**)&zone);
    return zone;
}

static AutoPtr<ITimeZone> CreateUTC()
{
    AutoPtr<ITimeZone> zone;
    CSimpleTimeZone::New(0, "UTC", IID_ITimeZone, (IInterface**)&zone);
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
    /* [out] */ Integer& offset)
{
    AutoPtr<IDate> dateObj;
    CDate::New(date, IID_IDate, (IInterface**)&dateObj);
    Boolean daylightTime;
    if (InDaylightTime(dateObj, daylightTime), daylightTime) {
        Integer ro, st;
        GetRawOffset(ro);
        GetDSTSavings(st);
        offset = ro + st;
        return NOERROR;
    }
    return GetRawOffset(offset);
}

Integer TimeZone::GetOffsets(
    /* [in] */ Long date,
    /* [out] */ Array<Integer>& offsets)
{
    Integer rawoffset;
    GetRawOffset(rawoffset);
    Integer dstoffset = 0;
    AutoPtr<IDate> dateObj;
    CDate::New(date, IID_IDate, (IInterface**)&dateObj);
    Boolean daylightTime;
    if (InDaylightTime(dateObj, daylightTime), daylightTime) {
        GetDSTSavings(dstoffset);
    }
    if (!offsets.IsNull()) {
        offsets[0] = rawoffset;
        offsets[1] = dstoffset;
    }
    return rawoffset + dstoffset;
}

ECode TimeZone::GetID(
    /* [out] */ String& id)
{
    id = mID;
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
    /* [out] */ String& name)
{
    return GetDisplayName(false, LONG,
            Locale::GetDefault(Locale::Category::GetDISPLAY()), name);
}

ECode TimeZone::GetDisplayName(
    /* [in] */ ILocale* locale,
    /* [out] */ String& name)
{
    return GetDisplayName(false, LONG, locale, name);
}

ECode TimeZone::GetDisplayName(
    /* [in] */ Boolean daylight,
    /* [in] */ Integer style,
    /* [out] */ String& name)
{
    return GetDisplayName(daylight, style,
            Locale::GetDefault(Locale::Category::GetDISPLAY()), name);
}

ECode TimeZone::GetDisplayName(
    /* [in] */ Boolean daylightTime,
    /* [in] */ Integer style,
    /* [in] */ ILocale* locale,
    /* [out] */ String& name)
{
    if (style != SHORT && style != LONG) {
        Logger::E("TimeZone", "Illegal style: %d", style);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Array<Array<String>> zoneStrings;
    TimeZoneNames::GetZoneStrings(locale, &zoneStrings);
    String id;
    GetID(id);
    String result;
    TimeZoneNames::GetDisplayName(zoneStrings, id, daylightTime, style, &result);
    if (!result.IsNull()) {
        name = result;
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
    GetRawOffset(offsetMillis);
    if (daylightTime) {
        Integer savingTime;
        GetDSTSavings(savingTime);
        offsetMillis += savingTime;
    }
    name = CreateGmtOffsetString(true /* includeGmt */, true /* includeMinuteSeparator */,
            offsetMillis);
    return NOERROR;
}

String TimeZone::CreateGmtOffsetString(
    /* [in] */ Boolean includeGmt,
    /* [in] */ Boolean includeMinuteSeparator,
    /* [in] */ Integer offsetMillis)
{
    Integer offsetMinutes = offsetMillis / 60000;
    Char sign = U'+';
    if (offsetMinutes < 0) {
        sign = U'-';
        offsetMinutes = -offsetMinutes;
    }
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(9, IID_IStringBuilder, (IInterface**)&sb);
    if (includeGmt) {
        sb->Append(String("GMT"));
    }
    sb->Append(sign);
    AppendNumber(sb, 2, offsetMinutes / 60);
    if (includeMinuteSeparator) {
        sb->Append(U':');
    }
    AppendNumber(sb, 2, offsetMinutes % 60);
    String offsetStr;
    sb->ToString(offsetStr);
    return offsetStr;
}

void TimeZone::AppendNumber(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ Integer count,
    /* [in] */ Integer value)
{
    String string = StringUtils::ToString(value);
    for (Integer i = 0; i < count - string.GetLength(); i++) {
        sb->Append(U'0');
    }
    sb->Append(string);
}

ECode TimeZone::GetDSTSavings(
    /* [out] */ Integer& savingTime)
{
    Boolean daylightTime;
    if (UseDaylightTime(daylightTime), daylightTime) {
        savingTime = 3600000;
        return NOERROR;
    }
    savingTime = 0;
    return NOERROR;
}

ECode TimeZone::ObservesDaylightTime(
    /* [out] */ Boolean& result)
{
    Boolean daylightTime;
    UseDaylightTime(daylightTime);
    if (daylightTime) {
        result = true;
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
        tz.MoveTo(zone);
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
    GetCUSTOM_ZONE_ID_PATTERN()->Matcher(CoreUtils::Box(id), m);
    Boolean matched;
    if (m->Matches(matched), !matched) {
        *zone = nullptr;
        return NOERROR;
    }

    Integer hour;
    Integer minute = 0;
    String part;
    m->Group(1, part);
    ECode ec = StringUtils::ParseInteger(part, hour);
    if (ec == E_NUMBER_FORMAT_EXCEPTION) {
        return E_ASSERTION_ERROR;
    }
    m->Group(3, part);
    if (!part.IsNull()) {
        ec = StringUtils::ParseInteger(part, minute);
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
    if (sign == U'-') {
        raw = -raw;
    }

    String cleanId = String::Format("GMT%c%02d:%02d", sign, hour, minute);

    return CSimpleTimeZone::New(raw, cleanId, IID_ITimeZone, (IInterface**)zone);
}

Array<String> TimeZone::GetAvailableIDs(
    /* [in] */ Integer rawOffset)
{
    AutoLock lock(GetClassLock());

    Array<String> ids;
    ZoneInfoDB::GetInstance()->GetAvailableIDs(rawOffset, &ids);
    return ids;
}

Array<String> TimeZone::GetAvailableIDs()
{
    AutoLock lock(GetClassLock());

    Array<String> ids;
    ZoneInfoDB::GetInstance()->GetAvailableIDs(&ids);
    return ids;
}

AutoPtr<ITimeZone> TimeZone::GetDefault()
{
    AutoPtr<ITimeZone> zone;
    ICloneable::Probe(GetDefaultRef())->Clone(IID_ITimeZone, (IInterface**)&zone);
    return zone;
}

AutoPtr<ITimeZone> TimeZone::GetDefaultRef()
{
    VOLATILE_GET(AutoPtr<ITimeZone> tz, sDefaultTimeZone);
    if (tz == nullptr) {
        AutoPtr<ITimeZoneGetter> tzGetter = TimeZoneGetter::GetInstance();
        String zoneName;
        if (tzGetter != nullptr) {
            tzGetter->GetId(zoneName);
        }
        if (!zoneName.IsNull()) {
            zoneName = zoneName.Trim();
        }
        if (zoneName.IsEmpty()) {
            ECode ec = IoUtils::ReadFileAsString(String("/etc/timezone"), &zoneName);
            if (FAILED(ec)) {
                zoneName = "GMT";
            }
        }
        TimeZone::GetTimeZone(zoneName, (ITimeZone**)&tz);
        VOLATILE_SET(sDefaultTimeZone, tz);
    }
    return tz;
}

ECode TimeZone::SetDefault(
    /* [in] */ ITimeZone* timeZone)
{
    AutoLock lock(GetClassLock());

    AutoPtr<ISecurityManager> sm = System::GetSecurityManager();
    if (sm != nullptr) {
        AutoPtr<IPermission> perm;
        CPropertyPermission::New(String("user.timeZone"), String("write"),
                IID_IPermission, (IInterface**)&perm);
        FAIL_RETURN(sm->CheckPermission(perm));
    }

    sDefaultTimeZone = timeZone != nullptr ?
            (ITimeZone*)CoreUtils::Clone(timeZone, IID_ITimeZone).Get() : nullptr;
    return NOERROR;
}

ECode TimeZone::HasSameRules(
    /* [in] */ ITimeZone* other,
    /* [out] */ Boolean& result)
{
    if (other == nullptr) {
        result = false;
        return NOERROR;
    }

    Integer thisOffset, otherOffset;
    GetRawOffset(thisOffset);
    other->GetRawOffset(otherOffset);
    if (thisOffset != otherOffset) {
        result = false;
        return NOERROR;
    }

    Boolean thisDaylight, otherDaylight;
    UseDaylightTime(thisDaylight);
    other->UseDaylightTime(otherDaylight);
    result = thisDaylight == otherDaylight;
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
