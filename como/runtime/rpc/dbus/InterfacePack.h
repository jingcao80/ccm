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

#ifndef __COMO_INTERFACEPACK_H__
#define __COMO_INTERFACEPACK_H__

#include "comotypes.h"
#include "rpc/registry.h"
#include "util/comoref.h"

namespace como {

extern const InterfaceID IID_IDBusInterfacePack;

INTERFACE_ID(6447561d-49aa-48b3-9faa-ef72ed76f8e2)
interface IDBusInterfacePack : public IInterface
{
    using IInterface::Probe;

    inline static IDBusInterfacePack* Probe(
        /* [in] */ IInterface* object)
    {
        if (object == nullptr) return nullptr;
        return (IDBusInterfacePack*)object->Probe(IID_IDBusInterfacePack);
    }
};

class InterfacePack
    : public LightRefBase
    , public IInterfacePack
    , public IDBusInterfacePack
    , public IParcelable
{
public:
    ~InterfacePack();

    COMO_INTERFACE_DECL();

    ECode GetCoclassID(
        /* [out] */ CoclassID& cid) override;

    ECode GetInterfaceID(
        /* [out] */ InterfaceID& iid) override;

    ECode IsParcelable(
        /* [out] */ Boolean& parcelable) override;

    ECode GetHashCode(
        /* [out] */ Integer& hash) override;

    ECode ReadFromParcel(
        /* [in] */ IParcel* source) override;

    ECode WriteToParcel(
        /* [in] */ IParcel* dest) override;

    String GetDBusName();

    void SetDBusName(
        /* [in] */ const String& name);

    void SetCoclassID(
        /* [in] */ const CoclassID& cid);

    void SetInterfaceID(
        /* [in] */ const InterfaceID& iid);

    void SetParcelable(
        /* [in] */ Boolean parcelable);

    inline static InterfacePack* From(
        /* [in] */ IInterfacePack* ipack);

private:
    String mDBusName;
    CoclassID mCid;
    InterfaceID mIid;
    Boolean mIsParcelable { false };
};

InterfacePack* InterfacePack::From(
    /* [in] */ IInterfacePack* ipack)
{
    return (InterfacePack*)ipack;
}

} // namespace como

#endif // __COMO_INTERFACEPACK_H__
