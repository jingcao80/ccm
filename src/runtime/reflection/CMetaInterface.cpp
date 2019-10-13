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
#include "CMetaConstant.h"
#include "CMetaInterface.h"
#include "CMetaMethod.h"

namespace como {

COMO_INTERFACE_IMPL_LIGHT_1(CMetaInterface, LightRefBase, IMetaInterface);

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
    BuildBaseInterface();
    mMetaMethods = Array<IMetaMethod*>(CalculateMethodNumber());
}

ECode CMetaInterface::GetComponent(
    /* [out] */ AutoPtr<IMetaComponent>& metaComp)
{
    metaComp = (IMetaComponent*)mOwner;
    return NOERROR;
}

ECode CMetaInterface::GetName(
    /* [out] */ String& name)
{
    name = mName;
    return NOERROR;
}

ECode CMetaInterface::GetNamespace(
    /* [out] */ String& ns)
{
    ns = mNamespace;
    return NOERROR;
}

ECode CMetaInterface::GetInterfaceID(
    /* [out] */ InterfaceID& iid)
{
    iid = mIid;
    return NOERROR;
}

ECode CMetaInterface::GetBaseInterface(
    /* [out] */ AutoPtr<IMetaInterface>& baseIntf)
{
    baseIntf = mBaseInterface;
    return NOERROR;
}

ECode CMetaInterface::GetConstantNumber(
    /* [out] */ Integer& number)
{
    number = mMetaConstants.GetLength();
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
    /* [out] */ AutoPtr<IMetaConstant>& constt)
{
    BuildAllConstants();

    if (name.IsEmpty()) {
        constt = nullptr;
        return NOERROR;
    }

    for (Integer i = 0; i < mMetaConstants.GetLength(); i++) {
        String mcName;
        mMetaConstants[i]->GetName(mcName);
        if (mcName.Equals(name)) {
            constt = mMetaConstants[i];
            return NOERROR;
        }
    }
    constt = nullptr;
    return NOERROR;
}

ECode CMetaInterface::GetConstant(
    /* [in] */ Integer index,
    /* [out] */ AutoPtr<IMetaConstant>& constt)
{
    if (index < 0 || index >= mMetaConstants.GetLength()) {
        constt = nullptr;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    BuildAllConstants();

    constt = mMetaConstants[index];
    return NOERROR;
}

ECode CMetaInterface::GetMethodNumber(
    /* [out] */ Integer& number)
{
    number = mMetaMethods.GetLength();
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
    /* [out] */ AutoPtr<IMetaMethod>& method)
{
    if (name.IsEmpty()) {
        method = nullptr;
        return NOERROR;
    }

    BuildAllMethods();

    for (Integer i = 0; i < mMetaMethods.GetLength(); i++) {
        IMetaMethod* mmObj = mMetaMethods[i];
        String mmName, mmSignature;
        mmObj->GetName(mmName);
        mmObj->GetSignature(mmSignature);
        if (mmName.Equals(name) && mmSignature.Equals(signature)) {
            method = mmObj;
            return NOERROR;
        }
    }
    method = nullptr;
    return NOERROR;
}

ECode CMetaInterface::GetMethod(
    /* [in] */ Integer index,
    /* [out] */ AutoPtr<IMetaMethod>& method)
{
    if (index < 0 || index >= mMetaMethods.GetLength()) {
        method = nullptr;
        return NOERROR;
    }

    BuildAllMethods();

    method = mMetaMethods[index];
    return NOERROR;
}

Integer CMetaInterface::CalculateMethodNumber()
{
    Integer number = 0;
    MetaInterface* mi = mMetadata;
    do {
        number += mi->mMethodNumber;
        if (mi->mBaseInterfaceIndex != -1) {
            mi = mOwner->mMetadata->mInterfaces[
                    mMetadata->mBaseInterfaceIndex];
        }
        else {
            mi = nullptr;
        }
    } while (mi != nullptr);
    return number;
}

void CMetaInterface::BuildBaseInterface()
{
    if (mMetadata->mBaseInterfaceIndex != -1) {
        AutoPtr<IMetaInterface> base =
                mOwner->BuildInterface(mMetadata->mBaseInterfaceIndex);
        mBaseInterface = (CMetaInterface*)base.Get();
    }
}

void CMetaInterface::BuildAllConstants()
{
    if (mMetaConstants[0] == nullptr) {
        for (Integer i = 0; i < mMetadata->mConstantNumber; i++) {
            AutoPtr<CMetaConstant> mcObj = new CMetaConstant(
                    mOwner->mMetadata, mMetadata->mConstants[i]);
            mMetaConstants.Set(i, mcObj);
        }
    }
}

void CMetaInterface::BuildAllMethods()
{
    if (mMetaMethods[0] == nullptr) {
        BuildInterfaceMethod(mMetadata);
    }
}

Integer CMetaInterface::BuildInterfaceMethod(
    /* [in] */ MetaInterface* mi)
{
    Integer startIndex = 0;
    if (mi->mBaseInterfaceIndex != -1) {
        startIndex = BuildInterfaceMethod(mOwner->mMetadata->mInterfaces[
                mi->mBaseInterfaceIndex]);
    }

    for (Integer i = 0; i < mi->mMethodNumber; i++) {
        AutoPtr<CMetaMethod> mmObj = new CMetaMethod(mOwner->mMetadata,
                this, startIndex + i, mi->mMethods[i]);
        mMetaMethods.Set(startIndex + i, mmObj);
    }
    return startIndex + mi->mMethodNumber;
}

} // namespace como
