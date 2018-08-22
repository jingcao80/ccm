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

#ifndef __CCM_CORE_VOLATILE_H__
#define __CCM_CORE_VOLATILE_H__

#include "ccm/core/NativeAtomic.h"

using namespace ccm;

#define COMPARE_AND_SWAP_INT_INADDR(intAddr, expectedValue, newValue) \
    reinterpret_cast<ccm::core::AtomicInteger*>(intAddr)-> \
        CompareExchangeStrongSequentiallyConsistent( \
            expectedValue, newValue)

#define COMPARE_AND_SWAP_INT(object, field, expectedValue, newValue) \
    COMPARE_AND_SWAP_INT_INADDR(&object->field, expectedValue, newValue)

inline Integer GetAndAddInt(Integer* addr, Integer delta)
{
    Integer v;
    do {
        v = reinterpret_cast<ccm::core::AtomicInteger*>(addr)->LoadSequentiallyConsistent();
    } while (!COMPARE_AND_SWAP_INT_INADDR(addr, v, v + delta));
    return v;
}

#define GET_AND_ADD_INT_INADDR(intAddr, delta) \
    GetAndAddInt(intAddr, delta)

#define GET_AND_ADD_INT(object, field, delta) \
    GET_AND_ADD_INT_INADDR(&object->field, delta)

#define COMPARE_AND_SWAP_LONG_INADDR(longAddr, expectedValue, newValue) \
    reinterpret_cast<ccm::core::AtomicLong*>(longAddr)-> \
        CompareExchangeStrongSequentiallyConsistent( \
            expectedValue, newValue)

#define COMPARE_AND_SWAP_LONG(object, field, expectedValue, newValue) \
    COMPARE_AND_SWAP_LONG_INADDR(&object->field, expectedValue, newValue)


#define PUT_OBJECT(object, field, value) \
    object->field = value;

#define PUT_ORDERED_OBJECT(object, field, value) \
    ccm::core::QuasiAtomic::ThreadFenceRelease(); \
    object->field = value;

#define COMPARE_AND_SWAP_OBJECT_INADDR(handleAddr, expectedValue, newValue) \
    reinterpret_cast<ccm::core::AtomicLong*>(handleAddr)-> \
        CompareExchangeStrongSequentiallyConsistent( \
            reinterpret_cast<int64_t>(expectedValue), \
            reinterpret_cast<int64_t>(newValue));

#define COMPARE_AND_SWAP_OBJECT(object, field, expectedValue, newValue) \
    COMPARE_AND_SWAP_OBJECT_INADDR(&object->field, expectedValue, newValue)

#define VOLATILE

#define VOLATILE_GET(lvalue, variable) \
        lvalue = variable; \
        ccm::core::QuasiAtomic::ThreadFenceAcquire();

#define VOLATILE_SET(variable, rvalue) \
        ccm::core::QuasiAtomic::ThreadFenceRelease(); \
        variable = rvalue; \
        ccm::core::QuasiAtomic::ThreadFenceSequentiallyConsistent();

#define VOLATILE_ARRAY_SET(array, index, value) \
        ccm::core::QuasiAtomic::ThreadFenceRelease(); \
        array.Set(index, value); \
        ccm::core::QuasiAtomic::ThreadFenceSequentiallyConsistent();

#define VOLATILE_GET_INLINE(lvalue, variable) \
        lvalue = variable, ccm::core::QuasiAtomic::ThreadFenceAcquire()

template<typename T>
inline Boolean VOLATILE_EQUALS(T& lvalue, T& rvalue)
{
    Boolean equal = lvalue == rvalue;
    ccm::core::QuasiAtomic::ThreadFenceAcquire();
    return equal;
}

template<typename T>
inline Boolean VOLATILE_EQUALS(T& lvalue, T&& rvalue)
{
    Boolean equal = lvalue == rvalue;
    ccm::core::QuasiAtomic::ThreadFenceAcquire();
    return equal;
}

#endif // __CCM_CORE_VOLATILE_H__
