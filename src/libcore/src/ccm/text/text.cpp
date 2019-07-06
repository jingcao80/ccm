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
#include "ccm/text/CChoiceFormat.h"
#include "ccm/text/CDateFormatField.h"
#include "ccm/text/CDateFormatSymbols.h"
#include "ccm/text/CDecimalFormat.h"
#include "ccm/text/CDecimalFormatSymbols.h"
#include "ccm/text/CFieldPosition.h"
#include "ccm/text/CMessageFormat.h"
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

CCM_OBJECT_IMPL(CChoiceFormat);
ECode CChoiceFormat::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CChoiceFormat, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CChoiceFormat), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CChoiceFormat* cfObj = new(addr) CChoiceFormat();
    ec = ChoiceFormat::CloneImpl(cfObj);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    cfObj->AttachMetadata(comp, String("ccm::text::CChoiceFormat"));
    *obj = cfObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

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

CCM_OBJECT_IMPL(CDateFormatSymbols)
ECode CDateFormatSymbols::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CDateFormatSymbols, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CDateFormatSymbols), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CDateFormatSymbols* dfsObj = new(addr) CDateFormatSymbols();
    ec = DateFormatSymbols::CloneImpl(dfsObj);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    dfsObj->AttachMetadata(comp, String("ccm::text::CDateFormatSymbols"));
    *obj = dfsObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CDecimalFormat);
ECode CDecimalFormat::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CDecimalFormat, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CDecimalFormat), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CDecimalFormat* dfObj = new(addr) CDecimalFormat();
    ec = DecimalFormat::CloneImpl(dfObj);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    dfObj->AttachMetadata(comp, String("ccm::text::CDecimalFormat"));
    *obj = dfObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CDecimalFormatSymbols);
ECode CDecimalFormatSymbols::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CDecimalFormatSymbols, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CDecimalFormatSymbols), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CDecimalFormatSymbols* dfsObj = new(addr) CDecimalFormatSymbols();
    ec = DecimalFormatSymbols::CloneImpl(dfsObj);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    dfsObj->AttachMetadata(comp, String("ccm::text::CDecimalFormatSymbols"));
    *obj = dfsObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CFieldPosition);

CCM_OBJECT_IMPL(CMessageFormat);
ECode CMessageFormat::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CMessageFormat, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CMessageFormat), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CMessageFormat* mfObj = new(addr) CMessageFormat();
    ec = MessageFormat::CloneImpl(mfObj);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    mfObj->AttachMetadata(comp, String("ccm::text::CMessageFormat"));
    *obj = mfObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CSimpleDateFormat);
ECode CSimpleDateFormat::New(
    /* [in] */ Integer timeStyle,
    /* [in] */ Integer dateStyle,
    /* [in] */ ILocale* loc,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ ccm::IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CSimpleDateFormat, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CSimpleDateFormat), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CSimpleDateFormat* sdfObj = new(addr) CSimpleDateFormat();
    ec = sdfObj->Constructor(timeStyle, dateStyle, loc);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    sdfObj->AttachMetadata(comp, String("ccm::text::CSimpleDateFormat"));
    *object = sdfObj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CSimpleDateFormat::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CSimpleDateFormat, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CSimpleDateFormat), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CSimpleDateFormat* sdfObj = new(addr) CSimpleDateFormat();
    ec = CSimpleDateFormat::CloneImpl(sdfObj);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    sdfObj->AttachMetadata(comp, String("ccm::text::CSimpleDateFormat"));
    *obj = sdfObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CParsePosition);

}
}

