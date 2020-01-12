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

#ifndef __COMO_UTIL_HASHSET_H__
#define __COMO_UTIL_HASHSET_H__

#include "como/util/AbstractSet.h"
#include "como.core.ICloneable.h"
#include "como.io.ISerializable.h"
#include "como.util.ICollection.h"
#include "como.util.IHashMap.h"
#include "como.util.IHashSet.h"
#include "como.util.IIterator.h"
#include <comosp.h>

using como::core::ICloneable;
using como::io::ISerializable;

namespace como {
namespace util {

class HashSet
    : public AbstractSet
    , public IHashSet
    , public ICloneable
    , public ISerializable
{
public:
    COMO_INTERFACE_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ ICollection* c);

    ECode Constructor(
        /* [in] */ Integer initialCapacity,
        /* [in] */ Float loadFactor);

    ECode Constructor(
        /* [in] */ Integer initialCapacity);

    ECode Constructor(
        /* [in] */ Integer initialCapacity,
        /* [in] */ Float loadFactor,
        /* [in] */ Boolean dummy);

    ECode GetIterator(
        /* [out] */ IIterator** it) override;

    ECode GetSize(
        /* [out] */ Integer* size) override;

    ECode IsEmpty(
        /* [out] */ Boolean* result) override;

    ECode Contains(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result) override;

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

    ECode ContainsAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* result) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& result) override;

    ECode GetHashCode(
        /* [out] */ Integer& hash) override;

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
    ECode CloneImpl(
        /* [in] */ IHashSet* newObj);

private:
    static AutoPtr<IInterface> GetPRESENT();

private:
    AutoPtr<IHashMap> mMap;
};

}
}

#endif // __COMO_UTIL_HASHSET_H__
