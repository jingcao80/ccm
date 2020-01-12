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

#include "como/core/CInteger.h"
#include "como/core/Math.h"
#include "como/util/Arrays.h"
#include "como/util/CGregorianCalendar.h"
#include "libcore/util/ZoneInfo.h"
#include <comolog.h>

using como::core::CInteger;
using como::core::E_ILLEGAL_STATE_EXCEPTION;
using como::core::IID_IInteger;
using como::core::Math;
using como::io::E_IO_EXCEPTION;
using como::util::Arrays;
using como::util::CGregorianCalendar;
using como::util::IID_ICalendar;

namespace libcore {
namespace util {

const Array<Integer> ZoneInfo::NORMAL{
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334,
};

const Array<Integer> ZoneInfo::LEAP{
    0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335,
};

COMO_INTERFACE_IMPL_1(ZoneInfo, TimeZone, IZoneInfo);

ECode ZoneInfo::ReadTimeZone(
    /* [in] */ const String& id,
    /* [in] */ IBufferIterator* it,
    /* [in] */ Long currentTimeMillis,
    /* [out] */ IZoneInfo** zoneInfo)
{
    // Variable names beginning tzh_ correspond to those in "tzfile.h".

    // Check tzh_magic.
    Integer tzh_magic;
    it->ReadInteger(&tzh_magic);
    if (tzh_magic != 0x545a6966) {
        // "TZif"
        Logger::E("ZoneInfo", "Timezone id=%s has an invalid header=0x%08x", id.string(), tzh_magic);
        return E_IO_EXCEPTION;
    }

    // Skip the uninteresting part of the header.
    it->Skip(28);

    // Read the sizes of the arrays we're about to read.
    Integer tzh_timecnt;
    it->ReadInteger(&tzh_timecnt);
    // Arbitrary ceiling to prevent allocating memory for corrupt data.
    // 2 per year with 2^32 seconds would give ~272 transitions.
    constexpr Integer MAX_TRANSITIONS = 2000;
    if (tzh_timecnt < 0 || tzh_timecnt > MAX_TRANSITIONS) {
        Logger::E("ZoneInfo", "Timezone id=%s has an invalid number of transitions=%d",
                id.string(), tzh_timecnt);
        return E_IO_EXCEPTION;
    }

    Integer tzh_typecnt;
    it->ReadInteger(&tzh_typecnt);
    constexpr Integer MAX_TYPES = 256;
    if (tzh_typecnt < 1) {
        Logger::E("ZoneInfo", "ZoneInfo requires at least one type to be provided"
                " for each timezone but could not find one for '%s'", id.string());
        return E_IO_EXCEPTION;
    }
    else if (tzh_typecnt > MAX_TYPES) {
        Logger::E("ZoneInfo", "Timezone with id %s  has too many types=%d",
                id.string(), tzh_typecnt);
        return E_IO_EXCEPTION;
    }

    // Skip tzh_charcnt.
    it->Skip(4);

    // Transitions are signed 32 bit integers, but we store them as signed 64 bit
    // integers since it's easier to compare them against 64 bit inputs (see getOffset
    // and isDaylightTime) with much less risk of an overflow in our calculations.
    //
    // The alternative of checking the input against the first and last transition in
    // the array is far more awkward and error prone.
    Array<Integer> transitions32(tzh_timecnt);
    it->ReadIntegerArray(transitions32, 0, transitions32.GetLength());

    Array<Long> transitions64(tzh_timecnt);
    for (Integer i = 0; i < tzh_timecnt; ++i) {
        transitions64[i] = transitions32[i];
        if (i > 0 && transitions64[i] <= transitions64[i - 1]) {
            Logger::E("ZoneInfo", "%s transition at %i is not sorted correctly, is %lld, previous is %lld",
                    id.string(), i, transitions64[i], transitions64[i - 1]);
            return E_IO_EXCEPTION;
        }
    }

    Array<Byte> type(tzh_timecnt);
    it->ReadByteArray(type, 0, type.GetLength());
    for (Integer i = 0; i < type.GetLength(); i++) {
        Integer typeIndex = type[i] & 0xff;
        if (typeIndex >= tzh_typecnt) {
            Logger::E("ZoneInfo", "%s type at %d is not < %d, is %d", id.string(), i, tzh_typecnt, typeIndex);
            return E_IO_EXCEPTION;
        }
    }

    Array<Integer> gmtOffsets(tzh_typecnt);
    Array<Byte> isDsts(tzh_typecnt);
    for (Integer i = 0; i < tzh_typecnt; ++i) {
        Integer offset;
        it->ReadInteger(&offset);
        gmtOffsets[i] = offset;
        Byte isDst;
        it->ReadByte(&isDst);
        if (isDst != 0 && isDst != 1) {
            Logger::E("ZoneInfo", "%s dst at %d is not 0 or 1, is %d", id.string(), i, isDst);
            return E_IO_EXCEPTION;
        }
        isDsts[i] = isDst;
        // We skip the abbreviation index. This would let us provide historically-accurate
        // time zone abbreviations (such as "AHST", "YST", and "AKST" for standard time in
        // America/Anchorage in 1982, 1983, and 1984 respectively). ICU only knows the current
        // names, though, so even if we did use this data to provide the correct abbreviations
        // for en_US, we wouldn't be able to provide correct abbreviations for other locales,
        // nor would we be able to provide correct long forms (such as "Yukon Standard Time")
        // for any locale. (The RI doesn't do any better than us here either.)
        it->Skip(1);
    }

    AutoPtr<ZoneInfo> zone = new ZoneInfo();
    FAIL_RETURN(zone->Constructor(id, transitions64, type, gmtOffsets, isDsts, currentTimeMillis))
    *zoneInfo = (IZoneInfo*)zone.Get();
    REFCOUNT_ADD(*zoneInfo);
    return NOERROR;
}

ECode ZoneInfo::Constructor(
    /* [in] */ const String& name,
    /* [in] */ const Array<Long>& transitions,
    /* [in] */ const Array<Byte>& types,
    /* [in] */ const Array<Integer>& gmtOffsets,
    /* [in] */ const Array<Byte>& isDsts,
    /* [in] */ Long currentTimeMillis)
{
    if (gmtOffsets.GetLength() == 0) {
        Logger::E("ZoneInfo", "ZoneInfo requires at least one offset to be provided "
                "for each timezone but could not find one for '%s'", name.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mTransitions = transitions;
    mTypes = types;
    mIsDsts = isDsts;
    SetID(name);

    // Find the latest daylight and standard offsets (if any).
    Integer lastStd = -1;
    Integer lastDst = -1;
    for (Integer i = mTransitions.GetLength() - 1; (lastStd == -1 || lastDst == -1) && i >= 0; --i) {
        Integer type = mTypes[i] & 0xff;
        if (lastStd == -1 && mIsDsts[type] == 0) {
            lastStd = i;
        }
        if (lastDst == -1 && mIsDsts[type] != 0) {
            lastDst = i;
        }
    }

    // Use the latest non-daylight offset (if any) as the raw offset.
    if (mTransitions.GetLength() == 0) {
        // If there are no transitions then use the first GMT offset.
        mRawOffset = gmtOffsets[0];
    }
    else {
        if (lastStd == -1) {
            Logger::E("ZoneInfo", "ZoneInfo requires at least one non-DST "
                    "transition to be provided for each timezone that has at least one "
                    "transition but could not find one for '%s'", name.string());
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        mRawOffset = gmtOffsets[mTypes[lastStd] & 0xff];
    }

    if (lastDst != -1) {
        // Check to see if the last DST transition is in the future or the past. If it is in
        // the past then we treat it as if it doesn't exist, at least for the purposes of
        // setting mDstSavings and mUseDst.
        Long lastDSTTransitionTime = mTransitions[lastDst];

        // Convert the current time in millis into seconds. Unlike other places that convert
        // time in milliseconds into seconds in order to compare with transition time this
        // rounds up rather than down. It does that because this is interested in what
        // transitions apply in future
        Long currentUnixTimeSeconds = RoundUpMillisToSeconds(currentTimeMillis);

        // Is this zone observing DST currently or in the future?
        // We don't care if they've historically used it: most places have at least once.
        // See http://code.google.com/p/android/issues/detail?id=877.
        // This test means that for somewhere like Morocco, which tried DST in 2009 but has
        // no future plans (and thus no future schedule info) will report "true" from
        // useDaylightTime at the start of 2009 but "false" at the end. This seems appropriate.
        if (lastDSTTransitionTime < currentUnixTimeSeconds) {
            // The last DST transition is before now so treat it as if it doesn't exist.
            lastDst = -1;
        }
    }

    if (lastDst == -1) {
        // There were no DST transitions or at least no future DST transitions so DST is not
        // used.
        mDstSavings = 0;
        mUseDst = false;
    }
    else {
        // Use the latest transition's pair of offsets to compute the DST savings.
        // This isn't generally useful, but it's exposed by TimeZone.getDSTSavings.
        Integer lastGmtOffset = gmtOffsets[mTypes[lastStd] & 0xff];
        Integer lastDstOffset = gmtOffsets[mTypes[lastDst] & 0xff];
        mDstSavings = Math::Abs(lastGmtOffset - lastDstOffset) * 1000;
        mUseDst = true;
    }

    // Cache the oldest known raw offset, in case we're asked about times that predate our
    // transition data.
    Integer firstStd = -1;
    for (Integer i = 0; i < mTransitions.GetLength(); ++i) {
        if (mIsDsts[mTypes[i] & 0xff] == 0) {
            firstStd = i;
            break;
        }
    }
    Integer earliestRawOffset = (firstStd != -1) ? gmtOffsets[mTypes[firstStd] & 0xff] : mRawOffset;

    // Rather than keep offsets from UTC, we use offsets from local time, so the raw offset
    // can be changed and automatically affect all the offsets.
    mOffsets = gmtOffsets;
    for (Integer i = 0; i < mOffsets.GetLength(); i++) {
        mOffsets[i] -= mRawOffset;
    }

    mRawOffset *= 1000;
    mEarliestRawOffset = earliestRawOffset * 1000;

    return NOERROR;
}

ECode ZoneInfo::GetOffset(
    /* [in] */ Integer era,
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer day,
    /* [in] */ Integer dayOfWeek,
    /* [in] */ Integer millis,
    /* [out] */ Integer* offset)
{
    VALIDATE_NOT_NULL(offset);

    Long calc = (year / 400) * MILLISECONDS_PER_400_YEARS;
    year %= 400;

    calc += year * (365 * MILLISECONDS_PER_DAY);
    calc += ((year + 3) / 4) * MILLISECONDS_PER_DAY;

    if (year > 0) {
        calc -= ((year - 1) / 100) * MILLISECONDS_PER_DAY;
    }

    Boolean isLeap = (year == 0 || (year % 4 == 0 && year % 100 != 0));
    Array<Integer> mlen = isLeap ? LEAP : NORMAL;

    calc += mlen[month] * MILLISECONDS_PER_DAY;
    calc += (day - 1) * MILLISECONDS_PER_DAY;
    calc += millis;

    calc -= mRawOffset;
    calc -= UNIX_OFFSET;

    return GetOffset(calc, offset);
}

ECode ZoneInfo::FindTransitionIndex(
    /* [in] */ Long seconds,
    /* [out] */ Integer* index)
{
    VALIDATE_NOT_NULL(index);

    Integer transition = Arrays::BinarySearch(mTransitions, seconds);
    if (transition < 0) {
        transition = ~transition - 1;
        if (transition < 0) {
            *index = -1;
            return NOERROR;
        }
    }

    *index = transition;
    return NOERROR;
}

Integer ZoneInfo::FindOffsetIndexForTimeInSeconds(
    /* [in] */ Long seconds)
{
    Integer transition;
    FindTransitionIndex(seconds, &transition);
    if (transition < 0) {
        return -1;
    }

    return mTypes[transition] & 0xff;
}

Integer ZoneInfo::FindOffsetIndexForTimeInMilliseconds(
    /* [in] */ Long millis)
{
    // This rounds the time in milliseconds down to the time in seconds.
    //
    // It can't just divide a timestamp in millis by 1000 to obtain a transition time in
    // seconds because / (div) in Java rounds towards zero. Times before 1970 are negative and
    // if they have a millisecond component then div would result in obtaining a time that is
    // one second after what we need.
    //
    // e.g. dividing -12,001 milliseconds by 1000 would result in -12 seconds. If there was a
    //      transition at -12 seconds then that would be incorrectly treated as being active
    //      for a time of -12,001 milliseconds even though that time is before the transition
    //      should occur.
    return FindOffsetIndexForTimeInSeconds(RoundDownMillisToSeconds(millis));
}

Long ZoneInfo::RoundDownMillisToSeconds(
    /* [in] */ Long millis)
{
    if (millis < 0) {
        // If the time is less than zero then subtract 999 and then divide by 1000 rounding
        // towards 0 as usual, e.g.
        // -12345 -> -13344 / 1000 = -13
        // -12000 -> -12999 / 1000 = -12
        // -12001 -> -13000 / 1000 = -13
        return (millis - 999) / 1000;
    }
    else {
        return millis / 1000;
    }
}

Long ZoneInfo::RoundUpMillisToSeconds(
    /* [in] */ Long millis)
{
    if (millis > 0) {
        // If the time is greater than zero then add 999 and then divide by 1000 rounding
        // towards 0 as usual, e.g.
        // 12345 -> 13344 / 1000 = 13
        // 12000 -> 12999 / 1000 = 12
        // 12001 -> 13000 / 1000 = 13
        return (millis + 999) / 1000;
    }
    else {
        return millis / 1000;
    }
}

ECode ZoneInfo::GetOffsetsByUtcTime(
    /* [in] */ Long utcTimeInMillis,
    /* [out] */ Array<Integer>& offsets,
    /* [out] */ Integer* total)
{
    VALIDATE_NOT_NULL(total);

    Integer transitionIndex;
    FindTransitionIndex(RoundDownMillisToSeconds(utcTimeInMillis), &transitionIndex);
    Integer totalOffset;
    Integer rawOffset;
    Integer dstOffset;
    if (transitionIndex == -1) {
        // See getOffset(long) and inDaylightTime(Date) for an explanation as to why these
        // values are used for times before the first transition.
        rawOffset = mEarliestRawOffset;
        dstOffset = 0;
        totalOffset = rawOffset;
    }
    else {
        Integer type = mTypes[transitionIndex] & 0xff;

        // Get the total offset used for the transition.
        totalOffset = mRawOffset + mOffsets[type] * 1000;
        if (mIsDsts[type] == 0) {
            // Offset does not include DST so DST is 0 and the raw offset is the total offset.
            rawOffset = totalOffset;
            dstOffset = 0;
        }
        else {
            // Offset does include DST, we need to find the preceding transition that did not
            // include the DST offset so that we can calculate the DST offset.
            rawOffset = -1;
            for (transitionIndex -= 1; transitionIndex >= 0; --transitionIndex) {
                type = mTypes[transitionIndex] & 0xff;
                if (mIsDsts[type] == 0) {
                    rawOffset = mRawOffset + mOffsets[type] * 1000;
                    break;
                }
            }
            // If no previous transition was found then use the earliest raw offset.
            if (rawOffset == -1) {
                rawOffset = mEarliestRawOffset;
            }

            // The DST offset is the difference between the total and the raw offset.
            dstOffset = totalOffset - rawOffset;
        }
    }

    offsets[0] = rawOffset;
    offsets[1] = dstOffset;

    *total = totalOffset;
    return NOERROR;
}

ECode ZoneInfo::GetOffset(
    /* [in] */ Long date,
    /* [out] */ Integer* offset)
{
    VALIDATE_NOT_NULL(offset);

    Integer offsetIndex = FindOffsetIndexForTimeInMilliseconds(date);
    if (offsetIndex == -1) {
        // Assume that all times before our first transition correspond to the
        // oldest-known non-daylight offset. The obvious alternative would be to
        // use the current raw offset, but that seems like a greater leap of faith.
        *offset = mEarliestRawOffset;
        return NOERROR;
    }
    *offset = mRawOffset + mOffsets[offsetIndex] * 1000;
    return NOERROR;
}

ECode ZoneInfo::InDaylightTime(
    /* [in] */ IDate* time,
    /* [out] */ Boolean* daylight)
{
    VALIDATE_NOT_NULL(daylight);

    Long date;
    time->GetTime(&date);
    Integer offsetIndex = FindOffsetIndexForTimeInMilliseconds(date);
    if (offsetIndex == -1) {
        // Assume that all times before our first transition are non-daylight.
        // Transition data tends to start with a transition to daylight, so just
        // copying the first transition would assume the opposite.
        *daylight = false;
        return NOERROR;
    }
    *daylight = mIsDsts[offsetIndex] == 1;
    return NOERROR;
}

ECode ZoneInfo::GetRawOffset(
    /* [out] */ Integer* rawOffset)
{
    VALIDATE_NOT_NULL(rawOffset);

    *rawOffset = mRawOffset;
    return NOERROR;
}

ECode ZoneInfo::SetRawOffset(
    /* [in] */ Integer rawOffset)
{
    mRawOffset = rawOffset;
    return NOERROR;
}

ECode ZoneInfo::GetDSTSavings(
    /* [out] */ Integer* savingTime)
{
    VALIDATE_NOT_NULL(savingTime);

    *savingTime = mDstSavings;
    return NOERROR;
}

ECode ZoneInfo::UseDaylightTime(
    /* [out] */ Boolean* daylight)
{
    VALIDATE_NOT_NULL(daylight);

    *daylight = mUseDst;
    return NOERROR;
}

ECode ZoneInfo::HasSameRules(
    /* [in] */ ITimeZone* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    ZoneInfo* otherZone = (ZoneInfo*)IZoneInfo::Probe(other);
    if (otherZone == nullptr) {
        *result = false;
        return NOERROR;
    }
    if (mUseDst != otherZone->mUseDst) {
        *result = false;
        return NOERROR;
    }
    if (!mUseDst) {
        *result = mRawOffset == otherZone->mRawOffset;
        return NOERROR;
    }
    *result = mRawOffset == otherZone->mRawOffset
            && Arrays::Equals(mOffsets, otherZone->mOffsets)
            && Arrays::Equals(mIsDsts, otherZone->mIsDsts)
            && Arrays::Equals(mTypes, otherZone->mTypes)
            && Arrays::Equals(mTransitions, otherZone->mTransitions);
    return NOERROR;
}

ECode ZoneInfo::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& same)
{
    ZoneInfo* other = (ZoneInfo*)IZoneInfo::Probe(obj);
    if (other == nullptr) {
        same = false;
        return NOERROR;
    }
    String thisID, otherID;
    GetID(&thisID);
    other->GetID(&otherID);
    Boolean result;
    same = thisID.Equals(otherID) && (HasSameRules(other, &result), result);
    return NOERROR;
}

ECode ZoneInfo::GetHashCode(
    /* [out] */ Integer& hash)
{
    constexpr Integer prime = 31;
    hash = 1;
    String id;
    GetID(&id);
    hash = prime * hash + id.GetHashCode();
    hash = prime * hash + Arrays::GetHashCode(mOffsets);
    hash = prime * hash + Arrays::GetHashCode(mIsDsts);
    hash = prime * hash + mRawOffset;
    hash = prime * hash + Arrays::GetHashCode(mTransitions);
    hash = prime * hash + Arrays::GetHashCode(mTypes);
    hash = prime * hash + (mUseDst ? 1231 : 1237);
    return NOERROR;
}

ECode ZoneInfo::ToString(
    /* [out] */ String& desc)
{
    String id;
    GetID(&id);
    desc = String::Format("ZoneInfo[id=\"%s\",mRawOffset=%d,"
            "mEarliestRawOffset=%d,mUseDst=%s,mDstSavings=%d,"
            "transitions=%d]", id.string(), mRawOffset, mEarliestRawOffset,
            mUseDst ? "true" : "false", mDstSavings, mTransitions.GetLength());
    return NOERROR;
}

ECode ZoneInfo::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<ZoneInfo> zone = new ZoneInfo();
    FAIL_RETURN(TimeZone::CloneImpl(zone));

    zone->mRawOffset = mRawOffset;
    zone->mEarliestRawOffset = mEarliestRawOffset;
    zone->mUseDst = mUseDst;
    zone->mDstSavings = mDstSavings;
    zone->mTransitions = mTransitions;
    zone->mTypes = mTypes;
    zone->mOffsets = mOffsets;
    zone->mIsDsts = mIsDsts;

    *obj = (IZoneInfo*)zone.Get();
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode ZoneInfo::CheckedAdd(
    /* [in] */ Long a,
    /* [in] */ Integer b,
    /* [out] */ Integer* result)
{
    Long lresult = a + b;
    if (lresult != (Integer)lresult) {
        return E_CHECKED_ARITHMETIC_EXCEPTION;
    }
    *result = (Integer)lresult;
    return NOERROR;
}

ECode ZoneInfo::CheckedSubtract(
    /* [in] */ Integer a,
    /* [in] */ Integer b,
    /* [out] */ Integer* result)
{
    Long lresult = (Long)a - b;
    if (lresult != (Integer)lresult) {
        return E_CHECKED_ARITHMETIC_EXCEPTION;
    }
    *result = (Integer)lresult;
    return NOERROR;
}

//-----------------------------------------------------------------------

COMO_INTERFACE_IMPL_1(ZoneInfo::WallTime, SyncObject, IZoneInfoWallTime);

ECode ZoneInfo::WallTime::Constructor()
{
    CGregorianCalendar::New(0, 0, 0, 0, 0, 0, IID_ICalendar, (IInterface**)&mCalendar);
    AutoPtr<ITimeZone> tz;
    TimeZone::GetTimeZone(String("UTC"), &tz);
    mCalendar->SetTimeZone(tz);
    return NOERROR;
}

ECode ZoneInfo::WallTime::Localtime(
    /* [in] */ Integer timeSeconds,
    /* [in] */ IZoneInfo* zoneInfo)
{
    ZoneInfo* zoneInfoObj = (ZoneInfo*)zoneInfo;

    Integer offsetSeconds = zoneInfoObj->mRawOffset / 1000;

    // Find out the timezone DST state and adjustment.
    Byte isDst;
    if (zoneInfoObj->mTransitions.GetLength() == 0) {
        isDst = 0;
    }
    else {
        // offsetIndex can be in the range -1..zoneInfo.mOffsets.length - 1
        Integer offsetInex = zoneInfoObj->FindOffsetIndexForTimeInSeconds(timeSeconds);
        if (offsetInex == -1) {
            // -1 means timeSeconds is "before the first recorded transition". The first
            // recorded transition is treated as a transition from non-DST and the raw
            // offset.
            isDst = 0;
        }
        else {
            offsetSeconds += zoneInfoObj->mOffsets[offsetInex];
            isDst = zoneInfoObj->mIsDsts[offsetInex];
        }
    }

    // Perform arithmetic that might underflow before setting fields.
    Integer wallTimeSeconds;
    FAIL_RETURN(CheckedAdd(timeSeconds, offsetSeconds, &wallTimeSeconds));

    // Set fields.
    mCalendar->SetTimeInMillis(wallTimeSeconds * 1000LL);
    CopyFieldsFromCalendar();
    mIsDst = isDst;
    mGmtOffsetSeconds = offsetSeconds;
    return NOERROR;
}

ECode ZoneInfo::WallTime::Mktime(
    /* [in] */ IZoneInfo* zoneInfo,
    /* [out] */ Integer* time)
{
    VALIDATE_NOT_NULL(time);

    ZoneInfo* zoneInfoObj = (ZoneInfo*)zoneInfo;

    // Normalize isDst to -1, 0 or 1 to simplify isDst equality checks below.
    mIsDst = mIsDst > 0 ? mIsDst = 1 : mIsDst < 0 ? mIsDst = -1 : 0;

    CopyFieldsToCalendar();
    Long timeInMillies;
    mCalendar->GetTimeInMillis(&timeInMillies);
    Long longWallTimeSeconds = timeInMillies / 1000;
    if (IInteger::MIN_VALUE > longWallTimeSeconds
            || longWallTimeSeconds > IInteger::MAX_VALUE) {
        // For compatibility with the old native 32-bit implementation we must treat
        // this as an error. Note: -1 could be confused with a real time.
        *time = -1;
        return NOERROR;
    }

    Integer wallTimeSeconds = longWallTimeSeconds;
    Integer rawOffsetSeconds = zoneInfoObj->mRawOffset / 1000;
    Integer rawTimeSeconds;
    ECode ec = CheckedSubtract(wallTimeSeconds, rawOffsetSeconds, &rawTimeSeconds);
    if (FAILED(ec)) {
        *time = -1;
        return NOERROR;
    }

    if (zoneInfoObj->mTransitions.GetLength() == 0) {
        // There is no transition information. There is just a raw offset for all time.
        if (mIsDst > 0) {
            // Caller has asserted DST, but there is no DST information available.
            *time = -1;
            return NOERROR;
        }
        CopyFieldsFromCalendar();
        mIsDst = 0;
        mGmtOffsetSeconds = rawOffsetSeconds;
        *time = rawTimeSeconds;
        return NOERROR;
    }

    // We cannot know for sure what instant the wall time will map to. Unfortunately, in
    // order to know for sure we need the timezone information, but to get the timezone
    // information we need an instant. To resolve this we use the raw offset to find an
    // OffsetInterval; this will get us the OffsetInterval we need or very close.

    // The initialTransition can be between -1 and (zoneInfo.mTransitions - 1). -1
    // indicates the rawTime is before the first transition and is handled gracefully by
    // createOffsetInterval().
    Integer initialTransitionIndex;
    zoneInfoObj->FindTransitionIndex(rawTimeSeconds, &initialTransitionIndex);

    if (mIsDst < 0) {
        // This is treated as a special case to get it out of the way:
        // When a caller has set isDst == -1 it means we can return the first match for
        // the wall time we find. If the caller has specified a wall time that cannot
        // exist this always returns -1.

        AutoPtr<IInteger> result;
        ec = DoWallTimeSearch(zoneInfoObj, initialTransitionIndex,
                wallTimeSeconds, true, &result);
        if (FAILED(ec) || result == nullptr) {
            *time = -1;
            return NOERROR;
        }
        return result->GetValue(time);
    }

    // If the wall time asserts a DST (isDst == 0 or 1) the search is performed twice:
    // 1) The first attempts to find a DST offset that matches isDst exactly.
    // 2) If it fails, isDst is assumed to be incorrect and adjustments are made to see
    // if a valid wall time can be created. The result can be somewhat arbitrary.

    AutoPtr<IInteger> result;
    ec = DoWallTimeSearch(zoneInfoObj, initialTransitionIndex,
            wallTimeSeconds, true, &result);
    if (FAILED(ec)) {
        *time = -1;
        return NOERROR;
    }
    if (result == nullptr) {
        ec = DoWallTimeSearch(zoneInfoObj, initialTransitionIndex,
                wallTimeSeconds, false, &result);
    }
    if (FAILED(ec) || result == nullptr) {
        *time = -1;
        return NOERROR;
    }
    return result->GetValue(time);
}

ECode ZoneInfo::WallTime::TryOffsetAdjustments(
    /* [in] */ ZoneInfo* zoneInfo,
    /* [in] */ Integer oldWallTimeSeconds,
    /* [in] */ OffsetInterval* targetInterval,
    /* [in] */ Integer transitionIndex,
    /* [in] */ Integer isDstToFind,
    /* [out] */ IInteger** time)
{
    Array<Integer> offsetsToTry = GetOffsetsOfType(zoneInfo, transitionIndex, isDstToFind);
    for (Integer j = 0; j < offsetsToTry.GetLength(); j++) {
        Integer rawOffsetSeconds = zoneInfo->mRawOffset / 1000;
        Integer jOffsetSeconds = rawOffsetSeconds + offsetsToTry[j];
        Integer targetIntervalOffsetSeconds = targetInterval->GetTotalOffsetSeconds();
        Integer adjustmentSeconds = targetIntervalOffsetSeconds - jOffsetSeconds;
        Integer adjustedWallTimeSeconds;
        FAIL_RETURN(CheckedAdd(oldWallTimeSeconds, adjustmentSeconds, &adjustedWallTimeSeconds));
        if (targetInterval->ContainsWallTime(adjustedWallTimeSeconds)) {
            // Perform any arithmetic that might overflow.
            Integer returnValue;
            FAIL_RETURN(CheckedSubtract(adjustedWallTimeSeconds,
                    targetIntervalOffsetSeconds, &returnValue));

            // Modify field state and return the result.
            mCalendar->SetTimeInMillis(adjustedWallTimeSeconds * 1000LL);
            CopyFieldsFromCalendar();
            mIsDst = targetInterval->GetIsDst();
            mGmtOffsetSeconds = targetIntervalOffsetSeconds;
            return CInteger::New(returnValue, IID_IInteger, (IInterface**)time);
        }
    }
    *time = nullptr;
    return NOERROR;
}

Array<Integer> ZoneInfo::WallTime::GetOffsetsOfType(
    /* [in] */ ZoneInfo* zoneInfo,
    /* [in] */ Integer startIndex,
    /* [in] */ Integer isDst)
{
    // +1 to account for the synthetic transition we invent before the first recorded one.
    Array<Integer> offsets(zoneInfo->mOffsets.GetLength() + 1);
    Array<Boolean> seen(zoneInfo->mOffsets.GetLength());
    Integer numFound = 0;

    Integer delta = 0;
    Boolean clampTop = false;
    Boolean clampBottom = false;
    do {
        // delta = { 1, -1, 2, -2, 3, -3...}
        delta *= -1;
        if (delta >= 0) {
            delta++;
        }

        Integer transitionIndex = startIndex + delta;
        if (delta < 0 && transitionIndex < -1) {
            clampBottom = true;
            continue;
        }
        else if (delta > 0 && transitionIndex >= zoneInfo->mTypes.GetLength()) {
            clampTop = true;
            continue;
        }

        if (transitionIndex == -1) {
            if (isDst == 0) {
                // Synthesize a non-DST transition before the first transition we have
                // data for.
                offsets[numFound++] = 0; // offset of 0 from raw offset
            }
            continue;
        }
        Integer type = zoneInfo->mTypes[transitionIndex] & 0xff;
        if (!seen[type]) {
            if (zoneInfo->mIsDsts[type] == isDst) {
                offsets[numFound++] = zoneInfo->mOffsets[type];
            }
            seen[type] = true;
        }
    } while (!(clampTop && clampBottom));

    Array<Integer> toReturn(numFound);
    toReturn.Copy(0, offsets, 0, numFound);
    return toReturn;
}

ECode ZoneInfo::WallTime::DoWallTimeSearch(
    /* [in] */ ZoneInfo* zoneInfo,
    /* [in] */ Integer initialTransitionIndex,
    /* [in] */ Integer wallTimeSeconds,
    /* [in] */ Boolean mustMatchDst,
    /* [out] */ IInteger** time)
{
    // The loop below starts at the initialTransitionIndex and radiates out from that point
    // up to 24 hours in either direction by applying transitionIndexDelta to inspect
    // adjacent transitions (0, -1, +1, -2, +2). 24 hours is used because we assume that no
    // total offset from UTC is ever > 24 hours. clampTop and clampBottom are used to
    // indicate whether the search has either searched > 24 hours or exhausted the
    // transition data in that direction. The search stops when a match is found or if
    // clampTop and clampBottom are both true.
    // The match logic employed is determined by the mustMatchDst parameter.
    constexpr Integer MAX_SEARCH_SECONDS = 24 * 60 * 60;
    Boolean clampTop = false;
    Boolean clampBottom = false;
    Integer loop = 0;
    do {
        // transitionIndexDelta = { 0, -1, 1, -2, 2,..}
        Integer transitionIndexDelta = (loop + 1) / 2;
        if (loop % 2 == 1) {
            transitionIndexDelta *= -1;
        }
        loop++;

        // Only do any work in this iteration if we need to.
        if ((transitionIndexDelta > 0 && clampTop) ||
                (transitionIndexDelta < 0 && clampBottom)) {
            continue;
        }

        // Obtain the OffsetInterval to use.
        Integer currentTransitionIndex = initialTransitionIndex + transitionIndexDelta;
        AutoPtr<OffsetInterval> offsetInterval;
        FAIL_RETURN(OffsetInterval::Create(zoneInfo, currentTransitionIndex, &offsetInterval));
        if (offsetInterval == nullptr) {
            // No transition exists with the index we tried: Stop searching in the
            // current direction.
            clampTop |= (transitionIndexDelta > 0);
            clampBottom |= (transitionIndexDelta < 0);
            continue;
        }

        // Match the wallTimeSeconds against the OffsetInterval.
        if (mustMatchDst) {
            // Work out if the interval contains the wall time the caller specified and
            // matches their isDst value.
            if (offsetInterval->ContainsWallTime(wallTimeSeconds)) {
                if (mIsDst == -1 || offsetInterval->GetIsDst() == mIsDst) {
                    // This always returns the first OffsetInterval it finds that matches
                    // the wall time and isDst requirements. If this.isDst == -1 this means
                    // the result might be a DST or a non-DST answer for wall times that can
                    // exist in two OffsetIntervals.
                    Integer totalOffsetSeconds = offsetInterval->GetTotalOffsetSeconds();
                    Integer returnValue;
                    FAIL_RETURN(CheckedSubtract(wallTimeSeconds, totalOffsetSeconds, &returnValue));

                    CopyFieldsFromCalendar();
                    mIsDst = offsetInterval->GetIsDst();
                    mGmtOffsetSeconds = totalOffsetSeconds;
                    return CInteger::New(returnValue, IID_IInteger, (IInterface**)time);
                }
            }
        }
        else {
            // To retain similar behavior to the old native implementation: if the caller is
            // asserting the same isDst value as the OffsetInterval we are looking at we do
            // not try to find an adjustment from another OffsetInterval of the same isDst
            // type. If you remove this you get different results in situations like a
            // DST -> DST transition or STD -> STD transition that results in an interval of
            // "skipped" wall time. For example: if 01:30 (DST) is invalid and between two
            // DST intervals, and the caller has passed isDst == 1, this results in a -1
            // being returned.
            if (mIsDst != offsetInterval->GetIsDst()) {
                const Integer isDstToFind = mIsDst;
                AutoPtr<IInteger> returnValue;
                FAIL_RETURN(TryOffsetAdjustments(zoneInfo, wallTimeSeconds, offsetInterval,
                        currentTransitionIndex, isDstToFind, &returnValue));
                if (returnValue != nullptr) {
                    returnValue.MoveTo(time);
                    return NOERROR;
                }
            }
        }

        // See if we can avoid another loop in the current direction.
        if (transitionIndexDelta > 0) {
            // If we are searching forward and the OffsetInterval we have ends
            // > MAX_SEARCH_SECONDS after the wall time, we don't need to look any further
            // forward.
            Boolean endSearch = offsetInterval->GetEndWallTimeSeconds() - wallTimeSeconds
                    > MAX_SEARCH_SECONDS;
            if (endSearch) {
                clampTop = true;
            }
        }
        else if (transitionIndexDelta < 0) {
            Boolean endSearch = wallTimeSeconds - offsetInterval->GetStartWallTimeSeconds()
                    >= MAX_SEARCH_SECONDS;
            if (endSearch) {
                // If we are searching backward and the OffsetInterval starts
                // > MAX_SEARCH_SECONDS before the wall time, we don't need to look any
                // further backwards.
                clampBottom = true;
            }
        }

    } while (!(clampTop && clampBottom));
    *time = nullptr;
    return NOERROR;
}

ECode ZoneInfo::WallTime::SetYear(
    /* [in] */ Integer year)
{
    mYear = year;
    return NOERROR;
}

ECode ZoneInfo::WallTime::SetMonth(
    /* [in] */ Integer month)
{
    mMonth = month;
    return NOERROR;
}

ECode ZoneInfo::WallTime::SetMonthDay(
    /* [in] */ Integer monthDay)
{
    mMonthDay = monthDay;
    return NOERROR;
}

ECode ZoneInfo::WallTime::SetHour(
    /* [in] */ Integer hour)
{
    mHour = hour;
    return NOERROR;
}

ECode ZoneInfo::WallTime::SetMinute(
    /* [in] */ Integer minute)
{
    mMinute = minute;
    return NOERROR;
}

ECode ZoneInfo::WallTime::SetSecond(
    /* [in] */ Integer second)
{
    mSecond = second;
    return NOERROR;
}

ECode ZoneInfo::WallTime::SetWeekDay(
    /* [in] */ Integer weekDay)
{
    mWeekDay = weekDay;
    return NOERROR;
}

ECode ZoneInfo::WallTime::SetYearDay(
    /* [in] */ Integer yearDay)
{
    mYearDay = yearDay;
    return NOERROR;
}

ECode ZoneInfo::WallTime::SetIsDst(
    /* [in] */ Integer isDst)
{
    mIsDst = isDst;
    return NOERROR;
}

ECode ZoneInfo::WallTime::SetGmtOffset(
    /* [in] */ Integer gmtoff)
{
    mGmtOffsetSeconds = gmtoff;
    return NOERROR;
}

ECode ZoneInfo::WallTime::GetYear(
    /* [out] */ Integer* year)
{
    VALIDATE_NOT_NULL(year);

    *year = mYear;
    return NOERROR;
}

ECode ZoneInfo::WallTime::GetMonth(
    /* [out] */ Integer* month)
{
    VALIDATE_NOT_NULL(month);

    *month = mMonth;
    return NOERROR;
}

ECode ZoneInfo::WallTime::GetMonthDay(
    /* [out] */ Integer* monthDay)
{
    VALIDATE_NOT_NULL(monthDay);

    *monthDay = mMonthDay;
    return NOERROR;
}

ECode ZoneInfo::WallTime::GetHour(
    /* [out] */ Integer* hour)
{
    VALIDATE_NOT_NULL(hour);

    *hour = mHour;
    return NOERROR;
}

ECode ZoneInfo::WallTime::GetMinute(
    /* [out] */ Integer* minute)
{
    VALIDATE_NOT_NULL(minute);

    *minute = mMinute;
    return NOERROR;
}

ECode ZoneInfo::WallTime::GetSecond(
    /* [out] */ Integer* second)
{
    VALIDATE_NOT_NULL(second);

    *second = mSecond;
    return NOERROR;
}

ECode ZoneInfo::WallTime::GetWeekDay(
    /* [out] */ Integer* weekDay)
{
    VALIDATE_NOT_NULL(weekDay);

    *weekDay = mWeekDay;
    return NOERROR;
}

ECode ZoneInfo::WallTime::GetYearDay(
    /* [out] */ Integer* yearDay)
{
    VALIDATE_NOT_NULL(yearDay);

    *yearDay = mYearDay;
    return NOERROR;
}

ECode ZoneInfo::WallTime::GetGmtOffset(
    /* [out] */ Integer* gmtoff)
{
    VALIDATE_NOT_NULL(gmtoff);

    *gmtoff = mGmtOffsetSeconds;
    return NOERROR;
}

ECode ZoneInfo::WallTime::GetIsDst(
    /* [out] */ Integer* isDst)
{
    VALIDATE_NOT_NULL(isDst);

    *isDst = mIsDst;
    return NOERROR;
}

void ZoneInfo::WallTime::CopyFieldsToCalendar()
{
    mCalendar->Set(ICalendar::YEAR, mYear);
    mCalendar->Set(ICalendar::MONTH, mMonth);
    mCalendar->Set(ICalendar::DAY_OF_MONTH, mMonthDay);
    mCalendar->Set(ICalendar::HOUR_OF_DAY, mHour);
    mCalendar->Set(ICalendar::MINUTE, mMinute);
    mCalendar->Set(ICalendar::SECOND, mSecond);
    mCalendar->Set(ICalendar::MILLISECOND, 0);
}

void ZoneInfo::WallTime::CopyFieldsFromCalendar()
{
    mCalendar->Get(ICalendar::YEAR, &mYear);
    mCalendar->Get(ICalendar::MONTH, &mMonth);
    mCalendar->Get(ICalendar::DAY_OF_MONTH, &mMonthDay);
    mCalendar->Get(ICalendar::HOUR_OF_DAY, &mHour);
    mCalendar->Get(ICalendar::MINUTE, &mMinute);
    mCalendar->Get(ICalendar::SECOND, &mSecond);

    // Calendar uses Sunday == 1, COMO Time uses Sunday = 0.
    mCalendar->Get(ICalendar::DAY_OF_WEEK, &mWeekDay);
    mWeekDay -= 1;
    // Calendar enumerates from 1, COMO Time enumerates from 0.
    mCalendar->Get(ICalendar::DAY_OF_YEAR, &mYearDay);
    mYearDay -= 1;
}

//-------------------------------------------------------------------------

ECode ZoneInfo::OffsetInterval::Create(
    /* [in] */ ZoneInfo* timeZone,
    /* [in] */ Integer transitionIndex,
    /* [out] */ OffsetInterval** offsetInterval)
{
    if (transitionIndex < -1 || transitionIndex >= timeZone->mTransitions.GetLength()) {
        *offsetInterval = nullptr;
        return NOERROR;
    }

    Integer rawOffsetSeconds = timeZone->mRawOffset / 1000;
    if (transitionIndex == -1) {
        Integer endWallTimeSeconds;
        FAIL_RETURN(CheckedAdd(timeZone->mTransitions[0], rawOffsetSeconds, &endWallTimeSeconds))
        *offsetInterval = new OffsetInterval(IInteger::MIN_VALUE, endWallTimeSeconds, 0, rawOffsetSeconds);
        REFCOUNT_ADD(*offsetInterval);
        return NOERROR;
    }

    Integer type = timeZone->mTypes[transitionIndex] & 0xff;
    Integer totalOffsetSeconds = timeZone->mOffsets[type] + rawOffsetSeconds;
    Integer endWallTimeSeconds;
    if (transitionIndex == timeZone->mTransitions.GetLength() - 1) {
        // If this is the last transition, make up the end time.
        endWallTimeSeconds = IInteger::MAX_VALUE;
    }
    else {
        FAIL_RETURN(CheckedAdd(timeZone->mTransitions[transitionIndex + 1],
                totalOffsetSeconds, &endWallTimeSeconds));
    }
    Integer isDst = timeZone->mIsDsts[type];
    Integer startWallTimeSeconds;
    FAIL_RETURN(CheckedAdd(timeZone->mTransitions[transitionIndex],
            totalOffsetSeconds, &startWallTimeSeconds));
    *offsetInterval = new OffsetInterval(startWallTimeSeconds, endWallTimeSeconds, isDst, totalOffsetSeconds);
    REFCOUNT_ADD(*offsetInterval);
    return NOERROR;
}

}
}
