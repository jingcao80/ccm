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

#include "core/NativeMonitorPool.h"

namespace ccm {
namespace core {

NativeMonitorPool::NativeMonitorPool()
    : mCurrentChunkListIndex(0)
    , mNumChunks(0)
    , mCurrentChunkListCapacity(0)
    , mFirstFree(nullptr)
{
    for (size_t i = 0; i < kMaxChunkLists; ++i) {
        mMonitorChunks[i] = nullptr;
    }
    AllocateChunk();
}

void NativeMonitorPool::AllocateChunk()
{
    CHECK(mFirstFree == nullptr);

    if (mNumChunks == mCurrentChunkListCapacity) {
        if (mCurrentChunkListCapacity != 0) {
            ++mCurrentChunkListIndex;
            CHECK(mCurrentChunkListIndex < kMaxChunkLists);
        }
        mCurrentChunkListCapacity = ChunkListCapacity(mCurrentChunkListIndex);
        uintptr_t* newList = new uintptr_t[mCurrentChunkListCapacity]();
        CHECK(mMonitorChunks[mCurrentChunkListIndex] == nullptr);
        mMonitorChunks[mCurrentChunkListIndex] = newList;
        mNumChunks = 0;
    }

    void* chunk = mAllocator.allocate(kChunkSize);
    CHECK(chunk != nullptr);
    CHECK(reinterpret_cast<uintptr_t>(chunk) % kMonitorAlignment == 0);

    // Add the chunk.
    mMonitorChunks[mCurrentChunkListIndex][mNumChunks] = reinterpret_cast<uintptr_t>(chunk);
    mNumChunks++;

    // Set up the free list
    NativeMonitor* last = reinterpret_cast<NativeMonitor*>(reinterpret_cast<uintptr_t>(chunk) +
                                             (kChunkCapacity - 1) * kAlignedMonitorSize);
    last->mNextFree = nullptr;
    // Eagerly compute id.
    last->mMonitorId = OffsetToMonitorId(mCurrentChunkListIndex* (kMaxListSize * kChunkSize)
            + (mNumChunks - 1) * kChunkSize + (kChunkCapacity - 1) * kAlignedMonitorSize);
    for (size_t i = 0; i < kChunkCapacity - 1; ++i) {
        NativeMonitor* before = reinterpret_cast<NativeMonitor*>(reinterpret_cast<uintptr_t>(last) -
                kAlignedMonitorSize);
        before->mNextFree = last;
        // Derive monitor_id from last.
        before->mMonitorId = OffsetToMonitorId(MonitorIdToOffset(last->mMonitorId) -
                kAlignedMonitorSize);
        last = before;
    }
    CHECK(last == reinterpret_cast<NativeMonitor*>(chunk));
    mFirstFree = last;
}

void NativeMonitorPool::FreeInternal()
{
    // This is on shutdown with NO_THREAD_SAFETY_ANALYSIS, can't/don't need to lock.
    CHECK(mCurrentChunkListCapacity != 0);
    for (size_t i = 0; i <= mCurrentChunkListIndex; ++i) {
        CHECK(mMonitorChunks[i] != static_cast<uintptr_t*>(nullptr));
        for (size_t j = 0; j < ChunkListCapacity(i); ++j) {
            if (i < mCurrentChunkListIndex || j < mNumChunks) {
                CHECK(mMonitorChunks[i][j] != 0);
                mAllocator.deallocate(reinterpret_cast<uint8_t*>(mMonitorChunks[i][j]), kChunkSize);
            }
            else {
                CHECK(mMonitorChunks[i][j] == 0);
            }
        }
        delete[] mMonitorChunks[i];
    }
}

NativeMonitor* NativeMonitorPool::CreateMonitorInPool(
    /* [in] */ NativeThread* self,
    /* [in] */ NativeThread* owner,
    /* [in] */ NativeObject* obj)
{
    NativeMutex::AutoLock lock(self, *Locks::sAllocatedMonitorIdsLock);

    // Enough space, or need to resize?
    if (mFirstFree == nullptr) {
        AllocateChunk();
    }

    NativeMonitor* monUninitialized = mFirstFree;
    mFirstFree = mFirstFree->mNextFree;

    // Pull out the id which was preinitialized.
    MonitorId id = monUninitialized->mMonitorId;

    // Initialize it.
    NativeMonitor* monitor = new(monUninitialized)
            NativeMonitor(self, owner, obj, id);

    return monitor;
}

void NativeMonitorPool::ReleaseMonitorToPool(
    /* [in] */ NativeThread* self,
    /* [in] */ NativeMonitor* monitor)
{
    // Might be racy with allocation, so acquire lock.
    NativeMutex::AutoLock lock(self, *Locks::sAllocatedMonitorIdsLock);

    // Keep the monitor id. Don't trust it's not cleared.
    MonitorId id = monitor->mMonitorId;

    // Call the destructor.
    monitor->~NativeMonitor();

    // Add to the head of the free list.
    monitor->mNextFree = mFirstFree;
    mFirstFree = monitor;

    // Rewrite monitor id.
    monitor->mMonitorId = id;
}

void NativeMonitorPool::ReleaseMonitorsToPool(
    /* [in] */ NativeThread* self,
    /* [in] */ NativeMonitorList::Monitors* monitors)
{
    for (NativeMonitor* mon : *monitors) {
        ReleaseMonitorToPool(self, mon);
    }
}

NativeMonitor* NativeMonitorPool::LookupMonitor(
    /* [in] */ MonitorId monId)
{
    size_t offset = MonitorIdToOffset(monId);
    size_t index = offset / kChunkSize;
    size_t top_index = index / kMaxListSize;
    size_t list_index = index % kMaxListSize;
    size_t offset_in_chunk = offset % kChunkSize;
    uintptr_t base = mMonitorChunks[top_index][list_index];
    return reinterpret_cast<NativeMonitor*>(base + offset_in_chunk);
}

}
}
