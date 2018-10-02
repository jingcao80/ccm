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
#include "ccm/core/CStringBuilder.h"
#include "ccm/core/StringUtils.h"
#include "ccm/util/CDate.h"
#include "ccm/util/Locale.h"
#include "ccm/util/TimeZone.h"
#include "ccm.core.ICloneable.h"
#include <ccmlogger.h>

using ccm::core::AutoLock;
using ccm::core::CStringBuilder;
using ccm::core::ICloneable;
using ccm::core::IStringBuilder;
using ccm::core::IID_ICloneable;
using ccm::core::IID_IStringBuilder;
using ccm::core::StringUtils;
using ccm::io::IID_ISerializable;

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_3(TimeZone, SyncObject, ITimeZone, ISerializable, ICloneable);

AutoPtr<ITimeZone> TimeZone::NO_TIMEZONE;

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

        }
        if (id.Equals("UTC")) {

        }
    }
}

ECode TimeZone::GetCustomTimeZone(
    /* [in] */ const String& id,
    /* [out] */ ITimeZone** zone)
{
    return NOERROR;
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
