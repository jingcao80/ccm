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

#include "como/util/CArrayList.h"
#include "como/util/CDate.h"
#include "como/util/CFormatter.h"
#include "como/util/CGregorianCalendar.h"
#include "como/util/CHashMap.h"
#include "como/util/CHashSet.h"
#include "como/util/CHashtable.h"
#include "como/util/CLinkedHashMap.h"
#include "como/util/CLinkedHashSet.h"
#include "como/util/CLinkedList.h"
#include "como/util/CLocale.h"
#include "como/util/CLocaleBuilder.h"
#include "como/util/CProperties.h"
#include "como/util/CPropertyPermission.h"
#include "como/util/CRandom.h"
#include "como/util/CSimpleEntry.h"
#include "como/util/CSimpleImmutableEntry.h"
#include "como/util/CSimpleTimeZone.h"
#include "como/util/CStringTokenizer.h"
#include "como/util/CTreeMap.h"
#include "como/util/CTreeSet.h"
#include "como/util/CVector.h"
#include "como/util/calendar/CEra.h"
#include "como/util/calendar/CGregorian.h"
#include "como/util/calendar/CJulianCalendar.h"
#include "como/util/calendar/CLocalGregorianCalendar.h"
#include "como/util/concurrent/CConcurrentLinkedQueue.h"
#include "como/util/concurrent/CConcurrentHashMap.h"
#include "como/util/concurrent/atomic/CAtomicBoolean.h"
#include "como/util/concurrent/atomic/CAtomicInteger.h"
#include "como/util/concurrent/atomic/CAtomicLong.h"
#include "como.util.ICLocaleClassObject.h"
#include <comoapi.h>
#include <new>

