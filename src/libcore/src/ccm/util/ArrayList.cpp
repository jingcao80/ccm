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

#include "ccm/core/Math.h"
#include "ccm/util/Arrays.h"
#include "ccm/util/ArrayList.h"
#include <ccmlogger.h>

using ccm::core::IID_ICloneable;
using ccm::core::Math;
using ccm::io::IID_ISerializable;

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_4(ArrayList, AbstractList, IArrayList, IRandomAccess, ICloneable, ISerializable);

Array<IInterface*> ArrayList::GetEMPTY_ELEMENTDATA()
{
    static const Array<IInterface*> EMPTY_ELEMENTDATA(0);
    return EMPTY_ELEMENTDATA;
}

Array<IInterface*> ArrayList::GetDEFAULTCAPACITY_EMPTY_ELEMENTDATA()
{
    static const Array<IInterface*> DEFAULTCAPACITY_EMPTY_ELEMENTDATA(0);
    return DEFAULTCAPACITY_EMPTY_ELEMENTDATA;
}

ECode ArrayList::Constructor(
    /* [in] */ Integer initialCapacity)
{
    if (initialCapacity > 0) {
        mElementData = Array<IInterface*>(initialCapacity);
    }
    else if (initialCapacity == 0) {
        mElementData = GetEMPTY_ELEMENTDATA();
    }
    Logger::E("ArrayList", "Illegal Capacity: %d", initialCapacity);
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode ArrayList::Constructor()
{
    mElementData = GetDEFAULTCAPACITY_EMPTY_ELEMENTDATA();
    return NOERROR;
}

ECode ArrayList::Constructor(
    /* [in] */ ICollection* c)
{
    c->ToArray(&mElementData);
    if (mSize = mElementData.GetLength(), mSize == 0) {
        mElementData = GetEMPTY_ELEMENTDATA();
    }
    return NOERROR;
}

ECode ArrayList::TrimToSize()
{
    mModCount++;
    if (mSize < mElementData.GetLength()) {
        if (mSize != 0) {
            Arrays::CopyOf(mElementData, mSize, &mElementData);
        }
        else {
            mElementData = GetEMPTY_ELEMENTDATA();
        }
    }
    return NOERROR;
}

ECode ArrayList::EnsureCapacity(
    /* [in] */ Integer minCapacity)
{
    Integer minExpand = (mElementData != GetDEFAULTCAPACITY_EMPTY_ELEMENTDATA())
            // any size if not default element table
            ? 0
            // larger than default for default empty table. It's already
            // supposed to be at default size.
            : DEFAULT_CAPACITY;

    if (minCapacity > minExpand) {
        EnsureExplicitCapacity(minCapacity);
    }
    return NOERROR;
}

void ArrayList::EnsureCapacityInternal(
    /* [in] */ Integer minCapacity)
{
    if (mElementData == GetDEFAULTCAPACITY_EMPTY_ELEMENTDATA()) {
        minCapacity = Math::Max(DEFAULT_CAPACITY, minCapacity);
    }

    EnsureExplicitCapacity(minCapacity);
}

void ArrayList::EnsureExplicitCapacity(
    /* [in] */ Integer minCapacity)
{
    mModCount++;

    if (minCapacity - mElementData.GetLength() > 0) {
        Grow(minCapacity);
    }
}

void ArrayList::Grow(
    /* [in] */ Integer minCapacity)
{
    Integer oldCapacity = mElementData.GetLength();
    Integer newCapacity = oldCapacity + (oldCapacity >> 1);
    if (newCapacity - minCapacity < 0) {
        newCapacity = minCapacity;
    }
    if (newCapacity - MAX_ARRAY_SIZE > 0) {
        newCapacity = HugeCapacity(minCapacity);
    }
    Arrays::CopyOf(mElementData, newCapacity, &mElementData);
}

Integer ArrayList::HugeCapacity(
    /* [in] */ Integer minCapacity)
{
    return (minCapacity > MAX_ARRAY_SIZE) ?
            IInteger::MAX_VALUE : MAX_ARRAY_SIZE;
}

ECode ArrayList::GetSize(
    /* [out] */ Integer* size)
{
    VALIDATE_NOT_NULL(size);

    *size = mSize;
    return NOERROR;
}

ECode ArrayList::IsEmpty(
    /* [out] */ Boolean* empty)
{
    VALIDATE_NOT_NULL(empty);

    *empty = mSize == 0;
    return NOERROR;
}

ECode ArrayList::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Integer index;
    IndexOf(obj, &index);
    *result = index >= 0;
    return NOERROR;
}

ECode ArrayList::IndexOf(
    /* [in] */ IInterface* obj,
    /* [out] */ Integer* index)
{
    VALIDATE_NOT_NULL(index);

    if (obj == nullptr) {
        for (Integer i = 0; i < mSize; i++) {
            if (mElementData[i] == nullptr) {
                *index = i;
                return NOERROR;
            }
        }
    }
    else {
        for (Integer i = 0; i < mSize; i++) {
            if (Object::Equals(obj, mElementData[i])) {
                *index = i;
                return NOERROR;
            }
        }
    }
    *index = -1;
    return NOERROR;
}

ECode ArrayList::LastIndexOf(
    /* [in] */ IInterface* obj,
    /* [out] */ Integer* index)
{
    VALIDATE_NOT_NULL(index);

    if (obj == nullptr) {
        for (Integer i = mSize - 1; i >= 0; i--) {
            if (mElementData[i] == nullptr) {
                *index = i;
                return NOERROR;
            }
        }
    }
    else {
        for (Integer i = mSize - 1; i >= 0; i--) {
            if (Object::Equals(obj, mElementData[i])) {
                *index = i;
                return NOERROR;
            }
        }
    }
    *index = -1;
    return NOERROR;
}

