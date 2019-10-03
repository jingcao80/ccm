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

#include "CMetaEnumeration.h"
#include "CMetaEnumerator.h"

namespace ccm {

COMO_INTERFACE_IMPL_LIGHT_1(CMetaEnumerator, LightRefBase, IMetaEnumerator)

CMetaEnumerator::CMetaEnumerator(
    /* [in] */ CMetaEnumeration* menObj,
    /* [in] */ MetaEnumerator* me)
    : mMetadata(me)
    , mOwner(menObj)
    , mName(me->mName)
    , mValue(me->mValue)
{}

CMetaEnumerator::~CMetaEnumerator()
{
    mMetadata = nullptr;
    mOwner = nullptr;
}

ECode CMetaEnumerator::GetEnumeration(
    /* [out] */ IMetaEnumeration** metaEnumn)
{
    VALIDATE_NOT_NULL(metaEnumn);

    *metaEnumn = (IMetaEnumeration*)mOwner;
    REFCOUNT_ADD(*metaEnumn);
    return NOERROR;
}

ECode CMetaEnumerator::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mName;
    return NOERROR;
}

ECode CMetaEnumerator::GetValue(
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mValue;
    return NOERROR;
}

}
