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

#include "ccmobjectapi.h"
#include "CMetaCoclass.h"
#include "CMetaComponent.h"

using ccm::metadata::MetaInterface;

namespace ccm {

CCM_INTERFACE_IMPL_LIGHT_1(CMetaCoclass, IMetaCoclass);

CMetaCoclass::CMetaCoclass(
    /* [in] */ CMetaComponent* mcObj,
    /* [in] */ MetaComponent* mc,
    /* [in] */ MetaCoclass* mk)
    : mMetadata(mk)
    , mMetaComponent(mcObj)
    , mName(mk->mName)
    , mNamespace(mk->mNamespace)
    , mMetaInterfaces(mk->mInterfaceNumber)
{
    mCid.mUuid = mk->mUuid;
    mCid.mCid = &mcObj->mCid;
}

CMetaCoclass::~CMetaCoclass()
{
    mMetadata = nullptr;
    mMetaComponent = nullptr;
}

ECode CMetaCoclass::GetMetaComponent(
    /* [out] */ IMetaComponent** metaComp)
{
    VALIDATE_NOT_NULL(metaComp);

    *metaComp = (IMetaComponent*)mMetaComponent;
    REFCOUNT_ADD(*metaComp);
    return NOERROR;
}

ECode CMetaCoclass::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mName;
    return NOERROR;
}

ECode CMetaCoclass::GetNamespace(
    /* [out] */ String* ns)
{
    VALIDATE_NOT_NULL(ns);

    *ns = mNamespace;
    return NOERROR;
}

ECode CMetaCoclass::GetCoclassID(
    /* [out] */ CoclassID* cid)
{
    VALIDATE_NOT_NULL(cid);

    cid->mUuid = mCid.mUuid;
    cid->mCid = mCid.mCid;
    return NOERROR;
}

ECode CMetaCoclass::GetConstructorNumber(
    /* [out] */ Integer* number)
{
    return NOERROR;
}

ECode CMetaCoclass::GetAllConstructors(
    /* [out] */ Array<IMetaConstructor*>& constrs)
{
    return NOERROR;
}

ECode CMetaCoclass::GetConstructors(
    /* [in] */ Integer paramNumber,
    /* [out] */ IMetaConstructor** constr)
{
    return NOERROR;
}

ECode CMetaCoclass::GetInterfaceNumber(
    /* [out] */ Integer* number)
{
    VALIDATE_NOT_NULL(number);

    *number = mMetadata->mInterfaceNumber;
    return NOERROR;
}

ECode CMetaCoclass::GetAllInterfaces(
    /* [out] */ Array<IMetaInterface*>& intfs)
{
    if (mMetaInterfaces.IsEmpty()) {
        return NOERROR;
    }

    if (mMetaInterfaces[0] == nullptr) {
        mMetaComponent->BuildAllInterfaces();
        for (Integer i = 0; i < mMetadata->mInterfaceNumber; i++) {
            MetaInterface* mi = mMetaComponent->mMetadata->mInterfaces[
                    mMetadata->mInterfaceIndexes[i]];
            IMetaInterface* miObj = mMetaComponent->mMetaInterfaceMap.Get(String::Format("%s%s",
                    mi->mNamespace, mi->mName));
            intfs.Set(i, miObj);
        }
    }

    return NOERROR;
}

ECode CMetaCoclass::CreateObject(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    return CoCreateObjectInstance(mCid, IID_IInterface, object);
}

ECode CMetaCoclass::CreateObject(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    return CoCreateObjectInstance(mCid, iid, object);
}

}