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

#ifndef __CCM_UTIL_CONCURRENT_ATOMIC_ATOMICLONG_H__
#define __CCM_UTIL_CONCURRENT_ATOMIC_ATOMICLONG_H__

#include "ccm/core/NativeAtomic.h"
#include "ccm/core/SyncObject.h"
#include "ccm.core.INumber.h"
#include "ccm.io.ISerializable.h"
#include "ccm.util.concurrent.atomic.IAtomicLong.h"

using ccm::core::SyncObject;
using ccm::core::INumber;
using ccm::io::ISerializable;

namespace ccm {
namespace util {
namespace concurrent {
namespace atomic {

class AtomicLong
    : public SyncObject
    , public IAtomicLong
    , public INumber
    , public ISerializable
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Long initialValue);

    ECode Constructor();

    ECode Get(
        /* [out] */ Long* value) override;

    ECode Set(
        /* [in] */ Long value) override;

    ECode LzaySet(
        /* [in] */ Long value) override;

    ECode GetAndSet(
        /* [in] */ Long newValue,
        /* [out] */ Long* prevValue) override;

    ECode CompareAndSet(
        /* [in] */ Long expect,
        /* [in] */ Long update,
        /* [out] */ Boolean* succeeded = nullptr) override;

    ECode WeakCompareAndSet(
        /* [in] */ Long expect,
        /* [in] */ Long update,
        /* [out] */ Boolean* succeeded = nullptr) override;

    ECode GetAndIncrement(
        /* [out] */ Long* prevValue) override;

    ECode GetAndDecrement(
        /* [out] */ Long* prevValue) override;

    ECode GetAndAdd(
        /* [in] */ Long delta,
        /* [out] */ Long* prevValue) override;

    ECode IncrementAndGet(
        /* [out] */ Long* value) override;

    ECode DecrementAndGet(
        /* [out] */ Long* value) override;

    ECode AddAndGet(
        /* [in] */ Long delta,
        /* [out] */ Long* value) override;

    ECode ToString(
        /* [out] */ String* desc) override;

    ECode IntValue(
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
    ccm::core::AtomicLong mValue;
};

}
}
}
}

#endif // __CCM_UTIL_CONCURRENT_ATOMIC_ATOMICLONG_H__
