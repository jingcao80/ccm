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

namespace como {

const InterfaceID IID_IDBusInterfacePack =
        {{0x6447561d,0x49aa,0x48b3,0x9faa,{0xef,0x72,0xed,0x76,0xf8,0xe2}}, &CID_COMORuntime};

COMO_INTERFACE_IMPL_LIGHT_3(InterfacePack, LightRefBase, IInterfacePack, IDBusInterfacePack, IParcelable);

InterfacePack::~InterfacePack()
{
    ReleaseComponentID(mCid.mCid);
    ReleaseComponentID(mIid.mCid);
}

ECode InterfacePack:: GetCoclassID(
    /* [out] */ CoclassID& cid)
{
    cid = mCid;
    return NOERROR;
}

ECode InterfacePack::GetInterfaceID(
    /* [out] */ InterfaceID& iid)
{
    iid = mIid;
    return NOERROR;
}

ECode InterfacePack::GetHashCode(
    /* [out] */ Integer& hash)
{
    hash = mDBusName.GetHashCode();
    return NOERROR;
}

ECode InterfacePack::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(mDBusName);
    source->ReadCoclassID(mCid);
    source->ReadInterfaceID(mIid);
    mCid.mCid = CloneComponentID(mCid.mCid);
    mIid.mCid = CloneComponentID(mIid.mCid);
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
            if (cid.mCid->mUri != nullptr) {
                char* uri = (char*)malloc(strlen(cid.mCid->mUri) + 1);
                if (uri != nullptr) {
                    strcpy(uri, cid.mCid->mUri);
                }
                comid->mUri = uri;
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
            if (iid.mCid->mUri != nullptr) {
                char* uri = (char*)malloc(strlen(iid.mCid->mUri) + 1);
                if (uri != nullptr) {
                    strcpy(uri, iid.mCid->mUri);
                }
                comid->mUri = uri;
            }
        }
        mIid.mCid = comid;
    }
}

} // namespace como
