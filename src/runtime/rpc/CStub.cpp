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

//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
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

#include "CStub.h"
#include "registry.h"

namespace ccm {

Integer InterfaceStub::AddRef(
    /* [in] */ HANDLE id)
{
    return 1;
}

Integer InterfaceStub::Release(
    /* [in] */ HANDLE id)
{
    return 1;
}

//----------------------------------------------------------------------

const CoclassID CID_CStub =
        {{0x52068014,0xe347,0x453f,0x87a9,{0x0,0xb,0xe,0xc,0xf,0xb,0x6,0x9,0xd,0x8,0xe,0xd}}, &CID_CCMRuntime};

CCM_INTERFACE_IMPL_1(CStub, Object, IStub);

CCM_OBJECT_IMPL(CStub);

ECode CStub::Invoke(
    /* [in] */ IMetaMethod* method,
    /* [in] */ IArgumentList* args)
{
    return NOERROR;
}

ECode CStub::CreateObject(
    /* [in] */ IInterface* object,
    /* [in] */ IRPCChannel* channel,
    /* [in] */ IStub** stub)
{
    VALIDATE_NOT_NULL(stub);
    *stub = nullptr;

    IObject* obj = IObject::Probe(object);
    if (obj == nullptr) {
        Logger::E("CStub", "Object does not have \"IObject\" interface.");
        return E_INTERFACE_NOT_FOUND_EXCEPTION;
    }

    AutoPtr<IMetaCoclass> mc;
    obj->GetCoclass((IMetaCoclass**)&mc);
    if (mc == nullptr) {
        Logger::E("CStub", "Fail to get object's Coclass.");
        return E_NOT_FOUND_EXCEPTION;
    }

    CoclassID cid;
    mc->GetCoclassID(&cid);

    if (DEBUG) {
        Logger::D("CStub", "Object's CoclassID is %s",
                DumpUuid(cid.mUuid).string());
    }

    CStub* stubObj = new CStub();
    stubObj->mTarget = obj;
    stubObj->mCid = cid;
    stubObj->mTargetMetadata = mc;
    stubObj->mChannel = channel;

    Integer interfaceNumber;
    mc->GetInterfaceNumber(&interfaceNumber);
    Array<IMetaInterface*> interfaces(interfaceNumber);
    mc->GetAllInterfaces(interfaces);
    stubObj->mInterfaces = Array<InterfaceStub*>(interfaceNumber);
    for (Integer i = 0; i < interfaceNumber; i++) {
        InterfaceStub* istub = new InterfaceStub();
        istub->mTargetMetadata = interfaces[i];
        istub->mTargetMetadata->GetInterfaceID(&istub->mIid);
        istub->mObject = object->Probe(istub->mIid);
        if (istub->mObject == nullptr) {
            String name, ns;
            interfaces[i]->GetNamespace(&ns);
            interfaces[i]->GetName(&name);
            Logger::E("CStub", "Object does not have \"%s%s\" interface.",
                    ns.string(), name.string());
            delete stubObj;
            return E_INTERFACE_NOT_FOUND_EXCEPTION;
        }
        stubObj->mInterfaces[i] = istub;
    }

    RPCType type;
    channel->GetRPCType(&type);
    ECode ec = RegisterExportObject(type, obj, stubObj);
    if (FAILED(ec)) {
        Logger::E("CStub", "Fail to register export object with ec is 0x%x", ec);
        delete stubObj;
        return ec;
    }

    ec = channel->StartListening();
    if (FAILED(ec)) {
        Logger::E("CStub", "Channel start listening failed with ec is 0x%x", ec);
        delete stubObj;
        return ec;
    }

    *stub = stubObj;
    REFCOUNT_ADD(*stub);
    return NOERROR;
}

}
