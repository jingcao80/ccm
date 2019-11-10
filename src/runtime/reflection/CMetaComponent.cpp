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
#include "CMetaCoclass.h"
#include "CMetaConstant.h"
#include "CMetaEnumeration.h"
#include "CMetaInterface.h"
#include "CMetaMethod.h"
#include "CMetaParameter.h"
#include "CMetaType.h"
#include <cstdlib>

namespace como {

COMO_INTERFACE_IMPL_LIGHT_1(CMetaComponent, LightRefBase, IMetaComponent);

CMetaComponent::CMetaComponent(
    /* [in] */ IClassLoader* loader,
    /* [in] */ ComoComponent* component,
    /* [in] */ MetaComponent* metadata)
    : mLoader(loader)
    , mComponent(component)
    , mMetadata(metadata)
    , mName(metadata->mName)
    , mUri(metadata->mUri)
    , mConstants(mMetadata->mConstantNumber)
    , mConstantNameMap(mMetadata->mConstantNumber)
    , mConstantsAlreadyBuilt(false)
    , mCoclasses(mMetadata->mCoclassNumber)
    , mCoclassNameMap(mMetadata->mCoclassNumber)
    , mCoclassIdMap(mMetadata->mCoclassNumber)
    , mCoclassesAlreadyBuilt(false)
    , mEnumerations(mMetadata->mEnumerationNumber -
            mMetadata->mExternalEnumerationNumber)
    , mEnumerationNameMap(mMetadata->mEnumerationNumber -
            mMetadata->mExternalEnumerationNumber)
    , mEnumerationsAlreadyBuilt(false)
    , mInterfaces(mMetadata->mInterfaceNumber -
            mMetadata->mExternalInterfaceNumber)
    , mInterfaceNameMap(mMetadata->mInterfaceNumber -
            mMetadata->mExternalInterfaceNumber)
    , mInterfaceIdMap(mMetadata->mInterfaceNumber -
            mMetadata->mExternalInterfaceNumber)
    , mInterfacesAlreadyBuilt(false)
{
    mCid.mUuid = metadata->mUuid;
    mCid.mUri = mUri.string();

    LoadAllClassObjectGetters();
    BuildIInterface();
}

CMetaComponent::~CMetaComponent()
{
    ReleaseResources();
}

ECode CMetaComponent::GetName(
    /* [ou] */ String& name)
{
    name = mName;
    return NOERROR;
}

ECode CMetaComponent::GetComponentID(
    /* [out] */ ComponentID& cid)
{
    cid = mCid;
    return NOERROR;
}

ECode CMetaComponent::GetConstantNumber(
    /* [out] */ Integer& number)
{
    number = mConstants.GetLength();
    return NOERROR;
}

ECode CMetaComponent::GetAllConstants(
    /* [out] */ Array<IMetaConstant*>& consts)
{
    if (mConstants.IsEmpty()) {
        return NOERROR;
    }

    BuildAllConstants();

    Integer N = MIN(mConstants.GetLength(), consts.GetLength());
    for (Integer i = 0; i < N; i++) {
        consts.Set(i, mConstants[i]);
    }

    return NOERROR;
}

ECode CMetaComponent::GetConstant(
    /* [in] */ const String& name,
    /* [out] */ AutoPtr<IMetaConstant>& constt)
{
    if (name.IsEmpty() || mConstants.IsEmpty()) {
        constt = nullptr;
        return NOERROR;
    }

    BuildAllConstants();

    constt = mConstantNameMap.Get(name);
    return NOERROR;
}

ECode CMetaComponent::GetCoclassNumber(
    /* [out] */ Integer& number)
{
    number = mCoclasses.GetLength();
    return NOERROR;
}

ECode CMetaComponent::GetAllCoclasses(
    /* [out] */ Array<IMetaCoclass*>& klasses)
{
    if (mCoclasses.IsEmpty()) {
        return NOERROR;
    }

    BuildAllCoclasses();

    Integer N = MIN(mCoclasses.GetLength(), klasses.GetLength());
    for (Integer i = 0; i < N; i++) {
        klasses.Set(i, mCoclasses[i]);
    }

    return NOERROR;
}

ECode CMetaComponent::GetCoclass(
    /* [in] */ const String& fullName,
    /* [out] */ AutoPtr<IMetaCoclass>& klass)
{
    if (fullName.IsEmpty() || mCoclasses.IsEmpty()) {
        klass = nullptr;
        return NOERROR;
    }

    BuildAllCoclasses();

    klass = mCoclassNameMap.Get(fullName);
    return NOERROR;
}

ECode CMetaComponent::GetCoclass(
    /* [in] */ const CoclassID& cid,
    /* [out] */ AutoPtr<IMetaCoclass>& klass)
{
    BuildAllCoclasses();

    klass = mCoclassIdMap.Get(cid.mUuid);
    return NOERROR;
}

ECode CMetaComponent::GetEnumerationNumber(
    /* [out] */ Integer& number)
{
    number = mEnumerations.GetLength();
    return NOERROR;
}

ECode CMetaComponent::GetAllEnumerations(
    /* [out] */ Array<IMetaEnumeration*>& enumns)
{
    if (mEnumerations.IsEmpty()) {
        return NOERROR;
    }

    BuildAllEnumerations();

    Integer N = MIN(mEnumerations.GetLength(), enumns.GetLength());
    for (Integer i = 0; i < N; i++) {
        enumns.Set(i, mEnumerations[i]);
    }

    return NOERROR;
}

ECode CMetaComponent::GetEnumeration(
    /* [in] */ const String& fullName,
    /* [out] */ AutoPtr<IMetaEnumeration>& enumn)
{
    if (fullName.IsEmpty() || mEnumerations.IsEmpty()) {
        enumn = nullptr;
        return NOERROR;
    }

    BuildAllEnumerations();

    enumn = mEnumerationNameMap.Get(fullName);
    return NOERROR;
}

ECode CMetaComponent::GetInterfaceNumber(
    /* [out] */ Integer& number)
{
    number = mInterfaces.GetLength();
    return NOERROR;
}

ECode CMetaComponent::GetAllInterfaces(
    /* [out] */ Array<IMetaInterface*>& intfs)
{
    if (mInterfaces.IsEmpty()) {
        return NOERROR;
    }

    BuildAllInterfaces();

    Integer N = MIN(mInterfaces.GetLength(), intfs.GetLength());
    for (Integer i = 0; i < N; i++) {
        intfs.Set(i, mInterfaces[i]);
    }

    return NOERROR;
}

ECode CMetaComponent::GetInterface(
    /* [in] */ const String& fullName,
    /* [out] */ AutoPtr<IMetaInterface>& metaIntf)
{
    if (fullName.IsEmpty() || mInterfaces.IsEmpty()) {
        metaIntf = nullptr;
        return NOERROR;
    }

    BuildAllInterfaces();

    metaIntf = mInterfaceNameMap.Get(fullName);
    return NOERROR;
}

ECode CMetaComponent::GetInterface(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ AutoPtr<IMetaInterface>& metaIntf)
{
    BuildAllInterfaces();
    metaIntf = mInterfaceIdMap.Get(iid.mUuid);
    return NOERROR;
}

ECode CMetaComponent::Resolve()
{
    BuildAllConstants();
    BuildAllEnumerations();
    BuildAllInterfaces();
    BuildAllCoclasses();
    return NOERROR;
}

ECode CMetaComponent::CanUnload(
    /* [out] */ Boolean& unload)
{
    unload = mComponent->mSoCanUnload();
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
    /* [out] */ AutoPtr<IClassObject>& object)
{
    ClassObjectGetter* getter = mClassObjects.Get(cid.mUuid);
    if (getter != nullptr) {
        return getter->mGetter(object);
    }
    return mComponent->mSoGetClassObject(cid, object);
}

void CMetaComponent::BuildAllConstants()
{
    if (mMetadata->mConstantNumber == 0) {
        return;
    }

    if (mConstantsAlreadyBuilt) {
        return;
    }

    Mutex::AutoLock lock(mConstantsLock);

    if (mConstantsAlreadyBuilt) {
        return;
    }

    for (Integer i = 0; i < mMetadata->mConstantNumber; i++) {
        MetaConstant* mc = mMetadata->mConstants[i];
        String fullName = String::Format("%s::%s",
                mc->mNamespace, mc->mName);
        AutoPtr<CMetaConstant> mcObj = new CMetaConstant(
                mMetadata, mc);
        mConstants.Set(i, mcObj);
        mConstantNameMap.Put(fullName, mcObj);
    }
    mConstantsAlreadyBuilt = true;
}

void CMetaComponent::BuildAllCoclasses()
{
    if (mMetadata->mCoclassNumber == 0) {
        return;
    }

    if (mCoclassesAlreadyBuilt) {
        return;
    }

    Mutex::AutoLock lock(mCoclassesLock);

    if (mCoclassesAlreadyBuilt) {
        return;
    }

    for (Integer i = 0; i < mMetadata->mCoclassNumber; i++) {
        MetaCoclass* mc = mMetadata->mCoclasses[i];
        String fullName = String::Format("%s::%s",
                mc->mNamespace, mc->mName);
        AutoPtr<CMetaCoclass> mcObj = new CMetaCoclass(
                this, mMetadata, mc);
        mCoclasses.Set(i, mcObj);
        mCoclassNameMap.Put(fullName, mcObj);
        mCoclassIdMap.Put(mcObj->mCid.mUuid, mcObj);
    }
    mCoclassesAlreadyBuilt = true;
}

void CMetaComponent::BuildAllEnumerations()
{
    if (mMetadata->mEnumerationNumber == 0) {
        return;
    }

    if (mEnumerationsAlreadyBuilt) {
        return;
    }

    Mutex::AutoLock lock(mEnumerationsLock);

    if (mEnumerationsAlreadyBuilt) {
        return;
    }

    Integer index = 0;
    for (Integer i = 0; i < mMetadata->mEnumerationNumber; i++) {
        MetaEnumeration* me = mMetadata->mEnumerations[i];
        if (me->mProperties & TYPE_EXTERNAL) {
            continue;
        }
        String fullName = String::Format("%s::%s",
                me->mNamespace, me->mName);
        AutoPtr<CMetaEnumeration> meObj = new CMetaEnumeration(
                this, mMetadata, me);
        mEnumerations.Set(index, meObj);
        mEnumerationNameMap.Put(fullName, meObj);
        index++;
    }
    mEnumerationsAlreadyBuilt = true;
}

void CMetaComponent::BuildAllInterfaces()
{
    if (mMetadata->mInterfaceNumber == 0) {
        return;
    }

    if (mInterfacesAlreadyBuilt) {
        return;
    }

    Mutex::AutoLock lock(mInterfacesLock);

    if (mInterfacesAlreadyBuilt) {
        return;
    }

    Integer index = 0;
    for (Integer i = 0; i < mMetadata->mInterfaceNumber; i++) {
        MetaInterface* mi = mMetadata->mInterfaces[i];
        if (mi->mProperties & TYPE_EXTERNAL) {
            continue;
        }
        String fullName = String::Format("%s::%s",
                mi->mNamespace, mi->mName);
        if (!mInterfaceNameMap.ContainsKey(fullName)) {
            AutoPtr<CMetaInterface> miObj = new CMetaInterface(
                    this, mMetadata, mi);
            mInterfaces.Set(index, miObj);
            mInterfaceNameMap.Put(fullName, miObj);
            mInterfaceIdMap.Put(miObj->mIid.mUuid, miObj);
        }
        index++;
    }
    mInterfacesAlreadyBuilt = true;
}

AutoPtr<IMetaInterface> CMetaComponent::BuildInterface(
    /* [in] */ Integer index)
{
    if (index < 0 || index >= mMetadata->mInterfaceNumber) {
        return nullptr;
    }

    Mutex::AutoLock lock(mInterfacesLock);

    MetaInterface* mi = mMetadata->mInterfaces[index];
    String fullName = String::Format("%s::%s",
            mi->mNamespace, mi->mName);
    if (!mInterfaceNameMap.ContainsKey(fullName)) {
        AutoPtr<CMetaInterface> miObj = new CMetaInterface(
                this, mMetadata, mi);
        Integer realIndex = index;
        for (Integer i = 0; i <= index; i++) {
            if (mMetadata->mInterfaces[i]->mProperties & TYPE_EXTERNAL) {
                realIndex--;
            }
        }
        if (!(mi->mProperties & TYPE_EXTERNAL)) {
            mInterfaces.Set(realIndex, miObj);
        }
        mInterfaceNameMap.Put(fullName, miObj);
        mInterfaceIdMap.Put(miObj->mIid.mUuid, miObj);
        return miObj;
    }
    else {
        return mInterfaceNameMap.Get(fullName);
    }
}

void CMetaComponent::LoadAllClassObjectGetters()
{
    Integer N;
    ClassObjectGetter* getters = mComponent->mSoGetAllClassObjects(N);
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
    miObj->mNamespace = "como::";
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
    mtObj->mKind = TypeKind::HANDLE;
    mtObj->mName = "HANDLE";
    mpObj->mType = mtObj;
    mmObj->mParameters.Set(0, mpObj);
    mtObj = new CMetaType();
    mtObj->mKind = TypeKind::Integer;
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
    mtObj->mKind = TypeKind::HANDLE;
    mtObj->mName = "HANDLE";
    mpObj->mType = mtObj;
    mmObj->mParameters.Set(0, mpObj);
    mtObj = new CMetaType();
    mtObj->mKind = TypeKind::Integer;
    mtObj->mName = "Integer";
    mmObj->mReturnType = mtObj;
    miObj->mMetaMethods.Set(1, mmObj);

    // Probe
    mmObj = new CMetaMethod();
    mmObj->mOwner = miObj;
    mmObj->mIndex = 2;
    mmObj->mName = "Probe";
    mmObj->mSignature = "(U)Lcomo/IInterface*";
    mmObj->mParameters = Array<IMetaParameter*>(1);
    mpObj = new CMetaParameter();
    mpObj->mOwner = mmObj;
    mpObj->mName = "iid";
    mpObj->mIndex = 0;
    mpObj->mIOAttr = IOAttribute::IN;
    mtObj = new CMetaType();
    mtObj->mKind = TypeKind::InterfaceID;
    mtObj->mName = "InterfaceID";
    mpObj->mType = mtObj;
    mmObj->mParameters.Set(0, mpObj);
    mtObj = new CMetaType();
    mtObj->mKind = TypeKind::Interface;
    mtObj->mName = "IInterface";
    mtObj->mMode = TypeModification::POINTER;
    mmObj->mReturnType = mtObj;
    miObj->mMetaMethods.Set(2, mmObj);

    // GetInterfaceID
    mmObj = new CMetaMethod();
    mmObj->mOwner = miObj;
    mmObj->mIndex = 3;
    mmObj->mName = "GetInterfaceID";
    mmObj->mSignature = "(Lcomo/IInterface*U&)E";
    mmObj->mParameters = Array<IMetaParameter*>(2);
    mpObj = new CMetaParameter();
    mpObj->mOwner = mmObj;
    mpObj->mName = "object";
    mpObj->mIndex = 0;
    mpObj->mIOAttr = IOAttribute::IN;
    mtObj = new CMetaType();
    mtObj->mKind = TypeKind::Interface;
    mtObj->mName = "IInterface";
    mtObj->mMode = TypeModification::POINTER;
    mpObj->mType = mtObj;
    mmObj->mParameters.Set(0, mpObj);
    mpObj = new CMetaParameter();
    mpObj->mOwner = mmObj;
    mpObj->mName = "iid";
    mpObj->mIndex = 1;
    mpObj->mIOAttr = IOAttribute::OUT;
    mtObj = new CMetaType();
    mtObj->mKind = TypeKind::InterfaceID;
    mtObj->mName = "InterfaceID";
    mtObj->mMode = TypeModification::REFERENCE;
    mpObj->mType = mtObj;
    mmObj->mParameters.Set(1, mpObj);
    mtObj = new CMetaType();
    mtObj->mKind = TypeKind::ECode;
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
    mCid.mUri = nullptr;
    mName = nullptr;
    mUri = nullptr;
    mConstants.Clear();
    mConstantNameMap.Clear();
    mConstantsAlreadyBuilt = false;
    mCoclasses.Clear();
    mCoclassNameMap.Clear();
    mCoclassIdMap.Clear();
    mCoclassesAlreadyBuilt = false;
    mEnumerations.Clear();
    mEnumerationNameMap.Clear();
    mEnumerationsAlreadyBuilt = false;
    mInterfaces.Clear();
    mInterfaceNameMap.Clear();
    mInterfaceIdMap.Clear();
    mInterfacesAlreadyBuilt = false;
}

} // namespace como
