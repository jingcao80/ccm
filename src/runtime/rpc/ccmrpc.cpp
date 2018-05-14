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

#include "ccmrpc.h"
#include "CProxy.h"
#include "CStub.h"
#include "dbus/CDBusChannelFactory.h"

namespace ccm {

static AutoPtr<IRPCChannelFactory> sLocalFactory = new CDBusChannelFactory(RPCType::Local);
static AutoPtr<IRPCChannelFactory> sRemoteFactory;

ECode CoCreateParcel(
    /* [in] */ RPCType type,
    /* [out] */ IParcel** parcel)
{
    VALIDATE_NOT_NULL(parcel);

    AutoPtr<IRPCChannelFactory> factory =
            type == RPCType::Local ? sLocalFactory : sRemoteFactory;
    AutoPtr<IRPCChannel> channel;
    ECode ec = factory->CreateChannel(RPCPeer::Proxy, (IRPCChannel**)&channel);
    if (FAILED(ec)) {
        *parcel = nullptr;
        return ec;
    }
    return channel->CreateParcel(parcel);
}

ECode CoCreateInterfacePack(
    /* [in] */ RPCType type,
    /* [out] */ IInterfacePack** ipack)
{
    VALIDATE_NOT_NULL(ipack);

    AutoPtr<IRPCChannelFactory> factory =
            type == RPCType::Local ? sLocalFactory : sRemoteFactory;
    AutoPtr<IRPCChannel> channel;
    ECode ec = factory->CreateChannel(RPCPeer::Proxy, (IRPCChannel**)&channel);
    if (FAILED(ec)) {
        *ipack = nullptr;
        return ec;
    }
    return channel->CreateInterfacePack(ipack);
}

ECode CoCreateProxy(
    /* [in] */ const CoclassID& cid,
    /* [in] */ RPCType type,
    /* [out] */ IProxy** proxy)
{
    VALIDATE_NOT_NULL(proxy);

    AutoPtr<IRPCChannelFactory> factory =
            type == RPCType::Local ? sLocalFactory : sRemoteFactory;
    AutoPtr<IRPCChannel> channel;
    ECode ec = factory->CreateChannel(RPCPeer::Proxy, (IRPCChannel**)&channel);
    if (FAILED(ec)) {
        *proxy = nullptr;
        return ec;
    }
    return CProxy::CreateObject(cid, channel, proxy);
}

ECode CoCreateStub(
    /* [in] */ IInterface* object,
    /* [in] */ RPCType type,
    /* [out] */ IStub** stub)
{
    VALIDATE_NOT_NULL(stub);

    AutoPtr<IRPCChannelFactory> factory =
            type == RPCType::Local ? sLocalFactory : sRemoteFactory;
    AutoPtr<IRPCChannel> channel;
    ECode ec = factory->CreateChannel(RPCPeer::Stub, (IRPCChannel**)&channel);
    if (FAILED(ec)) {
        *stub = nullptr;
        return ec;
    }
    return CStub::CreateObject(object, channel, stub);
}

ECode CoMarshalInterface(
    /* [in] */ IInterface* object,
    /* [in] */ RPCType type,
    /* [out] */ IInterfacePack** ipack)
{
    VALIDATE_NOT_NULL(ipack);

    if (object == nullptr) {
        *ipack = nullptr;
        return NOERROR;
    }

    AutoPtr<IRPCChannelFactory> factory =
            type == RPCType::Local ? sLocalFactory : sRemoteFactory;
    AutoPtr<IRPCChannel> channel;
    ECode ec = factory->CreateChannel(RPCPeer::Stub, (IRPCChannel**)&channel);
    if (FAILED(ec)) {
        *ipack = nullptr;
        return ec;
    }
    return channel->MarshalInterface(object, ipack);
}

ECode CoUnmarshalInterface(
    /* [in] */ RPCType type,
    /* [in] */ IInterfacePack* data,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    if (data == nullptr) {
        *object = nullptr;
        return NOERROR;
    }

    AutoPtr<IRPCChannelFactory> factory =
            type == RPCType::Local ? sLocalFactory : sRemoteFactory;
    AutoPtr<IRPCChannel> channel;
    ECode ec = factory->CreateChannel(RPCPeer::Stub, (IRPCChannel**)&channel);
    if (FAILED(ec)) {
        *object = nullptr;
        return ec;
    }
    return channel->UnmarshalInterface(data, object);
}

}
