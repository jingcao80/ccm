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

#ifndef __COMO_UTIL_COLLECTIONS_H__
#define __COMO_UTIL_COLLECTIONS_H__

#include "como/core/SyncObject.h"
#include "como/util/AbstractList.h"
#include "como/util/AbstractMap.h"
#include "como/util/AbstractSet.h"
#include "como.core.IComparable.h"
#include "como.core.ISynchronize.h"
#include "como.io.ISerializable.h"
#include "como.util.ICollection.h"
#include "como.util.IComparator.h"
#include "como.util.IEnumeration.h"
#include "como.util.IIterator.h"
#include "como.util.IList.h"
#include "como.util.IMap.h"
#include "como.util.IRandomAccess.h"
#include "como.util.ISet.h"
#include "como.util.ISortedMap.h"
#include <comosp.h>
#include <comoref.h>

using como::core::IComparable;
using como::core::ISynchronize;
using como::core::SyncObject;
using como::io::ISerializable;

namespace como {
namespace util {

class Collections
{
private:
    class UnmodifiableCollection
        : public SyncObject
        , public ICollection
        , public ISerializable
    {
    public:
        UnmodifiableCollection(
            /* [in] */ ICollection* c)
            : mC(c)
        {}

        COMO_INTERFACE_DECL();

        ECode GetSize(
            /* [out] */ Integer& size) override;

        ECode IsEmpty(
            /* [out] */ Boolean& empty) override;

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode ToArray(
            /* [out, callee] */ Array<IInterface*>* objs) override;

        ECode ToArray(
            /* [in] */ const InterfaceID& iid,
            /* [out, callee] */ Array<IInterface*>* objs) override;

        ECode ToString(
            /* [out] */ String& desc) override;

        ECode GetIterator(
            /* [out] */ AutoPtr<IIterator>& it) override;

        ECode Add(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode Remove(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode ContainsAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean& result) override;

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

    protected:
        AutoPtr<ICollection> mC;
    };

    class UnmodifiableSet
        : public UnmodifiableCollection
        , public ISet
    {
    public:
        UnmodifiableSet(
            /* [in] */ ISet* set)
            : UnmodifiableCollection(ICollection::Probe(set))
        {}

        COMO_INTERFACE_DECL();

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode GetHashCode(
            /* [out] */ Integer& hash) override;

        ECode Add(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* modified = nullptr) override;

        ECode AddAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode Clear() override;

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode ContainsAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean& result) override;

        ECode GetIterator(
            /* [out] */ AutoPtr<IIterator>& it) override;

        ECode GetSize(
            /* [out] */ Integer& size) override;

        ECode IsEmpty(
            /* [out] */ Boolean& result) override;

        ECode Remove(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* contained = nullptr) override;

        ECode RemoveAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode RetainAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode ToArray(
            /* [out, callee] */ Array<IInterface*>* objs) override;

        ECode ToArray(
            /* [in] */ const InterfaceID& iid,
            /* [out, callee] */ Array<IInterface*>* objs) override;
    };

