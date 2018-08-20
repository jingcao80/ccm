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

#ifndef __CCM_COREDEF_H__
#define __CCM_COREDEF_H__

#define FOR_EACH(Type, element, TypeCastFunc, container) \
    { \
        AutoPtr<IIterator> it; \
        container->GetIterator((IIterator**)&it); \
        Boolean hasNext; \
        while (it->HasNext(&hasNext), hasNext) { \
            AutoPtr<IInterface> obj; \
            it->Next((IInterface**)&obj); \
            Type element = (Type)TypeCastFunc(obj);

#define END_FOR_EACH() }}

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


#endif // __CCM_COREDEF_H__
