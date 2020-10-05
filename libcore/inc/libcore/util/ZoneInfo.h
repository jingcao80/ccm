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

#ifndef __LIBCORE_UTIL_ZONEINFO_H__
#define __LIBCORE_UTIL_ZONEINFO_H__

#include "como/core/SyncObject.h"
#include "como/util/TimeZone.h"
#include "como.core.IInteger.h"
#include "como.util.ICalendar.h"
#include "como.util.IDate.h"
#include "libcore.io.IBufferIterator.h"
#include "libcore.util.IZoneInfo.h"
#include "libcore.util.IZoneInfoWallTime.h"

using como::core::IInteger;
using como::core::SyncObject;
using como::util::ICalendar;
using como::util::IDate;
using como::util::TimeZone;
using libcore::io::IBufferIterator;

namespace libcore {
namespace util {

class ZoneInfo final
    : public TimeZone
    , public IZoneInfo
{
private:
    class OffsetInterval;

public:
    class WallTime
        : public SyncObject
        , public IZoneInfoWallTime
    {
    public:
        COMO_INTERFACE_DECL();

        ECode Constructor();

        ECode Localtime(
            /* [in] */ Integer timeSeconds,
            /* [in] */ IZoneInfo* zoneInfo) override;

        ECode Mktime(
            /* [in] */ IZoneInfo* zoneInfo,
            /* [out] */ Integer* time) override;

        ECode SetYear(
            /* [in] */ Integer year) override;

        ECode SetMonth(
            /* [in] */ Integer month) override;

        ECode SetMonthDay(
            /* [in] */ Integer monthDay) override;

        ECode SetHour(
            /* [in] */ Integer hour) override;

        ECode SetMinute(
            /* [in] */ Integer minute) override;

        ECode SetSecond(
            /* [in] */ Integer second) override;

        ECode SetWeekDay(
            /* [in] */ Integer weekDay) override;

        ECode SetYearDay(
            /* [in] */ Integer yearDay) override;

        ECode SetIsDst(
            /* [in] */ Integer isDst) override;

        ECode SetGmtOffset(
            /* [in] */ Integer gmtoff) override;

        ECode GetYear(
            /* [out] */ Integer* year) override;

        ECode GetMonth(
            /* [out] */ Integer* month) override;

        ECode GetMonthDay(
            /* [out] */ Integer* monthDay) override;

        ECode GetHour(
            /* [out] */ Integer* hour) override;

        ECode GetMinute(
            /* [out] */ Integer* minute) override;

        ECode GetSecond(
            /* [out] */ Integer* second) override;

        ECode GetWeekDay(
            /* [out] */ Integer* weekDay) override;

        ECode GetYearDay(
            /* [out] */ Integer* yearDay) override;

        ECode GetGmtOffset(
            /* [out] */ Integer* gmtoff) override;

        ECode GetIsDst(
            /* [out] */ Integer* isDst) override;

    private:
        ECode TryOffsetAdjustments(
            /* [in] */ ZoneInfo* zoneInfo,
            /* [in] */ Integer oldWallTimeSeconds,
            /* [in] */ OffsetInterval* targetInterval,
            /* [in] */ Integer transitionIndex,
            /* [in] */ Integer isDstToFind,
            /* [out] */ IInteger** time);

        static Array<Integer> GetOffsetsOfType(
            /* [in] */ ZoneInfo* zoneInfo,
            /* [in] */ Integer startIndex,
            /* [in] */ Integer isDst);

        ECode DoWallTimeSearch(
            /* [in] */ ZoneInfo* zoneInfo,
            /* [in] */ Integer initialTransitionIndex,
            /* [in] */ Integer wallTimeSeconds,
            /* [in] */ Boolean mustMatchDst,
            /* [out] */ IInteger** time);

        void CopyFieldsToCalendar();

        void CopyFieldsFromCalendar();

    private:
        AutoPtr<ICalendar> mCalendar;

        Integer mYear = 0;
        Integer mMonth = 0;
        Integer mMonthDay = 0;
        Integer mHour = 0;
        Integer mMinute = 0;
        Integer mSecond = 0;
        Integer mWeekDay = 0;
        Integer mYearDay = 0;
        Integer mIsDst = 0;
        Integer mGmtOffsetSeconds = 0;
    };

private:
    class OffsetInterval
        : public Object
    {
    public:
        static ECode Create(
            /* [in] */ ZoneInfo* timeZone,
            /* [in] */ Integer transitionIndex,
            /* [out] */ OffsetInterval** offsetInterval);

        inline Boolean ContainsWallTime(
            /* [in] */ Long wallTimeSeconds)
        {
            return wallTimeSeconds >= mStartWallTimeSeconds &&
                    wallTimeSeconds < mEndWallTimeSeconds;
        }

        inline Integer GetIsDst()
        {
            return mIsDst;
        }

        inline Integer GetTotalOffsetSeconds()
        {
            return mTotalOffsetSeconds;
        }

        inline Long GetEndWallTimeSeconds()
        {
            return mEndWallTimeSeconds;
        }

        inline Long GetStartWallTimeSeconds()
        {
            return mStartWallTimeSeconds;
        }

    private:
        inline OffsetInterval(
            /* [in] */ Integer startWallTimeSeconds,
            /* [in] */ Integer endWallTimeSeconds,
            /* [in] */ Integer isDst,
            /* [in] */ Integer totalOffsetSeconds)
            : mStartWallTimeSeconds(startWallTimeSeconds)
            , mEndWallTimeSeconds(endWallTimeSeconds)
            , mIsDst(isDst)
            , mTotalOffsetSeconds(totalOffsetSeconds)
        {}

    private:
        Integer mStartWallTimeSeconds;
        Integer mEndWallTimeSeconds;
        Integer mIsDst;
        Integer mTotalOffsetSeconds;
    };

public:
    COMO_INTERFACE_DECL();

    static ECode ReadTimeZone(
        /* [in] */ const String& id,
        /* [in] */ IBufferIterator* it,
        /* [in] */ Long currentTimeMillis,
        /* [out] */ IZoneInfo** zoneInfo);

    ECode GetOffset(
        /* [in] */ Integer era,
        /* [in] */ Integer year,
        /* [in] */ Integer month,
        /* [in] */ Integer day,
        /* [in] */ Integer dayOfWeek,
        /* [in] */ Integer millis,
        /* [out] */ Integer& offset) override;

    ECode FindTransitionIndex(
        /* [in] */ Long seconds,
        /* [out] */ Integer* index) override;

    ECode GetOffsetsByUtcTime(
        /* [in] */ Long utcTimeInMillis,
        /* [out] */ Array<Integer>& offsets,
        /* [out] */ Integer* total = nullptr) override;

    ECode GetOffset(
        /* [in] */ Long date,
        /* [out] */ Integer& offset) override;

    ECode InDaylightTime(
        /* [in] */ IDate* time,
        /* [out] */ Boolean& daylight) override;

    ECode GetRawOffset(
        /* [out] */ Integer& rawOffset) override;

    ECode SetRawOffset(
        /* [in] */ Integer rawOffset) override;

    ECode GetDSTSavings(
        /* [out] */ Integer& savingTime) override;

    ECode UseDaylightTime(
        /* [out] */ Boolean& daylight) override;

    ECode HasSameRules(
        /* [in] */ ITimeZone* other,
        /* [out] */ Boolean& result) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& same) override;

