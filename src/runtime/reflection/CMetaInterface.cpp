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

#include "CMetaComponent.h"
#include "CMetaInterface.h"

namespace ccm {

CCM_INTERFACE_IMPL_LIGHT_1(CMetaInterface, IMetaInterface);

CMetaInterface::CMetaInterface(
    /* [in] */ CMetaComponent* mcObj,
    /* [in] */ MetaComponent* mc,
    /* [in] */ MetaInterface* mi)
    : mMetadata(mi)
    , mMetaComponent(mcObj)
    , mName(mi->mName)
    , mNamespace(mi->mNamespace)
{
    mIid.mUuid = mi->mUuid;
    mIid.mCid = &mcObj->mCid;
}

CMetaInterface::~CMetaInterface()
{
    mMetadata = nullptr;
    mMetaComponent = nullptr;
}

ECode CMetaInterface::GetMetaComponent(
    /* [out] */ IMetaComponent** metaComp)
{
    VALIDATE_NOT_NULL(metaComp);

    *metaComp = (IMetaComponent*)mMetaComponent;
    REFCOUNT_ADD(*metaComp);
    return NOERROR;
}

ECode CMetaInterface::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mName;
    return NOERROR;
}

ECode CMetaInterface::GetNamespace(
    /* [out] */ String* ns)
{
    VALIDATE_NOT_NULL(ns);

    *ns = mNamespace;
    return NOERROR;
}

ECode CMetaInterface::GetInterfaceID(
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid);

    *iid = mIid;
    return NOERROR;
}

}
