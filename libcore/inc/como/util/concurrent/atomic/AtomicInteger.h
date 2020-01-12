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

#ifndef __COMO_UTIL_CONCURRENT_ATOMIC_ATOMICINTEGER_H__
#define __COMO_UTIL_CONCURRENT_ATOMIC_ATOMICINTEGER_H__

#include "como/core/NativeAtomic.h"
#include "como/core/SyncObject.h"
#include "como.core.INumber.h"
#include "como.io.ISerializable.h"
#include "como.util.concurrent.atomic.IAtomicInteger.h"

using como::core::SyncObject;
using como::core::INumber;
using como::io::ISerializable;

namespace como {
namespace util {
namespace concurrent {
namespace atomic {

class AtomicInteger
    : public SyncObject
    , public IAtomicInteger
    , public INumber
    , public ISerializable
{
public:
    COMO_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Integer initialValue);

    ECode Constructor();

    ECode Get(
        /* [out] */ Integer* value) override;

    ECode Set(
        /* [in] */ Integer value) override;

    ECode LzaySet(
        /* [in] */ Integer value) override;

    ECode GetAndSet(
        /* [in] */ Integer newValue,
        /* [out] */ Integer* prevValue) override;

    ECode CompareAndSet(
        /* [in] */ Integer expect,
        /* [in] */ Integer update,
        /* [out] */ Boolean* succeeded = nullptr) override;

    ECode WeakCompareAndSet(
        /* [in] */ Integer expect,
        /* [in] */ Integer update,
        /* [out] */ Boolean* succeeded = nullptr) override;

    ECode GetAndIncrement(
        /* [out] */ Integer* prevValue) override;

    ECode GetAndDecrement(
        /* [out] */ Integer* prevValue) override;

    ECode GetAndAdd(
        /* [in] */ Integer delta,
        /* [out] */ Integer* prevValue) override;

    ECode IncrementAndGet(
        /* [out] */ Integer* value) override;

    ECode DecrementAndGet(
        /* [out] */ Integer* value) override;

    ECode AddAndGet(
        /* [in] */ Integer delta,
        /* [out] */ Integer* value) override;

    ECode ToString(
        /* [out] */ String& desc) override;

    ECode IntegerValue(
        /* [out] */ Integer* value) override;

    ECode LongValue(
        /* [out] */ Long* value) override;

    ECode FloatValue(
        /* [out] */ Float* value) override;

    ECode DoubleValue(
        /* [out] */ Double* value) override;

    ECode ByteValue(
        /* [out] */ Byte* value) override;

    ECode ShortValue(
        /* [out] */ Short* value) override;

private:
    como::core::AtomicInteger mValue;
};

}
}
}
}

#endif // __COMO_UTIL_CONCURRENT_ATOMIC_ATOMICINTEGER_H__
