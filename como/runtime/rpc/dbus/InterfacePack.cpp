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
    ReleaseCoclassID(mCid);
    ReleaseInterfaceID(mIid);
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

ECode InterfacePack::IsParcelable(
    /* [out] */ Boolean& parcelable)
{
    parcelable = mIsParcelable;
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
    source->ReadBoolean(mIsParcelable);
    return NOERROR;
}

ECode InterfacePack::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mDBusName);
    dest->WriteCoclassID(mCid);
    dest->WriteInterfaceID(mIid);
    dest->WriteBoolean(mIsParcelable);
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
    mCid = CloneCoclassID(cid);
}

void InterfacePack::SetInterfaceID(
    /* [in] */ const InterfaceID& iid)
{
    mIid = CloneInterfaceID(iid);
}

void InterfacePack::SetParcelable(
    /* [in] */ Boolean parcelable)
{
    mIsParcelable = parcelable;
}

} // namespace como
