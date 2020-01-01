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

#ifndef __COMO_UTIL_ABSTRACTSEQUENTIALLIST_H__
#define __COMO_UTIL_ABSTRACTSEQUENTIALLIST_H__

#include "como/util/AbstractList.h"

namespace como {
namespace util {

class AbstractSequentialList
    : public AbstractList
{
protected:
    ECode Constructor();

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

    ECode AddAll(
        /* [in] */ Integer index,
        /* [in] */ ICollection* c,
        /* [out] */ Boolean* result = nullptr) override;

    ECode GetIterator(
        /* [out] */ IIterator** it) override;
};

}
}

#endif // __COMO_UTIL_ABSTRACTSEQUENTIALLIST_H__