namespace como {
namespace util {

COMO_OBJECT_IMPL(CArrayList);
ECode CArrayList::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CArrayList, nullptr, clsObject);
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
    clsObject->GetMetadate(comp);
    newObj->AttachMetadata(comp, String("como::util::CArrayList"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

COMO_OBJECT_IMPL(CDate);
ECode CDate::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CDate, nullptr, clsObject);
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
    clsObject->GetMetadate(comp);
    newObj->AttachMetadata(comp, String("como::util::CDate"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

COMO_OBJECT_IMPL(CFormatter);

COMO_OBJECT_IMPL(CGregorianCalendar);
ECode CGregorianCalendar::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CGregorianCalendar, nullptr, clsObject);
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
    clsObject->GetMetadate(comp);
    newObj->AttachMetadata(comp, String("como::util::CGregorianCalendar"));
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
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CGregorianCalendar, nullptr, clsObject);
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
    clsObject->GetMetadate(comp);
    _obj->AttachMetadata(comp, String("como::util::CGregorianCalendar"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CGregorianCalendar::New(
    /* [in] */ ITimeZone* zone,
    /* [in] */ ILocale* locale,
    /* [in] */ Boolean flag,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CGregorianCalendar, nullptr, clsObject);
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
    clsObject->GetMetadate(comp);
    _obj->AttachMetadata(comp, String("como::util::CGregorianCalendar"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CGregorianCalendar::New(
    /* [in] */ Long milliseconds,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CGregorianCalendar, nullptr, clsObject);
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
    clsObject->GetMetadate(comp);
    _obj->AttachMetadata(comp, String("como::util::CGregorianCalendar"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

COMO_OBJECT_IMPL(CHashMap);
ECode CHashMap::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CHashMap, nullptr, clsObject);
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
    clsObject->GetMetadate(comp);
    newObj->AttachMetadata(comp, String("como::util::CHashMap"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

COMO_OBJECT_IMPL(CHashSet);
ECode CHashSet::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CHashSet, nullptr, clsObject);
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
    clsObject->GetMetadate(comp);
    newObj->AttachMetadata(comp, String("como::util::CHashSet"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

COMO_OBJECT_IMPL(CHashtable);
ECode CHashtable::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CHashtable, nullptr, clsObject);
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
    clsObject->GetMetadate(comp);
    newObj->AttachMetadata(comp, String("como::util::CHashtable"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

COMO_OBJECT_IMPL(CLinkedHashMap);
ECode CLinkedHashMap::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CLinkedHashMap, nullptr, clsObject);
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
    clsObject->GetMetadate(comp);
    newObj->AttachMetadata(comp, String("como::util::CLinkedHashMap"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

COMO_OBJECT_IMPL(CLinkedHashSet);
ECode CLinkedHashSet::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CLinkedHashSet, nullptr, clsObject);
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
    clsObject->GetMetadate(comp);
    newObj->AttachMetadata(comp, String("como::util::CLinkedHashSet"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

COMO_OBJECT_IMPL(CLinkedList);
ECode CLinkedList::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CLinkedList, nullptr, clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CLinkedList), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CLinkedList* newObj = new(addr) CLinkedList();
    ec = LinkedList::CloneImpl(newObj);
    if (FAILED(ec)) {
        free(addr);
        return ec;
    }
    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(comp);
    newObj->AttachMetadata(comp, String("como::util::CLinkedList"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

COMO_OBJECT_IMPL(CLocale);
ECode CLocale::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CLocale, nullptr, clsObject);
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
    clsObject->GetMetadate(comp);
    newObj->AttachMetadata(comp, String("como::util::CLocale"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode CLocale::New(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** object)
{
    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CLocale, nullptr, clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CLocale), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CLocale* _obj = new(addr) CLocale();

    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(comp);
    _obj->AttachMetadata(comp, String("como::util::CLocale"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
};

ECode CLocale::New(
    /* [in] */ BaseLocale* baseLocale,
    /* [in] */ LocaleExtensions* extensions,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** object)
{
    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CLocale, nullptr, clsObject);
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
    clsObject->GetMetadate(comp);
    _obj->AttachMetadata(comp, String("como::util::CLocale"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
};

COMO_OBJECT_IMPL(CLocaleBuilder);

COMO_OBJECT_IMPL(CProperties);
ECode CProperties::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CProperties, nullptr, clsObject);
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
    clsObject->GetMetadate(comp);
    newObj->AttachMetadata(comp, String("como::util::CProperties"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

COMO_OBJECT_IMPL(CPropertyPermission);
COMO_OBJECT_IMPL(CRandom);
COMO_OBJECT_IMPL(CSimpleEntry);
COMO_OBJECT_IMPL(CSimpleImmutableEntry);

COMO_OBJECT_IMPL(CSimpleTimeZone);
ECode CSimpleTimeZone::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CSimpleTimeZone, nullptr, clsObject);
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
    clsObject->GetMetadate(comp);
    newObj->AttachMetadata(comp, String("como::util::CSimpleTimeZone"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode CSimpleTimeZone::New(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** object)
{
    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CSimpleTimeZone, nullptr, clsObject);
    if (FAILED(ec)) return ec;

    void* addr = calloc(sizeof(CSimpleTimeZone), 1);
    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;

    CSimpleTimeZone* _obj = new(addr) CSimpleTimeZone();

    AutoPtr<IMetaComponent> comp;
    clsObject->GetMetadate(comp);
    _obj->AttachMetadata(comp, String("como::util::CSimpleTimeZone"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
};

COMO_OBJECT_IMPL(CStringTokenizer);

COMO_OBJECT_IMPL(CTreeMap);
ECode CTreeMap::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CTreeMap, nullptr, clsObject);
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
    clsObject->GetMetadate(comp);
    newObj->AttachMetadata(comp, String("como::util::CTreeMap"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

COMO_OBJECT_IMPL(CTreeSet);
ECode CTreeSet::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CTreeSet, nullptr, clsObject);
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
    clsObject->GetMetadate(comp);
    newObj->AttachMetadata(comp, String("como::util::CTreeSet"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode CTreeSet::New(
    /* [in] */ INavigableMap* m,
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** object)
{
    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CTreeSet, nullptr, clsObject);
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
    clsObject->GetMetadate(comp);
    _obj->AttachMetadata(comp, String("como::util::CTreeSet"));
    *object = _obj->Probe(iid);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

COMO_OBJECT_IMPL(CVector);
ECode CVector::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IClassObject> clsObject;
    ECode ec = CoAcquireClassFactory(CID_CVector, nullptr, clsObject);
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
    clsObject->GetMetadate(comp);
    newObj->AttachMetadata(comp, String("como::util::CVector"));
    *obj = newObj->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

namespace calendar {

COMO_OBJECT_IMPL(CEra);
COMO_OBJECT_IMPL(CGregorian);
COMO_OBJECT_IMPL(CJulianCalendar);
COMO_OBJECT_IMPL(CLocalGregorianCalendar);

}

namespace concurrent {

COMO_OBJECT_IMPL(CConcurrentLinkedQueue);
COMO_OBJECT_IMPL(CConcurrentHashMap);

namespace atomic {

COMO_OBJECT_IMPL(CAtomicBoolean);
COMO_OBJECT_IMPL(CAtomicInteger);
COMO_OBJECT_IMPL(CAtomicLong);

}
}

}
}
