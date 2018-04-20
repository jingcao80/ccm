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
#include "CMetaConstant.h"
#include "CMetaInterface.h"
#include "CMetaMethod.h"

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
    VALIDATE_NOT_NULL(baseIntf);

    *baseIntf = mBaseInterface;
    REFCOUNT_ADD(*baseIntf);
    return NOERROR;
}

ECode CMetaInterface::GetConstantNumber(
    /* [out] */ Integer* number)
{
    VALIDATE_NOT_NULL(number);

    *number = mMetaConstants.GetLength();
    return NOERROR;
}

ECode CMetaInterface::GetAllConstants(
    /* [out] */ Array<IMetaConstant*>& consts)
{
    BuildAllConstants();

    Integer N = MIN(mMetaConstants.GetLength(), consts.GetLength());
    for (Integer i = 0; i < N; i++) {
        consts.Set(i, mMetaConstants[i]);
    }
    return NOERROR;
}

ECode CMetaInterface::GetConstant(
    /* [in] */ const String& name,
    /* [out] */ IMetaConstant** constt)
{
    VALIDATE_NOT_NULL(constt);

    BuildAllConstants();

    if (name.IsNullOrEmpty()) {
        *constt = nullptr;
        return NOERROR;
    }

    for (Integer i = 0; i < mMetaConstants.GetLength(); i++) {
        String mcName;
        mMetaConstants[i]->GetName(&mcName);
        if (mcName.Equals(name)) {
            *constt = mMetaConstants[i];
            REFCOUNT_ADD(*constt);
            return NOERROR;
        }
    }
    *constt = nullptr;
    return NOERROR;
}

ECode CMetaInterface::GetConstant(
    /* [in] */ Integer index,
    /* [out] */ IMetaConstant** constt)
{
    VALIDATE_NOT_NULL(constt);

    if (index < 0 || index >= mMetaConstants.GetLength()) {
        *constt = nullptr;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    BuildAllConstants();

    *constt = mMetaConstants[index];
    REFCOUNT_ADD(*constt);
    return NOERROR;
}

ECode CMetaInterface::GetMethodNumber(
    /* [out] */ Integer* number)
{
    VALIDATE_NOT_NULL(number);

    *number = mMetaMethods.GetLength();
    return NOERROR;
}

ECode CMetaInterface::GetAllMethods(
    /* [out] */ Array<IMetaMethod*>& methods)
{
    BuildAllMethods();

    Integer N = MIN(mMetaMethods.GetLength(), methods.GetLength());
    for (Integer i = 0; i < N; i++) {
        methods.Set(i, mMetaMethods[i]);
    }
    return NOERROR;
}

ECode CMetaInterface::GetMethod(
    /* [in] */ const String& name,
    /* [in] */ const String& signature,
    /* [out] */ IMetaMethod** method)
{
    VALIDATE_NOT_NULL(method);

    if (name.IsNullOrEmpty()) {
        *method = nullptr;
        return NOERROR;
    }

    for (Integer i = 0; i < mMetaMethods.GetLength(); i++) {
        IMetaMethod* mmObj = mMetaMethods[i];
        String mmName, mmSignature;
        mmObj->GetName(&mmName);
        mmObj->GetSignature(&mmSignature);
        if (mmName.Equals(name) && mmSignature.Equals(signature)) {
            *method = mmObj;
            REFCOUNT_ADD(*method);
            return NOERROR;
        }
    }
    *method = nullptr;
    return NOERROR;
}

ECode CMetaInterface::GetMethod(
    /* [in] */ Integer index,
    /* [out] */ IMetaMethod** method)
{
    VALIDATE_NOT_NULL(method);

    if (index < 0 || index >= mMetaMethods.GetLength()) {
        *method = nullptr;
        return NOERROR;
    }

    *method = mMetaMethods[index];
    REFCOUNT_ADD(*method);
    return NOERROR;
}

void CMetaInterface::BuildAllConstants()
{
    if (mMetaConstants[0] == nullptr) {
        for (Integer i = 0; i < mMetadata->mConstantNumber; i++) {
            CMetaConstant* mcObj = new CMetaConstant(
                    mOwner->mMetadata, mMetadata->mConstants[i]);
            mMetaConstants.Set(i, mcObj);
        }
    }
}

void CMetaInterface::BuildAllMethods()
{
    if (mMetaMethods[0] == nullptr) {
        for (Integer i = 0; i < mMetadata->mMethodNumber; i++) {
            CMetaMethod* mmObj = new CMetaMethod(mOwner->mMetadata,
                    this, mMetadata->mMethods[i]);
            mMetaMethods.Set(i, mmObj);
        }
    }
}

}
