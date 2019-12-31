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

/*
 * Copyright (C) 2008 The Android Open Source Project
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

#ifndef __COMO_CORE_NATIVEATOMIC_H__
#define __COMO_CORE_NATIVEATOMIC_H__

#include <ccmtypes.h>
#include <atomic>
#include <limits>

namespace como {
namespace core {

// QuasiAtomic encapsulates two separate facilities that we are
// trying to move away from:  "quasiatomic" 64 bit operations
// and custom memory fences.  For the time being, they remain
// exposed.  Clients should be converted to use either class Atomic
// below whenever possible, and should eventually use C++11 atomics.
// The two facilities that do not have a good C++11 analog are
// ThreadFenceForConstructor and Atomic::*JavaData.
//
// NOTE: Two "quasiatomic" operations on the exact same memory address
// are guaranteed to operate atomically with respect to each other,
// but no guarantees are made about quasiatomic operations mixed with
// non-quasiatomic operations on the same address, nor about
// quasiatomic operations that are performed on partially-overlapping
// memory.
class QuasiAtomic
{
public:
    static void ThreadFenceAcquire();

    static void ThreadFenceRelease();

    static void ThreadFenceSequentiallyConsistent();
};

inline void QuasiAtomic::ThreadFenceAcquire()
{
    std::atomic_thread_fence(std::memory_order_acquire);
}

inline void QuasiAtomic::ThreadFenceRelease()
{
    std::atomic_thread_fence(std::memory_order_release);
}

inline void QuasiAtomic::ThreadFenceSequentiallyConsistent()
{
    std::atomic_thread_fence(std::memory_order_seq_cst);
}


template<typename T>
class PACKED(sizeof(T)) Atomic : public std::atomic<T>
{
public:
    Atomic<T>();

    explicit Atomic<T>(
        /* [in] */ T value);

    // Load from memory without ordering or synchronization constraints.
    T LoadRelaxed() const;

    // Load from memory with acquire ordering.
    T LoadAcquire() const;

    // Load from memory with a total ordering.
    // Corresponds exactly to a Java volatile load.
    T LoadSequentiallyConsistent() const;

    // Store to memory without ordering or synchronization constraints.
    void StoreRelaxed(
        /* [in] */ T desired);

    // Store to memory with release ordering.
    void StoreRelease(
        /* [in] */ T desired);

    // Store to memory with a total ordering.
    void StoreSequentiallyConsistent(
        /* [in] */ T desired);

    // Atomically replace the value with desired value.
    T ExchangeRelaxed(
        /* [in] */ T desired);

    // Atomically replace the value with desired value if it matches the expected value.
    // Participates in total ordering of atomic operations.
    Boolean CompareExchangeStrongSequentiallyConsistent(
        /* [in] */ T expected,
        /* [in] */ T desired);

    // The same, except it may fail spuriously.
    Boolean CompareExchangeWeakSequentiallyConsistent(
        /* [in] */ T expected,
        /* [in] */ T desired);

    // Atomically replace the value with desired value if it matches the expected value. Doesn't
    // imply ordering or synchronization constraints.
    Boolean CompareExchangeStrongRelaxed(
        /* [in] */ T expected,
        /* [in] */ T desired);

    // Atomically replace the value with desired value if it matches the expected value. Prior writes
    // to other memory locations become visible to the threads that do a consume or an acquire on the
    // same location.
    Boolean CompareExchangeStrongRelease(
        /* [in] */ T expected,
        /* [in] */ T desired);

    // The same, except it may fail spuriously.
    Boolean CompareExchangeWeakRelaxed(
        /* [in] */ T expected,
        /* [in] */ T desired);

    // Atomically replace the value with desired value if it matches the expected value. Prior writes
    // made to other memory locations by the thread that did the release become visible in this
    // thread.
    Boolean CompareExchangeWeakAcquire(
        /* [in] */ T expected,
        /* [in] */ T desired);

    // Atomically replace the value with desired value if it matches the expected value. prior writes
    // to other memory locations become visible to the threads that do a consume or an acquire on the
    // same location.
    Boolean CompareExchangeWeakRelease(
        /* [in] */ T expected,
        /* [in] */ T desired);

    T FetchAndAddSequentiallyConsistent(
        /* [in] */ const T value);

    T FetchAndAddRelaxed(
        /* [in] */ const T value);

    T FetchAndSubSequentiallyConsistent(
        /* [in] */ const T value);

    T FetchAndSubRelaxed(
        /* [in] */ const T value);

    T FetchAndOrSequentiallyConsistent(
        /* [in] */ const T value);

    T FetchAndAndSequentiallyConsistent(
        /* [in] */ const T value);

    volatile T* Address();

    static T MaxValue();
};

template<typename T>
Atomic<T>::Atomic()
    : std::atomic<T>(0)
{}

template<typename T>
Atomic<T>::Atomic(
    /* [in] */ T value)
    : std::atomic<T>(value)
{}

// Load from memory without ordering or synchronization constraints.
template<typename T>
T Atomic<T>::LoadRelaxed() const
{
    return this->load(std::memory_order_relaxed);
}

// Load from memory with acquire ordering.
template<typename T>
T Atomic<T>::LoadAcquire() const
{
    return this->load(std::memory_order_acquire);
}

