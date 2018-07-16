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

#ifndef __CCM_UTIL_CARRAYLIST_H__
#define __CCM_UTIL_CARRAYLIST_H__

#include "core/SyncObject.h"
#include "ccm.core.ICloneable.h"
#include "ccm.core.IIterable.h"
#include "ccm.io.ISerializable.h"
#include "ccm.util.IArrayList.h"
#include "ccm.util.ICollection.h"
#include "ccm.util.IList.h"
#include "ccm.util.IRandomAccess.h"
#include "_ccm_util_CArrayList.h"

using ccm::core::IIterable;
using ccm::core::ICloneable;
using ccm::core::SyncObject;
using ccm::io::ISerializable;

namespace ccm {
namespace util {

Coclass(CArrayList)
    , public SyncObject
    , public IArrayList
    , public IList
    , public ICollection
    , public IIterable
    , public IRandomAccess
    , public ICloneable
    , public ISerializable
{
public:
    CCM_INTERFACE_DECL();

    CCM_OBJECT_DECL();

    ECode Constructor();

    ECode Add(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result = nullptr) override;

    ECode Add(
        /* [in] */ Integer index,
        /* [in] */ IInterface* obj) override;

    ECode AddAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* result = nullptr) override;

    ECode AddAll(
        /* [in] */ Integer index,
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* result = nullptr) override;

    ECode Clear() override;

    ECode Contains(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result) override;

    ECode ContainsAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* result) override;

    ECode EnsureCapacity(
        /* [in] */ Integer minCapacity) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result) override;

    ECode Get(
        /* [in] */ Integer index,
        /* [out] */ IInterface** obj) override;

    ECode GetHashCode(
        /* [out] */ Integer* hash) override;

    ECode GetIterator(
        /* [out] */ IIterator** it) override;

    ECode GetListIterator(
        /* [out] */ IListIterator** it) override;

    ECode GetListIterator(
        /* [in] */ Integer index,
        /* [out] */ IListIterator** it) override;

    ECode GetSize(
        /* [out] */ Integer* size) override;

    ECode IndexOf(
        /* [in] */ IInterface* obj,
        /* [out] */ Integer* index) override;

    ECode IsEmpty(
        /* [out] */ Boolean* empty) override;

    ECode LastIndexOf(
        /* [in] */ IInterface* obj,
        /* [out] */ Integer* index) override;

    ECode Remove(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result = nullptr) override;

    ECode Remove(
        /* [in] */ Integer index,
        /* [out] */ IInterface** obj = nullptr) override;

    ECode RemoveAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* result = nullptr) override;

    ECode RetainAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* result = nullptr) override;

    ECode Set(
        /* [in] */ Integer index,
        /* [in] */ IInterface* obj,
        /* [out] */ IInterface** prevObj = nullptr) override;

    ECode SubList(
        /* [in] */ Integer fromIndex,
        /* [in] */ Integer toIndex,
        /* [out] */ IList** subList) override;

    ECode ToArray(
        /* [out, callee] */ Array<IInterface*>* objs) override;

    ECode ToArray(
        /* [in] */ const InterfaceID& iid,
        /* [out, callee] */ Array<IInterface*>* objs) override;

    ECode TrimToSize() override;

    ECode Clone(
        /* [out] */ IInterface** obj) override;
};

}
}

#endif //__CCM_UTIL_CARRAYLIST_H__
