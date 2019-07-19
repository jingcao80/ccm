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

#include "ccm/util/CArrayList.h"
#include "ccm/util/CDate.h"
#include "ccm/util/CFormatter.h"
#include "ccm/util/CGregorianCalendar.h"
#include "ccm/util/CHashMap.h"
#include "ccm/util/CHashSet.h"
#include "ccm/util/CHashtable.h"
#include "ccm/util/CLinkedHashMap.h"
#include "ccm/util/CLinkedHashSet.h"
#include "ccm/util/CLocale.h"
#include "ccm/util/CLocaleBuilder.h"
#include "ccm/util/CProperties.h"
#include "ccm/util/CPropertyPermission.h"
#include "ccm/util/CRandom.h"
#include "ccm/util/CSimpleEntry.h"
#include "ccm/util/CSimpleImmutableEntry.h"
#include "ccm/util/CSimpleTimeZone.h"
#include "ccm/util/CStringTokenizer.h"
#include "ccm/util/CTreeMap.h"
#include "ccm/util/CTreeSet.h"
#include "ccm/util/CVector.h"
#include "ccm/util/calendar/CEra.h"
#include "ccm/util/calendar/CGregorian.h"
#include "ccm/util/calendar/CJulianCalendar.h"
#include "ccm/util/calendar/CLocalGregorianCalendar.h"
#include "ccm/util/concurrent/CConcurrentLinkedQueue.h"
#include "ccm/util/concurrent/CConcurrentHashMap.h"
#include "ccm/util/concurrent/atomic/CAtomicBoolean.h"
#include "ccm/util/concurrent/atomic/CAtomicInteger.h"
#include "ccm/util/concurrent/atomic/CAtomicLong.h"
#include "ccm.util.ICLocaleClassObject.h"
#include <ccmapi.h>
#include <new>

