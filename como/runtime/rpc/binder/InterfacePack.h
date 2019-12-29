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
#include <binder/Binder.h>

namespace como {

extern const InterfaceID IID_IBinderInterfacePack;

INTERFACE_ID(91180119-7044-4e55-a67f-df585992b14e)
interface IBinderInterfacePack : public IInterface
{
    using IInterface::Probe;

    inline static IBinderInterfacePack* Probe(
        /* [in] */ IInterface* object)
    {
        if (object == nullptr) return nullptr;
        return (IBinderInterfacePack*)object->Probe(IID_IBinderInterfacePack);
    }
};

class InterfacePack
    : public LightRefBase
    , public IInterfacePack
    , public IBinderInterfacePack
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

    android::sp<android::IBinder> GetAndroidBinder();

    void SetAndroidBinder(
        /* [in] */ android::sp<android::IBinder>& binder);

    void SetCoclassID(
        /* [in] */ const CoclassID& cid);

    void SetInterfaceID(
        /* [in] */ const InterfaceID& iid);

    void SetParcelable(
        /* [in] */ Boolean parcelable);

    inline static InterfacePack* From(
        /* [in] */ IInterfacePack* ipack);

private:
    android::sp<android::IBinder> mBinder;
    CoclassID mCid;
    InterfaceID mIid;
    Boolean mIsParcelable { false };
};

InterfacePack* InterfacePack::From(
    /* [in] */ IInterfacePack* ipack)
{
    return (InterfacePack*)ipack;
}

}

#endif // __COMO_INTERFACEPACK_H__
