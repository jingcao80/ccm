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

#ifndef __CCM_CProxy_H__
#define __CCM_CProxy_H__

#include "reflection/ccmreflectionapi.h"
#include "type/ccmarray.h"
#include "util/ccmobject.h"

namespace ccm {

class CProxy;

class InterfaceProxy
{
public:
    Integer AddRef(
        /* [in] */ HANDLE id = 0);

    Integer Release(
        /* [in] */ HANDLE id = 0);

    static Integer S_AddRef(
        /* [in] */ InterfaceProxy* thisObj,
        /* [in] */ HANDLE id);

    static Integer S_Release(
        /* [in] */ InterfaceProxy* thisObj,
        /* [in] */ HANDLE id);

    static IInterface* S_Probe(
        /* [in] */ InterfaceProxy* thisObj,
        /* [in] */ const InterfaceID& iid);

    static ECode S_GetInterfaceID(
        /* [in] */ InterfaceProxy* thisObj,
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    ECode PackingArguments(
        /* [in] */ IMetaMethod* method,
        /* [in] */ IArgumentList* argList);

    static ECode ProxyEntry(
        /* [in] */ HANDLE args);

private:
    friend class CProxy;

    static constexpr Boolean DEBUG = false;
    HANDLE* mVtable;    // must be the first member
    HANDLE mProxyFunc;  // must be the second member
    InterfaceID mIid;
    IMetaInterface* mMetadata;
    CProxy* mObject;
};

extern const CoclassID CID_CProxy;

COCLASS_ID(228c4e6a-1df5-4130-b46e-d0322b676976)
class CProxy
    : public Object
    , public IProxy
{
public:
    ~CProxy();

    CCM_OBJECT_DECL();

    CCM_INTERFACE_DECL();

    ECode SetInvocationHandler(
        /* [in] */ IInvocationHandler* handler) override;

    ECode IsStubAlive(
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

    static ECode CreateObject(
        /* [in] */ const CoclassID& cid,
        /* [in] */ IProxy** proxy);

private:
    CoclassID mCid;
    IMetaCoclass* mMetadata;
    Array<InterfaceProxy*> mInterfaces;
};

}

#endif // __CCM_CProxy_H__
