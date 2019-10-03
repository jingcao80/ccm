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

#include "InterfacePack.h"

namespace ccm {

static void ReleaseComponentID(
    /* [in] */ const ComponentID* cid)
{
    if (cid != nullptr) {
        if (cid->mUrl != nullptr) {
            free(const_cast<char*>(cid->mUrl));
        }
        free(const_cast<ComponentID*>(cid));
    }
}

const InterfaceID IID_IDBusInterfacePack =
        {{0x6447561d,0x49aa,0x48b3,0x9faa,{0xe,0xf,0x7,0x2,0xe,0xd,0x7,0x6,0xf,0x8,0xe,0x2}}, &CID_COMORuntime};

COMO_INTERFACE_IMPL_LIGHT_2(InterfacePack, LightRefBase, IInterfacePack, IDBusInterfacePack);

InterfacePack::~InterfacePack()
{
    ReleaseComponentID(mCid.mCid);
    ReleaseComponentID(mIid.mCid);
}

ECode InterfacePack:: GetCoclassID(
    /* [out] */ CoclassID* cid)
{
    VALIDATE_NOT_NULL(cid);

    *cid = mCid;
    return NOERROR;
}

ECode InterfacePack::GetInterfaceID(
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid);

    *iid = mIid;
    return NOERROR;
}

ECode InterfacePack::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    *hash = mDBusName.GetHashCode();
    return NOERROR;
}

ECode InterfacePack::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mDBusName);
    source->ReadCoclassID(&mCid);
    source->ReadInterfaceID(&mIid);
    return NOERROR;
}

ECode InterfacePack::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mDBusName);
    dest->WriteCoclassID(mCid);
    dest->WriteInterfaceID(mIid);
    return NOERROR;
}

String InterfacePack::GetDBusName()
{
    return mDBusName;
}

void InterfacePack::SetDBusName(
    /* [in] */ const String& name)
{
    mDBusName = name;
}

void InterfacePack::SetCoclassID(
    /* [in] */ const CoclassID& cid)
{
    mCid = cid;
    if (cid.mCid != nullptr) {
        ComponentID* comid = (ComponentID*)malloc(sizeof(ComponentID));
        if (comid != nullptr) {
            *comid = *cid.mCid;
            if (cid.mCid->mUrl != nullptr) {
                char* url = (char*)malloc(strlen(cid.mCid->mUrl) + 1);
                if (url != nullptr) {
                    strcpy(url, cid.mCid->mUrl);
                }
                comid->mUrl = url;
            }
        }
        mCid.mCid = comid;
    }
}

void InterfacePack::SetInterfaceID(
    /* [in] */ const InterfaceID& iid)
{
    mIid = iid;
    if (iid.mCid != nullptr) {
        ComponentID* comid = (ComponentID*)malloc(sizeof(ComponentID));
        if (comid != nullptr) {
            *comid = *iid.mCid;
            if (iid.mCid->mUrl != nullptr) {
                char* url = (char*)malloc(strlen(iid.mCid->mUrl) + 1);
                if (url != nullptr) {
                    strcpy(url, iid.mCid->mUrl);
                }
                comid->mUrl = url;
            }
        }
        mIid.mCid = comid;
    }
}

}
