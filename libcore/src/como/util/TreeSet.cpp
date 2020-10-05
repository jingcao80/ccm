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

#include "como/util/CTreeMap.h"
#include "como/util/CTreeSet.h"
#include "como/util/TreeMap.h"
#include "como/util/TreeSet.h"
#include "como.util.ISortedMap.h"

using como::core::IID_ICloneable;
using como::io::IID_ISerializable;

namespace como {
namespace util {

AutoPtr<IInterface> TreeSet::GetPRESENT()
{
    static AutoPtr<IObject> PRESENT = new SyncObject();
    return PRESENT.Get();
}

COMO_INTERFACE_IMPL_5(TreeSet, AbstractSet, ITreeSet, INavigableSet, ISortedSet, ICloneable, ISerializable);

ECode TreeSet::Constructor(
    /* [in] */ INavigableMap* m)
{
    mMap = m;
    return NOERROR;
}

ECode TreeSet::Constructor()
{
    AutoPtr<INavigableMap> m;
    CTreeMap::New(IID_INavigableMap, (IInterface**)&m);
    return Constructor(m);
}

ECode TreeSet::Constructor(
    /* [in] */ IComparator* comparator)
{
    AutoPtr<INavigableMap> m;
    CTreeMap::New(comparator, IID_INavigableMap, (IInterface**)&m);
    return Constructor(m);
}

ECode TreeSet::Constructor(
    /* [in] */ ICollection* c)
{
    Constructor();
    return AddAll(c);
}

ECode TreeSet::Constructor(
    /* [in] */ ISortedSet* s)
{
    AutoPtr<IComparator> comparator;
    s->Comparator(comparator);
    Constructor(comparator);
    return AddAll(ICollection::Probe(s));
}

ECode TreeSet::GetIterator(
    /* [out] */ AutoPtr<IIterator>& it)
{
    AutoPtr<INavigableSet> s;
    mMap->NavigableKeySet(s);
    return s->GetIterator(it);
}

ECode TreeSet::GetDescendingIterator(
    /* [out] */ AutoPtr<IIterator>& it)
{
    AutoPtr<INavigableSet> s;
    mMap->DescendingKeySet(s);
    return s->GetIterator(it);
}

ECode TreeSet::DescendingSet(
    /* [out] */ AutoPtr<INavigableSet>& set)
{
    AutoPtr<INavigableMap> m;
    mMap->DescendingMap(m);
    set = nullptr;
    return CTreeSet::New(m, IID_INavigableSet, (IInterface**)&set);
}

ECode TreeSet::GetSize(
    /* [out] */ Integer& size)
{
    return IMap::Probe(mMap)->GetSize(size);
}

ECode TreeSet::IsEmpty(
    /* [out] */ Boolean& result)
{
    return IMap::Probe(mMap)->IsEmpty(result);
}

ECode TreeSet::Contains(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& result)
{
    return IMap::Probe(mMap)->ContainsKey(obj, result);
}

ECode TreeSet::Add(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* modified)
{
    AutoPtr<IInterface> oldValue;
    IMap::Probe(mMap)->Put(obj, GetPRESENT(), &oldValue);
    if (modified != nullptr) {
        *modified = oldValue == nullptr;
    }
    return NOERROR;
}

ECode TreeSet::Remove(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* contained)
{
    AutoPtr<IInterface> oldValue;
    IMap::Probe(mMap)->Remove(obj, &oldValue);
    if (contained != nullptr) {
        *contained = oldValue == GetPRESENT();
    }
    return NOERROR;
}

ECode TreeSet::Clear()
{
    return IMap::Probe(mMap)->Clear();
}

ECode TreeSet::AddAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    Integer size;
    if ((IMap::Probe(mMap)->GetSize(size), size == 0) &&
            (c->GetSize(size), size > 0) &&
            ISortedSet::Probe(c) != nullptr &&
            ITreeMap::Probe(mMap) != nullptr) {
        ISortedSet* set = ISortedSet::Probe(c);
        TreeMap* map = (TreeMap*)ITreeMap::Probe(mMap);
        AutoPtr<IComparator> cc, mc;
        set->Comparator(cc);
        map->Comparator(mc);
        if (cc == mc || (cc != nullptr && Object::Equals(cc, mc))) {
            map->AddAllForTreeSet(set, GetPRESENT());
            if (changed != nullptr) {
                *changed = true;
            }
            return NOERROR;
        }
    }
    return AbstractSet::AddAll(c, changed);
}

ECode TreeSet::SubSet(
    /* [in] */ IInterface* fromElement,
    /* [in] */ Boolean fromInclusive,
    /* [in] */ IInterface* toElement,
    /* [in] */ Boolean toInclusive,
    /* [out] */ AutoPtr<INavigableSet>& subset)
{
    AutoPtr<INavigableMap> m;
    mMap->SubMap(fromElement, fromInclusive, toElement, toInclusive, m);
    subset = nullptr;
    return CTreeSet::New(m, IID_INavigableSet, (IInterface**)&subset);
}

ECode TreeSet::HeadSet(
    /* [in] */ IInterface* toElement,
    /* [in] */ Boolean inclusive,
    /* [out] */ AutoPtr<INavigableSet>& headset)
{
    AutoPtr<INavigableMap> m;
    mMap->HeadMap(toElement, inclusive, m);
    headset = nullptr;
    return CTreeSet::New(m, IID_INavigableSet, (IInterface**)&headset);
}

ECode TreeSet::TailSet(
    /* [in] */ IInterface* fromElement,
    /* [in] */ Boolean inclusive,
    /* [out] */ AutoPtr<INavigableSet>& tailset)
{
    AutoPtr<INavigableMap> m;
    mMap->TailMap(fromElement, inclusive, m);
    tailset = nullptr;
    return CTreeSet::New(m, IID_INavigableSet, (IInterface**)&tailset);
}

ECode TreeSet::SubSet(
    /* [in] */ IInterface* fromElement,
    /* [in] */ IInterface* toElement,
    /* [out] */ AutoPtr<ISortedSet>& subset)
{
    AutoPtr<INavigableSet> navSubset;
    SubSet(fromElement, true, toElement, false, navSubset);
    subset = std::move(navSubset);
    return NOERROR;
}

ECode TreeSet::HeadSet(
    /* [in] */ IInterface* toElement,
    /* [out] */ AutoPtr<ISortedSet>& headset)
{
    AutoPtr<INavigableSet> navSubset;
    HeadSet(toElement, false, navSubset);
    headset = std::move(navSubset);
    return NOERROR;
}

ECode TreeSet::TailSet(
    /* [in] */ IInterface* fromElement,
    /* [out] */ AutoPtr<ISortedSet>& tailset)
{
    AutoPtr<INavigableSet> navSubset;
    TailSet(fromElement, true, navSubset);
    tailset = std::move(navSubset);
    return NOERROR;
}

ECode TreeSet::Comparator(
    /* [out] */ AutoPtr<IComparator>& comparator)
{
    return ISortedMap::Probe(mMap)->Comparator(comparator);
}

ECode TreeSet::First(
    /* [out] */ AutoPtr<IInterface>& element)
{
    return ISortedMap::Probe(mMap)->FirstKey(element);
}

ECode TreeSet::Last(
    /* [out] */ AutoPtr<IInterface>& element)
{
    return ISortedMap::Probe(mMap)->LastKey(element);
}

ECode TreeSet::Lower(
    /* [in] */ IInterface* e,
    /* [out] */ AutoPtr<IInterface>& lowerE)
{
    return mMap->LowerKey(e, lowerE);
}

ECode TreeSet::Floor(
    /* [in] */ IInterface* e,
    /* [out] */ AutoPtr<IInterface>& floorE)
{
    return mMap->FloorKey(e, floorE);
}

ECode TreeSet::Ceiling(
    /* [in] */ IInterface* e,
    /* [out] */ AutoPtr<IInterface>& ceilingE)
{
    return mMap->CeilingKey(e, ceilingE);
}

ECode TreeSet::Higher(
    /* [in] */ IInterface* e,
    /* [out] */ AutoPtr<IInterface>& higherE)
{
    return mMap->HigherKey(e, higherE);
}

ECode TreeSet::PollFirst(
    /* [out] */ AutoPtr<IInterface>& e)
{
    AutoPtr<IMapEntry> entry;
    mMap->PollFirstEntry(entry);
    if (entry == nullptr) {
        e = nullptr;
        return NOERROR;
    }
    return entry->GetKey(e);
}

ECode TreeSet::PollLast(
    /* [out] */ AutoPtr<IInterface>& e)
{
    AutoPtr<IMapEntry> entry;
    mMap->PollLastEntry(entry);
    if (entry == nullptr) {
        e = nullptr;
        return NOERROR;
    }
    return entry->GetKey(e);
}

ECode TreeSet::CloneImpl(
    /* [in] */ ITreeSet* newObj)
{
    TreeSet* clone = (TreeSet*)newObj;
    AutoPtr<INavigableMap> map;
    CTreeMap::New(ISortedMap::Probe(mMap), IID_INavigableMap, (IInterface**)&map);
    clone->mMap = std::move(map);
    return NOERROR;
}

}
}