    class UnmodifiableList
        : public UnmodifiableCollection
        , public IList
    {
    public:
        UnmodifiableList(
            /* [in] */ IList* list)
            : UnmodifiableCollection(ICollection::Probe(list))
            , mList(list)
        {}

        COMO_INTERFACE_DECL();

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode GetHashCode(
            /* [out] */ Integer& hash) override;

        ECode Get(
            /* [in] */ Integer index,
            /* [out] */ AutoPtr<IInterface>& obj) override;

        ECode Set(
            /* [in] */ Integer index,
            /* [in] */ IInterface* obj,
            /* [out] */ IInterface** prevObj = nullptr) override;

        ECode Add(
            /* [in] */ Integer index,
            /* [in] */ IInterface* obj) override;

        ECode Remove(
            /* [in] */ Integer index,
            /* [out] */ IInterface** obj = nullptr) override;

        ECode IndexOf(
            /* [in] */ IInterface* obj,
            /* [out] */ Integer& index) override;

        ECode LastIndexOf(
            /* [in] */ IInterface* obj,
            /* [out] */ Integer& index) override;

        ECode AddAll(
            /* [in] */ Integer index,
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* result = nullptr) override;

        ECode GetListIterator(
            /* [out] */ AutoPtr<IListIterator>& it) override;

        ECode GetListIterator(
            /* [in] */ Integer index,
            /* [out] */ AutoPtr<IListIterator>& it) override;

        ECode SubList(
            /* [in] */ Integer fromIndex,
            /* [in] */ Integer toIndex,
            /* [out] */ AutoPtr<IList>& subList) override;

        ECode Add(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode AddAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode Clear() override;

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode ContainsAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean& result) override;

        ECode GetIterator(
            /* [out] */ AutoPtr<IIterator>& it) override;

        ECode GetSize(
            /* [out] */ Integer& size) override;

        ECode IsEmpty(
            /* [out] */ Boolean& empty) override;

        ECode Remove(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode RemoveAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode RetainAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode ToArray(
            /* [out, callee] */ Array<IInterface*>* objs) override;

        ECode ToArray(
            /* [in] */ const InterfaceID& iid,
            /* [out, callee] */ Array<IInterface*>* objs) override;

    protected:
        AutoPtr<IList> mList;
    };

    class UnmodifiableRandomAccessList
        : public UnmodifiableList
        , public IRandomAccess
    {
    public:
        UnmodifiableRandomAccessList(
            /* [in] */ IList* list)
            : UnmodifiableList(list)
        {}

        COMO_INTERFACE_DECL();

        ECode SubList(
            /* [in] */ Integer fromIndex,
            /* [in] */ Integer toIndex,
            /* [out] */ AutoPtr<IList>& subList) override;
    };

    class SynchronizedCollection
        : public SyncObject
        , public ICollection
        , public ISerializable
    {
    public:
        SynchronizedCollection(
            /* [in] */ ICollection* c)
            : mC(c)
            , mMutex(this)
        {}

        SynchronizedCollection(
            /* [in] */ ICollection* c,
            /* [in] */ ISynchronize* mutex)
            : mC(c)
            , mMutex(mutex)
        {}

        COMO_INTERFACE_DECL();

        ECode GetSize(
            /* [out] */ Integer& size) override;

        ECode IsEmpty(
            /* [out] */ Boolean& empty) override;

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode ToArray(
            /* [out, callee] */ Array<IInterface*>* objs) override;

        ECode ToArray(
            /* [in] */ const InterfaceID& iid,
            /* [out, callee] */ Array<IInterface*>* objs) override;

        ECode GetIterator(
            /* [out] */ AutoPtr<IIterator>& it) override;

        ECode Add(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode Remove(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode ContainsAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean& result) override;

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
            /* [out] */ String& desc) override;

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode GetHashCode(
            /* [out] */ Integer& hash) override;

    protected:
        AutoPtr<ICollection> mC; // Backing Collection

        ISynchronize* mMutex; // Object on which to synchronize
    };

    class SynchronizedSet
        : public SynchronizedCollection
        , public ISet
    {
    public:
        SynchronizedSet(
            /* [in] */ ISet* s)
            : SynchronizedCollection(ICollection::Probe(s))
        {}

        SynchronizedSet(
            /* [in] */ ISet* s,
            /* [in] */ ISynchronize* mutex)
            : SynchronizedCollection(ICollection::Probe(s), mutex)
        {}

        COMO_INTERFACE_DECL();

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode GetHashCode(
            /* [out] */ Integer& hash) override;

        ECode Add(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode AddAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode Clear() override;

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode ContainsAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean& result) override;

        ECode GetIterator(
            /* [out] */ AutoPtr<IIterator>& it) override;

        ECode GetSize(
            /* [out] */ Integer& size) override;

        ECode IsEmpty(
            /* [out] */ Boolean& empty) override;

        ECode Remove(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode RemoveAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode RetainAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode ToArray(
            /* [out, callee] */ Array<IInterface*>* objs) override;

        ECode ToArray(
            /* [in] */ const InterfaceID& iid,
            /* [out, callee] */ Array<IInterface*>* objs) override;
    };


    class SynchronizedList
        : public SynchronizedCollection
        , public IList
    {
    public:
        SynchronizedList(
            /* [in] */ IList* list)
            : SynchronizedCollection(ICollection::Probe(list))
            , mList(list)
        {}

        SynchronizedList(
            /* [in] */ IList* list,
            /* [in] */ ISynchronize* mutex)
            : SynchronizedCollection(ICollection::Probe(list), mutex)
            , mList(list)
        {}

        COMO_INTERFACE_DECL();

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode GetHashCode(
            /* [out] */ Integer& hash) override;

        ECode Get(
            /* [in] */ Integer index,
            /* [out] */ AutoPtr<IInterface>& obj) override;

        ECode Set(
            /* [in] */ Integer index,
            /* [in] */ IInterface* obj,
            /* [out] */ IInterface** prevObj = nullptr) override;

        ECode Add(
            /* [in] */ Integer index,
            /* [in] */ IInterface* obj) override;

        ECode Remove(
            /* [in] */ Integer index,
            /* [out] */ IInterface** obj = nullptr) override;

        ECode IndexOf(
            /* [in] */ IInterface* obj,
            /* [out] */ Integer& index) override;

        ECode LastIndexOf(
            /* [in] */ IInterface* obj,
            /* [out] */ Integer& index) override;

        ECode AddAll(
            /* [in] */ Integer index,
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* result = nullptr) override;

        ECode GetListIterator(
            /* [out] */ AutoPtr<IListIterator>& it) override;

        ECode GetListIterator(
            /* [in] */ Integer index,
            /* [out] */ AutoPtr<IListIterator>& it) override;

        ECode SubList(
            /* [in] */ Integer fromIndex,
            /* [in] */ Integer toIndex,
            /* [out] */ AutoPtr<IList>& subList) override;

        ECode Add(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode AddAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode Clear() override;

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode ContainsAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean& result) override;

        ECode GetIterator(
            /* [out] */ AutoPtr<IIterator>& it) override;

        ECode GetSize(
            /* [out] */ Integer& size) override;

        ECode IsEmpty(
            /* [out] */ Boolean& empty) override;

        ECode Remove(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode RemoveAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode RetainAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode ToArray(
            /* [out, callee] */ Array<IInterface*>* objs) override;

        ECode ToArray(
            /* [in] */ const InterfaceID& iid,
            /* [out, callee] */ Array<IInterface*>* objs) override;

    public:
        AutoPtr<IList> mList;
    };

    class SynchronizedRandomAccessList
        : public SynchronizedList
        , public IRandomAccess
    {
    public:
        SynchronizedRandomAccessList(
            /* [in] */ IList* list)
            : SynchronizedList(list)
        {}

        SynchronizedRandomAccessList(
            /* [in] */ IList* list,
            /* [in] */ ISynchronize* mutex)
            : SynchronizedList(list, mutex)
        {}

        COMO_INTERFACE_DECL();

        ECode SubList(
            /* [in] */ Integer fromIndex,
            /* [in] */ Integer toIndex,
            /* [out] */ AutoPtr<IList>& subList) override;
    };

    class EmptyIterator
        : public LightRefBase
        , public IIterator
    {
    public:
        COMO_INTERFACE_DECL();

        ECode HasNext(
            /* [out] */ Boolean& result) override;

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;

        ECode Remove() override;

        static AutoPtr<IIterator> Get_EMPTY_ITERATOR();
    };

    class EmptyListIterator
        : public EmptyIterator
        , public IListIterator
    {
    public:
        COMO_INTERFACE_DECL();

        ECode HasPrevious(
            /* [out] */ Boolean* result) override;

        ECode Previous(
            /* [out] */ IInterface** object = nullptr) override;

        ECode GetNextIndex(
            /* [out] */ Integer* index) override;

        ECode GetPreviousIndex(
            /* [out] */ Integer* index) override;

        ECode Set(
            /* [in] */ IInterface* object) override;

        ECode Add(
            /* [in] */ IInterface* object) override;

        ECode Next(
            /* [out] */ IInterface** object = nullptr) override;

        ECode HasNext(
            /* [out] */ Boolean& result) override;

        ECode Remove() override;

        static AutoPtr<IListIterator> Get_EMPTY_ITERATOR();
    };

    class EmptyEnumeration
        : public LightRefBase
        , public IEnumeration
    {
    public:
        COMO_INTERFACE_DECL();

        ECode HasMoreElements(
            /* [out] */ Boolean& result) override;

        ECode NextElement(
            /* [out] */ IInterface** object = nullptr) override;

        static AutoPtr<IEnumeration> Get_EMPTY_ENUMERATION();
    };

    class EmptySet
        : public AbstractSet
        , public ISerializable
    {
    public:
        COMO_INTERFACE_DECL();

        ECode GetIterator(
            /* [out] */ AutoPtr<IIterator>& it) override;

        ECode GetSize(
            /* [out] */ Integer& size) override;

        ECode IsEmpty(
            /* [out] */ Boolean& empty) override;

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode ContainsAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean& result) override;

        ECode ToArray(
            /* [out, callee] */ Array<IInterface*>* objs) override;

        ECode ToArray(
            /* [in] */ const InterfaceID& iid,
            /* [out, callee] */ Array<IInterface*>* objs) override;
    };

    class EmptyList
        : public AbstractList
        , public IRandomAccess
        , public ISerializable
    {
    public:
        COMO_INTERFACE_DECL();

        ECode GetIterator(
            /* [out] */ AutoPtr<IIterator>& it) override;

        ECode GetListIterator(
            /* [out] */ AutoPtr<IListIterator>& it) override;

        ECode GetSize(
            /* [out] */ Integer& size) override;

        ECode IsEmpty(
            /* [out] */ Boolean& empty) override;

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode ContainsAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean& result) override;

        ECode ToArray(
            /* [out, callee] */ Array<IInterface*>* objs) override;

        ECode ToArray(
            /* [in] */ const InterfaceID& iid,
            /* [out, callee] */ Array<IInterface*>* objs) override;

        ECode Get(
            /* [in] */ Integer index,
            /* [out] */ AutoPtr<IInterface>& obj) override;

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode GetHashCode(
            /* [out] */ Integer& hash) override;

        ECode AddAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* result = nullptr) override;
    };

