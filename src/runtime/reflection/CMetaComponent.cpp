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
    , mMetaConstants(mMetadata->mConstantNumber)
    , mMetaConstantNameMap(mMetadata->mConstantNumber)
    , mMetaConstantsAllBuilt(false)
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
    number = mMetaConstants.GetLength();
    return NOERROR;
}

ECode CMetaComponent::GetAllConstants(
    /* [out] */ Array<IMetaConstant*>& consts)
{
    if (mMetaConstants.IsEmpty()) {
        return NOERROR;
    }

    BuildAllConstants();

    Integer N = MIN(mMetaConstants.GetLength(), consts.GetLength());
    for (Integer i = 0; i < N; i++) {
        consts.Set(i, mMetaConstants[i]);
    }

    return NOERROR;
}

ECode CMetaComponent::GetConstant(
    /* [in] */ const String& name,
    /* [out] */ AutoPtr<IMetaConstant>& constt)
{
    if (name.IsEmpty() || mMetaConstants.IsEmpty()) {
        constt = nullptr;
        return NOERROR;
    }

    BuildAllConstants();

    constt = mMetaConstantNameMap.Get(name);
    return NOERROR;
}

ECode CMetaComponent::GetCoclassNumber(
    /* [out] */ Integer& number)
{
    number = mMetaCoclasses.GetLength();
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
    /* [out] */ AutoPtr<IMetaCoclass>& klass)
{
    if (fullName.IsEmpty() || mMetaCoclasses.IsEmpty()) {
        klass = nullptr;
        return NOERROR;
    }

    BuildAllCoclasses();

    klass = mMetaCoclassNameMap.Get(fullName);
    return NOERROR;
}

ECode CMetaComponent::GetCoclass(
    /* [in] */ const CoclassID& cid,
    /* [out] */ AutoPtr<IMetaCoclass>& klass)
{
    BuildAllCoclasses();

    klass = mMetaCoclassIdMap.Get(cid.mUuid);
    return NOERROR;
}

ECode CMetaComponent::GetEnumerationNumber(
    /* [out] */ Integer& number)
{
    number = mMetaEnumerations.GetLength();
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
    /* [out] */ AutoPtr<IMetaEnumeration>& enumn)
{
    if (fullName.IsEmpty() || mMetaEnumerations.IsEmpty()) {
        enumn = nullptr;
        return NOERROR;
    }

    BuildAllEnumerations();

    enumn = mMetaEnumerationMap.Get(fullName);
    return NOERROR;
}

ECode CMetaComponent::GetInterfaceNumber(
    /* [out] */ Integer& number)
{
    number = mMetaInterfaces.GetLength();
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
    /* [out] */ AutoPtr<IMetaInterface>& metaIntf)
{
    if (fullName.IsEmpty() || mMetaInterfaces.IsEmpty()) {
        metaIntf = nullptr;
        return NOERROR;
    }

    BuildAllInterfaces();

    metaIntf = mMetaInterfaceNameMap.Get(fullName);
    return NOERROR;
}

ECode CMetaComponent::GetInterface(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ AutoPtr<IMetaInterface>& metaIntf)
{
    BuildAllInterfaces();
    metaIntf = mMetaInterfaceIdMap.Get(iid.mUuid);
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
    if (mMetadata->mConstantNumber == 0 || mMetaConstantsAllBuilt) {
        return;
    }

    for (Integer i = 0; i < mMetadata->mConstantNumber; i++) {
        MetaConstant* mc = mMetadata->mConstants[i];
        if (!mMetaConstantNameMap.ContainsKey(mc->mName)) {
            AutoPtr<CMetaConstant> mcObj = new CMetaConstant(
                    mMetadata, mc);
            mMetaConstants.Set(i, mcObj);
            mMetaConstantNameMap.Put(mc->mName, mcObj);
        }
    }
    mMetaConstantsAllBuilt = true;
}

void CMetaComponent::BuildAllCoclasses()
{
    if (mMetadata->mCoclassNumber == 0 || mMetaCoclassesAllBuilt) {
        return;
    }

    for (Integer i = 0; i < mMetadata->mCoclassNumber; i++) {
        MetaCoclass* mc = mMetadata->mCoclasses[i];
        String fullName = String::Format("%s%s",
                mc->mNamespace, mc->mName);
        if (!mMetaCoclassNameMap.ContainsKey(fullName)) {
            AutoPtr<CMetaCoclass> mcObj = new CMetaCoclass(
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
        AutoPtr<CMetaCoclass> mcObj = new CMetaCoclass(
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
    if (mMetadata->mEnumerationNumber == 0 || mMetaEnumerationsAllBuilt) {
        return;
    }

    Integer index = 0;
    for (Integer i = 0; i < mMetadata->mEnumerationNumber; i++) {
        MetaEnumeration* me = mMetadata->mEnumerations[i];
        if (me->mProperties & TYPE_EXTERNAL) {
            continue;
        }
        String fullName = String::Format("%s%s",
                me->mNamespace, me->mName);
        if (!mMetaEnumerationMap.ContainsKey(fullName)) {
            AutoPtr<CMetaEnumeration> meObj = new CMetaEnumeration(
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
    if (me->mProperties & TYPE_EXTERNAL) {
        return ret;
    }
    String fullName = String::Format("%s%s",
            me->mNamespace, me->mName);
    if (!mMetaEnumerationMap.ContainsKey(fullName)) {
        AutoPtr<CMetaEnumeration> meObj = new CMetaEnumeration(
                this, mMetadata, me);
        Integer realIndex = index;
        for (Integer i = 0; i <= index; i++) {
            if (mMetadata->mEnumerations[i]->mProperties & TYPE_EXTERNAL) {
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
    if (mMetadata->mInterfaceNumber == 0 || mMetaInterfacesAllBuilt) {
        return;
    }

    Integer index = 0;
    for (Integer i = 0; i < mMetadata->mInterfaceNumber; i++) {
        MetaInterface* mi = mMetadata->mInterfaces[i];
        if (mi->mProperties & TYPE_EXTERNAL) {
            continue;
        }
        String fullName = String::Format("%s%s",
                mi->mNamespace, mi->mName);
        if (!mMetaInterfaceNameMap.ContainsKey(fullName)) {
            AutoPtr<CMetaInterface> miObj = new CMetaInterface(
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
        AutoPtr<CMetaInterface> miObj = new CMetaInterface(
                this, mMetadata, mi);
        Integer realIndex = index;
        for (Integer i = 0; i <= index; i++) {
            if (mMetadata->mInterfaces[i]->mProperties & TYPE_EXTERNAL) {
                realIndex--;
            }
        }
        if (!(mi->mProperties & TYPE_EXTERNAL)) {
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
    mMetaConstants.Clear();
    mMetaConstantNameMap.Clear();
    mMetaConstantsAllBuilt = false;
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

} // namespace como
