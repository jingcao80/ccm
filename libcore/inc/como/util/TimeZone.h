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

#ifndef __COMO_UTIL_TIMEZONE_H__
#define __COMO_UTIL_TIMEZONE_H__

#include "como/core/SyncObject.h"
#include "como/core/volatile.h"
#include "como.core.ICloneable.h"
#include "como.core.IStringBuilder.h"
#include "como.io.ISerializable.h"
#include "como.util.ITimeZone.h"
#include "como.util.regex.IPattern.h"
#include <comosp.h>

using como::core::ICloneable;
using como::core::IStringBuilder;
using como::core::SyncObject;
using como::io::ISerializable;
using como::util::regex::IPattern;

namespace como {
namespace util {

class TimeZone
    : public SyncObject
    , public ITimeZone
    , public ISerializable
    , public ICloneable
{
public:
    COMO_INTERFACE_DECL();

    ECode Constructor();

    ECode GetOffset(
        /* [in] */ Long date,
        /* [out] */ Integer& offset) override;

    Integer GetOffsets(
        /* [in] */ Long date,
        /* [out] */ Array<Integer>& offsets);

    ECode GetID(
        /* [out] */ String& id) override;

    ECode SetID(
        /* [in] */ const String& ID) override;

    ECode GetDisplayName(
        /* [out] */ String& name) override final;

    ECode GetDisplayName(
        /* [in] */ ILocale* locale,
        /* [out] */ String& name) override final;

    ECode GetDisplayName(
        /* [in] */ Boolean daylight,
        /* [in] */ Integer style,
        /* [out] */ String& name) override final;

    ECode GetDisplayName(
        /* [in] */ Boolean daylightTime,
        /* [in] */ Integer style,
        /* [in] */ ILocale* locale,
        /* [out] */ String& name) override;

    static String CreateGmtOffsetString(
        /* [in] */ Boolean includeGmt,
        /* [in] */ Boolean includeMinuteSeparator,
        /* [in] */ Integer offsetMillis);

    ECode GetDSTSavings(
        /* [out] */ Integer& savingTime) override;

    ECode ObservesDaylightTime(
        /* [out] */ Boolean& result) override;

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
        /* [out] */ Boolean& result) override;

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

#endif // __COMO_UTIL_TIMEZONE_H__
