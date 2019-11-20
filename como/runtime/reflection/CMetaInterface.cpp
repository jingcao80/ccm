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
#include "reflection/CMetaConstant.h"
#include "reflection/CMetaInterface.h"
#include "reflection/CMetaMethod.h"
#include "reflection/reflection.h"

namespace como {

COMO_INTERFACE_IMPL_LIGHT_1(CMetaInterface, LightRefBase, IMetaInterface);

CMetaInterface::CMetaInterface(
    /* [in] */ CMetaComponent* mcObj,
    /* [in] */ MetaComponent* mc,
    /* [in] */ MetaInterface* mi)
    : mMetadata(mi)
    , mOwner(mcObj)
    , mName(mi->mName)
    , mNamespace(mi->mNamespace)
    , mConstants(mi->mConstantNumber)
    , mMethods(mi->mMethodNumber)
{
    mIid.mUuid = mi->mUuid;
    mIid.mCid = &mcObj->mCid;
    BuildBaseInterface();
    mMethods = Array<IMetaMethod*>(CalculateMethodNumber());
}

ECode CMetaInterface::GetComponent(
    /* [out] */ AutoPtr<IMetaComponent>& metaComp)
{
    metaComp = mOwner;
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
    ns = mNamespace.Equals(NAMESPACE_GLOBAL) ? "" : mNamespace;
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
    number = mConstants.GetLength();
    return NOERROR;
}

ECode CMetaInterface::GetAllConstants(
    /* [out] */ Array<IMetaConstant*>& consts)
{
    BuildAllConstants();

    Integer N = MIN(mConstants.GetLength(), consts.GetLength());
    for (Integer i = 0; i < N; i++) {
        consts.Set(i, mConstants[i]);
    }
    return NOERROR;
}

ECode CMetaInterface::GetConstant(
    /* [in] */ const String& name,
    /* [out] */ AutoPtr<IMetaConstant>& constt)
{
    if (name.IsEmpty() || mConstants.IsEmpty()) {
        constt = nullptr;
        return NOERROR;
    }

    BuildAllConstants();

    for (Integer i = 0; i < mConstants.GetLength(); i++) {
        String mcName;
        mConstants[i]->GetName(mcName);
        if (mcName.Equals(name)) {
            constt = mConstants[i];
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
    if (index < 0 || index >= mConstants.GetLength()) {
        constt = nullptr;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    BuildAllConstants();

    constt = mConstants[index];
    return NOERROR;
}

ECode CMetaInterface::GetMethodNumber(
    /* [out] */ Integer& number)
{
    number = mMethods.GetLength();
    return NOERROR;
}

ECode CMetaInterface::GetAllMethods(
    /* [out] */ Array<IMetaMethod*>& methods)
{
    BuildAllMethods();

    Integer N = MIN(mMethods.GetLength(), methods.GetLength());
    for (Integer i = 0; i < N; i++) {
        methods.Set(i, mMethods[i]);
    }
    return NOERROR;
}

ECode CMetaInterface::GetDeclaredMethodNumber(
    /* [out] */ Integer& number)
{
    number = mMetadata->mMethodNumber;
    return NOERROR;
}

ECode CMetaInterface::GetDeclaredMethods(
    /* [out] */ Array<IMetaMethod*>& methods)
{
    BuildAllMethods();

    Integer offset = mMethods.GetLength() - mMetadata->mMethodNumber;
    Integer N = MIN(mMetadata->mMethodNumber, methods.GetLength());
    for (Integer i = 0; i < N; i++) {
        methods.Set(i, mMethods[i + offset]);
    }
    return NOERROR;
}

ECode CMetaInterface::GetMethod(
    /* [in] */ const String& name,
    /* [in] */ const String& signature,
    /* [out] */ AutoPtr<IMetaMethod>& method)
{
    if (name.IsEmpty() || mMethods.IsEmpty()) {
        method = nullptr;
        return NOERROR;
    }

    BuildAllMethods();

    for (Integer i = 0; i < mMethods.GetLength(); i++) {
        IMetaMethod* mmObj = mMethods[i];
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
    if (index < 0 || index >= mMethods.GetLength()) {
        method = nullptr;
        return NOERROR;
    }

    BuildAllMethods();

    method = mMethods[index];
    return NOERROR;
}

Integer CMetaInterface::CalculateMethodNumber()
{
    MetaInterface* mi = mMetadata;
    Integer number = 0;
    while (mi != nullptr) {
        number += mi->mMethodNumber;
        if (mi->mBaseInterfaceIndex != -1) {
            mi = mOwner->mMetadata->mInterfaces[mi->mBaseInterfaceIndex];
        }
        else {
            mi = nullptr;
        }
    }
    return number;
}

void CMetaInterface::BuildBaseInterface()
{
    if (mMetadata->mBaseInterfaceIndex != -1) {
        AutoPtr<IMetaInterface> base =
                mOwner->BuildInterface(mMetadata->mBaseInterfaceIndex);
        mBaseInterface = static_cast<CMetaInterface*>(base.Get());
    }
}

void CMetaInterface::BuildAllConstants()
{
    if (mConstants[0] == nullptr) {
        Mutex::AutoLock lock(mConstantsLock);
        if (mConstants[0] == nullptr) {
            for (Integer i = 0; i < mMetadata->mConstantNumber; i++) {
                AutoPtr<CMetaConstant> mcObj = new CMetaConstant(
                        mOwner->mMetadata, mMetadata->mConstants[i]);
                mConstants.Set(i, mcObj);
            }
        }
    }
}

void CMetaInterface::BuildAllMethods()
{
    if (mMethods[0] == nullptr) {
        Mutex::AutoLock lock(mMethodsLock);
        if (mMethods[0] == nullptr) {
            BuildInterfaceMethod(mMetadata);
        }
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
        mMethods.Set(startIndex + i, mmObj);
    }
    return startIndex + mi->mMethodNumber;
}

} // namespace como
