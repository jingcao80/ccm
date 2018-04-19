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

CMetaInterface::CMetaInterface()
    : mMetadata(nullptr)
    , mOwner(nullptr)
{}

CMetaInterface::CMetaInterface(
    /* [in] */ CMetaComponent* mcObj,
    /* [in] */ MetaComponent* mc,
    /* [in] */ MetaInterface* mi)
    : mMetadata(mi)
    , mOwner(mcObj)
    , mName(mi->mName)
    , mNamespace(mi->mNamespace)
    , mMetaConstants(mi->mConstantNumber)
    , mMetaMethods(mi->mMethodNumber)
{
    mIid.mUuid = mi->mUuid;
    mIid.mCid = &mcObj->mCid;
}

CMetaInterface::~CMetaInterface()
{
    mMetadata = nullptr;
    mOwner = nullptr;
}

ECode CMetaInterface::GetComponent(
    /* [out] */ IMetaComponent** metaComp)
{
    VALIDATE_NOT_NULL(metaComp);

    *metaComp = (IMetaComponent*)mOwner;
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

ECode CMetaInterface::GetBaseInterface(
    /* [out] */ IMetaInterface** baseIntf)
{
    return NOERROR;
}

ECode CMetaInterface::GetConstantNumber(
    /* [out] */ Integer* number)
{
    return NOERROR;
}

ECode CMetaInterface::GetAllConstants(
    /* [out] */ Array<IMetaConstant*>& consts)
{
    return NOERROR;
}

ECode CMetaInterface::GetConstant(
    /* [in] */ const String& name,
    /* [out] */ IMetaConstant** constt)
{
    return NOERROR;
}

ECode CMetaInterface::GetConstant(
    /* [in] */ Integer index,
    /* [out] */ IMetaConstant** constt)
{
    return NOERROR;
}

ECode CMetaInterface::GetMethodNumber(
    /* [out] */ Integer* number)
{
    return NOERROR;
}

ECode CMetaInterface::GetAllMethods(
    /* [out] */ Array<IMetaMethod*>& methods)
{
    return NOERROR;
}

ECode CMetaInterface::GetMethod(
    /* [in] */ const String& name,
    /* [in] */ const String& signature,
    /* [out] */ IMetaMethod** method)
{
    return NOERROR;
}

ECode CMetaInterface::GetMethod(
    /* [in] */ Integer index,
    /* [out] */ IMetaMethod** method)
{
    return NOERROR;
}

}
