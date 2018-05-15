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

#ifndef CCM_CDBUSCHANNEL_H__
#define CCM_CDBUSCHANNEL_H__

#include "CProxy.h"
#include "CStub.h"
#include "threadpoolexecutor.h"
#include "util/ccmobject.h"
#include "util/condition.h"
#include "util/mutex.h"
#include <dbus/dbus.h>

namespace ccm {

extern const CoclassID CID_CDBusChannel;

COCLASS_ID(8efc6167-e82e-4c7d-89aa-668f397b23cc)
class CDBusChannel
    : public Object
    , public IRPCChannel
{
private:
    class ServiceRunnable
        : public ThreadPoolExecutor::Runnable
    {
    public:
        ServiceRunnable(
            /* [in] */ CDBusChannel* owner,
            /* [in] */ IStub* target);

        ECode Run();

    private:
        static DBusHandlerResult HandleMessage(
            /* [in] */ DBusConnection* conn,
            /* [in] */ DBusMessage* msg,
            /* [in] */ void* arg);

    private:
        CDBusChannel* mOwner;
        IStub* mTarget;
        Boolean mRequestToQuit;
    };

public:
    CDBusChannel(
        /* [in] */ RPCType type,
        /* [in] */ RPCPeer peer);

    CCM_INTERFACE_DECL();

    CCM_OBJECT_DECL();

    ECode GetRPCType(
        /* [out] */ RPCType* type) override;

    ECode IsPeerAlive(
        /* [out] */ Boolean* alive) override;

    ECode LinkToDeath(
        /* [in] */ IDeathRecipient* recipient,
        /* [in] */ HANDLE cookie = 0,
        /* [in] */ Integer flags = 0) override;

    ECode UnlinkToDeath(
        /* [in] */ IDeathRecipient* recipient,
        /* [in] */ HANDLE cookie = 0,
        /* [in] */ Integer flags = 0,
        /* [out] */ IDeathRecipient** outRecipient = nullptr) override;

    ECode Invoke(
        /* [in] */ IProxy* proxy,
        /* [in] */ IMetaMethod* method,
        /* [in] */ IParcel* argParcel,
        /* [out] */ IParcel** resParcel) override;

    ECode StartListening(
        /* [in] */ IStub* stub) override;

    ECode Match(
        /* [in] */ IInterfacePack* ipack,
        /* [out] */ Boolean* matched) override;

    static CDBusChannel* GetProxyChannel(
        /* [in] */ IProxy* proxy);

    static CDBusChannel* GetStubChannel(
        /* [in] */ IStub* stub);

private:
    ECode UnmarshalArguments(
        /* [in] */ void* data,
        /* [in] */ Long size,
        /* [in] */ IMetaMethod* method,
        /* [in] */ IParcel* argParcel);

private:
    friend class CDBusChannelFactory;

    static constexpr Boolean DEBUG = false;
    static constexpr const char* STUB_OBJECT_PATH = "/ccm/rpc/CStub";
    static constexpr const char* STUB_INTERFACE_PATH = "ccm.rpc.IStub";

    RPCType mType;
    RPCPeer mPeer;
    String mName;
    Boolean mStarted;
    Mutex mLock;
    Condition mCond;
};

inline CDBusChannel* CDBusChannel::GetProxyChannel(
    /* [in] */ IProxy* proxy)
{
    return (CDBusChannel*)((CProxy*)proxy)->GetChannel().Get();
}

inline CDBusChannel* CDBusChannel::GetStubChannel(
    /* [in] */ IStub* stub)
{
    return (CDBusChannel*)((CStub*)stub)->GetChannel().Get();
}

}

#endif // CCM_CDBUSCHANNEL_H__