namespace ccm {
namespace util {

CCM_OBJECT_IMPL(CArrayList);
ECode CArrayList::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CArrayList, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CArrayList), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CArrayList* newObj = new(addr) CArrayList();
    ec = ArrayList::CloneImpl(newObj);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    newObj->AttachMetadata(comp, String("ccm::util::CArrayList"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CDate);
ECode CDate::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CDate, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CDate), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CDate* newObj = new(addr) CDate();
    ec = Date::CloneImpl(newObj);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    newObj->AttachMetadata(comp, String("ccm::util::CDate"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CFormatter);

CCM_OBJECT_IMPL(CGregorianCalendar);
ECode CGregorianCalendar::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CGregorianCalendar, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CGregorianCalendar), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CGregorianCalendar* newObj = new(addr) CGregorianCalendar();
    ec = GregorianCalendar::CloneImpl(newObj);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    newObj->AttachMetadata(comp, String("ccm::util::CGregorianCalendar"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode CGregorianCalendar::New(
    /* [in] */ Integer year,
    /* [in] */ Integer month,
    /* [in] */ Integer dayOfMonth,
    /* [in] */ Integer hourOfDay,
    /* [in] */ Integer minute,
    /* [in] */ Integer second,
    /* [in] */ Integer millis,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ ccm::IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CGregorianCalendar, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CGregorianCalendar), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CGregorianCalendar* _obj = new(addr) CGregorianCalendar();
    ec = _obj->Constructor(year, month, dayOfMonth, hourOfDay, minute, second, millis);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    _obj->AttachMetadata(comp, String("ccm::util::CGregorianCalendar"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CGregorianCalendar::New(
    /* [in] */ ITimeZone* zone,
    /* [in] */ ILocale* locale,
    /* [in] */ Boolean flag,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ ccm::IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CGregorianCalendar, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CGregorianCalendar), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CGregorianCalendar* _obj = new(addr) CGregorianCalendar();
    ec = _obj->Constructor(zone, locale, flag);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    _obj->AttachMetadata(comp, String("ccm::util::CGregorianCalendar"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CGregorianCalendar::New(
    /* [in] */ Long milliseconds,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ ccm::IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CGregorianCalendar, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CGregorianCalendar), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CGregorianCalendar* _obj = new(addr) CGregorianCalendar();
    ec = _obj->Constructor(milliseconds);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    _obj->AttachMetadata(comp, String("ccm::util::CGregorianCalendar"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

CCM_OBJECT_IMPL(CHashMap);
ECode CHashMap::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CHashMap, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CHashMap), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CHashMap* newObj = new(addr) CHashMap();
    ec = HashMap::CloneImpl(newObj);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    newObj->AttachMetadata(comp, String("ccm::util::CHashMap"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CHashSet);
ECode CHashSet::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CHashSet, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CHashSet), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CHashSet* newObj = new(addr) CHashSet();
    ec = HashSet::CloneImpl(newObj);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    newObj->AttachMetadata(comp, String("ccm::util::CHashSet"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CHashtable);
ECode CHashtable::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CHashtable, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CHashtable), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CHashtable* newObj = new(addr) CHashtable();
    ec = Hashtable::CloneImpl(newObj);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    newObj->AttachMetadata(comp, String("ccm::util::CHashtable"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CLinkedHashMap);
ECode CLinkedHashMap::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CLinkedHashMap, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CLinkedHashMap), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CLinkedHashMap* newObj = new(addr) CLinkedHashMap();
    ec = LinkedHashMap::CloneImpl(newObj);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    newObj->AttachMetadata(comp, String("ccm::util::CLinkedHashMap"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CLinkedHashSet);
ECode CLinkedHashSet::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CLinkedHashSet, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CLinkedHashSet), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CLinkedHashSet* newObj = new(addr) CLinkedHashSet();
    ec = LinkedHashSet::CloneImpl(newObj);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    newObj->AttachMetadata(comp, String("ccm::util::CLinkedHashSet"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CLocale);
ECode CLocale::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CLocale, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CLocale), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CLocale* newObj = new(addr) CLocale();
    ec = Locale::CloneImpl(newObj);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    newObj->AttachMetadata(comp, String("ccm::util::CLocale"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode CLocale::New(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ ccm::IInterface** object)
{
    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CLocale, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CLocale), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CLocale* _obj = new(addr) CLocale();

    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    _obj->AttachMetadata(comp, String("ccm::util::CLocale"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
};

ECode CLocale::New(
    /* [in] */ BaseLocale* baseLocale,
    /* [in] */ LocaleExtensions* extensions,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ ccm::IInterface** object)
{
    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CLocale, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CLocale), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CLocale* _obj = new(addr) CLocale();
    ec = _obj->Constructor(baseLocale, extensions);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    _obj->AttachMetadata(comp, String("ccm::util::CLocale"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
};

CCM_OBJECT_IMPL(CLocaleBuilder);

CCM_OBJECT_IMPL(CProperties);
ECode CProperties::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CProperties, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CProperties), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CProperties* newObj = new(addr) CProperties();
    ec = Properties::CloneImpl(newObj);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    newObj->AttachMetadata(comp, String("ccm::util::CProperties"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CPropertyPermission);
CCM_OBJECT_IMPL(CRandom);
CCM_OBJECT_IMPL(CSimpleEntry);
CCM_OBJECT_IMPL(CSimpleImmutableEntry);

CCM_OBJECT_IMPL(CSimpleTimeZone);
ECode CSimpleTimeZone::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CSimpleTimeZone, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CSimpleTimeZone), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CSimpleTimeZone* newObj = new(addr) CSimpleTimeZone();
    ec = SimpleTimeZone::CloneImpl(newObj);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    newObj->AttachMetadata(comp, String("ccm::util::CSimpleTimeZone"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode CSimpleTimeZone::New(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ ccm::IInterface** object)
{
    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CSimpleTimeZone, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CSimpleTimeZone), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CSimpleTimeZone* _obj = new(addr) CSimpleTimeZone();

    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    _obj->AttachMetadata(comp, String("ccm::util::CSimpleTimeZone"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
};

CCM_OBJECT_IMPL(CStringTokenizer);

CCM_OBJECT_IMPL(CTreeMap);
ECode CTreeMap::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CTreeMap, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CTreeMap), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CTreeMap* newObj = new(addr) CTreeMap();
    ec = CTreeMap::CloneImpl(newObj);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    newObj->AttachMetadata(comp, String("ccm::util::CTreeMap"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CTreeSet);
ECode CTreeSet::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CTreeSet, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CTreeSet), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CTreeSet* newObj = new(addr) CTreeSet();
    ec = CTreeSet::CloneImpl(newObj);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    newObj->AttachMetadata(comp, String("ccm::util::CTreeSet"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode CTreeSet::New(
    /* [in] */ INavigableMap* m,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ ccm::IInterface** object)
{
    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CTreeSet, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CTreeSet), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CTreeSet* _obj = new(addr) CTreeSet();
    ec = _obj->Constructor(m);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    _obj->AttachMetadata(comp, String("ccm::util::CTreeSet"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

CCM_OBJECT_IMPL(CVector);
ECode CVector::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CVector, nullptr, &clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CVector), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CVector* newObj = new(addr) CVector();
    ec = CVector::CloneImpl(newObj);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(&comp);
    newObj->AttachMetadata(comp, String("ccm::util::CVector"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

namespace calendar {

CCM_OBJECT_IMPL(CEra);
CCM_OBJECT_IMPL(CGregorian);
CCM_OBJECT_IMPL(CJulianCalendar);
CCM_OBJECT_IMPL(CLocalGregorianCalendar);

}

namespace concurrent {

CCM_OBJECT_IMPL(CConcurrentLinkedQueue);
CCM_OBJECT_IMPL(CConcurrentHashMap);

namespace atomic {

CCM_OBJECT_IMPL(CAtomicBoolean);
CCM_OBJECT_IMPL(CAtomicInteger);
CCM_OBJECT_IMPL(CAtomicLong);

}
}

}
}
