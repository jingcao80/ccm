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

#include "ccmautoptr.h"
#include "CMetaComponent.h"
#include "CMetaCoclass.h"

#include <stdlib.h>

namespace ccm {

CCM_INTERFACE_IMPL_LIGHT_1(CMetaComponent, IMetaComponent);

CMetaComponent::CMetaComponent(
    /* [in] */ MetaComponent* metadata)
    : mMetadata(metadata)
    , mMetaCoclasses(mMetadata->mCoclassNumber)
    , mMetaCoclassMap(mMetadata->mCoclassNumber)
    , mMetaInterfaces(mMetadata->mInterfaceNumber)
    , mMetaInterfaceMap(mMetadata->mInterfaceNumber)
{
    for (Integer i = 0; i < mMetadata->mCoclassNumber; i++) {
        AutoPtr<IMetaCoclass> mc = new CMetaCoclass();
        mMetaCoclasses.Set(i, mc);
    }
}

CMetaComponent::~CMetaComponent()
{
    if (mMetadata != nullptr) {
        free(mMetadata);
        mMetadata = nullptr;
    }
}

ECode CMetaComponent::GetName(
    /* [ou] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mMetadata->mName;
    return NOERROR;
}

ECode CMetaComponent::GetComponentID(
    /* [out] */ ComponentID* cid)
{
    VALIDATE_NOT_NULL(cid);

    cid->mUuid = mMetadata->mUuid;
    cid->mUrl = mMetadata->mUrl;
    return NOERROR;
}

ECode CMetaComponent::GetCoclassNumber(
    /* [out] */ Integer* number)
{
    VALIDATE_NOT_NULL(number);

    *number = mMetadata->mCoclassNumber;
    return NOERROR;
}

ECode CMetaComponent::GetAllCoclasses(
    /* [out] */ Array<IMetaCoclass*>& klasses)
{
    return NOERROR;
}

ECode CMetaComponent::GetInterfaceNumber(
    /* [out] */ Integer* number)
{
    VALIDATE_NOT_NULL(number);

    *number = mMetadata->mInterfaceNumber;
    return NOERROR;
}

ECode CMetaComponent::GetAllInterfaces(
    /* [out] */ Array<IMetaInterface*>& intfs)
{
    return NOERROR;
}

}
