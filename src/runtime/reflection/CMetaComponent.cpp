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

COMO_INTERFACE_IMPL_LIGHT_1(CMetaComponent, LightRefBase, IMetaComponent);

CMetaComponent::CMetaComponent(
    /* [in] */ IClassLoader* loader,
    /* [in] */ CcmComponent* component,
    /* [in] */ MetaComponent* metadata)
    : mLoader(loader)
    , mComponent(component)
    , mMetadata(metadata)
    , mName(metadata->mName)
    , mUrl(metadata->mUrl)
    , mMetaCoclasses(mMetadata->mCoclassNumber)
    , mMetaCoclassNameMap(mMetadata->mCoclassNumber)
    , mMetaCoclassIdMap(mMetadata->mCoclassNumber)
    , mMetaCoclassesAllBuilt(false)
    , mMetaEnumerations(mMetadata->mEnumerationNumber -
            mMetadata->mExternalEnumerationNumber)
    , mMetaEnumerationMap(mMetadata->mEnumerationNumber -
            mMetadata->mExternalEnumerationNumber)
    , mMetaEnumerationsAllBuilt(false)
    , mMetaInterfaces(mMetadata->mInterfaceNumber -
            mMetadata->mExternalInterfaceNumber)
    , mMetaInterfaceNameMap(mMetadata->mInterfaceNumber -
            mMetadata->mExternalInterfaceNumber)
    , mMetaInterfaceIdMap(mMetadata->mInterfaceNumber -
            mMetadata->mExternalInterfaceNumber)
    , mMetaInterfacesAllBuilt(false)
{
    mCid.mUuid = metadata->mUuid;
    mCid.mUrl = mUrl.string();

    LoadAllClassObjectGetters();
    BuildIInterface();
}

CMetaComponent::~CMetaComponent()
{
    ReleaseResources();
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

    *cid = mCid;
    return NOERROR;
}

ECode CMetaComponent::GetCoclassNumber(
    /* [out] */ Integer* number)
{
    VALIDATE_NOT_NULL(number);

    *number = mMetaCoclasses.GetLength();
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

    *metaKls = mMetaCoclassNameMap.Get(fullName);
    REFCOUNT_ADD(*metaKls);
    return NOERROR;
}

ECode CMetaComponent::GetCoclass(
    /* [in] */ const CoclassID& cid,
    /* [out] */ IMetaCoclass** metaKls)
{
    VALIDATE_NOT_NULL(metaKls);

    BuildAllCoclasses();

    *metaKls = mMetaCoclassIdMap.Get(cid.mUuid);
    REFCOUNT_ADD(*metaKls);
    return NOERROR;
}

ECode CMetaComponent::GetEnumerationNumber(
    /* [out] */ Integer* number)
{
    VALIDATE_NOT_NULL(number);

    *number = mMetaEnumerations.GetLength();
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

    *number = mMetaInterfaces.GetLength();
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

    *metaIntf = mMetaInterfaceNameMap.Get(fullName);
    REFCOUNT_ADD(*metaIntf);
    return NOERROR;
}

ECode CMetaComponent::GetInterface(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IMetaInterface** metaIntf)
{
    VALIDATE_NOT_NULL(metaIntf);

    BuildAllInterfaces();
    *metaIntf = mMetaInterfaceIdMap.Get(iid.mUuid);
    REFCOUNT_ADD(*metaIntf);
    return NOERROR;
}

ECode CMetaComponent::CanUnload(
    /* [out] */ Boolean* unload)
{
    VALIDATE_NOT_NULL(unload);

    *unload = mComponent->mSoCanUnload();
    return NOERROR;
}

ECode CMetaComponent::Unload()
{
    ECode ec = mLoader->UnloadComponent(mCid);
    if (SUCCEEDED(ec)) {
        ReleaseResources();
    }
    return ec;
}

ECode CMetaComponent::GetClassObject(
    /* [in] */ const CoclassID& cid,
    /* [out] */ IClassObject** object)
{
    VALIDATE_NOT_NULL(object);

    ClassObjectGetter* getter = mClassObjects.Get(cid.mUuid);
    if (getter != nullptr) {
        return getter->mGetter(object);
    }
    return mComponent->mSoGetClassObject(cid, object);
}

