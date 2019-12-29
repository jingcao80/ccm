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

#ifndef __COMO_CBINDERCHANNEL_H__
#define __COMO_CBINDERCHANNEL_H__

#include "rpc/CProxy.h"
#include "rpc/CStub.h"
#include "util/arraylist.h"
#include "util/comoobj.h"
#include "util/mutex.h"
#include <binder/Binder.h>

namespace como {

class DeathRecipient;

class DeathRecipientList
    : public LightRefBase
{
public:
    void Add(
        /* [in] */ DeathRecipient* recipient);

    void Remove(
        /* [in] */ DeathRecipient* recipient);

    android::sp<DeathRecipient> Find(
        /* [in] */ IDeathRecipient* recipient);

private:
    ArrayList<android::sp<DeathRecipient>> mDeathRecipients;
    Mutex mLock;
};

class DeathRecipient
    : public android::IBinder::DeathRecipient
{
public:
    DeathRecipient(
        /* [in] */ IProxy* proxy,
        /* [in] */ IDeathRecipient* recipient,
        /* [in] */ DeathRecipientList* list);

    void binderDied(
        /* [in] */ const android::wp<android::IBinder>& who) override;

    void ClearReference();

    Boolean Matches(
        /* [in] */ IDeathRecipient* recipient);

private:
    AutoPtr<IWeakReference> mProxy;
    AutoPtr<IWeakReference> mObject;
    DeathRecipientList* mDeathRecipients;
};

extern const CoclassID CID_CBinderChannel;

COCLASS_ID(cc32c56d-2cc7-4627-9594-54b19bec8614)
class CBinderChannel
    : public Object
    , public IRPCChannel
{
private:
    class ServiceRunnable
        : public android::BBinder
    {
    public:
        ServiceRunnable(
            /* [in] */ CBinderChannel* owner,
            /* [in] */ IStub* target);

    private:
        android::status_t onTransact(
            /* [in] */ uint32_t code,
            /* [in] */ const android::Parcel& data,
            /* [in] */ android::Parcel* reply,
            /* [in] */ uint32_t flags = 0) override;

    private:
        CBinderChannel* mOwner;
        IStub* mTarget;
    };

public:
    CBinderChannel(
        /* [in] */ RPCType type,
        /* [in] */ RPCPeer peer);

    COMO_INTERFACE_DECL();

    COMO_OBJECT_DECL();

    ECode Apply(
        /* [in] */ IInterfacePack* ipack) override;

    ECode GetRPCType(
        /* [out] */ RPCType& type) override;

    ECode IsPeerAlive(
        /* [out] */ Boolean& alive) override;

    ECode LinkToDeath(
        /* [in] */ IProxy* proxy,
        /* [in] */ IDeathRecipient* recipient,
        /* [in] */ HANDLE cookie = 0,
        /* [in] */ Integer flags = 0) override;

    ECode UnlinkToDeath(
        /* [in] */ IProxy* proxy,
        /* [in] */ IDeathRecipient* recipient,
        /* [in] */ HANDLE cookie = 0,
        /* [in] */ Integer flags = 0,
        /* [out] */ AutoPtr<IDeathRecipient>* outRecipient = nullptr) override;

    ECode GetComponentMetadata(
        /* [in] */ const CoclassID& cid,
        /* [out, callee] */ Array<Byte>& metadata) override;

    ECode Invoke(
        /* [in] */ IMetaMethod* method,
        /* [in] */ IParcel* argParcel,
        /* [out] */ AutoPtr<IParcel>& resParcel) override;

    ECode StartListening(
        /* [in] */ IStub* stub) override;

    ECode Match(
        /* [in] */ IInterfacePack* ipack,
        /* [out] */ Boolean& matched) override;

    static CBinderChannel* GetProxyChannel(
        /* [in] */ IProxy* proxy);

    static CBinderChannel* GetStubChannel(
        /* [in] */ IStub* stub);

private:
    friend class CBinderChannelFactory;

    static constexpr Boolean DEBUG { false };
    static constexpr Integer COMMAND_GET_COMPONENT_METADATA { android::IBinder::FIRST_CALL_TRANSACTION };
    static constexpr Integer COMMAND_INVOKE { android::IBinder::FIRST_CALL_TRANSACTION + 1 };
    static constexpr Integer COMMAND_RELEASE { android::IBinder::FIRST_CALL_TRANSACTION + 2 };

    RPCType mType;
    RPCPeer mPeer;
    android::sp<android::IBinder> mBinder;
    AutoPtr<DeathRecipientList> mOrgue;

    static Boolean sThreadPoolStarted;
    static Mutex sThreadPoolLock;
};

inline CBinderChannel* CBinderChannel::GetProxyChannel(
    /* [in] */ IProxy* proxy)
{
    return (CBinderChannel*)((CProxy*)proxy)->GetChannel().Get();
}

inline CBinderChannel* CBinderChannel::GetStubChannel(
    /* [in] */ IStub* stub)
{
    return (CBinderChannel*)((CStub*)stub)->GetChannel().Get();
}

}

#endif // __COMO_CBINDERCHANNEL_H__
