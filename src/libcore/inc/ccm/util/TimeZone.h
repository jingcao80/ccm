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

#ifndef __CCM_UTIL_TIMEZONE_H__
#define __CCM_UTIL_TIMEZONE_H__

#include "ccm/core/SyncObject.h"
#include "ccm/core/volatile.h"
#include "ccm.core.ICloneable.h"
#include "ccm.core.IStringBuilder.h"
#include "ccm.io.ISerializable.h"
#include "ccm.util.ITimeZone.h"
#include "ccm.util.regex.IPattern.h"
#include <ccmautoptr.h>

using ccm::core::ICloneable;
using ccm::core::IStringBuilder;
using ccm::core::SyncObject;
using ccm::io::ISerializable;
using ccm::util::regex::IPattern;

namespace ccm {
namespace util {

class TimeZone
    : public SyncObject
    , public ITimeZone
    , public ISerializable
    , public ICloneable
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor();

    ECode GetOffset(
        /* [in] */ Long date,
        /* [out] */ Integer* offset);

    Integer GetOffsets(
        /* [in] */ Long date,
        /* [out] */ Array<Integer>& offsets);

    ECode GetID(
        /* [out] */ String* id);

    ECode SetID(
        /* [in] */ const String& ID);

    ECode GetDisplayName(
        /* [out] */ String* name) override final;

    ECode GetDisplayName(
        /* [in] */ ILocale* locale,
        /* [out] */ String* name) override final;

    ECode GetDisplayName(
        /* [in] */ Boolean daylight,
        /* [in] */ Integer style,
        /* [out] */ String* name) override final;

    ECode GetDisplayName(
        /* [in] */ Boolean daylightTime,
        /* [in] */ Integer style,
        /* [in] */ ILocale* locale,
        /* [out] */ String* name) override;

    static String CreateGmtOffsetString(
        /* [in] */ Boolean includeGmt,
        /* [in] */ Boolean includeMinuteSeparator,
        /* [in] */ Integer offsetMillis);

    ECode GetDSTSavings(
        /* [out] */ Integer* savingTime) override;

    ECode ObservesDaylightTime(
        /* [out] */ Boolean* result) override;

    static ECode GetTimeZone(
        /* [in] */ const String& id,
        /* [out] */ ITimeZone** zone);

    static ECode GetCustomTimeZone(
        /* [in] */ const String& id,
        /* [out] */ ITimeZone** zone);

    static Array<String> GetAvailableIDs(
        /* [in] */ Integer rawOffset);

    static Array<String> GetAvailableIDs();

    static AutoPtr<ITimeZone> GetDefault();

    static AutoPtr<ITimeZone> GetDefaultRef();

    static ECode SetDefault(
        /* [in] */ ITimeZone* timeZone);

    ECode HasSameRules(
        /* [in] */ ITimeZone* other,
        /* [out] */ Boolean* result) override;

    static TimeZone* From(
        /* [in] */ ITimeZone* zone);

protected:
    ECode CloneImpl(
        /* [in] */ ITimeZone* newObj);

private:
    static SyncObject& GetClassLock();

    static void AppendNumber(
        /* [in] */ IStringBuilder* sb,
        /* [in] */ Integer count,
        /* [in] */ Integer value);

    static AutoPtr<IPattern> GetCUSTOM_ZONE_ID_PATTERN();

    static AutoPtr<ITimeZone> GetGMT();

    static AutoPtr<ITimeZone> GetUTC();

public:
    static AutoPtr<ITimeZone> NO_TIMEZONE;

private:
    String mID;
    static VOLATILE AutoPtr<ITimeZone> sDefaultTimeZone;
};

inline ECode TimeZone::Constructor()
{
    return NOERROR;
}

inline TimeZone* TimeZone::From(
    /* [in] */ ITimeZone* zone)
{
    return (TimeZone*)zone;
}

}
}

#endif // __CCM_UTIL_TIMEZONE_H__
