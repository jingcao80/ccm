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

#ifndef __CCM_UTIL_ABSTRACTCOLLECTION_H__
#define __CCM_UTIL_ABSTRACTCOLLECTION_H__

#include "ccm/core/SyncObject.h"
#include "ccm.core.IInteger.h"
#include "ccm.core.IIterable.h"
#include "ccm.util.ICollection.h"

using ccm::core::IInteger;
using ccm::core::IIterable;
using ccm::core::SyncObject;

namespace ccm {
namespace util {

class AbstractCollection
    : public SyncObject
    , public ICollection
    , public IIterable
{
public:
    CCM_INTERFACE_DECL();

    ECode GetIterator(
        /* [out] */ IIterator** it) = 0;

    ECode IsEmpty(
        /* [out] */ Boolean* empty) override;

    ECode Contains(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result) override;

    ECode ToArray(
        /* [out, callee] */ Array<IInterface*>* objs) override;

    ECode ToArray(
        /* [in] */ const InterfaceID& iid,
        /* [out, callee] */ Array<IInterface*>* objs) override;

    ECode Add(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode Remove(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode ContainsAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* result) override;

    ECode AddAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode RemoveAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode RetainAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode Clear() override;

    ECode ToString(
        /* [out] */ String* str) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result) override;

    ECode GetHashCode(
        /* [out] */ Integer* hash) override;

private:
    static ECode FinishToArray(
        /* [in] */ Array<IInterface*>& r,
        /* [in] */ IIterator* it,
        /* [out, callee] */ Array<IInterface*>* objs);

    static ECode FinishToArray(
        /* [in] */ Array<IInterface*>& r,
        /* [in] */ IIterator* it,
        /* [in] */ const InterfaceID& iid,
        /* [out, callee] */ Array<IInterface*>* objs);

    static ECode HugeCapacity(
        /* [in] */ Integer minCapacity,
        /* [out] */ Integer* newCapacity);

private:
    static constexpr Integer MAX_ARRAY_SIZE = IInteger::MAX_VALUE - 8;
};

}
}

#endif // __CCM_UTIL_ABSTRACTCOLLECTION_H__
