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

#include "como/core/StringUtils.h"
#include "como/util/concurrent/atomic/AtomicInteger.h"

using como::core::IID_INumber;
using como::core::StringUtils;
using como::io::IID_ISerializable;

namespace como {
namespace util {
namespace concurrent {
namespace atomic {

COMO_INTERFACE_IMPL_3(AtomicInteger, SyncObject, IAtomicInteger, INumber, ISerializable);

ECode AtomicInteger::Constructor(
    /* [in] */ Integer initialValue)
{
    mValue.StoreRelaxed(initialValue);
    return NOERROR;
}

ECode AtomicInteger::Constructor()
{
    return NOERROR;
}

ECode AtomicInteger::Get(
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mValue.LoadAcquire();
    return NOERROR;
}

ECode AtomicInteger::Set(
    /* [in] */ Integer value)
{
    mValue.StoreSequentiallyConsistent(value);
    return NOERROR;
}

ECode AtomicInteger::LzaySet(
    /* [in] */ Integer value)
{
    mValue.StoreRelease(value);
    return NOERROR;
}

ECode AtomicInteger::GetAndSet(
    /* [in] */ Integer newValue,
    /* [out] */ Integer* prevValue)
{
    VALIDATE_NOT_NULL(prevValue);

    Integer v;
    do {
        v = mValue.LoadSequentiallyConsistent();
    } while (!mValue.CompareExchangeStrongSequentiallyConsistent(v, newValue));
    *prevValue = v;
    return NOERROR;
}

ECode AtomicInteger::CompareAndSet(
    /* [in] */ Integer expect,
    /* [in] */ Integer update,
    /* [out] */ Boolean* succeeded)
{
    Boolean result = mValue.CompareExchangeStrongSequentiallyConsistent(
            expect, update);
    if (succeeded != nullptr) {
        *succeeded = result;
    }
    return NOERROR;
}

ECode AtomicInteger::WeakCompareAndSet(
    /* [in] */ Integer expect,
    /* [in] */ Integer update,
    /* [out] */ Boolean* succeeded)
{
    Boolean result = mValue.CompareExchangeStrongSequentiallyConsistent(
            expect, update);
    if (succeeded != nullptr) {
        *succeeded = result;
    }
    return NOERROR;
}

ECode AtomicInteger::GetAndIncrement(
    /* [out] */ Integer* prevValue)
{
    VALIDATE_NOT_NULL(prevValue);

    Integer v;
    do {
        v = mValue.LoadSequentiallyConsistent();
    } while (!mValue.CompareExchangeStrongSequentiallyConsistent(v, v + 1));
    *prevValue = v;
    return NOERROR;
}

ECode AtomicInteger::GetAndDecrement(
    /* [out] */ Integer* prevValue)
{
    VALIDATE_NOT_NULL(prevValue);

    Integer v;
    do {
        v = mValue.LoadSequentiallyConsistent();
    } while (!mValue.CompareExchangeStrongSequentiallyConsistent(v, v - 1));
    *prevValue = v;
    return NOERROR;
}

ECode AtomicInteger::GetAndAdd(
    /* [in] */ Integer delta,
    /* [out] */ Integer* prevValue)
{
    VALIDATE_NOT_NULL(prevValue);

    Integer v;
    do {
        v = mValue.LoadSequentiallyConsistent();
    } while (!mValue.CompareExchangeStrongSequentiallyConsistent(v, v + delta));
    *prevValue = v;
    return NOERROR;
}

ECode AtomicInteger::IncrementAndGet(
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    Integer v;
    do {
        v = mValue.LoadSequentiallyConsistent();
    } while (!mValue.CompareExchangeStrongSequentiallyConsistent(v, v + 1));
    *value = v + 1;
    return NOERROR;
}

ECode AtomicInteger::DecrementAndGet(
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    Integer v;
    do {
        v = mValue.LoadSequentiallyConsistent();
    } while (!mValue.CompareExchangeStrongSequentiallyConsistent(v, v - 1));
    *value = v - 1;
    return NOERROR;
}

ECode AtomicInteger::AddAndGet(
    /* [in] */ Integer delta,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    Integer v;
    do {
        v = mValue.LoadSequentiallyConsistent();
    } while (!mValue.CompareExchangeStrongSequentiallyConsistent(v, v + delta));
    *value = v + delta;
    return NOERROR;
}

ECode AtomicInteger::ToString(
    /* [out] */ String* desc)
{
    VALIDATE_NOT_NULL(desc);

    *desc = StringUtils::ToString(mValue.LoadAcquire());
    return NOERROR;
}

ECode AtomicInteger::IntegerValue(
    /* [out] */ Integer* value)
{
    return Get(value);
}

ECode AtomicInteger::LongValue(
    /* [out] */ Long* value)
{
    VALIDATE_NOT_NULL(value);

    Integer v;
    Get(&v);
    *value = (Long)v;
    return NOERROR;
}

ECode AtomicInteger::FloatValue(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    Integer v;
    Get(&v);
    *value = (Float)v;
    return NOERROR;
}

ECode AtomicInteger::DoubleValue(
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);

    Integer v;
    Get(&v);
    *value = (Double)v;
    return NOERROR;
}

ECode AtomicInteger::ByteValue(
    /* [out] */ Byte* value)
{
    VALIDATE_NOT_NULL(value);

    Integer v;
    Get(&v);
    *value = (Byte)v;
    return NOERROR;
}

ECode AtomicInteger::ShortValue(
    /* [out] */ Short* value)
{
    VALIDATE_NOT_NULL(value);

    Integer v;
    Get(&v);
    *value = (Short)v;
    return NOERROR;
}

}
}
}
}
