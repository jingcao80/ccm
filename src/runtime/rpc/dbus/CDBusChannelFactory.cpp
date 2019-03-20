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
#include "CDBusChannelFactory.h"
#include "CDBusChannel.h"
#include "CDBusParcel.h"
#include "CProxy.h"
#include "CStub.h"
#include "InterfacePack.h"
#include "registry.h"
#include "util/ccmautoptr.h"

namespace ccm {

CCM_INTERFACE_IMPL_LIGHT_1(CDBusChannelFactory, LightRefBase, IRPCChannelFactory);

CDBusChannelFactory::CDBusChannelFactory(
    /* [in] */ RPCType type)
    : mType(type)
{}

ECode CDBusChannelFactory::CreateInterfacePack(
    /* [out] */ IInterfacePack** ipack)
{
    VALIDATE_NOT_NULL(ipack);

    *ipack = new InterfacePack();
    REFCOUNT_ADD(*ipack);
    return NOERROR;
}

ECode CDBusChannelFactory::CreateParcel(
    /* [out] */ IParcel** parcel)
{
    VALIDATE_NOT_NULL(parcel)

    *parcel = new CDBusParcel();
    REFCOUNT_ADD(*parcel);
    return NOERROR;
}

ECode CDBusChannelFactory::CreateChannel(
    /* [in] */ RPCPeer peer,
    /* [out] */ IRPCChannel** channel)
{
    VALIDATE_NOT_NULL(channel);

    *channel = (IRPCChannel*)new CDBusChannel(mType, peer);
    REFCOUNT_ADD(*channel);
    return NOERROR;
}

ECode CDBusChannelFactory::MarshalInterface(
    /* [in] */ IInterface* object,
    /* [out] */ IInterfacePack** ipack)
{
    VALIDATE_NOT_NULL(ipack);

    InterfaceID iid;
    object->GetInterfaceID(object, &iid);
    InterfacePack* pack = new InterfacePack();

    AutoPtr<IStub> stub;
    ECode ec = FindExportObject(mType, IObject::Probe(object), &stub);
    if (SUCCEEDED(ec)) {
        CDBusChannel* channel = CDBusChannel::GetStubChannel(stub);
        pack->SetDBusName(channel->mName);
        pack->SetCoclassID(((CStub*)stub.Get())->GetTargetCoclassID());
        pack->SetInterfaceID(iid);
    }
    else {
        IProxy* proxy = IProxy::Probe(object);
        if (proxy != nullptr) {
            CDBusChannel* channel = CDBusChannel::GetProxyChannel(proxy);
            pack->SetDBusName(channel->mName);
            pack->SetCoclassID(((CProxy*)proxy)->GetTargetCoclassID());
            pack->SetInterfaceID(iid);
        }
        else {
            ec = CoCreateStub(object, mType, &stub);
            if (FAILED(ec)) {
                Logger::E("CDBusChannel", "Marshal interface failed.");
                *ipack = nullptr;
                return ec;
            }
            CDBusChannel* channel = CDBusChannel::GetStubChannel(stub);
            pack->SetDBusName(channel->mName);
            pack->SetCoclassID(((CStub*)stub.Get())->GetTargetCoclassID());
            pack->SetInterfaceID(iid);
            RegisterExportObject(mType, IObject::Probe(object), stub);
        }
    }

    *ipack = (IInterfacePack*)pack;
    REFCOUNT_ADD(*ipack);
    return NOERROR;
}

ECode CDBusChannelFactory::UnmarshalInterface(
    /* [in] */ IInterfacePack* ipack,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IObject> iobject;
    ECode ec = FindImportObject(mType, ipack, &iobject);
    if (SUCCEEDED(ec)) {
        InterfaceID iid;
        ipack->GetInterfaceID(&iid);
        *object = iobject->Probe(iid);
        REFCOUNT_ADD(*object);
        return NOERROR;
    }

    AutoPtr<IStub> stub;
    ec = FindExportObject(mType, ipack, &stub);
    if (SUCCEEDED(ec)) {
        CStub* stubObj = (CStub*)stub.Get();
        InterfaceID iid;
        ipack->GetInterfaceID(&iid);
        *object = stubObj->GetTarget()->Probe(iid);
        REFCOUNT_ADD(*object);
        return NOERROR;
    }

    CoclassID cid;
    ipack->GetCoclassID(&cid);
    AutoPtr<IProxy> proxy;
    ec = CoCreateProxy(cid, mType, &proxy);
    if (FAILED(ec)) {
        *object = nullptr;
        return ec;
    }
    CDBusChannel* channel = CDBusChannel::GetProxyChannel(proxy);
    channel->mName = ((InterfacePack*)ipack)->GetDBusName();
    RegisterImportObject(mType, ipack, IObject::Probe(proxy));

    proxy.MoveTo((IProxy**)object);
    return NOERROR;
}

}
