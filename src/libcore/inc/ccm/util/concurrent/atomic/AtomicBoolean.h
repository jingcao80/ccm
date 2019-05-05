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

#ifndef __CCM_UTIL_CONCURRENT_ATOMIC_ATOMICBOOLEAN_H__
#define __CCM_UTIL_CONCURRENT_ATOMIC_ATOMICBOOLEAN_H__

#include "ccm/core/NativeAtomic.h"
#include "ccm/core/SyncObject.h"
#include "ccm.io.ISerializable.h"
#include "ccm.util.concurrent.atomic.IAtomicBoolean.h"

using ccm::core::SyncObject;
using ccm::io::ISerializable;

namespace ccm {
namespace util {
namespace concurrent {
namespace atomic {

class AtomicBoolean
    : public SyncObject
    , public IAtomicBoolean
    , public ISerializable
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Boolean initialValue);

    ECode Constructor();

    ECode Get(
        /* [out] */ Boolean* value) override final;

    ECode CompareAndSet(
        /* [in] */ Boolean expect,
        /* [in] */ Boolean update,
        /* [out] */ Boolean* succeeded = nullptr) override final;

    ECode WeakCompareAndSet(
        /* [in] */ Boolean expect,
        /* [in] */ Boolean update,
        /* [out] */ Boolean* succeeded = nullptr) override;

    ECode Set(
        /* [in] */ Boolean value) override final;

    ECode LzaySet(
        /* [in] */ Boolean value) override final;

    ECode GetAndSet(
        /* [in] */ Boolean newValue,
        /* [out] */ Boolean* prevValue) override final;

    ECode ToString(
        /* [out] */ String* desc) override;

private:
    ccm::core::AtomicInteger mValue;
};

}
}
}
}

#endif // __CCM_UTIL_CONCURRENT_ATOMIC_ATOMICBOOLEAN_H__