ECode ArrayList::CloneImpl(
    /* [in] */ IArrayList* newObj)
{
    ArrayList* v = (ArrayList*)newObj;
    Arrays::CopyOf(mElementData, mSize, &v->mElementData);
    v->mModCount = 0;
    return NOERROR;
}

ECode ArrayList::ToArray(
    /* [out, callee] */ Array<IInterface*>* objs)
{
    return Arrays::CopyOf(mElementData, mSize, objs);
}

ECode ArrayList::ToArray(
    /* [in] */ const InterfaceID& iid,
    /* [out, callee] */ Array<IInterface*>* objs)
{
    VALIDATE_NOT_NULL(objs);

    Array<IInterface*> newArray(mSize);
    for (Integer i = 0; i < mSize; i++) {
        newArray.Set(i, mElementData[i] != nullptr ?
                mElementData[i]->Probe(iid) : nullptr);
    }
    *objs = newArray;
    return NOERROR;
}

ECode ArrayList::Get(
    /* [in] */ Integer index,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    if (index > mSize) {
        Logger::E("ArrayList", "%s", OutOfBoundsMsg(index).string());
        return ccm::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *obj = mElementData[index];
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode ArrayList::Set(
    /* [in] */ Integer index,
    /* [in] */ IInterface* obj,
    /* [out] */ IInterface** prevObj)
{
    if (index > mSize) {
        Logger::E("ArrayList", "%s", OutOfBoundsMsg(index).string());
        return ccm::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (prevObj != nullptr) {
        *prevObj = mElementData[index];
        REFCOUNT_ADD(*prevObj);
    }
    mElementData.Set(index, obj);
    return NOERROR;
}

ECode ArrayList::Add(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    EnsureCapacityInternal(mSize + 1);
    mElementData.Set(mSize++, obj);
    if (result != nullptr) *result = true;
    return NOERROR;
}

ECode ArrayList::Add(
    /* [in] */ Integer index,
    /* [in] */ IInterface* obj)
{
    if (index > mSize || index < 0) {
        Logger::E("ArrayList", "%s", OutOfBoundsMsg(index).string());
        return ccm::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    EnsureCapacityInternal(mSize + 1);
    for (Integer i = mSize; i > index; i--) {
        mElementData.Set(i, mElementData[i - 1]);
    }
    mElementData.Set(index, obj);
    mSize++;
    return NOERROR;
}

ECode ArrayList::Remove(
    /* [in] */ Integer index,
    /* [out] */ IInterface** obj)
{
    if (index > mSize) {
        Logger::E("ArrayList", "%s", OutOfBoundsMsg(index).string());
        return ccm::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    mModCount++;
    if (obj != nullptr) {
        *obj = mElementData[index];
        REFCOUNT_ADD(*obj);
    }

    Integer numMoved = mSize - index - 1;
    if (numMoved > 0) {
        for (Integer i = index; i < mSize - 1; i++) {
            mElementData.Set(i, mElementData[i + 1]);
        }
    }
    mElementData.Set(--mSize, nullptr);
    return NOERROR;
}

ECode ArrayList::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    if (obj == nullptr) {
        for (Integer index = 0; index < mSize; index++) {
            if (mElementData[index] == nullptr) {
                FastRemove(index);
                if (result != nullptr) *result = true;
                return NOERROR;
            }
        }
    }
    else {
        for (Integer index = 0; index < mSize; index++) {
            if (Object::Equals(obj, mElementData[index])) {
                FastRemove(index);
                if (result != nullptr) *result = true;
                return NOERROR;
            }
        }
    }
    if (result != nullptr) *result = false;
    return NOERROR;
}

void ArrayList::FastRemove(
    /* [in] */ Integer index)
{
    mModCount++;
    Integer numMoved = mSize - index - 1;
    if (numMoved > 0) {
        for (Integer i = index; i < mSize - 1; i++) {
            mElementData.Set(i, mElementData[i + 1]);
        }
    }
    mElementData.Set(--mSize, nullptr);
}

ECode ArrayList::Clear()
{
    mModCount++;

    for (Integer i = 0; i < mSize; i++) {
        mElementData.Set(i, nullptr);
    }

    mSize = 0;
    return NOERROR;
}

ECode ArrayList::AddAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    Array<IInterface*> a;
    c->ToArray(&a);
    Integer numNew = a.GetLength();
    EnsureCapacityInternal(mSize + numNew);
    mElementData.Copy(mSize, a, 0, numNew);
    mSize += numNew;
    if (result != nullptr) *result = numNew != 0;
    return NOERROR;
}

ECode ArrayList::AddAll(
    /* [in] */ Integer index,
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* result)
{
    if (index > mSize || index < 0) {
        Logger::E("ArrayList", "%s", OutOfBoundsMsg(index).string());
        return ccm::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Array<IInterface*> a;
    c->ToArray(&a);
    Integer numNew = a.GetLength();
    EnsureCapacityInternal(mSize + numNew);

    if (numNew > 0) {
        for (Integer i = mSize - 1; i > mSize - numNew - 1; i--) {
            mElementData.Set(i, mElementData[i - numNew]);
        }
    }

    mElementData.Copy(index, a, 0, numNew);
    mSize += numNew;
    if (result != nullptr) *result = numNew != 0;
    return NOERROR;
}

}
}
