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

#ifndef __COMO_UTIL_TREESET_H__
#define __COMO_UTIL_TREESET_H__

#include "como/util/AbstractSet.h"
#include "como.core.ICloneable.h"
#include "como.io.ISerializable.h"
#include "como.util.IComparator.h"
#include "como.util.INavigableMap.h"
#include "como.util.INavigableSet.h"
#include "como.util.ISortedSet.h"
#include "como.util.ITreeSet.h"

using como::core::ICloneable;
using como::io::ISerializable;

namespace como {
namespace util {

class TreeSet
    : public AbstractSet
    , public ITreeSet
    , public INavigableSet
    , public ISortedSet
    , public ICloneable
    , public ISerializable
{
public:
    COMO_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ INavigableMap* m);

    ECode Constructor();

    ECode Constructor(
        /* [in] */ IComparator* comparator);

    ECode Constructor(
        /* [in] */ ICollection* c);

    ECode Constructor(
        /* [in] */ ISortedSet* s);

    ECode GetIterator(
        /* [out] */ AutoPtr<IIterator>& it) override;

    ECode GetDescendingIterator(
        /* [out] */ AutoPtr<IIterator>& it) override;

    ECode DescendingSet(
        /* [out] */ AutoPtr<INavigableSet>& set) override;

    ECode GetSize(
        /* [out] */ Integer& size) override;

    ECode IsEmpty(
        /* [out] */ Boolean& result) override;

    ECode Contains(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& result) override;

    ECode Add(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* modified = nullptr) override;

    ECode Remove(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* contained = nullptr) override;

    ECode Clear() override;

    ECode AddAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode SubSet(
        /* [in] */ IInterface* fromElement,
        /* [in] */ Boolean fromInclusive,
        /* [in] */ IInterface* toElement,
        /* [in] */ Boolean toInclusive,
        /* [out] */ AutoPtr<INavigableSet>& subset) override;

    ECode HeadSet(
        /* [in] */ IInterface* toElement,
        /* [in] */ Boolean inclusive,
        /* [out] */ AutoPtr<INavigableSet>& headset) override;

    ECode TailSet(
        /* [in] */ IInterface* fromElement,
        /* [in] */ Boolean inclusive,
        /* [out] */ AutoPtr<INavigableSet>& tailset) override;

    ECode SubSet(
        /* [in] */ IInterface* fromElement,
        /* [in] */ IInterface* toElement,
        /* [out] */ AutoPtr<ISortedSet>& subset) override;

    ECode HeadSet(
        /* [in] */ IInterface* toElement,
        /* [out] */ AutoPtr<ISortedSet>& headset) override;

    ECode TailSet(
        /* [in] */ IInterface* fromElement,
        /* [out] */ AutoPtr<ISortedSet>& tailset) override;

    ECode Comparator(
        /* [out] */ AutoPtr<IComparator>& comparator) override;

    ECode First(
        /* [out] */ AutoPtr<IInterface>& element) override;

    ECode Last(
        /* [out] */ AutoPtr<IInterface>& element) override;

    ECode Lower(
        /* [in] */ IInterface* e,
        /* [out] */ AutoPtr<IInterface>& lowerE) override;

    ECode Floor(
        /* [in] */ IInterface* e,
        /* [out] */ AutoPtr<IInterface>& floorE) override;

    ECode Ceiling(
        /* [in] */ IInterface* e,
        /* [out] */ AutoPtr<IInterface>& ceilingE) override;

    ECode Higher(
        /* [in] */ IInterface* e,
        /* [out] */ AutoPtr<IInterface>& higherE) override;

    ECode PollFirst(
        /* [out] */ AutoPtr<IInterface>& e) override;

    ECode PollLast(
        /* [out] */ AutoPtr<IInterface>& e) override;

protected:
    ECode CloneImpl(
        /* [in] */ ITreeSet* newObj);

private:
    static AutoPtr<IInterface> GetPRESENT();

private:
    AutoPtr<INavigableMap> mMap;
};

}
}

#endif // __COMO_UTIL_TREESET_H__
