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
#include "CMetaEnumeration.h"
#include "CMetaInterface.h"
#include "CMetaMethod.h"
#include "CMetaParameter.h"
#include "CMetaType.h"

#include <stdlib.h>

using ccm::metadata::MetaCoclass;
using ccm::metadata::MetaInterface;

namespace ccm {

CCM_INTERFACE_IMPL_LIGHT_1(CMetaComponent, IMetaComponent);

CMetaComponent::CMetaComponent(
    /* [in] */ MetaComponent* metadata)
    : mMetadata(metadata)
    , mName(metadata->mName)
    , mUrl(metadata->mUrl)
    , mMetaCoclasses(mMetadata->mCoclassNumber)
    , mMetaCoclassMap(mMetadata->mCoclassNumber)
    , mMetaEnumerations(mMetadata->mEnumerationNumber -
            mMetadata->mExternalEnumerationNumber)
    , mMetaEnumerationMap(mMetadata->mEnumerationNumber -
            mMetadata->mExternalEnumerationNumber)
    , mMetaInterfaces(mMetadata->mInterfaceNumber -
            mMetadata->mExternalInterfaceNumber)
    , mMetaInterfaceMap(mMetadata->mInterfaceNumber -
            mMetadata->mExternalInterfaceNumber)
{
    mCid.mUuid = metadata->mUuid;
    mCid.mUrl = mUrl.string();

    BuildIInterface();
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

    *name = mName;
    return NOERROR;
}

ECode CMetaComponent::GetComponentID(
    /* [out] */ ComponentID* cid)
{
    VALIDATE_NOT_NULL(cid);

    cid->mUuid = mCid.mUuid;
    cid->mUrl = mCid.mUrl;
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
    if (mMetaCoclasses.IsEmpty()) {
        return NOERROR;
    }

    BuildAllCoclasses();

    Integer N = MIN(mMetaCoclasses.GetLength(), klasses.GetLength());
    for (Integer i = 0; i < N; i++) {
        klasses.Set(i, mMetaCoclasses[i]);
    }

    return NOERROR;
}

ECode CMetaComponent::GetCoclass(
    /* [in] */ const String& fullName,
    /* [out] */ IMetaCoclass** metaKls)
{
    VALIDATE_NOT_NULL(metaKls);

    if (fullName.IsNullOrEmpty() || mMetaCoclasses.IsEmpty()) {
        *metaKls = nullptr;
        return NOERROR;
    }

    BuildAllCoclasses();

    *metaKls = mMetaCoclassMap.Get(fullName);
    REFCOUNT_ADD(*metaKls);
    return NOERROR;
}

ECode CMetaComponent::GetEnumerationNumber(
    /* [out] */ Integer* number)
{
    VALIDATE_NOT_NULL(number);

    *number = mMetadata->mEnumerationNumber -
            mMetadata->mExternalEnumerationNumber;
    return NOERROR;
}

ECode CMetaComponent::GetAllEnumerations(
    /* [out] */ Array<IMetaEnumeration*>& enumns)
{
    if (mMetaEnumerations.IsEmpty()) {
        return NOERROR;
    }

    BuildAllEnumerations();

    Integer N = MIN(mMetaEnumerations.GetLength(), enumns.GetLength());
    for (Integer i = 0; i < N; i++) {
        enumns.Set(i, mMetaEnumerations[i]);
    }

    return NOERROR;
}

ECode CMetaComponent::GetEnumeration(
    /* [in] */ const String& fullName,
    /* [out] */ IMetaEnumeration** enumn)
{
    VALIDATE_NOT_NULL(enumn);

    if (fullName.IsNullOrEmpty() || mMetaEnumerations.IsEmpty()) {
        *enumn = nullptr;
        return NOERROR;
    }

    BuildAllEnumerations();

    *enumn = mMetaEnumerationMap.Get(fullName);
    REFCOUNT_ADD(*enumn);
    return NOERROR;
}

ECode CMetaComponent::GetInterfaceNumber(
    /* [out] */ Integer* number)
{
    VALIDATE_NOT_NULL(number);

    *number = mMetadata->mInterfaceNumber -
            mMetadata->mExternalInterfaceNumber;
    return NOERROR;
}

ECode CMetaComponent::GetAllInterfaces(
    /* [out] */ Array<IMetaInterface*>& intfs)
{
    if (mMetaInterfaces.IsEmpty()) {
        return NOERROR;
    }

    BuildAllInterfaces();

    Integer N = MIN(mMetaInterfaces.GetLength(), intfs.GetLength());
    for (Integer i = 0; i < N; i++) {
        intfs.Set(i, mMetaInterfaces[i]);
    }

    return NOERROR;
}

ECode CMetaComponent::GetInterface(
    /* [in] */ const String& fullName,
    /* [out] */ IMetaInterface** metaIntf)
{
    VALIDATE_NOT_NULL(metaIntf);

    if (fullName.IsNullOrEmpty() || mMetaInterfaces.IsEmpty()) {
        *metaIntf = nullptr;
        return NOERROR;
    }

    BuildAllInterfaces();

    *metaIntf = mMetaInterfaceMap.Get(fullName);
    REFCOUNT_ADD(*metaIntf);
    return NOERROR;
}

void CMetaComponent::BuildAllCoclasses()
{
    if (mMetadata->mCoclassNumber == 0) {
        return;
    }

    for (Integer i = 0; i < mMetadata->mCoclassNumber; i++) {
        MetaCoclass* mc = mMetadata->mCoclasses[i];
        String fullName = String::Format("%s%s",
                mc->mNamespace, mc->mName);
        if (!mMetaCoclassMap.ContainsKey(fullName)) {
            IMetaCoclass* mcObj = new CMetaCoclass(
                    this, mMetadata, mc);
            mMetaCoclasses.Set(i, mcObj);
            mMetaCoclassMap.Put(fullName, mcObj);
        }
    }
}

AutoPtr<IMetaCoclass> CMetaComponent::BuildCoclass(
    /* [in] */ Integer index)
{
    AutoPtr<IMetaCoclass> ret;
    if (index < 0 || index >= mMetadata->mCoclassNumber) {
        return ret;
    }

    MetaCoclass* mc = mMetadata->mCoclasses[index];
    String fullName = String::Format("%s%s",
                mc->mNamespace, mc->mName);
    if (!mMetaCoclassMap.ContainsKey(fullName)) {
        IMetaCoclass* mcObj = new CMetaCoclass(
                this, mMetadata, mc);
        mMetaCoclasses.Set(index, mcObj);
        mMetaCoclassMap.Put(fullName, mcObj);
        ret = mcObj;
    }
    return ret;
}

void CMetaComponent::BuildAllEnumerations()
{
    if (mMetadata->mEnumerationNumber == 0) {
        return;
    }

    Integer index = 0;
    for (Integer i = 0; i < mMetadata->mEnumerationNumber; i++) {
        MetaEnumeration* me = mMetadata->mEnumerations[i];
        if (me->mExternal) continue;
        String fullName = String::Format("%s%s",
                me->mNamespace, me->mName);
        if (!mMetaEnumerationMap.ContainsKey(fullName)) {
            IMetaEnumeration* meObj = new CMetaEnumeration(
                    this, mMetadata, me);
            mMetaEnumerations.Set(index, meObj);
            mMetaEnumerationMap.Put(fullName, meObj);
        }
        index++;
    }
}

AutoPtr<IMetaEnumeration> CMetaComponent::BuildEnumeration(
    /* [in] */ Integer index)
{
    AutoPtr<IMetaEnumeration> ret;
    if (index < 0 || index >= mMetadata->mEnumerationNumber) {
        return ret;
    }

    MetaEnumeration* me = mMetadata->mEnumerations[index];
    if (me->mExternal) return ret;
    String fullName = String::Format("%s%s",
            me->mNamespace, me->mName);
    if (!mMetaEnumerationMap.ContainsKey(fullName)) {
        IMetaEnumeration* meObj = new CMetaEnumeration(
                this, mMetadata, me);
        Integer realIndex = index;
        for (Integer i = 0; i <= index; i++) {
            if (mMetadata->mEnumerations[i]->mExternal) {
                realIndex--;
            }
        }
        mMetaEnumerations.Set(realIndex, meObj);
        mMetaEnumerationMap.Put(fullName, meObj);
        ret = meObj;
    }
    return ret;
}

void CMetaComponent::BuildAllInterfaces()
{
    if (mMetadata->mInterfaceNumber == 0) {
        return;
    }

    Integer index = 0;
    for (Integer i = 0; i < mMetadata->mInterfaceNumber; i++) {
        MetaInterface* mi = mMetadata->mInterfaces[i];
        if (mi->mExternal) continue;
        String fullName = String::Format("%s%s",
                mi->mNamespace, mi->mName);
        if (!mMetaInterfaceMap.ContainsKey(fullName)) {
            IMetaInterface* miObj = new CMetaInterface(
                    this, mMetadata, mi);
            mMetaInterfaces.Set(index, miObj);
            mMetaInterfaceMap.Put(fullName, miObj);
        }
        index++;
    }
}

AutoPtr<IMetaInterface> CMetaComponent::BuildInterface(
    /* [in] */ Integer index)
{
    AutoPtr<IMetaInterface> ret;
    if (index < 0 || index >= mMetadata->mInterfaceNumber) {
        return ret;
    }

    MetaInterface* mi = mMetadata->mInterfaces[index];
    String fullName = String::Format("%s%s",
            mi->mNamespace, mi->mName);
    if (!mMetaInterfaceMap.ContainsKey(fullName)) {
        IMetaInterface* miObj = new CMetaInterface(
                this, mMetadata, mi);
        Integer realIndex = index;
        for (Integer i = 0; i <= index; i++) {
            if (mMetadata->mInterfaces[i]->mExternal) {
                realIndex--;
            }
        }
        if (!mi->mExternal) {
            mMetaInterfaces.Set(realIndex, miObj);
        }
        mMetaInterfaceMap.Put(fullName, miObj);
        ret = miObj;
    }
    else {
        ret = mMetaInterfaceMap.Get(fullName);
    }
    return ret;
}

void CMetaComponent::BuildIInterface()
{
    CMetaInterface* miObj = new CMetaInterface();
    miObj->mOwner = this;
    miObj->mIid = IID_IInterface;
    miObj->mName = "IInterface";
    miObj->mNamespace = "ccm::";
    miObj->mMetaMethods = Array<IMetaMethod*>(4);

    // AddRef
    CMetaMethod* mmObj = new CMetaMethod();
    mmObj->mOwner = miObj;
    mmObj->mIndex = 0;
    mmObj->mName = "AddRef";
    mmObj->mSignature = "(H)I";
    mmObj->mParameters = Array<IMetaParameter*>(1);
    CMetaParameter* mpObj = new CMetaParameter();
    mpObj->mOwner = mmObj;
    mpObj->mName = "id";
    mpObj->mIndex = 0;
    mpObj->mIOAttr = IOAttribute::IN;
    CMetaType* mtObj = new CMetaType();
    mtObj->mKind = CcmTypeKind::HANDLE;
    mtObj->mName = "HANDLE";
    mpObj->mType = mtObj;
    mmObj->mParameters.Set(0, mpObj);
    mtObj = new CMetaType();
    mtObj->mKind = CcmTypeKind::Integer;
    mtObj->mName = "Integer";
    mmObj->mReturnType = mtObj;
    miObj->mMetaMethods.Set(0, mmObj);

    // Release
    mmObj = new CMetaMethod();
    mmObj->mOwner = miObj;
    mmObj->mIndex = 1;
    mmObj->mName = "Release";
    mmObj->mSignature = "(H)I";
    mmObj->mParameters = Array<IMetaParameter*>(1);
    mpObj = new CMetaParameter();
    mpObj->mOwner = mmObj;
    mpObj->mName = "id";
    mpObj->mIndex = 0;
    mpObj->mIOAttr = IOAttribute::IN;
    mtObj = new CMetaType();
    mtObj->mKind = CcmTypeKind::HANDLE;
    mtObj->mName = "HANDLE";
    mpObj->mType = mtObj;
    mmObj->mParameters.Set(0, mpObj);
    mtObj = new CMetaType();
    mtObj->mKind = CcmTypeKind::Integer;
    mtObj->mName = "Integer";
    mmObj->mReturnType = mtObj;
    miObj->mMetaMethods.Set(1, mmObj);

    // Probe
    mmObj = new CMetaMethod();
    mmObj->mOwner = miObj;
    mmObj->mIndex = 2;
    mmObj->mName = "Probe";
    mmObj->mSignature = "(U)Lccm/IInterface*";
    mmObj->mParameters = Array<IMetaParameter*>(1);
    mpObj = new CMetaParameter();
    mpObj->mOwner = mmObj;
    mpObj->mName = "iid";
    mpObj->mIndex = 0;
    mpObj->mIOAttr = IOAttribute::IN;
    mtObj = new CMetaType();
    mtObj->mKind = CcmTypeKind::InterfaceID;
    mtObj->mName = "InterfaceID";
    mpObj->mType = mtObj;
    mmObj->mParameters.Set(0, mpObj);
    mtObj = new CMetaType();
    mtObj->mKind = CcmTypeKind::Interface;
    mtObj->mName = "IInterface";
    mtObj->mPointerNumber = 1;
    mmObj->mReturnType = mtObj;
    miObj->mMetaMethods.Set(2, mmObj);

    // GetInterfaceID
    mmObj = new CMetaMethod();
    mmObj->mOwner = miObj;
    mmObj->mIndex = 3;
    mmObj->mName = "GetInterfaceID";
    mmObj->mSignature = "(Lccm/IInterface*U*)E";
    mmObj->mParameters = Array<IMetaParameter*>(2);
    mpObj = new CMetaParameter();
    mpObj->mOwner = mmObj;
    mpObj->mName = "object";
    mpObj->mIndex = 0;
    mpObj->mIOAttr = IOAttribute::IN;
    mtObj = new CMetaType();
    mtObj->mKind = CcmTypeKind::Interface;
    mtObj->mName = "IInterface";
    mtObj->mPointerNumber = 1;
    mpObj->mType = mtObj;
    mmObj->mParameters.Set(0, mpObj);
    mpObj = new CMetaParameter();
    mpObj->mOwner = mmObj;
    mpObj->mName = "iid";
    mpObj->mIndex = 1;
    mpObj->mIOAttr = IOAttribute::OUT;
    mtObj = new CMetaType();
    mtObj->mKind = CcmTypeKind::InterfaceID;
    mtObj->mName = "InterfaceID";
    mtObj->mPointerNumber = 1;
    mpObj->mType = mtObj;
    mmObj->mParameters.Set(1, mpObj);
    mtObj = new CMetaType();
    mtObj->mKind = CcmTypeKind::ECode;
    mtObj->mName = "ECode";
    mmObj->mReturnType = mtObj;
    miObj->mMetaMethods.Set(3, mmObj);

    mIInterface = miObj;
}

}
