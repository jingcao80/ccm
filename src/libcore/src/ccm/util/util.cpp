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

#include "ccm/util/CDate.h"
#include "ccm/util/CHashMap.h"
#include "ccm/util/CHashtable.h"
#include "ccm/util/CProperties.h"

namespace ccm {
namespace util {

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

}
}
