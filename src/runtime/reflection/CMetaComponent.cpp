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
#include "CMetaInterface.h"

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
    , mMetaInterfaces(mMetadata->mInterfaceNumber -
            mMetadata->mSystemPreDeclaredInterfaceNumber)
    , mMetaInterfaceMap(mMetadata->mInterfaceNumber -
            mMetadata->mSystemPreDeclaredInterfaceNumber)
{
    mCid.mUuid = metadata->mUuid;
    mCid.mUrl = mUrl.string();
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

    for (Integer i = 0; i < mMetaCoclasses.GetLength(); i++) {
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

ECode CMetaComponent::GetInterfaceNumber(
    /* [out] */ Integer* number)
{
    VALIDATE_NOT_NULL(number);

    *number = mMetadata->mInterfaceNumber -
            mMetadata->mSystemPreDeclaredInterfaceNumber;
    return NOERROR;
}

ECode CMetaComponent::GetAllInterfaces(
    /* [out] */ Array<IMetaInterface*>& intfs)
{
    if (mMetaInterfaces.IsEmpty()) {
        return NOERROR;
    }

    BuildAllInterfaces();

    for (Integer i = 0; i < mMetaInterfaces.GetLength(); i++) {
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

    if (mMetaCoclasses[0] == nullptr) {
        for (Integer i = 0; i < mMetadata->mCoclassNumber; i++) {
            MetaCoclass* mc = mMetadata->mCoclasses[i];
            IMetaCoclass* mcObj = new CMetaCoclass(
                    this, mMetadata, mc);
            mMetaCoclasses.Set(i, mcObj);
            mMetaCoclassMap.Put(String::Format("%s%s",
                    mc->mNamespace, mc->mName), mcObj);
        }
    }
}

void CMetaComponent::BuildAllInterfaces()
{
    if (mMetadata->mInterfaceNumber == 0) {
        return;
    }

    if (mMetaInterfaces[0] == nullptr) {
        Integer idx = 0;
        for (Integer i = 0; i < mMetadata->mInterfaceNumber; i++) {
            MetaInterface* mi = mMetadata->mInterfaces[i];
            if (mi->mSystemPreDeclared) continue;
            IMetaInterface* miObj = new CMetaInterface(
                    this, mMetadata, mi);
            mMetaInterfaces.Set(idx, miObj);
            mMetaInterfaceMap.Put(String::Format("%s%s",
                    mi->mNamespace, mi->mName), miObj);
            idx++;
        }
    }
}

}
