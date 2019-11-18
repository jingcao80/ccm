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

#include "reflection/CMetaComponent.h"
#include "reflection/CMetaEnumeration.h"
#include "reflection/CMetaEnumerator.h"
#include "reflection/reflection.h"

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
    , mEnumerators(me->mEnumeratorNumber)
{}

ECode CMetaEnumeration::GetComponent(
    /* [out] */ AutoPtr<IMetaComponent>& comp)
{
    comp = mOwner;
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
    ns = mNamespace.Equals(NAMESPACE_GLOBAL) ? "" : mNamespace;
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
    if (mEnumerators.IsEmpty()) {
        return NOERROR;
    }

    BuildAllEnumerators();

    for (Integer i = 0; i < mEnumerators.GetLength(); i++) {
        enumrs.Set(i, mEnumerators[i]);
    }

    return NOERROR;
}

ECode CMetaEnumeration::GetEnumerator(
    /* [in] */ const String& name,
    /* [out] */ AutoPtr<IMetaEnumerator>& enumr)
{
    if (name.IsEmpty() || mEnumerators.IsEmpty()) {
        enumr = nullptr;
        return NOERROR;
    }

    BuildAllEnumerators();

    for (Integer i = 0; i < mEnumerators.GetLength(); i++) {
        String enumrName;
        mEnumerators[i]->GetName(enumrName);
        if (enumrName.Equals(name)) {
            enumr = mEnumerators[i];
            return NOERROR;
        }
    }

    enumr = nullptr;
    return NOERROR;
}

void CMetaEnumeration::BuildAllEnumerators()
{
    if (mEnumerators[0] == nullptr) {
        Mutex::AutoLock lock(mEnumeratorsLock);
        if (mEnumerators[0] == nullptr) {
            for (Integer i = 0; i < mMetadata->mEnumeratorNumber; i++) {
                MetaEnumerator* me = mMetadata->mEnumerators[i];
                AutoPtr<IMetaEnumerator> meObj = new CMetaEnumerator(this, me);
                mEnumerators.Set(i, meObj);
            }
        }
    }
}

} // namespace como
