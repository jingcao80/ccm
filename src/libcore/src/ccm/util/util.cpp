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
#include "ccm/util/CHashMap.h"
#include "ccm/util/CHashSet.h"
#include "ccm/util/CHashtable.h"
#include "ccm/util/CLinkedHashMap.h"
#include "ccm/util/CLinkedHashSet.h"
#include "ccm/util/CProperties.h"
#include "ccm/util/CStringTokenizer.h"
#include "ccm/util/calendar/CEra.h"
#include "ccm/util/calendar/CGregorian.h"
#include "ccm/util/calendar/CJulianCalendar.h"
#include "ccm/util/calendar/CLocalGregorianCalendar.h"

namespace ccm {
namespace util {

CCM_OBJECT_IMPL(CArrayList);
ECode CArrayList::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IArrayList> list;
    CArrayList::New(IID_IArrayList, (IInterface**)&list);
    FAIL_RETURN(ArrayList::CloneImpl(list));
    *obj = list;
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CDate);
ECode CDate::Clone(
    /* [out] */ IInterface** obj)
{
    return NOERROR;
}

CCM_OBJECT_IMPL(CHashMap);
ECode CHashMap::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IHashMap> map;
    CHashMap::New(IID_IHashMap, (IInterface**)&map);
    FAIL_RETURN(HashMap::CloneImpl(map));
    *obj = map;
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CHashSet);
ECode CHashSet::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IHashSet> set;
    CHashSet::New(IID_IHashSet, (IInterface**)&set);
    FAIL_RETURN(HashSet::CloneImpl(set));
    *obj = set;
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CHashtable);
ECode CHashtable::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IHashtable> ht;
    CHashtable::New(IID_IHashtable, (IInterface**)&ht);
    FAIL_RETURN(Hashtable::CloneImpl(ht));
    *obj = ht;
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CLinkedHashMap);
ECode CLinkedHashMap::Clone(
/* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<ILinkedHashMap> map;
    CLinkedHashMap::New(IID_ILinkedHashMap, (IInterface**)&map);
    FAIL_RETURN(LinkedHashMap::CloneImpl(map));
    *obj = map;
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CLinkedHashSet);
ECode CLinkedHashSet::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IHashSet> set;
    CLinkedHashSet::New(IID_IHashSet, (IInterface**)&set);
    FAIL_RETURN(LinkedHashSet::CloneImpl(set));
    *obj = set;
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CProperties);
ECode CProperties::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IProperties> prop;
    CProperties::New(IID_IProperties, (IInterface**)&prop);
    FAIL_RETURN(Properties::CloneImpl(IHashtable::Probe(prop)));
    *obj = prop;
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CCM_OBJECT_IMPL(CStringTokenizer);

namespace calendar {

CCM_OBJECT_IMPL(CEra);
CCM_OBJECT_IMPL(CGregorian);
CCM_OBJECT_IMPL(CJulianCalendar);
CCM_OBJECT_IMPL(CLocalGregorianCalendar);

}

}
}
