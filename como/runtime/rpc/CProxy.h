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

#ifndef __COMO_CPROXY_H__
#define __COMO_CPROXY_H__

#include "reflection/comoreflapi.h"
#include "type/comoarray.h"
#include "util/comosp.h"
#include "util/comoobj.h"

namespace como {

class CProxy;

class InterfaceProxy
{
private:
    struct Registers
    {
        typedef Long Reg_t;

        union GPReg {
            Reg_t   reg;
            Long    lVal;
            Integer iVal;
        };

        union SSEReg {
            Reg_t   reg;
            Double  dVal;
            Float   fVal;
        };

        GPReg rbp;
        GPReg rdi;
        GPReg rsi;
        GPReg rdx;
        GPReg rcx;
        GPReg r8;
        GPReg r9;

        SSEReg xmm0;
        SSEReg xmm1;
        SSEReg xmm2;
        SSEReg xmm3;
        SSEReg xmm4;
        SSEReg xmm5;
        SSEReg xmm6;
        SSEReg xmm7;

        Integer paramStartOffset;
    };

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
        /* [out] */ InterfaceID& iid);

    static ECode ProxyEntry(
        /* [in] */ HANDLE args);

private:
    ECode MarshalArguments(
        /* [in] */ Registers& regs,
        /* [in] */ IMetaMethod* method,
        /* [in] */ IParcel* argParcel);

    ECode UnmarshalResults(
        /* [in] */ Registers& regs,
        /* [in] */ IMetaMethod* method,
        /* [in] */ IParcel* resParcel);

    Integer GetIntegerValue(
        /* [in] */ Registers& regs,
        /* [in] */ Integer intParamIndex,
        /* [in] */ Integer fpParamIndex);

    Long GetLongValue(
        /* [in] */ Registers& regs,
        /* [in] */ Integer intParamIndex,
        /* [in] */ Integer fpParamIndex);

    Float GetFloatValue(
        /* [in] */ Registers& regs,
        /* [in] */ Integer intParamIndex,
        /* [in] */ Integer fpParamIndex);

    Double GetDoubleValue(
        /* [in] */ Registers& regs,
        /* [in] */ Integer intParamIndex,
        /* [in] */ Integer fpParamIndex);

    HANDLE GetHANDLEValue(
        /* [in] */ Registers& regs,
        /* [in] */ Integer intParamIndex,
        /* [in] */ Integer fpParamIndex);

private:
    friend class CProxy;

    static constexpr Boolean DEBUG = false;
    HANDLE* mVtable;    // must be the first member
    HANDLE mProxyEntry;  // must be the second member
    Integer mIndex;
    InterfaceID mIid;
    IMetaInterface* mTargetMetadata;
    CProxy* mOwner;
};

extern const CoclassID CID_CProxy;

COCLASS_ID(228c4e6a-1df5-4130-b46e-d0322b676976)
class CProxy
    : public Object
    , public IProxy
{
public:
    ~CProxy();

    COMO_OBJECT_DECL();

    COMO_INTERFACE_DECL();

    ECode GetTargetCoclass(
        /* [out] */ AutoPtr<IMetaCoclass>& target) override;

    ECode IsStubAlive(
        /* [out] */ Boolean& alive) override;

    ECode LinkToDeath(
        /* [in] */ IDeathRecipient* recipient,
        /* [in] */ HANDLE cookie = 0,
        /* [in] */ Integer flags = 0) override;

    ECode UnlinkToDeath(
        /* [in] */ IDeathRecipient* recipient,
        /* [in] */ HANDLE cookie = 0,
        /* [in] */ Integer flags = 0,
        /* [out] */ AutoPtr<IDeathRecipient>* outRecipient = nullptr) override;

    AutoPtr<IRPCChannel> GetChannel();

    CoclassID GetTargetCoclassID();

    static ECode CreateObject(
        /* [in] */ const CoclassID& cid,
        /* [in] */ IRPCChannel* channel,
        /* [in] */ IClassLoader* loader,
        /* [out] */ AutoPtr<IProxy>& proxy);

private:
    friend class InterfaceProxy;

    CoclassID mCid;
    IMetaCoclass* mTargetMetadata;
    Array<InterfaceProxy*> mInterfaces;
    AutoPtr<IRPCChannel> mChannel;
};

} // namespace como

#endif // __COMO_CPROXY_H__
