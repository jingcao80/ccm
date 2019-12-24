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

#include "comorpc.h"
#include "CDBusChannelFactory.h"
#include "CDBusChannel.h"
#include "CDBusParcel.h"
#include "CProxy.h"
#include "CStub.h"
#include "InterfacePack.h"
#include "registry.h"
#include "util/comosp.h"

namespace como {

COMO_INTERFACE_IMPL_LIGHT_1(CDBusChannelFactory, LightRefBase, IRPCChannelFactory);

CDBusChannelFactory::CDBusChannelFactory(
    /* [in] */ RPCType type)
    : mType(type)
{}

ECode CDBusChannelFactory::CreateInterfacePack(
    /* [out] */ AutoPtr<IInterfacePack>& ipack)
{
    ipack = new InterfacePack();
    return NOERROR;
}

ECode CDBusChannelFactory::CreateParcel(
    /* [out] */ AutoPtr<IParcel>& parcel)
{
    parcel = new CDBusParcel();
    return NOERROR;
}

ECode CDBusChannelFactory::CreateChannel(
    /* [in] */ RPCPeer peer,
    /* [out] */ AutoPtr<IRPCChannel>& channel)
{
    channel = (IRPCChannel*)new CDBusChannel(mType, peer);
    return NOERROR;
}

ECode CDBusChannelFactory::MarshalInterface(
    /* [in] */ IInterface* object,
    /* [out] */ AutoPtr<IInterfacePack>& ipack)
{
    InterfaceID iid;
    object->GetInterfaceID(object, iid);
    InterfacePack* pack = new InterfacePack();

    AutoPtr<IStub> stub;
    ECode ec = FindExportObject(mType, IObject::Probe(object), stub);
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
            ec = CoCreateStub(object, mType, stub);
            if (FAILED(ec)) {
                Logger::E("CDBusChannel", "Marshal interface failed.");
                ipack = nullptr;
                return ec;
            }
            CDBusChannel* channel = CDBusChannel::GetStubChannel(stub);
            pack->SetDBusName(channel->mName);
            pack->SetCoclassID(((CStub*)stub.Get())->GetTargetCoclassID());
            pack->SetInterfaceID(iid);
            RegisterExportObject(mType, IObject::Probe(object), stub);
        }
    }

    ipack = (IInterfacePack*)pack;
    return NOERROR;
}

ECode CDBusChannelFactory::UnmarshalInterface(
    /* [in] */ IInterfacePack* ipack,
    /* [out] */ AutoPtr<IInterface>& object)
{
    AutoPtr<IObject> iobject;
    ECode ec = FindImportObject(mType, ipack, iobject);
    if (SUCCEEDED(ec)) {
        InterfaceID iid;
        ipack->GetInterfaceID(iid);
        object = iobject->Probe(iid);
        return NOERROR;
    }

    AutoPtr<IStub> stub;
    ec = FindExportObject(mType, ipack, stub);
    if (SUCCEEDED(ec)) {
        CStub* stubObj = (CStub*)stub.Get();
        InterfaceID iid;
        ipack->GetInterfaceID(iid);
        object = stubObj->GetTarget()->Probe(iid);
        return NOERROR;
    }

    AutoPtr<IProxy> proxy;
    ec = CoCreateProxy(ipack, mType, nullptr, proxy);
    if (FAILED(ec)) {
        object = nullptr;
        return ec;
    }

    RegisterImportObject(mType, ipack, IObject::Probe(proxy));

    object = proxy;
    return NOERROR;
}

} // namespace como
