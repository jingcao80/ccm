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

#include "ccmautoptr.h"
#include "ccmlogger.h"
#include "CProxy.h"
#include <sys/mman.h>

namespace ccm {

#ifndef PAGE_SIZE
#define PAGE_SIZE (1u << 12)
#endif
#ifndef PAGE_MASK
#define PAGE_MASK (~(PAGE_SIZE - 1))
#endif
#ifndef PAGE_ALIGN
#define PAGE_ALIGN(va) (((va) + PAGE_SIZE - 1) & PAGE_MASK)
#endif

#define GET_REG(reg, var)       \
    __asm__ __volatile__(       \
        "mov    %%"#reg", %0;"  \
        : "=m"(var)             \
    )

EXTERN_C void __entry();
EXTERN_C ECode ProxyFunc(
    /* [in] */ HANDLE args)
{
    return InterfaceProxy::ProxyEntry(args);
}

__asm__ __volatile__(
    ".text;"
    ".align 8;"
    ".global __entry;"
    "__entry:"
    "push   %rbx;"
    "mov    $0xff, %ebx;"
    "call   *8(%rdi);"
    "pop    %rbx;"
    "ret;"
    "nop;"
    "nop;"
    "nop;"
    "nop;"
    "nop;"
);

HANDLE PROXY_ENTRY = 0;

static constexpr Integer PROXY_ENTRY_SIZE = 16;
static constexpr Integer PROXY_ENTRY_SHIFT = 4;
static constexpr Integer PROXY_ENTRY_NUMBER = 240;
static constexpr Integer PROXY_INDEX_OFFSET = 2;

static constexpr Integer METHOD_MAX_NUMBER = PROXY_ENTRY_NUMBER + 4;

static HANDLE sProxyVtable[METHOD_MAX_NUMBER];

void Init_Proxy_Entry()
{
    PROXY_ENTRY = reinterpret_cast<HANDLE>(mmap(nullptr,
            PAGE_ALIGN(PROXY_ENTRY_SIZE * PROXY_ENTRY_NUMBER),
            PROT_READ | PROT_WRITE | PROT_EXEC,
            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    if (PROXY_ENTRY == 0) {
        Logger::E("CProxy", "Mmap PROXY_ENTRY failed.\n");
        return;
    }

    Byte* p = (Byte*)PROXY_ENTRY;
    for (Integer i = 0; i < PROXY_ENTRY_NUMBER; i++) {
        memcpy(p, reinterpret_cast<void*>(&__entry), PROXY_ENTRY_SIZE);
        *(Integer*)(p + PROXY_INDEX_OFFSET) = i;
        p += PROXY_ENTRY_SIZE;
    }

    sProxyVtable[0] = reinterpret_cast<HANDLE>(&InterfaceProxy::S_AddRef);
    sProxyVtable[1] = reinterpret_cast<HANDLE>(&InterfaceProxy::S_Release);
    sProxyVtable[2] = reinterpret_cast<HANDLE>(&InterfaceProxy::S_Probe);
    sProxyVtable[3] = reinterpret_cast<HANDLE>(&InterfaceProxy::S_GetInterfaceID);
    for (Integer i = 4; i < METHOD_MAX_NUMBER; i++) {
        sProxyVtable[i] = PROXY_ENTRY + ((i - 4) << PROXY_ENTRY_SHIFT);
    }
}

void Uninit_Proxy_Entry()
{
    if (PROXY_ENTRY != 0) {
        munmap(reinterpret_cast<void*>(PROXY_ENTRY),
                PAGE_ALIGN(PROXY_ENTRY_SIZE * PROXY_ENTRY_NUMBER));
        PROXY_ENTRY = 0;
    }
}

//----------------------------------------------------------------------------------

Integer InterfaceProxy::AddRef(
    /* [in] */ HANDLE id)
{
    return 1;
}

Integer InterfaceProxy::Release(
    /* [in] */ HANDLE)
{
    return 1;
}

Integer InterfaceProxy::S_AddRef(
    /* [in] */ InterfaceProxy* thisObj,
    /* [in] */ HANDLE id)
{
    return thisObj->mObject->AddRef(id);
}

Integer InterfaceProxy::S_Release(
    /* [in] */ InterfaceProxy* thisObj,
    /* [in] */ HANDLE id)
{
    return thisObj->mObject->Release(id);
}

IInterface* InterfaceProxy::S_Probe(
    /* [in] */ InterfaceProxy* thisObj,
    /* [in] */ const InterfaceID& iid)
{
    return thisObj->mObject->Probe(iid);
}

ECode InterfaceProxy::S_GetInterfaceID(
    /* [in] */ InterfaceProxy* thisObj,
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    return thisObj->mObject->GetInterfaceID(object, iid);
}

ECode InterfaceProxy::PackingArguments(
    /* [in] */ IMetaMethod* method,
    /* [in] */ IArgumentList* argList)
{
    return NOERROR;
}

ECode InterfaceProxy::ProxyEntry(
    /* [in] */ HANDLE args)
{
    InterfaceProxy* thisObj = reinterpret_cast<InterfaceProxy*>(args);

    if (DEBUG) {
        String name, ns;
        thisObj->mMetadata->GetName(&name);
        thisObj->mMetadata->GetNamespace(&ns);
        Logger::D("CProxy", "Call ProxyEntry with interface \"%s%s\"",
                ns.string(), name.string());
    }

    Integer methodIndex;
    GET_REG(ebx, methodIndex);

    AutoPtr<IMetaMethod> method;
    thisObj->mMetadata->GetMethod(methodIndex, (IMetaMethod**)&method);

    if (DEBUG) {
        String name, signature;
        method->GetName(&name);
        method->GetSignature(&signature);
        Logger::D("CProxy", "Call ProxyEntry with method \"%s(%s)\"",
                name.string(), signature.string());
    }

    AutoPtr<IArgumentList> argList;
    method->CreateArgumentList((IArgumentList**)&argList);
    ECode ec = thisObj->PackingArguments(method, argList);
    if (FAILED(ec)) goto ProxyExit;

ProxyExit:
    if (DEBUG) {
        Logger::D("CProxy", "Exit ProxyEntry with ec(0x%x)", ec);
    }

    return NOERROR;
}

//----------------------------------------------------------------------

const CoclassID CID_CProxy =
        {{0x228c4e6a,0x1df5,0x4130,0xb46e,{0xd,0x0,0x3,0x2,0x2,0xb,0x6,0x7,0x6,0x9,0x7,0x6}}, &CID_CCMRuntime};

CCM_OBJECT_IMPL(CProxy);

CProxy::~CProxy()
{
    for (Integer i = 0; i < mInterfaces.GetLength(); i++) {
        InterfaceProxy* iproxy = mInterfaces[i];
        mInterfaces[i] = nullptr;
        delete iproxy;
    }
}

Integer CProxy::AddRef(
    /* [in] */ HANDLE id)
{
    return Object::AddRef(id);
}

Integer CProxy::Release(
    /* [in] */ HANDLE id)
{
    return Object::Release(id);
}

IInterface* CProxy::Probe(
    /* [in] */ const InterfaceID& iid)
{
    if (IID_IInterface == iid) {
        return (IObject*)this;
    }
    else if (IID_IObject == iid) {
        return (IObject*)this;
    }
    for (Integer i = 0; i < mInterfaces.GetLength(); i++) {
        InterfaceProxy* iproxy = mInterfaces[i];
        if (iproxy->mIid == iid) {
            return reinterpret_cast<IInterface*>(&iproxy->mVtable);
        }
    }
    return nullptr;
}

ECode CProxy::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid);

    if (object == (IObject*)this) {
        *iid = IID_IObject;
        return NOERROR;
    }
    for (Integer i = 0; i < mInterfaces.GetLength(); i++) {
        InterfaceProxy* iproxy = mInterfaces[i];
        if ((IInterface*)iproxy == object) {
            *iid = iproxy->mIid;
            return NOERROR;
        }
    }
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CProxy::SetInvocationHandler(
    /* [in] */ IInvocationHandler* handler)
{
    return NOERROR;
}

ECode CProxy::IsStubAlive(
    /* [out] */ Boolean* alive)
{
    return NOERROR;
}

ECode CProxy::LinkToDeath(
    /* [in] */ IDeathRecipient* recipient,
    /* [in] */ HANDLE cookie,
    /* [in] */ Integer flags)
{
    return NOERROR;
}

ECode CProxy::UnlinkToDeath(
    /* [in] */ IDeathRecipient* recipient,
    /* [in] */ HANDLE cookie,
    /* [in] */ Integer flags,
    /* [out] */ IDeathRecipient** outRecipient)
{
    return NOERROR;
}

ECode CProxy::CreateObject(
    /* [in] */ const CoclassID& cid,
    /* [in] */ IProxy** proxy)
{
    VALIDATE_NOT_NULL(proxy);
    *proxy = nullptr;

    AutoPtr<IMetaCoclass> mc;
    CoGetCoclassMetadata(cid, nullptr, (IMetaCoclass**)&mc);

    CProxy* proxyObj = new CProxy();
    proxyObj->mCid = cid;
    proxyObj->mMetadata = mc;

    Integer interfaceNumber;
    mc->GetInterfaceNumber(&interfaceNumber);
    Array<IMetaInterface*> interfaces(interfaceNumber);
    mc->GetAllInterfaces(interfaces);
    proxyObj->mInterfaces = Array<InterfaceProxy*>(interfaceNumber);
    for (Integer i = 0; i < interfaceNumber; i++) {
        InterfaceProxy* iproxy = new InterfaceProxy();
        iproxy->mObject = proxyObj;
        iproxy->mMetadata = interfaces[i];
        iproxy->mMetadata->GetInterfaceID(&iproxy->mIid);
        iproxy->mVtable = sProxyVtable;
        iproxy->mProxyFunc = reinterpret_cast<HANDLE>(&ProxyFunc);
        proxyObj->mInterfaces[i] = iproxy;
    }

    *proxy = proxyObj;
    REFCOUNT_ADD(*proxy);
    return NOERROR;
}

}
