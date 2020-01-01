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

#ifndef __COMO_CORE_NATIVELOCKWORD_H__
#define __COMO_CORE_NATIVELOCKWORD_H__

#include "ccmdef.h"
#include <stdint.h>

namespace como {
namespace core {

class NativeMonitor;

/* The lock value itself as stored in mirror::Object::monitor_.  The two most significant bits of
 * the state. The four possible states are fat locked, thin/unlocked, hash code, and forwarding
 * address. When the lock word is in the "thin" state and its bits are formatted as follows:
 *
 *  |33|2|2|222222221111|1111110000000000|
 *  |10|9|8|765432109876|5432109876543210|
 *  |00|p|p| lock count |thread id owner |
 *
 * When the lock word is in the "fat" state and its bits are formatted as follows:
 *
 *  |33|2|2|2222222211111111110000000000|
 *  |10|9|8|7654321098765432109876543210|
 *  |01|p|p| MonitorId                  |
 *
 * When the lock word is in hash state and its bits are formatted as follows:
 *
 *  |33|2|2|2222222211111111110000000000|
 *  |10|9|8|7654321098765432109876543210|
 *  |10|p|p| HashCode                   |
 *
 * When the lock word is in forwarding address state and its bits are formatted as follows:
 *
 *  |33|2|22222222211111111110000000000|
 *  |10|9|87654321098765432109876543210|
 *  |11|0| ForwardingAddress           |
 *
 * The `r` bit stores the read barrier state.
 * The `m` bit stores the mark state.
 */
class NativeLockWord
{
public:
    enum SizeShiftsAndMasks
    {
        // Number of bits to encode the state, currently just fat or thin/unlocked or hash code.
        kStateSize = 2,
        kPadSize = 2,
        // Number of bits to encode the thin lock owner.
        kThinLockOwnerSize = 16,
        // Remaining bits are the recursive lock count.
        kThinLockCountSize = 32 - kThinLockOwnerSize - kStateSize - kPadSize,

        // Thin lock bits. Owner in lowest bits.
        kThinLockOwnerShift = 0,
        kThinLockOwnerMask = (1 << kThinLockOwnerSize) - 1,
        kThinLockMaxOwner = kThinLockOwnerMask,
        // Count in higher bits.
        kThinLockCountShift = kThinLockOwnerSize + kThinLockOwnerShift,
        kThinLockCountMask = (1 << kThinLockCountSize) - 1,
        kThinLockMaxCount = kThinLockCountMask,

        // State in the highest bits.
        kStateShift = kThinLockCountShift + kThinLockCountSize + kPadSize,
        kStateMask = (1 << kStateSize) - 1,
        kStateThinOrUnlocked = 0,
        kStateFat = 1,

        kMonitorIdShift = 0,
        kMonitorIdSize = 32 - kStateSize - kPadSize,
        kMonitorIdMask = (1 << kMonitorIdSize) - 1,
    };

    enum LockState
    {
        kUnlocked,    // No lock owners.
        kThinLocked,  // Single uncontended owner.
        kFatLocked,   // See associated monitor.
    };

public:
    // Constructor a lock word for inflation to use a Monitor.
    NativeLockWord(
        /* [in] */ NativeMonitor* mon);

    static NativeLockWord FromThinLockId(
        /* [in] */ uint32_t threadId,
        /* [in] */ uint32_t count);

    static NativeLockWord FromDefault();

    static NativeLockWord Default();

    LockState GetState() const;

    // Return the owner thin lock thread id.
    uint32_t ThinLockOwner() const;

    // Return the number of times a lock value has been locked.
    uint32_t ThinLockCount() const;

    // Return the Monitor encoded in a fat lock.
    NativeMonitor* FatLockMonitor() const;

private:
    // Default constructor with no lock ownership.
    NativeLockWord();

    explicit NativeLockWord(
        /* [in] */ uint32_t val);

    uint32_t GetValue() const;

private:
    friend class NativeObject;

    // The encoded value holding all the state.
    uint32_t mValue;
};

inline NativeLockWord NativeLockWord::FromThinLockId(
    /* [in] */ uint32_t threadId,
    /* [in] */ uint32_t count)
{
    CHECK(threadId <= static_cast<uint32_t>(kThinLockMaxOwner));
    CHECK(count <= static_cast<uint32_t>(kThinLockMaxCount));
    return NativeLockWord((threadId << kThinLockOwnerShift) |
                          (count << kThinLockCountShift) |
                          (kStateThinOrUnlocked << kStateShift));
}

inline NativeLockWord NativeLockWord::FromDefault()
{
    return NativeLockWord();
}

inline NativeLockWord NativeLockWord::Default()
{
    return NativeLockWord();
}

inline uint32_t NativeLockWord::ThinLockOwner() const
{
    CHECK(GetState() == kThinLocked);
    return (mValue >> kThinLockOwnerShift) & kThinLockOwnerMask;
}

inline uint32_t NativeLockWord::ThinLockCount() const
{
    CHECK(GetState() == kThinLocked);
    return (mValue >> kThinLockCountShift) & kThinLockCountMask;
}

inline NativeLockWord::NativeLockWord()
    : mValue(0)
{}

inline NativeLockWord::NativeLockWord(
    /* [in] */ uint32_t val)
    : mValue(val)
{}

inline uint32_t NativeLockWord::GetValue() const
{
    return mValue;
}

}
}

#endif // __COMO_CORE_NATIVELOCKWORD_H__