// Load from memory with a total ordering.
// Corresponds exactly to a Java volatile load.
template<typename T>
T Atomic<T>::LoadSequentiallyConsistent() const
{
    return this->load(std::memory_order_seq_cst);
}

// Store to memory without ordering or synchronization constraints.
template<typename T>
void Atomic<T>::StoreRelaxed(
    /* [in] */ T desired)
{
    this->store(desired, std::memory_order_relaxed);
}

// Store to memory with release ordering.
template<typename T>
void Atomic<T>::StoreRelease(
    /* [in] */ T desired)
{
    this->store(desired, std::memory_order_release);
}

// Store to memory with a total ordering.
template<typename T>
void Atomic<T>::StoreSequentiallyConsistent(
    /* [in] */ T desired)
{
    this->store(desired, std::memory_order_seq_cst);
}

// Atomically replace the value with desired value.
template<typename T>
T Atomic<T>::ExchangeRelaxed(
    /* [in] */ T desired)
{
    return this->exchange(desired, std::memory_order_relaxed);
}

// Atomically replace the value with desired value if it matches the expected value.
// Participates in total ordering of atomic operations.
template<typename T>
Boolean Atomic<T>::CompareExchangeStrongSequentiallyConsistent(
    /* [in] */ T expected,
    /* [in] */ T desired)
{
    return this->compare_exchange_strong(expected, desired, std::memory_order_seq_cst);
}

// The same, except it may fail spuriously.
template<typename T>
Boolean Atomic<T>::CompareExchangeWeakSequentiallyConsistent(
    /* [in] */ T expected,
    /* [in] */ T desired)
{
    return this->compare_exchange_weak(expected, desired, std::memory_order_seq_cst);
}

// Atomically replace the value with desired value if it matches the expected value. Doesn't
// imply ordering or synchronization constraints.
template<typename T>
Boolean Atomic<T>::CompareExchangeStrongRelaxed(
    /* [in] */ T expected,
    /* [in] */ T desired)
{
    return this->compare_exchange_strong(expected, desired, std::memory_order_relaxed);
}

// Atomically replace the value with desired value if it matches the expected value. Prior writes
// to other memory locations become visible to the threads that do a consume or an acquire on the
// same location.
template<typename T>
Boolean Atomic<T>::CompareExchangeStrongRelease(
    /* [in] */ T expected,
    /* [in] */ T desired)
{
    return this->compare_exchange_strong(expected, desired, std::memory_order_release);
}

// The same, except it may fail spuriously.
template<typename T>
Boolean Atomic<T>::CompareExchangeWeakRelaxed(
    /* [in] */ T expected,
    /* [in] */ T desired)
{
    return this->compare_exchange_weak(expected, desired, std::memory_order_relaxed);
}

// Atomically replace the value with desired value if it matches the expected value. Prior writes
// made to other memory locations by the thread that did the release become visible in this
// thread.
template<typename T>
Boolean Atomic<T>::CompareExchangeWeakAcquire(
    /* [in] */ T expected,
    /* [in] */ T desired)
{
    return this->compare_exchange_weak(expected, desired, std::memory_order_acquire);
}

// Atomically replace the value with desired value if it matches the expected value. prior writes
// to other memory locations become visible to the threads that do a consume or an acquire on the
// same location.
template<typename T>
Boolean Atomic<T>::CompareExchangeWeakRelease(
    /* [in] */ T expected,
    /* [in] */ T desired)
{
    return this->compare_exchange_weak(expected, desired, std::memory_order_release);
}

template<typename T>
T Atomic<T>::FetchAndAddSequentiallyConsistent(
    /* [in] */ const T value)
{
    return this->fetch_add(value, std::memory_order_seq_cst);  // Return old_value.
}

template<typename T>
T Atomic<T>::FetchAndAddRelaxed(
    /* [in] */ const T value)
{
    return this->fetch_add(value, std::memory_order_relaxed);  // Return old_value.
}

template<typename T>
T Atomic<T>::FetchAndSubSequentiallyConsistent(
    /* [in] */ const T value)
{
    return this->fetch_sub(value, std::memory_order_seq_cst);  // Return old value.
}

template<typename T>
T Atomic<T>::FetchAndSubRelaxed(
    /* [in] */ const T value)
{
    return this->fetch_sub(value, std::memory_order_relaxed);  // Return old value.
}

template<typename T>
T Atomic<T>::FetchAndOrSequentiallyConsistent(
    /* [in] */ const T value)
{
    return this->fetch_or(value, std::memory_order_seq_cst);  // Return old_value.
}

template<typename T>
T Atomic<T>::FetchAndAndSequentiallyConsistent(
    /* [in] */ const T value)
{
    return this->fetch_and(value, std::memory_order_seq_cst);  // Return old_value.
}

template<typename T>
volatile T* Atomic<T>::Address()
{
    return reinterpret_cast<T*>(this);
}

template<typename T>
T Atomic<T>::MaxValue()
{
    return std::numeric_limits<T>::max();
}

typedef Atomic<int32_t> AtomicInteger;
typedef Atomic<int64_t> AtomicLong;

}
}

#endif // __COMO_CORE_NATIVEATOMIC_H__
