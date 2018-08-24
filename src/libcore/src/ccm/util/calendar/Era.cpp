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

#include "ccm/core/CStringBuilder.h"
#include "ccm/util/calendar/CalendarSystem.h"
#include "ccm/util/calendar/Era.h"
#include "ccm/util/calendar/ImmutableGregorianDate.h"

using ccm::core::CStringBuilder;
using ccm::core::IStringBuilder;
using ccm::core::IID_IStringBuilder;

namespace ccm {
namespace util {
namespace calendar {

CCM_INTERFACE_IMPL_1(Era, SyncObject, IEra);

ECode Era::Constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& abbr,
    /* [in] */ Long since,
    /* [in] */ Boolean localTime)
{
    mName = name;
    mAbbr = abbr;
    mSince = since;
    mLocalTime = localTime;
    AutoPtr<IGregorian> gcal = CalendarSystem::GetGregorianCalendar();
    AutoPtr<ICalendarDate> d;
    ICalendarSystem::Probe(gcal)->NewCalendarDate(nullptr, &d);
    ICalendarSystem::Probe(gcal)->GetCalendarDate(since, d);
    AutoPtr<ImmutableGregorianDate> date = new ImmutableGregorianDate();
    ECode ec = date->Constructor(IBaseCalendarDate::Probe(d));
    if (FAILED(ec)) return ec;
    mSinceDate = (ICalendarDate*)date.Get();
    return NOERROR;
}

ECode Era::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mName;
    return NOERROR;
}

ECode Era::GetDisplayName(
    /* [in] */ ILocale* locale,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mName;
    return NOERROR;
}

ECode Era::GetAbbreviation(
    /* [out] */ String* abbr)
{
    VALIDATE_NOT_NULL(abbr);

    *abbr = mAbbr;
    return NOERROR;
}

ECode Era::GetDiaplayAbbreviation(
    /* [in] */ ILocale* locale,
    /* [out] */ String* abbr)
{
    VALIDATE_NOT_NULL(abbr);

    *abbr = mAbbr;
    return NOERROR;
}

ECode Era::GetSince(
    /* [in] */ ITimeZone* zone,
    /* [out] */ Long* time)
{
    VALIDATE_NOT_NULL(time);

    if (zone == nullptr || !mLocalTime) {
        *time = mSince;
        return NOERROR;
    }
    Integer offset;
    zone->GetOffset(mSince, &offset);
    *time = mSince - offset;
    return NOERROR;
}

ECode Era::GetSinceDate(
    /* [out] */ ICalendarDate** date)
{
    VALIDATE_NOT_NULL(date);

    *date = mSinceDate;
    REFCOUNT_ADD(*date);
    return NOERROR;
}

ECode Era::IsLocalTime(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mLocalTime;
    return NOERROR;
}

ECode Era::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    if (IEra::Probe(obj) == nullptr) {
        *same = false;
        return NOERROR;
    }
    Era* that = (Era*)IEra::Probe(obj);
    *same = mName.Equals(that->mName) &&
            mAbbr.Equals(that->mAbbr) &&
            mSince == that->mSince &&
            mLocalTime == that->mLocalTime;
    return NOERROR;
}

ECode Era::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    if (mHash == 0) {
        mHash = mName.GetHashCode() ^ mAbbr.GetHashCode() ^ (Integer)mSince ^ (Integer)(mSince >> 32)
                ^ (mLocalTime ? 1 : 0);
    }
    *hash = mHash;
    return NOERROR;
}

ECode Era::ToString(
    /* [out] */ String* desc)
{
    VALIDATE_NOT_NULL(desc);

    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    sb->AppendChar('[');
    String name, abbr;
    GetName(&name);
    sb->Append(name);
    sb->Append(String(" ("));
    GetAbbreviation(&abbr);
    sb->Append(abbr);
    sb->AppendChar(')');
    sb->Append(String(" since "));
    AutoPtr<ICalendarDate> sinceDate;
    GetSinceDate(&sinceDate);
    sb->Append(Object::ToString(sinceDate));
    if (mLocalTime) {
        Integer length;
        sb->GetLength(&length);
        sb->SetLength(length - 1); // remove 'Z'
        sb->Append(String(" local time"));
    }
    sb->AppendChar(']');
    return sb->ToString(desc);
}

}
}
}
