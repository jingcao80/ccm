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

#include "ccm/text/CAttributedCharacterIteratorAttribute.h"
#include "ccm/text/CAttributedString.h"
#include "ccm/text/CDateFormatField.h"
#include "ccm/text/CSimpleDateFormat.h"
#include "ccm/text/CParsePosition.h"
#include <ccmapi.h>
#include <new>

namespace ccm {
namespace text {

CCM_OBJECT_IMPL(CAttributedCharacterIteratorAttribute);
ECode CAttributedCharacterIteratorAttribute::New(
    /* [in] */ const String& name,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ ccm::IInterface** object)
{
    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CAttributedCharacterIteratorAttribute, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CAttributedCharacterIteratorAttribute), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CAttributedCharacterIteratorAttribute* _obj = new(addr) CAttributedCharacterIteratorAttribute();
    ec = _obj->Constructor(name);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    _obj->AttachMetadata(comp, String("ccm::text::CAttributedCharacterIteratorAttribute"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
};

CCM_OBJECT_IMPL(CAttributedString);

CCM_OBJECT_IMPL(CDateFormatField);
ECode CDateFormatField::New(
    /* [in] */ const String& name,
    /* [in] */ Integer calendarField,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ ccm::IInterface** object)
{
    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CDateFormatField, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CDateFormatField), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CDateFormatField* _obj = new(addr) CDateFormatField();
    ec = _obj->Constructor(name, calendarField);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    _obj->AttachMetadata(comp, String("ccm::text::CDateFormatField"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

CCM_OBJECT_IMPL(CSimpleDateFormat);
CCM_OBJECT_IMPL(CParsePosition);

}
}