    ECode GetHashCode(
        /* [out] */ Integer& hash) override;

    ECode ToString(
        /* [out] */ String& desc) override;

    ECode Clone(
        /* [in] */ const InterfaceID& iid,
        /* [out] */ IInterface** obj) override;

private:
    ZoneInfo();

    ECode Constructor(
        /* [in] */ const String& name,
        /* [in] */ const Array<Long>& transitions,
        /* [in] */ const Array<Byte>& types,
        /* [in] */ const Array<Integer>& gmtOffsets,
        /* [in] */ const Array<Byte>& isDsts,
        /* [in] */ Long currentTimeMillis);

    Integer FindOffsetIndexForTimeInSeconds(
        /* [in] */ Long seconds);

    Integer FindOffsetIndexForTimeInMilliseconds(
        /* [in] */ Long millis);

    static Long RoundDownMillisToSeconds(
        /* [in] */ Long millis);

    static Long RoundUpMillisToSeconds(
        /* [in] */ Long millis);

    static ECode CheckedAdd(
        /* [in] */ Long a,
        /* [in] */ Integer b,
        /* [out] */ Integer* result);

    static ECode CheckedSubtract(
        /* [in] */ Integer a,
        /* [in] */ Integer b,
        /* [out] */ Integer* result);

private:
    static constexpr Long MILLISECONDS_PER_DAY = 24 * 60 * 60 * 1000;
    static constexpr Long MILLISECONDS_PER_400_YEARS =
            MILLISECONDS_PER_DAY * (400 * 365 + 100 - 3);