    class EmptyMap
        : public AbstractMap
        , public ISerializable
    {
    public:
        COMO_INTERFACE_DECL();

        ECode GetSize(
            /* [out] */ Integer& size) override;

        ECode IsEmpty(
            /* [out] */ Boolean& result) override;

        ECode ContainsKey(
            /* [in] */ IInterface* key,
            /* [out] */ Boolean& result) override;

        ECode ContainsValue(
            /* [in] */ IInterface* value,
            /* [out] */ Boolean& result) override;

        ECode Get(
            /* [in] */ IInterface* key,
            /* [out] */ AutoPtr<IInterface>& value) override;

        ECode GetKeySet(
            /* [out] */ AutoPtr<ISet>& keys) override;

        ECode GetValues(
            /* [out] */ AutoPtr<ICollection>& values) override;

        ECode GetEntrySet(
            /* [out] */ AutoPtr<ISet>& entries) override;

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& result) override;

        ECode GetHashCode(
            /* [out] */ Integer& hash) override;

        ECode PutIfAbsent(
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* value,
            /* [out] */ IInterface** prevValue = nullptr) override;
    };

    class ReverseComparator
        : public Object
        , public IComparator
    {
    public:
        COMO_INTERFACE_DECL();

        ECode Compare(
            /* [in] */ IInterface* c1,
            /* [in] */ IInterface* c2,
            /* [out] */ Integer& cmp) override;

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& isEqual) override;

