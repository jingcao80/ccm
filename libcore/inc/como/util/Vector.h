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

#ifndef __COMO_UTIL_VECTOR_H__
#define __COMO_UTIL_VECTOR_H__

#include "como/util/AbstractList.h"
#include "como.core.ICloneable.h"
#include "como.core.IInteger.h"
#include "como.io.ISerializable.h"
#include "como.util.IEnumeration.h"
#include "como.util.IListIterator.h"
#include "como.util.IRandomAccess.h"
#include "como.util.IVector.h"

using como::core::ICloneable;
using como::core::IInteger;
using como::io::ISerializable;

namespace como {
namespace util {

class Vector
    : public AbstractList
    , public IVector
    , public IRandomAccess
    , public ICloneable
    , public ISerializable
{
protected:
    class Itr
        : public SyncObject
        , public IIterator
    {
    public:
        Itr(
            /* [in] */ Vector* owner);

        COMO_INTERFACE_DECL();

        ECode HasNext(
            /* [out] */ Boolean& result) override;

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;

        ECode Remove() override;

        ECode CheckForComodification();

    public:
        Integer mCursor;
        Integer mLastRet = -1;
        Integer mExpectedModCount;

    protected:
        Integer mLimit;
        AutoPtr<Vector> mOwner;
    };

    class ListItr
        : public Itr
        , public IListIterator
    {
    public:
        ListItr(
            /* [in] */ Vector* owner,
            /* [in] */ Integer index);

        COMO_INTERFACE_DECL();

        ECode HasPrevious(
            /* [out] */ Boolean* result) override;

        ECode GetNextIndex(
            /* [out] */ Integer* index) override;

        ECode GetPreviousIndex(
            /* [out] */ Integer* index) override;

        ECode Previous(
            /* [out] */ IInterface** object = nullptr) override;

        ECode Set(
            /* [in] */ IInterface* object) override;

        ECode Add(
            /* [in] */ IInterface* object) override;

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;

        ECode HasNext(
            /* [out] */ Boolean& result) override;

        ECode Remove() override;
    };

public:
    COMO_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Integer initialCapacity,
        /* [in] */ Integer capacityIncrement);

    ECode Constructor(
        /* [in] */ Integer initialCapacity);

    ECode Constructor();

    ECode Constructor(
        /* [in] */ ICollection* c);

    ECode CopyInto(
        /* [out] */ Array<IInterface*>& anArray) override;

    ECode TrimToSize() override;

    ECode EnsureCapacity(
        /* [in] */ Integer minCapacity) override;

    ECode SetSize(
        /* [in] */ Integer newSize) override;

    ECode GetCapacity(
        /* [out] */ Integer* capacity) override;

    ECode GetSize(
        /* [out] */ Integer& size) override;

    ECode IsEmpty(
        /* [out] */ Boolean& empty) override;

    ECode GetElements(
        /* [out] */ IEnumeration** elements) override;

    ECode Contains(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& result) override;

    ECode IndexOf(
        /* [in] */ IInterface* obj,
        /* [out] */ Integer* index) override;

    ECode IndexOf(
        /* [in] */ IInterface* obj,
        /* [in] */ Integer fromIndex,
        /* [out] */ Integer* index) override;

    ECode LastIndexOf(
        /* [in] */ IInterface* obj,
        /* [out] */ Integer* index) override;

    ECode LastIndexOf(
        /* [in] */ IInterface* obj,
        /* [in] */ Integer fromIndex,
        /* [out] */ Integer* index) override;

    ECode GetElementAt(
        /* [in] */ Integer index,
        /* [out] */ IInterface** element) override;

    ECode GetFirstElement(
        /* [out] */ IInterface** element) override;

    ECode GetLastElement(
        /* [out] */ IInterface** element) override;

    ECode SetElementAt(
        /* [in] */ IInterface* obj,
        /* [in] */ Integer index) override;

    ECode RemoveElementAt(
        /* [in] */ Integer index) override;

    ECode InsertElementAt(
        /* [in] */ IInterface* obj,
        /* [in] */ Integer index) override;

    ECode AddElement(
        /* [in] */ IInterface* e) override;

    ECode RemoveElement(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode RemoveAllElements() override;

    ECode ToArray(
        /* [out, callee] */ Array<IInterface*>* objs) override;

    ECode ToArray(
        /* [in] */ const InterfaceID& iid,
        /* [out, callee] */ Array<IInterface*>* objs) override;

    AutoPtr<IInterface> ElementData(
        /* [in] */ Integer index);

    ECode Get(
        /* [in] */ Integer index,
        /* [out] */ AutoPtr<IInterface>& obj) override;

    ECode Set(
        /* [in] */ Integer index,
        /* [in] */ IInterface* obj,
        /* [out] */ IInterface** prevObj = nullptr) override;

    ECode Add(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode Remove(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode Add(
        /* [in] */ Integer index,
        /* [in] */ IInterface* obj) override;

    ECode Remove(
        /* [in] */ Integer index,
        /* [out] */ IInterface** obj = nullptr) override;

    ECode Clear() override;

    ECode ContainsAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean& result) override;

    ECode AddAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* changed) override;

    ECode RemoveAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode RetainAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode AddAll(
        /* [in] */ Integer index,
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& same) override;

    ECode GetHashCode(
        /* [out] */ Integer& hash) override;

    ECode ToString(
        /* [out] */ String& desc) override;

    ECode SubList(
        /* [in] */ Integer fromIndex,
        /* [in] */ Integer toIndex,
        /* [out] */ IList** subList) override;

    ECode GetListIterator(
        /* [in] */ Integer index,
        /* [out] */ IListIterator** it)override;

    ECode GetListIterator(
        /* [out] */ IListIterator** it) override;

    ECode GetIterator(
        /* [out] */ IIterator** it) override;

protected:
    ECode CloneImpl(
        /* [in] */ IVector* newObj);

    ECode RemoveRange(
        /* [in] */ Integer fromIndex,
        /* [in] */ Integer toIndex) override;

private:
    void EnsureCapacityHelper(
        /* [in] */ Integer minCapacity);

    void Grow(
        /* [in] */ Integer minCapacity);

    static Integer HugeCapacity(
        /* [in] */ Integer minCapacity);

protected:
    /**
     * The array buffer into which the components of the vector are
     * stored. The capacity of the vector is the length of this array buffer,
     * and is at least large enough to contain all the vector's elements.
     *
     * <p>Any array elements following the last element in the Vector are null.
     */
    Array<IInterface*> mElementData;

    /**
     * The number of valid components in this {@code Vector} object.
     * Components {@code elementData[0]} through
     * {@code elementData[elementCount-1]} are the actual items.
     */
    Integer mElementCount = 0;

    /**
     * The amount by which the capacity of the vector is automatically
     * incremented when its size becomes greater than its capacity.  If
     * the capacity increment is less than or equal to zero, the capacity
     * of the vector is doubled each time it needs to grow.
     */
    Integer mCapacityIncrement = 0;

private:
    static constexpr Integer MAX_ARRAY_SIZE = IInteger::MAX_VALUE - 8;
};

}
}

#endif // __COMO_UTIL_VECTOR_H__
