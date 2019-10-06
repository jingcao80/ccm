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

#include "CMetaComponent.h"
#include "CMetaEnumeration.h"
#include "CMetaEnumerator.h"

namespace como {

COMO_INTERFACE_IMPL_LIGHT_1(CMetaEnumeration, LightRefBase, IMetaEnumeration);

CMetaEnumeration::CMetaEnumeration(
    /* [in] */ CMetaComponent* mcObj,
    /* [in] */ MetaComponent* mc,
    /* [in] */ MetaEnumeration* me)
    : mMetadata(me)
    , mOwner(mcObj)
    , mName(me->mName)
    , mNamespace(me->mNamespace)
    , mMetaEnumerators(me->mEnumeratorNumber)
{}

CMetaEnumeration::~CMetaEnumeration()
{
    mMetadata = nullptr;
    mOwner = nullptr;
}

ECode CMetaEnumeration::GetComponent(
    /* [out] */ AutoPtr<IMetaComponent>& metaComp)
{
    metaComp = mOwner;
    return NOERROR;
}

ECode CMetaEnumeration::GetName(
    /* [out] */ String& name)
{
    name = mName;
    return NOERROR;
}

ECode CMetaEnumeration::GetNamespace(
    /* [out] */ String& ns)
{
    ns = mNamespace;
    return NOERROR;
}

ECode CMetaEnumeration::GetEnumeratorNumber(
    /* [out] */ Integer& number)
{
    number = mMetadata->mEnumeratorNumber;
    return NOERROR;
}

ECode CMetaEnumeration::GetAllEnumerators(
    /* [out] */ Array<IMetaEnumerator*>& enumrs)
{
    if (mMetaEnumerators.IsEmpty()) {
        return NOERROR;
    }

    BuildAllEnumerators();

    for (Integer i = 0; i < mMetaEnumerators.GetLength(); i++) {
        enumrs.Set(i, mMetaEnumerators[i]);
    }

    return NOERROR;
}

ECode CMetaEnumeration::GetEnumerator(
    /* [in] */ const String& name,
    /* [out] */ AutoPtr<IMetaEnumerator>& metaEnumr)
{
    if (name.IsEmpty() || mMetaEnumerators.IsEmpty()) {
        metaEnumr = nullptr;
        return NOERROR;
    }

    for (Integer i = 0; i < mMetaEnumerators.GetLength(); i++) {
        String enumrName;
        mMetaEnumerators[i]->GetName(enumrName);
        if (enumrName.Equals(name)) {
            metaEnumr = mMetaEnumerators[i];
            return NOERROR;
        }
    }

    metaEnumr = nullptr;
    return NOERROR;
}

void CMetaEnumeration::BuildAllEnumerators()
{
    if (mMetadata->mEnumeratorNumber == 0) {
        return;
    }

    if (mMetaEnumerators[0] == nullptr) {
        for (Integer i = 0; i < mMetadata->mEnumeratorNumber; i++) {
            MetaEnumerator* me = mMetadata->mEnumerators[i];
            IMetaEnumerator* meObj = new CMetaEnumerator(this, me);
            mMetaEnumerators.Set(i, meObj);
        }
    }
}

} // namespace como
