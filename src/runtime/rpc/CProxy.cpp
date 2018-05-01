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

#define GET_XREG(reg, var)      \
    __asm__ __volatile__(       \
        "movsd  %%"#reg", %0;"  \
        : "=m"(var)             \
    )

#define GET_RBP(var)                \
    __asm__ __volatile__(           \
        "mov    (%%rbp), %%rax;"    \
        "mov    %%rax, %0;"         \
        : "=m"(var)                 \
    );

#define GET_STACK(rsp, off, var)        \
    __asm__ __volatile__(               \
        "mov    %1, %%rax;"             \
        "mov    %2, %%ebx;"             \
        "add    %%rbx, %%rax;"          \
        "mov    (%%rax), %%rax;"        \
        "mov    %%rax, %0;"             \
        : "=m"(var)                     \
        : "m"(rsp)                      \
        , "m"(off)                      \
    )

EXTERN_C void __entry();

__asm__ __volatile__(
    ".text;"
    ".align 8;"
    ".global __entry;"
    "__entry:"
    "push   %rbp;"
    "mov    %rsp, %rbp;"
    "mov    $0xff, %ebx;"
    "call   *8(%rdi);"
    "leaveq;"
    "ret;"
);

HANDLE PROXY_ENTRY = 0;

static constexpr Integer PROXY_ENTRY_SIZE = 16;
static constexpr Integer PROXY_ENTRY_SHIFT = 4;
static constexpr Integer PROXY_ENTRY_NUMBER = 240;
static constexpr Integer PROXY_INDEX_OFFSET = 5;

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
    /* [in] */ Registers& regs,
    /* [in] */ IMetaMethod* method,
    /* [in] */ IArgumentList* argList)
{
    Integer N;
    method->GetParameterNumber(&N);
    Integer intNum = 1, fpNum = 0;
    for (Integer i = 0; i < N; i++) {
        AutoPtr<IMetaParameter> param;
        method->GetParameter(i, (IMetaParameter**)&param);
        AutoPtr<IMetaType> type;
        param->GetType((IMetaType**)&type);
        CcmTypeKind kind;
        type->GetTypeKind((Integer*)&kind);
        IOAttribute ioAttr;
        param->GetIOAttribute(&ioAttr);
        if (ioAttr == IOAttribute::IN) {
            switch (kind) {
                case CcmTypeKind::Char: {
                    Char value = (Char)GetLongValue(regs, intNum++, fpNum);
                    argList->SetInputArgumentOfChar(i, value);
                    break;
                }
                case CcmTypeKind::Byte: {
                    Byte value = (Byte)GetLongValue(regs, intNum++, fpNum);
                    argList->SetInputArgumentOfByte(i, value);
                    break;
                }
                case CcmTypeKind::Short: {
                    Short value = (Short)GetLongValue(regs, intNum++, fpNum);
                    argList->SetInputArgumentOfShort(i, value);
                    break;
                }
                case CcmTypeKind::Integer: {
                    Integer value = (Integer)GetLongValue(regs, intNum++, fpNum);
                    argList->SetInputArgumentOfInteger(i, value);
                    break;
                }
                case CcmTypeKind::Long: {
                    Long value = GetLongValue(regs, intNum++, fpNum);
                    argList->SetInputArgumentOfLong(i, value);
                    break;
                }
                case CcmTypeKind::Float: {
                    Float value = (Float)GetDoubleValue(regs, intNum, fpNum++);
                    argList->SetInputArgumentOfFloat(i, value);
                    break;
                }
                case CcmTypeKind::Double: {
                    Double value = GetDoubleValue(regs, intNum, fpNum++);
                    argList->SetInputArgumentOfDouble(i, value);
                    break;
                }
                case CcmTypeKind::Boolean: {
                    Boolean value = (Boolean)GetLongValue(regs, intNum++, fpNum);
                    argList->SetInputArgumentOfBoolean(i, value);
                    break;
                }
                case CcmTypeKind::String: {
                    String value = *reinterpret_cast<String*>(GetLongValue(regs, intNum++, fpNum));
                    argList->SetInputArgumentOfString(i, value);
                    break;
                }
                case CcmTypeKind::CoclassID: {
                    CoclassID cid = *reinterpret_cast<CoclassID*>(GetLongValue(regs, intNum++, fpNum));
                    argList->SetInputArgumentOfCoclassID(i, cid);
                    break;
                }
                case CcmTypeKind::ComponentID: {
                    ComponentID cid = *reinterpret_cast<ComponentID*>(GetLongValue(regs, intNum++, fpNum));
                    argList->SetInputArgumentOfComponentID(i, cid);
                    break;
                }
                case CcmTypeKind::InterfaceID: {
                    InterfaceID iid = *reinterpret_cast<InterfaceID*>(GetLongValue(regs, intNum++, fpNum));
                    argList->SetInputArgumentOfInterfaceID(i, iid);
                    break;
                }
                case CcmTypeKind::HANDLE: {
                    HANDLE value = (HANDLE)GetLongValue(regs, intNum++, fpNum);
                    argList->SetInputArgumentOfHANDLE(i, value);
                    break;
                }
                case CcmTypeKind::ECode: {
                    ECode value = (ECode)GetLongValue(regs, intNum++, fpNum);
                    argList->SetInputArgumentOfECode(i, value);
                    break;
                }
                case CcmTypeKind::Enum: {
                    Integer value = (Integer)GetLongValue(regs, intNum++, fpNum);
                    argList->SetInputArgumentOfEnumeration(i, value);
                    break;
                }
                case CcmTypeKind::Array: {
                    HANDLE value = (HANDLE)GetLongValue(regs, intNum++, fpNum);
                    argList->SetInputArgumentOfArray(i, value);
                    break;
                }
                case CcmTypeKind::Interface: {
                    IInterface* value = reinterpret_cast<IInterface*>(GetLongValue(regs, intNum++, fpNum));
                    argList->SetInputArgumentOfInterface(i, value);
                    break;
                }
                default:
                    Logger::E("CProxy", "Invalid [in] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        else if (ioAttr == IOAttribute::OUT ||
                 ioAttr == IOAttribute::IN_OUT) {
            switch (kind) {
                case CcmTypeKind::Char: {
                    HANDLE value = (HANDLE)GetLongValue(regs, intNum++, fpNum);
                    argList->SetOutputArgumentOfChar(i, value);
                    break;
                }
                case CcmTypeKind::Byte: {
                    HANDLE value = (HANDLE)GetLongValue(regs, intNum++, fpNum);
                    argList->SetOutputArgumentOfByte(i, value);
                    break;
                }
                case CcmTypeKind::Short: {
                    HANDLE value = (HANDLE)GetLongValue(regs, intNum++, fpNum);
                    argList->SetOutputArgumentOfShort(i, value);
                    break;
                }
                case CcmTypeKind::Integer: {
                    HANDLE value = (HANDLE)GetLongValue(regs, intNum++, fpNum);
                    argList->SetOutputArgumentOfInteger(i, value);
                    break;
                }
                case CcmTypeKind::Long: {
                    HANDLE value = (HANDLE)GetLongValue(regs, intNum++, fpNum);
                    argList->SetOutputArgumentOfLong(i, value);
                    break;
                }
                case CcmTypeKind::Float: {
                    HANDLE value = (HANDLE)GetLongValue(regs, intNum++, fpNum);
                    argList->SetOutputArgumentOfFloat(i, value);
                    break;
                }
                case CcmTypeKind::Double: {
                    HANDLE value = (HANDLE)GetLongValue(regs, intNum++, fpNum);
                    argList->SetOutputArgumentOfDouble(i, value);
                    break;
                }
                case CcmTypeKind::Boolean: {
                    HANDLE value = (HANDLE)GetLongValue(regs, intNum++, fpNum);
                    argList->SetOutputArgumentOfBoolean(i, value);
                    break;
                }
                case CcmTypeKind::String: {
                    HANDLE value = (HANDLE)GetLongValue(regs, intNum++, fpNum);
                    argList->SetOutputArgumentOfString(i, value);
                    break;
                }
                case CcmTypeKind::CoclassID: {
                    HANDLE value = (HANDLE)GetLongValue(regs, intNum++, fpNum);
                    argList->SetOutputArgumentOfCoclassID(i, value);
                    break;
                }
                case CcmTypeKind::ComponentID: {
                    HANDLE value = (HANDLE)GetLongValue(regs, intNum++, fpNum);
                    argList->SetOutputArgumentOfComponentID(i, value);
                    break;
                }
                case CcmTypeKind::InterfaceID: {
                    HANDLE value = (HANDLE)GetLongValue(regs, intNum++, fpNum);
                    argList->SetOutputArgumentOfInterfaceID(i, value);
                    break;
                }
                case CcmTypeKind::HANDLE: {
                    HANDLE value = (HANDLE)GetLongValue(regs, intNum++, fpNum);
                    argList->SetOutputArgumentOfHANDLE(i, value);
                    break;
                }
                case CcmTypeKind::ECode: {
                    HANDLE value = (HANDLE)GetLongValue(regs, intNum++, fpNum);
                    argList->SetOutputArgumentOfECode(i, value);
                    break;
                }
                case CcmTypeKind::Enum: {
                    HANDLE value = (HANDLE)GetLongValue(regs, intNum++, fpNum);
                    argList->SetOutputArgumentOfEnumeration(i, value);
                    break;
                }
                case CcmTypeKind::Array: {
                    HANDLE value = (HANDLE)GetLongValue(regs, intNum++, fpNum);
                    argList->SetOutputArgumentOfArray(i, value);
                    break;
                }
                case CcmTypeKind::Interface: {
                    HANDLE value = (HANDLE)GetLongValue(regs, intNum++, fpNum);
                    argList->SetOutputArgumentOfInterface(i, value);
                    break;
                }
                default:
                    Logger::E("CProxy", "Invalid [out] or [in, out] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
    }

    return NOERROR;
}

Long InterfaceProxy::GetLongValue(
    /* [in] */ Registers& regs,
    /* [in] */ Integer intIndex,
    /* [in] */ Integer fpIndex)
{
    switch (intIndex) {
        case 0:
            return regs.rdi;
        case 1:
            return regs.rsi;
        case 2:
            return regs.rdx;
        case 3:
            return regs.rcx;
        case 4:
            return regs.r8;
        case 5:
            return regs.r9;
        default: {
            Long val;
            Integer off = fpIndex <= 7 ? (intIndex - 5 + 1) * 8 :
                    (intIndex - 5 + fpIndex - 8 + 1) * 8;
            GET_STACK(regs.rbp, off, val);
            return val;
        }
    }
}

Double InterfaceProxy::GetDoubleValue(
    /* [in] */ Registers& regs,
    /* [in] */ Integer intIndex,
    /* [in] */ Integer fpIndex)
{
    switch (fpIndex) {
        case 0:
            return regs.xmm0;
        case 1:
            return regs.xmm1;
        case 2:
            return regs.xmm2;
        case 3:
            return regs.xmm3;
        case 4:
            return regs.xmm4;
        case 5:
            return regs.xmm5;
        case 6:
            return regs.xmm6;
        case 7:
            return regs.xmm7;
        default: {
            Double val;
            Integer off = intIndex <= 5 ? (fpIndex - 7 + 1) * 8 :
                    (fpIndex - 7 + intIndex - 6 + 1) * 8;
            GET_STACK(regs.rbp, off, val);
            return val;
        }
    }
}

ECode InterfaceProxy::ProxyEntry(
    /* [in] */ HANDLE args)
{
    InterfaceProxy* thisObj = reinterpret_cast<InterfaceProxy*>(args);

    Integer methodIndex;
    GET_REG(ebx, methodIndex);

    Registers regs;
    GET_RBP(regs.rbp);

    GET_REG(rdi, regs.rdi);
    GET_REG(rsi, regs.rsi);
    GET_REG(rdx, regs.rdx);
    GET_REG(rcx, regs.rcx);
    GET_REG(r8, regs.r8);
    GET_REG(r9, regs.r9);

    GET_XREG(xmm0, regs.xmm0);
    GET_XREG(xmm1, regs.xmm1);
    GET_XREG(xmm2, regs.xmm2);
    GET_XREG(xmm3, regs.xmm3);
    GET_XREG(xmm4, regs.xmm4);
    GET_XREG(xmm5, regs.xmm5);
    GET_XREG(xmm6, regs.xmm6);
    GET_XREG(xmm7, regs.xmm7);

    if (DEBUG) {
        String name, ns;
        thisObj->mMetadata->GetName(&name);
        thisObj->mMetadata->GetNamespace(&ns);
        Logger::D("CProxy", "Call ProxyEntry with interface \"%s%s\"",
                ns.string(), name.string());
    }

    AutoPtr<IMetaMethod> method;
    thisObj->mMetadata->GetMethod(methodIndex + 4, (IMetaMethod**)&method);

    if (DEBUG) {
        String name, signature;
        method->GetName(&name);
        method->GetSignature(&signature);
        Logger::D("CProxy", "Call ProxyEntry with method \"%s(%s)\"",
                name.string(), signature.string());
    }

    AutoPtr<IArgumentList> argList;
    method->CreateArgumentList((IArgumentList**)&argList);
    ECode ec = thisObj->PackingArguments(regs, method, argList);
    if (FAILED(ec)) goto ProxyExit;

    ec = thisObj->mObject->mHandler->Invoke((IProxy*)thisObj->mObject, method, argList);

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
    mHandler = handler;
    return NOERROR;
}

ECode CProxy::IsStubAlive(
    /* [out] */ Boolean* alive)
{
    return mHandler->IsStubAlive(alive);
}

ECode CProxy::LinkToDeath(
    /* [in] */ IDeathRecipient* recipient,
    /* [in] */ HANDLE cookie,
    /* [in] */ Integer flags)
{
    return mHandler->LinkToDeath(recipient, cookie, flags);
}

ECode CProxy::UnlinkToDeath(
    /* [in] */ IDeathRecipient* recipient,
    /* [in] */ HANDLE cookie,
    /* [in] */ Integer flags,
    /* [out] */ IDeathRecipient** outRecipient)
{
    return mHandler->UnlinkToDeath(recipient, cookie, flags, outRecipient);
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
        iproxy->mProxyEntry = reinterpret_cast<HANDLE>(&InterfaceProxy::ProxyEntry);
        proxyObj->mInterfaces[i] = iproxy;
    }

    *proxy = proxyObj;
    REFCOUNT_ADD(*proxy);
    return NOERROR;
}

}
