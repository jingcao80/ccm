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

#include "coredef.h"
#include "ccm/util/AbstractQueue.h"
#include "ccm.util.IIterator.h"
#include <ccmlogger.h>

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_1(AbstractQueue, AbstractCollection, IQueue);

ECode AbstractQueue::Add(
    /* [in] */ IInterface* e,
    /* [out] */ Boolean* changed)
{
    Boolean result;
    Offer(e, &result);
    if (result) {
        if (changed != nullptr) *changed = result;
        return NOERROR;
    }
    Logger::E("AbstractQueue", "Queue full");
    return ccm::core::E_ILLEGAL_STATE_EXCEPTION;
}

ECode AbstractQueue::Remove(
    /* [out] */ IInterface** head)
{
    AutoPtr<IInterface> x;
    Poll((IInterface**)&x);
    if (x != nullptr) {
        if (head != nullptr) {
            *head = x;
            REFCOUNT_ADD(*head);
        }
        return NOERROR;
    }
    return E_NO_SUCH_ELEMENT_EXCEPTION;
}

ECode AbstractQueue::Element(
    /* [out] */ IInterface** head)
{
    VALIDATE_NOT_NULL(head);

    Peek(head);
    if (*head != nullptr) {
        return NOERROR;
    }
    return E_NO_SUCH_ELEMENT_EXCEPTION;
}

ECode AbstractQueue::Clear()
{
    AutoPtr<IInterface> x;
    while (Poll((IInterface**)&x), x != nullptr) {
        x = nullptr;
    }
    return NOERROR;
}

ECode AbstractQueue::AddAll(
    /* [in] */ ICollection* c,
    /* [out] */ Boolean* changed)
{
    if (c == nullptr) {
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    if (c == (ICollection*)this) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean modified = false;
    FOR_EACH(IInterface*, e, , c) {
        Boolean result;
        FAIL_RETURN(Add(e, &result));
        if (result) {
            modified = true;
        }
    } END_FOR_EACH()
    if (changed != nullptr) {
        *changed = modified;
    }
    return NOERROR;
}

}
}
