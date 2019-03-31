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

#ifndef __CCM_UTIL_COLLECTIONS_H__
#define __CCM_UTIL_COLLECTIONS_H__

#include "ccm/core/SyncObject.h"
#include "ccm/util/AbstractList.h"
#include "ccm.core.ISynchronize.h"
#include "ccm.io.ISerializable.h"
#include "ccm.util.ICollection.h"
#include "ccm.util.IEnumeration.h"
#include "ccm.util.IIterator.h"
#include "ccm.util.IList.h"
#include "ccm.util.IMap.h"
#include "ccm.util.IRandomAccess.h"
#include "ccm.util.ISet.h"
#include <ccmautoptr.h>
#include <ccmrefbase.h>

using ccm::core::ISynchronize;
using ccm::core::SyncObject;
using ccm::io::ISerializable;

namespace ccm {
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

        CCM_INTERFACE_DECL();

        ECode GetSize(
            /* [out] */ Integer* size) override;

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

        ECode ToString(
            /* [out] */ String* desc) override;

        ECode GetIterator(
            /* [out] */ IIterator** it) override;

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

    protected:
        AutoPtr<ICollection> mC;
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

        CCM_INTERFACE_DECL();

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result) override;

        ECode GetHashCode(
            /* [out] */ Integer* hash) override;

        ECode Get(
            /* [in] */ Integer index,
            /* [out] */ IInterface** obj) override;

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
            /* [out] */ Integer* index) override;

        ECode LastIndexOf(
            /* [in] */ IInterface* obj,
            /* [out] */ Integer* index) override;

        ECode AddAll(
            /* [in] */ Integer index,
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* result = nullptr) override;

        ECode GetListIterator(
            /* [out] */ IListIterator** it) override;

        ECode GetListIterator(
            /* [in] */ Integer index,
            /* [out] */ IListIterator** it) override;

        ECode SubList(
            /* [in] */ Integer fromIndex,
            /* [in] */ Integer toIndex,
            /* [out] */ IList** subList) override;

        ECode Add(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode AddAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode Clear() override;

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result) override;

        ECode ContainsAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* result) override;

        ECode GetIterator(
            /* [out] */ IIterator** it) override;

        ECode GetSize(
            /* [out] */ Integer* size) override;

        ECode IsEmpty(
            /* [out] */ Boolean* empty) override;

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

        CCM_INTERFACE_DECL();

        ECode SubList(
            /* [in] */ Integer fromIndex,
            /* [in] */ Integer toIndex,
            /* [out] */ IList** subList) override;
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

        CCM_INTERFACE_DECL();

        ECode GetSize(
            /* [out] */ Integer* size) override;

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

        ECode GetIterator(
            /* [out] */ IIterator** it) override;

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
            /* [out] */ String* desc) override;

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result) override;

        ECode GetHashCode(
            /* [out] */ Integer* hash) override;

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

        CCM_INTERFACE_DECL();

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result) override;

        ECode GetHashCode(
            /* [out] */ Integer* hash) override;

        ECode Add(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode AddAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode Clear() override;

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result) override;

        ECode ContainsAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* result) override;

        ECode GetIterator(
            /* [out] */ IIterator** it) override;

        ECode GetSize(
            /* [out] */ Integer* size) override;

        ECode IsEmpty(
            /* [out] */ Boolean* empty) override;

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

    class EmptyIterator
        : public LightRefBase
        , public IIterator
    {
    public:
        CCM_INTERFACE_DECL();

        ECode HasNext(
            /* [out] */ Boolean* result) override;

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
        CCM_INTERFACE_DECL();

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
            /* [out] */ Boolean* result) override;

        ECode Remove() override;

        static AutoPtr<IListIterator> Get_EMPTY_ITERATOR();
    };

    class EmptyEnumeration
        : public LightRefBase
        , public IEnumeration
    {
    public:
        CCM_INTERFACE_DECL();

        ECode HasMoreElements(
            /* [out] */ Boolean* result) override;

        ECode NextElement(
            /* [out] */ IInterface** object = nullptr) override;

        static AutoPtr<IEnumeration> Get_EMPTY_ENUMERATION();
    };

    class EmptyList
        : public AbstractList
        , public IRandomAccess
        , public ISerializable
    {
    public:
        CCM_INTERFACE_DECL();

        ECode GetIterator(
            /* [out] */ IIterator** it) override;

        ECode GetListIterator(
            /* [out] */ IListIterator** it) override;

        ECode GetSize(
            /* [out] */ Integer* size) override;

        ECode IsEmpty(
            /* [out] */ Boolean* empty) override;

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result) override;

        ECode ContainsAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* result) override;

        ECode ToArray(
            /* [out, callee] */ Array<IInterface*>* objs) override;

        ECode ToArray(
            /* [in] */ const InterfaceID& iid,
            /* [out, callee] */ Array<IInterface*>* objs) override;

        ECode Get(
            /* [in] */ Integer index,
            /* [out] */ IInterface** obj) override;

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result) override;

        ECode GetHashCode(
            /* [out] */ Integer* hash) override;

        ECode AddAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* result = nullptr) override;
    };

public:
    static void Reverse(
        /* [in] */ IList* list);

    static void Swap(
        /* [in] */ IList* list,
        /* [in] */ Integer i,
        /* [in] */ Integer j);

    static AutoPtr<ISet> CreateUnmodifiableSet(
        /* [in] */ ISet* set)
    {
        return nullptr;
    }

    static AutoPtr<IList> CreateUnmodifiableList(
        /* [in] */ IList* list);

    static AutoPtr<ICollection> CreateSynchronizedCollection(
        /* [in] */ ICollection* c,
        /* [in] */ ISynchronize* mutex);

    static AutoPtr<ISet> CreateSynchronizedSet(
        /* [in] */ ISet* s,
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

    static AutoPtr<IList> Get_EMPTY_LIST();

private:
    static constexpr Integer REVERSE_THRESHOLD = 18;
};

}
}

#endif // __CCM_UTIL_COLLECTIONS_H__