void CMetaComponent::BuildAllCoclasses()
{
    if (mMetadata->mCoclassNumber == 0 ||
            mMetaCoclassesAllBuilt) {
        return;
    }

    for (Integer i = 0; i < mMetadata->mCoclassNumber; i++) {
        MetaCoclass* mc = mMetadata->mCoclasses[i];
        String fullName = String::Format("%s%s",
                mc->mNamespace, mc->mName);
        if (!mMetaCoclassNameMap.ContainsKey(fullName)) {
            CMetaCoclass* mcObj = new CMetaCoclass(
                    this, mMetadata, mc);
            mMetaCoclasses.Set(i, mcObj);
            mMetaCoclassNameMap.Put(fullName, mcObj);
            mMetaCoclassIdMap.Put(mcObj->mCid.mUuid, mcObj);
        }
    }
    mMetaCoclassesAllBuilt = true;
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
    if (!mMetaCoclassNameMap.ContainsKey(fullName)) {
        CMetaCoclass* mcObj = new CMetaCoclass(
                this, mMetadata, mc);
        mMetaCoclasses.Set(index, mcObj);
        mMetaCoclassNameMap.Put(fullName, mcObj);
        mMetaCoclassIdMap.Put(mcObj->mCid.mUuid, mcObj);
        ret = mcObj;
    }
    else {
        ret = mMetaCoclassNameMap.Get(fullName);
    }
    return ret;
}

void CMetaComponent::BuildAllEnumerations()
{
    if (mMetadata->mEnumerationNumber == 0 ||
            mMetaEnumerationsAllBuilt) {
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
    mMetaEnumerationsAllBuilt = true;
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
    else {
        ret = mMetaEnumerationMap.Get(fullName);
    }
    return ret;
}

void CMetaComponent::BuildAllInterfaces()
{
    if (mMetadata->mInterfaceNumber == 0 ||
            mMetaInterfacesAllBuilt) {
        return;
    }

    Integer index = 0;
    for (Integer i = 0; i < mMetadata->mInterfaceNumber; i++) {
        MetaInterface* mi = mMetadata->mInterfaces[i];
        if (mi->mExternal) continue;
        String fullName = String::Format("%s%s",
                mi->mNamespace, mi->mName);
        if (!mMetaInterfaceNameMap.ContainsKey(fullName)) {
            CMetaInterface* miObj = new CMetaInterface(
                    this, mMetadata, mi);
            mMetaInterfaces.Set(index, miObj);
            mMetaInterfaceNameMap.Put(fullName, miObj);
            mMetaInterfaceIdMap.Put(miObj->mIid.mUuid, miObj);
        }
        index++;
    }
    mMetaInterfacesAllBuilt = true;
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
    if (!mMetaInterfaceNameMap.ContainsKey(fullName)) {
        CMetaInterface* miObj = new CMetaInterface(
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
        mMetaInterfaceNameMap.Put(fullName, miObj);
        mMetaInterfaceIdMap.Put(miObj->mIid.mUuid, miObj);
        ret = miObj;
    }
    else {
        ret = mMetaInterfaceNameMap.Get(fullName);
    }
    return ret;
}

void CMetaComponent::LoadAllClassObjectGetters()
{
    Integer N;
    ClassObjectGetter* getters = mComponent->mSoGetAllClassObjects(&N);
    for (Integer i = 0; i < N; i++) {
        const CoclassID& cid = getters[i].mCid;
        mClassObjects.Put(cid.mUuid, &getters[i]);
    }
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

void CMetaComponent::ReleaseResources()
{
    mLoader = nullptr;
    if (mComponent != nullptr) {
        free(mComponent);
        mComponent = nullptr;
    }
    if (mMetadata != nullptr) {
        free(mMetadata);
        mMetadata = nullptr;
    }
    mCid.mUuid = UUID_ZERO;
    mCid.mUrl = nullptr;
    mName = nullptr;
    mUrl = nullptr;
    mMetaCoclasses.Clear();
    mMetaCoclassNameMap.Clear();
    mMetaCoclassIdMap.Clear();
    mMetaCoclassesAllBuilt = false;
    mMetaEnumerations.Clear();
    mMetaEnumerationMap.Clear();
    mMetaEnumerationsAllBuilt = false;
    mMetaInterfaces.Clear();
    mMetaInterfaceNameMap.Clear();
    mMetaInterfaceIdMap.Clear();
    mMetaInterfacesAllBuilt = false;
}

}
