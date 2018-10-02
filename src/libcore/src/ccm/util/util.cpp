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
#include "ccm/util/CStringTokenizer.h"
#include "ccm/util/CTreeMap.h"
#include "ccm/util/CTreeSet.h"
#include "ccm/util/calendar/CEra.h"
#include "ccm/util/calendar/CGregorian.h"
#include "ccm/util/calendar/CJulianCalendar.h"
#include "ccm/util/calendar/CLocalGregorianCalendar.h"
#include "ccm/util/concurrent/CConcurrentLinkedQueue.h"
#include "ccm/util/concurrent/CConcurrentHashMap.h"
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

    AutoPtr<IArrayList> list;
    CArrayList::New(IID_IArrayList, (IInterface**)&list);
    FAIL_RETURN(ArrayList::CloneImpl(list));
    *obj = list->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CDate);
ECode CDate::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IDate> date;
    CDate::New(IID_IDate, (IInterface**)&date);
    FAIL_RETURN(Date::CloneImpl(date));
    *obj = date->Probe(iid);
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

    AutoPtr<IGregorianCalendar> gcal;
    CGregorianCalendar::New(IID_IGregorianCalendar, (IInterface**)&gcal);
    FAIL_RETURN(GregorianCalendar::CloneImpl(gcal));
    *obj = gcal->Probe(iid);
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

    AutoPtr<IHashMap> map;
    CHashMap::New(IID_IHashMap, (IInterface**)&map);
    FAIL_RETURN(HashMap::CloneImpl(map));
    *obj = map->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CHashSet);
ECode CHashSet::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IHashSet> set;
    CHashSet::New(IID_IHashSet, (IInterface**)&set);
    FAIL_RETURN(HashSet::CloneImpl(set));
    *obj = set->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CHashtable);
ECode CHashtable::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IHashtable> ht;
    CHashtable::New(IID_IHashtable, (IInterface**)&ht);
    FAIL_RETURN(Hashtable::CloneImpl(ht));
    *obj = ht->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CLinkedHashMap);
ECode CLinkedHashMap::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<ILinkedHashMap> map;
    CLinkedHashMap::New(IID_ILinkedHashMap, (IInterface**)&map);
    FAIL_RETURN(LinkedHashMap::CloneImpl(map));
    *obj = map->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CLinkedHashSet);
ECode CLinkedHashSet::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IHashSet> set;
    CLinkedHashSet::New(IID_IHashSet, (IInterface**)&set);
    FAIL_RETURN(LinkedHashSet::CloneImpl(set));
    *obj = set->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CLocale);
ECode CLocale::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<ILocale> locale;
    CLocale::New(IID_ILocale, (IInterface**)&locale);
    FAIL_RETURN(Locale::CloneImpl(locale));
    *obj = locale->Probe(iid);
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

    AutoPtr<IProperties> prop;
    CProperties::New(IID_IProperties, (IInterface**)&prop);
    FAIL_RETURN(Properties::CloneImpl(IHashtable::Probe(prop)));
    *obj = prop->Probe(iid);
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CPropertyPermission);
CCM_OBJECT_IMPL(CRandom);
CCM_OBJECT_IMPL(CStringTokenizer);
CCM_OBJECT_IMPL(CTreeMap);
CCM_INTERFACE_IMPL_1(CTreeMap, SyncObject, ITreeMap);
CCM_OBJECT_IMPL(CTreeSet);
CCM_INTERFACE_IMPL_1(CTreeSet, SyncObject, ITreeSet);

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

CCM_OBJECT_IMPL(CAtomicInteger);
CCM_OBJECT_IMPL(CAtomicLong);

}
}

}
}
