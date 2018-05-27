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

/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __CCM_CORE_NATIVEMONITORPOOL_H__
#define __CCM_CORE_NATIVEMONITORPOOL_H__

#include "core/globals.h"
#include "core/NativeLockWord.h"
#include "core/NativeMonitor.h"
#include "core/NativeMutex.h"
#include "core/NativeRuntime.h"
#include <memory>

namespace ccm {
namespace core {

class NativeMonitorPool
{
public:
    static NativeMonitorPool* Create();

    static NativeMonitor* CreateMonitor(
        /* [in] */ NativeThread* self,
        /* [in] */ NativeThread* owner,
        /* [in] */ NativeObject* obj);

    static void ReleaseMonitor(
        /* [in] */ NativeThread* self,
        /* [in] */ NativeMonitor* monitor);

    static NativeMonitor* MonitorFromMonitorId(
        /* [in] */ MonitorId monId);

    static NativeMonitorPool* GetMonitorPool();

private:
    NativeMonitorPool();

    void AllocateChunk();

    // Release all chunks and metadata. This is done on shutdown, where threads have been destroyed,
    // so ignore thead-safety analysis.
    void FreeInternal();

    NativeMonitor* CreateMonitorInPool(
        /* [in] */ NativeThread* self,
        /* [in] */ NativeThread* owner,
        /* [in] */ NativeObject* obj);

    void ReleaseMonitorToPool(
        /* [in] */ NativeThread* self,
        /* [in] */ NativeMonitor* monitor);

    // Note: This is safe as we do not ever move chunks.  All needed entries in the monitor_chunks_
    // data structure are read-only once we get here.  Updates happen-before this call because
    // the lock word was stored with release semantics and we read it with acquire semantics to
    // retrieve the id.
    NativeMonitor* LookupMonitor(
        /* [in] */ MonitorId monId);

    static size_t MonitorIdToOffset(
        /* [in] */ MonitorId monId);

    static constexpr MonitorId OffsetToMonitorId(
        /* [in] */ size_t offset);

    static constexpr size_t ChunkListCapacity(
        /* [in] */ size_t index);

private:
    // TODO: There are assumptions in the code that monitor addresses are 8B aligned (>>3).
    static constexpr size_t kMonitorAlignment = 8;
    // Size of a monitor, rounded up to a multiple of alignment.
    static constexpr size_t kAlignedMonitorSize = (sizeof(NativeMonitor) + kMonitorAlignment - 1) &
                                                  -kMonitorAlignment;
    // As close to a page as we can get seems a good start.
    static constexpr size_t kChunkCapacity = kPageSize / kAlignedMonitorSize;
    // Chunk size that is referenced in the id. We can collapse this to the actually used storage
    // in a chunk, i.e., kChunkCapacity * kAlignedMonitorSize, but this will mean proper divisions.
    static constexpr size_t kChunkSize = kPageSize;
    // The number of chunks of storage that can be referenced by the initial chunk list.
    // The total number of usable monitor chunks is typically 255 times this number, so it
    // should be large enough that we don't run out. We run out of address bits if it's > 512.
    // Currently we set it a bit smaller, to save half a page per process.  We make it tiny in
    // debug builds to catch growth errors. The only value we really expect to tune.
    static constexpr size_t kInitialChunkStorage = 256;
    // The number of lists, each containing pointers to storage chunks.
    static constexpr size_t kMaxChunkLists = 8;  //  Dictated by 3 bit index. Don't increase above 8.
    static constexpr size_t kMaxListSize = kInitialChunkStorage << (kMaxChunkLists - 1);

    // Array of pointers to lists (again arrays) of pointers to chunks containing monitors.
    // Zeroth entry points to a list (array) of kInitialChunkStorage pointers to chunks.
    // Each subsequent list as twice as large as the preceding one.
    // Monitor Ids are interpreted as follows:s
    //     Top 3 bits (of 28): index into monitor_chunks_.
    //     Next 16 bits: index into the chunk list, i.e. monitor_chunks_[i].
    //     Last 9 bits: offset within chunk, expressed as multiple of kMonitorAlignment.
    // If we set kInitialChunkStorage to 512, this would allow us to use roughly 128K chunks of
    // monitors, which is 0.5GB of monitors.  With this maximum setting, the largest chunk list
    // contains 64K entries, and we make full use of the available index space. With a
    // kInitialChunkStorage value of 256, this is proportionately reduced to 0.25GB of monitors.
    // Updates to monitor_chunks_ are guarded by allocated_monitor_ids_lock_ .
    // No field in this entire data structure is ever updated once a monitor id whose lookup
    // requires it has been made visible to another thread.  Thus readers never race with
    // updates, in spite of the fact that they acquire no locks.
    uintptr_t* mMonitorChunks[kMaxChunkLists];  //  uintptr_t is really a Monitor* .

    // Highest currently used index in monitor_chunks_ . Used for newly allocated chunks.
    size_t mCurrentChunkListIndex;
    // Number of chunk pointers stored in monitor_chunks_[current_chunk_list_index_] so far.
    size_t mNumChunks;
    // After the initial allocation, this is always equal to
    // ChunkListCapacity(current_chunk_list_index_).
    size_t mCurrentChunkListCapacity;

    std::allocator<uint8_t> mAllocator;

    // Start of free list of monitors.
    // Note: these point to the right memory regions, but do *not* denote initialized objects.
    NativeMonitor* mFirstFree;
};

inline NativeMonitorPool* NativeMonitorPool::Create()
{
    return new NativeMonitorPool();
}

inline NativeMonitor* NativeMonitorPool::CreateMonitor(
    /* [in] */ NativeThread* self,
    /* [in] */ NativeThread* owner,
    /* [in] */ NativeObject* obj)
{
    return GetMonitorPool()->CreateMonitorInPool(self, owner, obj);
}

inline void NativeMonitorPool::ReleaseMonitor(
    /* [in] */ NativeThread* self,
    /* [in] */ NativeMonitor* monitor)
{
    GetMonitorPool()->ReleaseMonitorToPool(self, monitor);
}

inline NativeMonitor* NativeMonitorPool::MonitorFromMonitorId(
    /* [in] */ MonitorId monId)
{
    return GetMonitorPool()->LookupMonitor(monId);
}

inline NativeMonitorPool* NativeMonitorPool::GetMonitorPool()
{
    return NativeRuntime::Current()->GetMonitorPool();
}

inline size_t NativeMonitorPool::MonitorIdToOffset(
    /* [in] */ MonitorId monId)
{
    return monId << 3;
}

inline constexpr MonitorId NativeMonitorPool::OffsetToMonitorId(
    /* [in] */ size_t offset)
{
    return static_cast<MonitorId>(offset >> 3);
}

inline constexpr size_t NativeMonitorPool::ChunkListCapacity(
    /* [in] */ size_t index)
{
    return kInitialChunkStorage << index;
}

}
}

#endif // __CCM_CORE_NATIVEMONITORPOOL_H__
