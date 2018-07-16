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

#ifndef __CCM_UTIL_ARRAYLIST_H__
#define __CCM_UTIL_ARRAYLIST_H__

#include "ccm/util/AbstractList.h"
#include "ccm.core.ICloneable.h"
#include "ccm.core.IInteger.h"
#include "ccm.io.ISerializable.h"
#include "ccm.util.IArrayList.h"
#include "ccm.util.IRandomAccess.h"

using ccm::core::ICloneable;
using ccm::core::IInteger;
using ccm::io::ISerializable;

namespace ccm {
namespace util {

class ArrayList
    : public AbstractList
    , public IArrayList
    , public IRandomAccess
    , public ICloneable
    , public ISerializable

{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Integer initialCapacity);

    ECode Constructor();

    ECode Constructor(
        /* [in] */ ICollection* c);

    ECode TrimToSize() override;

    ECode EnsureCapacity(
        /* [in] */ Integer minCapacity) override;

    ECode GetSize(
        /* [out] */ Integer* size) override;

    ECode IsEmpty(
        /* [out] */ Boolean* empty) override;

    ECode Contains(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result) override;

    ECode IndexOf(
        /* [in] */ IInterface* obj,
        /* [out] */ Integer* index) override;

    ECode LastIndexOf(
        /* [in] */ IInterface* obj,
        /* [out] */ Integer* index) override;

    ECode ToArray(
        /* [out, callee] */ Array<IInterface*>* objs) override;

    ECode ToArray(
        /* [in] */ const InterfaceID& iid,
        /* [out, callee] */ Array<IInterface*>* objs) override;

    ECode Get(
        /* [in] */ Integer index,
        /* [out] */ IInterface** obj) override;

    ECode Set(
        /* [in] */ Integer index,
        /* [in] */ IInterface* obj,
        /* [out] */ IInterface** prevObj = nullptr) override;

    ECode Add(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result = nullptr) override;

    ECode Add(
        /* [in] */ Integer index,
        /* [in] */ IInterface* obj) override;

    ECode Remove(
        /* [in] */ Integer index,
        /* [out] */ IInterface** obj = nullptr) override;

    ECode Remove(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result = nullptr) override;

    ECode Clear() override;

    ECode AddAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* result = nullptr) override;

    ECode AddAll(
        /* [in] */ Integer index,
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* result = nullptr) override;

private:
    void EnsureCapacityInternal(
        /* [in] */ Integer minCapacity);

    void EnsureExplicitCapacity(
        /* [in] */ Integer minCapacity);

    void Grow(
        /* [in] */ Integer minCapacity);

    static Integer HugeCapacity(
        /* [in] */ Integer minCapacity);

    ECode CloneImpl(
        /* [in] */ IArrayList* newObj);

    void FastRemove(
        /* [in] */ Integer index);

    String OutOfBoundsMsg(
        /* [in] */ Integer index);

    static Array<IInterface*> GetEMPTY_ELEMENTDATA();

    static Array<IInterface*> GetDEFAULTCAPACITY_EMPTY_ELEMENTDATA();

private:
    /**
     * Default initial capacity.
     */
    static constexpr Integer DEFAULT_CAPACITY = 10;

    /**
     * The array buffer into which the elements of the ArrayList are stored.
     * The capacity of the ArrayList is the length of this array buffer. Any
     * empty ArrayList with elementData == DEFAULTCAPACITY_EMPTY_ELEMENTDATA
     * will be expanded to DEFAULT_CAPACITY when the first element is added.
     */
    Array<IInterface*> mElementData;

    /**
     * The size of the ArrayList (the number of elements it contains).
     */
    Integer mSize;

    static constexpr Integer MAX_ARRAY_SIZE = IInteger::MAX_VALUE - 8;
};

}
}

#endif //__CCM_UTIL_ARRAYLIST_H__
