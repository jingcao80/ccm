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

#ifndef __COMO_UTIL_ABSTRACTSET_H__
#define __COMO_UTIL_ABSTRACTSET_H__

#include "como/util/AbstractCollection.h"
#include "como.util.ISet.h"

namespace como {
namespace util {

class AbstractSet
    : public AbstractCollection
    , public ISet
{
public:
    COMO_INTERFACE_DECL();

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result) override;

    ECode GetHashCode(
        /* [out] */ Integer* hash) override;

    ECode RemoveAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode Add(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode AddAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode Clear() override;

    ECode ContainsAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* result) override;

    ECode IsEmpty(
        /* [out] */ Boolean* empty) override;

    ECode Remove(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* contained = nullptr) override;

    ECode RetainAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode ToArray(
        /* [out, callee] */ Array<IInterface*>* objs) override;

    ECode ToArray(
        /* [in] */ const InterfaceID& iid,
        /* [out, callee] */ Array<IInterface*>* objs) override;

    using ISet::ContainsAll;
    using ISet::GetIterator;
    using ISet::GetSize;
};

}
}

#endif // __COMO_UTIL_ABSTRACTSET_H__
