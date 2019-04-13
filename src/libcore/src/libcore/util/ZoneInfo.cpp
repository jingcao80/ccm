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

#include "ccm/core/Math.h"
#include "ccm/util/Arrays.h"
#include "libcore/util/ZoneInfo.h"
#include <ccmlogger.h>

using ccm::core::E_ILLEGAL_STATE_EXCEPTION;
using ccm::core::Math;
using ccm::io::E_IO_EXCEPTION;
using ccm::util::Arrays;

namespace libcore {
namespace util {

const Array<Integer> ZoneInfo::NORMAL{
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334,
};

const Array<Integer> ZoneInfo::LEAP{
    0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335,
};

CCM_INTERFACE_IMPL_1(ZoneInfo, TimeZone, IZoneInfo);

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
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    ZoneInfo* other = (ZoneInfo*)IZoneInfo::Probe(obj);
    if (other == nullptr) {
        *same = false;
        return NOERROR;
    }
    String thisID, otherID;
    GetID(&thisID);
    other->GetID(&otherID);
    Boolean result;
    *same = thisID.Equals(otherID) && (HasSameRules(other, &result), result);
    return NOERROR;
}

ECode ZoneInfo::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    constexpr Integer prime = 31;
    Integer result = 1;
    String id;
    GetID(&id);
    result = prime * result + id.GetHashCode();
    result = prime * result + Arrays::GetHashCode(mOffsets);
    result = prime * result + Arrays::GetHashCode(mIsDsts);
    result = prime * result + mRawOffset;
    result = prime * result + Arrays::GetHashCode(mTransitions);
    result = prime * result + Arrays::GetHashCode(mTypes);
    result = prime * result + (mUseDst ? 1231 : 1237);
    *hash = result;
    return NOERROR;
}

ECode ZoneInfo::ToString(
    /* [out] */ String* desc)
{
    VALIDATE_NOT_NULL(desc);

    String id;
    GetID(&id);
    *desc = String::Format("ZoneInfo[id=\"%s\",mRawOffset=%d,"
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

}
}
