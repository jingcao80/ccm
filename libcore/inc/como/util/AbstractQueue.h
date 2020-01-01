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

#ifndef __COMO_UTIL_ABSTRACTQUEUE_H__
#define __COMO_UTIL_ABSTRACTQUEUE_H__

#include "como/util/AbstractCollection.h"
#include "como.util.IQueue.h"

namespace como {
namespace util {

class AbstractQueue
    : public AbstractCollection
    , public IQueue
{
public:
    COMO_INTERFACE_DECL();

    ECode Add(
        /* [in] */ IInterface* e,
        /* [out] */ Boolean* changed = nullptr) override;

    ECode Remove(
        /* [out] */ IInterface** head = nullptr) override;

    ECode Element(
        /* [out] */ IInterface** head) override;

    ECode Clear() override;

    ECode AddAll(
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* changed = nullptr) override;

    using AbstractCollection::Equals;
};

}
}

#endif // __COMO_UTIL_ABSTRACTQUEUE_H__
