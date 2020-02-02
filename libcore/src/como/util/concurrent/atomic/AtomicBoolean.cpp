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

#include "como/util/concurrent/atomic/AtomicBoolean.h"

using como::io::IID_ISerializable;

namespace como {
namespace util {
namespace concurrent {
namespace atomic {

COMO_INTERFACE_IMPL_2(AtomicBoolean, SyncObject, IAtomicBoolean, ISerializable);

ECode AtomicBoolean::Constructor(
    /* [in] */ Boolean initialValue)
{
    mValue.StoreRelaxed(initialValue ? 1 : 0);
    return NOERROR;
}

ECode AtomicBoolean::Constructor()
{
    return Constructor(false);
}

ECode AtomicBoolean::Get(
    /* [out] */ Boolean& value)
{
    value = (mValue.LoadAcquire() != 0);
    return NOERROR;
}

ECode AtomicBoolean::CompareAndSet(
    /* [in] */ Boolean expect,
    /* [in] */ Boolean update,
    /* [out] */ Boolean* succeeded)
{
    Boolean result = mValue.CompareExchangeStrongSequentiallyConsistent(
            (expect ? 1 : 0), (update ? 1 : 0));
    if (succeeded != nullptr) {
        *succeeded = result;
    }
    return NOERROR;
}

ECode AtomicBoolean::WeakCompareAndSet(
    /* [in] */ Boolean expect,
    /* [in] */ Boolean update,
    /* [out] */ Boolean* succeeded)
{
    Boolean result = mValue.CompareExchangeStrongSequentiallyConsistent(
            (expect ? 1 : 0), (update ? 1 : 0));
    if (succeeded != nullptr) {
        *succeeded = result;
    }
    return NOERROR;
}

ECode AtomicBoolean::Set(
    /* [in] */ Boolean value)
{
    mValue.StoreSequentiallyConsistent(value ? 1 : 0);
    return NOERROR;
}

ECode AtomicBoolean::LzaySet(
    /* [in] */ Boolean value)
{
    mValue.StoreRelease(value ? 1 : 0);
    return NOERROR;
}

ECode AtomicBoolean::GetAndSet(
    /* [in] */ Boolean newValue,
    /* [out] */ Boolean& prevValue)
{
    Boolean v;
    do {
        v = (mValue.LoadSequentiallyConsistent() != 0);
    } while (!mValue.CompareExchangeStrongSequentiallyConsistent(
            (v ? 1 : 0), (newValue ? 1 : 0)));
    prevValue = v;
    return NOERROR;
}

ECode AtomicBoolean::ToString(
    /* [out] */ String& desc)
{
    desc = (mValue.LoadAcquire() != 0 ? "true" : "false");
    return NOERROR;
}

}
}
}
}