    static constexpr Long UNIX_OFFSET = 62167219200000LL;

    static const Array<Integer> NORMAL;

    static const Array<Integer> LEAP;

    Integer mRawOffset;
    Integer mEarliestRawOffset;

    /**
     * <p>True if the transition active at the time this instance was created, or future
     * transitions support DST. It is possible that caching this value at construction time and
     * using it for the lifetime of the instance does not match the contract of the
     * {@link TimeZone#useDaylightTime()} method but it appears to be what the RI does and that
     * method is not particularly useful when it comes to historical or future times as it does not
     * allow the time to be specified.
     *
     * <p>When this is false then {@link #mDstSavings} will be 0.
     */
    Boolean mUseDst;

    /**
     * <p>This should be final but is not because it may need to be fixed up by
     * {@link #readObject(ObjectInputStream)} to correct an inconsistency in the previous version
     * of the code whereby this was set to a non-zero value even though DST was not actually used.
     */
    Integer mDstSavings;

    /**
     * The times (in seconds) at which the offsets changes for any reason, whether that is a change
     * in the offset from UTC or a change in the DST.
     *
     * <p>These times are pre-calculated externally from a set of rules (both historical and
     * future) and stored in a file from which {@link ZoneInfo#readTimeZone(String, BufferIterator,
     * long)} reads the data. That is quite different to {@link java.util.SimpleTimeZone}, which has
     * essentially human readable rules (e.g. DST starts at 01:00 on the first Sunday in March and
     * ends at 01:00 on the last Sunday in October) that can be used to determine the DST transition
     * times across a number of years
     *
     * <p>In terms of {@link ZoneInfo tzfile} structure this array is of length {@code tzh_timecnt}
     * and contains the times in seconds converted to long to make them safer to use.
     *
     * <p>They are stored in order from earliest (lowest) time to latest (highest). A transition is
     * identified by its index within this array. A transition {@code T} is active at a specific
     * time {@code X} if {@code T} is the highest transition whose time is less than or equal to
     * {@code X}.
     */
    Array<Long> mTransitions;

    /**
     * The type of the transition, where type is a pair consisting of the offset and whether the
     * offset includes DST or not.
     *
     * <p>Each transition in {@link #mTransitions} has an associated type in this array at the same
     * index. The type is an index into the arrays {@link #mOffsets} and {@link #mIsDsts} that each
     * contain one part of the pair.
     *
     * <p>In the {@link ZoneInfo tzfile} structure the type array only contains unique instances of
     * the {@code struct ttinfo} to save space and each type may be referenced by multiple
     * transitions. However, the type pairs stored in this class are not guaranteed unique because
     * they do not include the {@code tt_abbrind}, which is the abbreviated identifier to use for
     * the time zone after the transition.
     */
    Array<Byte> mTypes;

    /**
     * The offset parts of the transition types, in seconds.
     *
     * <p>These are actually a delta to the {@link #mRawOffset}. So, if the offset is say +7200
     * seconds and {@link #mRawOffset} is say +3600 then this will have a value of +3600.
     *
     * <p>The offset in milliseconds can be computed using:
     * {@code mRawOffset + mOffsets[type] * 1000}
     */
    Array<Integer> mOffsets;

    /**
     * Specifies whether an associated offset includes DST or not.
     *
     * <p>Each entry in here is 1 if the offset at the same index in {@link #mOffsets} includes DST
     * and 0 otherwise.
     */
    Array<Byte> mIsDsts;

    static constexpr Integer E_CHECKED_ARITHMETIC_EXCEPTION = 0x80011af0;
};

inline ZoneInfo::ZoneInfo()
{}

}
}

#endif // __LIBCORE_UTIL_ZONEINFO_H__