        static AutoPtr<ReverseComparator> GetREVERSE_ORDER();
    };

    class ReverseComparator2
        : public Object
        , public IComparator
    {
    public:
        ReverseComparator2(
            /* [in] */ IComparator* cmp)
            : mCmp(cmp)
        {}

        COMO_INTERFACE_DECL();

        ECode Compare(
            /* [in] */ IInterface* c1,
            /* [in] */ IInterface* c2,
            /* [out] */ Integer& cmp) override;

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& isEqual) override;

        ECode GetHashCode(
            /* [out] */ Integer& hash) override;

    public:
        AutoPtr<IComparator> mCmp;
    };

public:
    static void Reverse(
        /* [in] */ IList* list);

    static void Swap(
        /* [in] */ IList* list,
        /* [in] */ Integer i,
        /* [in] */ Integer j);

    static AutoPtr<ISet> CreateUnmodifiableSet(
        /* [in] */ ISet* set);

    static AutoPtr<IList> CreateUnmodifiableList(
        /* [in] */ IList* list);

    static AutoPtr<ISortedMap> CreateUnmodifiableSortedMap(
        /* [in] */ ISortedMap* m)
    {
        return nullptr;
    }

    static AutoPtr<ICollection> CreateSynchronizedCollection(
        /* [in] */ ICollection* c,
        /* [in] */ ISynchronize* mutex);

    static AutoPtr<ISet> CreateSynchronizedSet(
        /* [in] */ ISet* s,
        /* [in] */ ISynchronize* mutex);

    static AutoPtr<IList> CreateSynchronizedList(
        /* [in] */ IList* l,
        /* [in] */ ISynchronize* mutex);

    static AutoPtr<IIterator> GetEmptyIterator();

    static AutoPtr<IEnumeration> GetEmptyEnumeration();

    static AutoPtr<ISet> GetEmptySet()
    {
        return nullptr;
    }

    static AutoPtr<IList> GetEmptyList();

    static AutoPtr<IMap> GetEmptyMap()
    {
        return nullptr;
    }

    static AutoPtr<IMap> CreateSingletonMap(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value)
    {
        return nullptr;
    }

    static AutoPtr<ISet> GetEMPTY_SET();

    static AutoPtr<IList> GetEMPTY_LIST();

    static AutoPtr<IMap> GetEMPTY_MAP();

    static AutoPtr<IComparator> ReverseOrder();

    static AutoPtr<IComparator> ReverseOrder(
        /* [in] */ IComparator* cmp);

private:
    static constexpr Integer REVERSE_THRESHOLD = 18;
};

}
}

#endif // __COMO_UTIL_COLLECTIONS_H__
