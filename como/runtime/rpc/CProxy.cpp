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

#include "component/CBootClassLoader.h"
#include "rpc/comorpc.h"
#include "rpc/CProxy.h"
#include "util/comolog.h"
#include <sys/mman.h>

namespace como {

#ifndef PAGE_SIZE
#define PAGE_SIZE (1u << 12)
#endif
#ifndef PAGE_MASK
#define PAGE_MASK (~(PAGE_SIZE - 1))
#endif
#ifndef PAGE_ALIGN
#define PAGE_ALIGN(va) (((va) + PAGE_SIZE - 1) & PAGE_MASK)
#endif

#if defined(__aarch64__)

#define GET_REG(reg, var)

#define GET_STACK_INTEGER(rbp, off, var)

#define GET_STACK_LONG(rbp, off, var)

#define GET_STACK_FLOAT(rbp, off, var)

#define GET_STACK_DOUBLE(rbp, off, var)

EXTERN_C void __entry();

__asm__(
    ".text;"
    ".align 8;"
    ".global __entry;"
    "__entry:"
    "stp    lr, x1, [sp, #-16];"
    "mov    w1, #0xff;"
    "ldr    lr, [x0, #8];"
    "br     lr;"
);

#elif defined(__x86_64__)

#define GET_REG(reg, var)           \
    __asm__ __volatile__(           \
        "movq   %%"#reg", %0;"      \
        : "=m"(var)                 \
    )

#define GET_STACK_INTEGER(rbp, off, var)    \
    __asm__ __volatile__(                   \
        "movq   %1, %%rax;"                 \
        "movl   %2, %%ebx;"                 \
        "addq   %%rbx, %%rax;"              \
        "movl   (%%rax), %%eax;"            \
        "movl   %%eax, %0;"                 \
        : "=m"(var)                         \
        : "m"(rbp), "m"(off)                \
        : "rax", "rbx"                      \
    )

#define GET_STACK_LONG(rbp, off, var)       \
    __asm__ __volatile__(                   \
        "movq   %1, %%rax;"                 \
        "movl   %2, %%ebx;"                 \
        "addq   %%rbx, %%rax;"              \
        "movq   (%%rax), %%rax;"            \
        "movq   %%rax, %0;"                 \
        : "=m"(var)                         \
        : "m"(rbp)                          \
        , "m"(off)                          \
        : "rax", "rbx"                      \
    )

#define GET_STACK_FLOAT(rbp, off, var)      \
    __asm__ __volatile__(                   \
        "movq   %1, %%rax;"                 \
        "movl   %2, %%ebx;"                 \
        "addq   %%rbx, %%rax;"              \
        "movl   (%%rax), %%eax;"            \
        "movl   %%eax, %0;"                 \
        : "=m"(var)                         \
        : "m"(rbp), "m"(off)                \
        : "rax", "rbx"                      \
    )

#define GET_STACK_DOUBLE(rbp, off, var)     \
    __asm__ __volatile__(                   \
        "movq   %1, %%rax;"                 \
        "movl   %2, %%ebx;"                 \
        "addq   %%rbx, %%rax;"              \
        "movq   (%%rax), %%rax;"            \
        "movq   %%rax, %0;"                 \
        : "=m"(var)                         \
        : "m"(rbp), "m"(off)                \
        : "rax", "rbx"                      \
    )

EXTERN_C void __entry();

__asm__(
    ".text;"
    ".align 8;"
    ".global __entry;"
    "__entry:"
    "pushq  %rbp;"
    "pushq  %rdi;"
    "subq   $8, %rsp;"
    "movl    $0xff, (%rsp);"
    "movq   %rdi, %rax;"
    "movq   %rsp, %rdi;"
    "call   *8(%rax);"
    "addq   $8, %rsp;"
    "popq   %rdi;"
    "popq   %rbp;"
    "ret;"
);

#endif

HANDLE PROXY_ENTRY = 0;

static constexpr Integer PROXY_ENTRY_SIZE = 32;
static constexpr Integer PROXY_ENTRY_SHIFT = 5;
static constexpr Integer PROXY_ENTRY_NUMBER = 240;
#if defined(__aarch64__)
static constexpr Integer PROXY_INDEX_OFFSET = 8;
#elif defined(__x86_64__)
static constexpr Integer PROXY_INDEX_OFFSET = 9;
#endif

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

#if defined(__aarch64__)
    Byte* p = (Byte*)PROXY_ENTRY;
    for (Integer i = 0; i < PROXY_ENTRY_NUMBER; i++) {
        memcpy(p, reinterpret_cast<void*>(&__entry), PROXY_ENTRY_SIZE);
        p[8] = i;
        p += PROXY_ENTRY_SIZE;
    }
#elif defined(__x86_64__)
    Byte* p = (Byte*)PROXY_ENTRY;
    for (Integer i = 0; i < PROXY_ENTRY_NUMBER; i++) {
        memcpy(p, reinterpret_cast<void*>(&__entry), PROXY_ENTRY_SIZE);
        p[PROXY_INDEX_OFFSET] = i;
        p += PROXY_ENTRY_SIZE;
    }
#endif

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
    return thisObj->mOwner->AddRef(id);
}

Integer InterfaceProxy::S_Release(
    /* [in] */ InterfaceProxy* thisObj,
    /* [in] */ HANDLE id)
{
    return thisObj->mOwner->Release(id);
}

IInterface* InterfaceProxy::S_Probe(
    /* [in] */ InterfaceProxy* thisObj,
    /* [in] */ const InterfaceID& iid)
{
    return thisObj->mOwner->Probe(iid);
}

ECode InterfaceProxy::S_GetInterfaceID(
    /* [in] */ InterfaceProxy* thisObj,
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID& iid)
{
    return thisObj->mOwner->GetInterfaceID(object, iid);
}

ECode InterfaceProxy::MarshalArguments(
    /* [in] */ Registers& regs,
    /* [in] */ IMetaMethod* method,
    /* [in] */ IParcel* argParcel)
{
    Integer N;
    method->GetParameterNumber(N);
    Integer intParamIndex = 1, fpParamIndex = 0;
    for (Integer i = 0; i < N; i++) {
        AutoPtr<IMetaParameter> param;
        method->GetParameter(i, param);
        AutoPtr<IMetaType> type;
        param->GetType(type);
        TypeKind kind;
        type->GetTypeKind(kind);
        IOAttribute ioAttr;
        param->GetIOAttribute(ioAttr);
        if (ioAttr == IOAttribute::IN) {
            switch (kind) {
                case TypeKind::Char: {
                    Char value = (Char)GetIntegerValue(regs, intParamIndex, fpParamIndex);
                    argParcel->WriteChar(value);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Byte: {
                    Byte value = (Byte)GetIntegerValue(regs, intParamIndex, fpParamIndex);
                    argParcel->WriteByte(value);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Short: {
                    Short value = (Short)GetIntegerValue(regs, intParamIndex, fpParamIndex);
                    argParcel->WriteShort(value);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Integer: {
                    Integer value = GetIntegerValue(regs, intParamIndex, fpParamIndex);
                    argParcel->WriteInteger(value);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Long: {
                    Long value = GetLongValue(regs, intParamIndex, fpParamIndex);
                    argParcel->WriteLong(value);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Float: {
                    Float value = GetFloatValue(regs, intParamIndex, fpParamIndex);
                    argParcel->WriteFloat(value);
                    fpParamIndex++;
                    break;
                }
                case TypeKind::Double: {
                    Double value = GetDoubleValue(regs, intParamIndex, fpParamIndex);
                    argParcel->WriteDouble(value);
                    fpParamIndex++;
                    break;
                }
                case TypeKind::Boolean: {
                    Boolean value = (Boolean)GetIntegerValue(regs, intParamIndex, fpParamIndex);
                    argParcel->WriteBoolean(value);
                    intParamIndex++;
                    break;
                }
                case TypeKind::String: {
                    String value = *reinterpret_cast<String*>(GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    argParcel->WriteString(value);
                    intParamIndex++;
                    break;
                }
                case TypeKind::ECode: {
                    ECode value = (ECode)GetIntegerValue(regs, intParamIndex, fpParamIndex);
                    argParcel->WriteECode(value);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Enum: {
                    Integer value = GetIntegerValue(regs, intParamIndex, fpParamIndex);
                    argParcel->WriteEnumeration(value);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Array: {
                    AutoPtr<IMetaType> arrType, elemType;
                    elemType = type;
                    TypeKind elemKind = kind;
                    while (elemKind == TypeKind::Array) {
                        arrType = elemType;
                        arrType->GetElementType(elemType);
                        elemType->GetTypeKind(elemKind);
                    }
                    if (elemKind == TypeKind::CoclassID ||
                            elemKind == TypeKind::ComponentID ||
                            elemKind == TypeKind::InterfaceID ||
                            elemKind == TypeKind::HANDLE) {
                        Logger::E("CProxy", "Invalid [in] Array(%d), param index: %d.\n", elemKind, i);
                        return E_ILLEGAL_ARGUMENT_EXCEPTION;
                    }

                    HANDLE value = GetHANDLEValue(regs, intParamIndex, fpParamIndex);
                    argParcel->WriteArray(*reinterpret_cast<Triple*>(value));
                    intParamIndex++;
                    break;
                }
                case TypeKind::Interface: {
                    IInterface* value = reinterpret_cast<IInterface*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    argParcel->WriteInterface(value);
                    intParamIndex++;
                    break;
                }
                case TypeKind::CoclassID:
                case TypeKind::ComponentID:
                case TypeKind::InterfaceID:
                case TypeKind::HANDLE:
                case TypeKind::Triple:
                default:
                    Logger::E("CProxy", "Invalid [in] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        else if (ioAttr == IOAttribute::IN_OUT) {
            switch (kind) {
                case TypeKind::Char: {
                    Char* addr = reinterpret_cast<Char*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    argParcel->WriteChar(*addr);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Byte: {
                    Byte* addr = reinterpret_cast<Byte*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    argParcel->WriteByte(*addr);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Short: {
                    Short* addr = reinterpret_cast<Short*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    argParcel->WriteShort(*addr);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Integer: {
                    Integer* addr = reinterpret_cast<Integer*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    argParcel->WriteInteger(*addr);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Long: {
                    Long* addr = reinterpret_cast<Long*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    argParcel->WriteLong(*addr);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Float: {
                    Float* addr = reinterpret_cast<Float*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    argParcel->WriteFloat(*addr);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Double: {
                    Double* addr = reinterpret_cast<Double*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    argParcel->WriteDouble(*addr);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Boolean: {
                    Boolean* addr = reinterpret_cast<Boolean*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    argParcel->WriteBoolean(*addr);
                    intParamIndex++;
                    break;
                }
                case TypeKind::String: {
                    String* addr = reinterpret_cast<String*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    argParcel->WriteString(*addr);
                    intParamIndex++;
                    break;
                }
                case TypeKind::ECode: {
                    ECode* addr = reinterpret_cast<ECode*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    argParcel->WriteECode(*addr);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Enum: {
                    Integer* addr = reinterpret_cast<Integer*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    argParcel->WriteInteger(*addr);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Array: {
                    AutoPtr<IMetaType> arrType, elemType;
                    elemType = type;
                    TypeKind elemKind = kind;
                    while (elemKind == TypeKind::Array) {
                        arrType = elemType;
                        arrType->GetElementType(elemType);
                        elemType->GetTypeKind(elemKind);
                    }
                    if (elemKind == TypeKind::CoclassID ||
                            elemKind == TypeKind::ComponentID ||
                            elemKind == TypeKind::InterfaceID ||
                            elemKind == TypeKind::HANDLE) {
                        Logger::E("CProxy", "Invalid [in, out] Array(%d), param index: %d.\n", elemKind, i);
                        return E_ILLEGAL_ARGUMENT_EXCEPTION;
                    }

                    HANDLE value = GetHANDLEValue(regs, intParamIndex, fpParamIndex);
                    argParcel->WriteArray(*reinterpret_cast<Triple*>(value));
                    intParamIndex++;
                    break;
                }
                case TypeKind::Interface: {
                    IInterface** value = reinterpret_cast<IInterface**>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    argParcel->WriteInterface(*value);
                    intParamIndex++;
                    break;
                }
                case TypeKind::CoclassID:
                case TypeKind::ComponentID:
                case TypeKind::InterfaceID:
                case TypeKind::HANDLE:
                case TypeKind::Triple:
                default:
                    Logger::E("CProxy", "Invalid [in, out] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        else if (ioAttr == IOAttribute::OUT) {
            switch (kind) {
                case TypeKind::Char:
                case TypeKind::Byte:
                case TypeKind::Short:
                case TypeKind::Integer:
                case TypeKind::Long:
                case TypeKind::Float:
                case TypeKind::Double:
                case TypeKind::Boolean:
                case TypeKind::String:
                case TypeKind::ECode:
                case TypeKind::Enum:
                case TypeKind::Interface:
                    intParamIndex++;
                    break;
                case TypeKind::Array: {
                    AutoPtr<IMetaType> arrType, elemType;
                    elemType = type;
                    TypeKind elemKind = kind;
                    while (elemKind == TypeKind::Array) {
                        arrType = elemType;
                        arrType->GetElementType(elemType);
                        elemType->GetTypeKind(elemKind);
                    }
                    if (elemKind == TypeKind::CoclassID ||
                            elemKind == TypeKind::ComponentID ||
                            elemKind == TypeKind::InterfaceID ||
                            elemKind == TypeKind::HANDLE) {
                        Logger::E("CProxy", "Invalid [out] Array(%d), param index: %d.\n", elemKind, i);
                        return E_ILLEGAL_ARGUMENT_EXCEPTION;
                    }
                    intParamIndex++;
                    break;
                }
                case TypeKind::CoclassID:
                case TypeKind::ComponentID:
                case TypeKind::InterfaceID:
                case TypeKind::HANDLE:
                case TypeKind::Triple:
                default:
                    Logger::E("CProxy", "Invalid [out] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        else if (ioAttr == IOAttribute::OUT_CALLEE) {
            switch (kind) {
                case TypeKind::Array: {
                    AutoPtr<IMetaType> arrType, elemType;
                    elemType = type;
                    TypeKind elemKind = kind;
                    while (elemKind == TypeKind::Array) {
                        arrType = elemType;
                        arrType->GetElementType(elemType);
                        elemType->GetTypeKind(elemKind);
                    }
                    if (elemKind == TypeKind::CoclassID ||
                            elemKind == TypeKind::ComponentID ||
                            elemKind == TypeKind::InterfaceID ||
                            elemKind == TypeKind::HANDLE) {
                        Logger::E("CProxy", "Invalid [out, callee] Array(%d), param index: %d.\n", elemKind, i);
                        return E_ILLEGAL_ARGUMENT_EXCEPTION;
                    }
                    intParamIndex++;
                    break;
                }
                case TypeKind::Char:
                case TypeKind::Byte:
                case TypeKind::Short:
                case TypeKind::Integer:
                case TypeKind::Long:
                case TypeKind::Float:
                case TypeKind::Double:
                case TypeKind::Boolean:
                case TypeKind::String:
                case TypeKind::ECode:
                case TypeKind::Enum:
                case TypeKind::Interface:
                case TypeKind::CoclassID:
                case TypeKind::ComponentID:
                case TypeKind::InterfaceID:
                case TypeKind::HANDLE:
                case TypeKind::Triple:
                default:
                    Logger::E("CProxy", "Invalid [out, callee] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
    }

    return NOERROR;
}

ECode InterfaceProxy::UnmarshalResults(
        /* [in] */ Registers& regs,
        /* [in] */ IMetaMethod* method,
        /* [in] */ IParcel* resParcel)
{
    Integer N;
    method->GetParameterNumber(N);
    Integer intParamIndex = 1, fpParamIndex = 0;
    for (Integer i = 0; i < N; i++) {
        AutoPtr<IMetaParameter> param;
        method->GetParameter(i, param);
        AutoPtr<IMetaType> type;
        param->GetType(type);
        TypeKind kind;
        type->GetTypeKind(kind);
        IOAttribute ioAttr;
        param->GetIOAttribute(ioAttr);
        if (ioAttr == IOAttribute::IN) {
            switch (kind) {
                case TypeKind::Char:
                case TypeKind::Byte:
                case TypeKind::Short:
                case TypeKind::Integer:
                case TypeKind::Long:
                case TypeKind::Boolean:
                case TypeKind::String:
                case TypeKind::ECode:
                case TypeKind::Enum:
                case TypeKind::Array:
                case TypeKind::Interface:
                    intParamIndex++;
                    break;
                case TypeKind::Float:
                case TypeKind::Double:
                    fpParamIndex++;
                    break;
                case TypeKind::CoclassID:
                case TypeKind::ComponentID:
                case TypeKind::InterfaceID:
                case TypeKind::HANDLE:
                case TypeKind::Triple:
                default:
                    Logger::E("CProxy", "Invalid [in] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        else if (ioAttr == IOAttribute::OUT || ioAttr == IOAttribute::IN_OUT) {
            switch (kind) {
                case TypeKind::Char: {
                    Char* addr = reinterpret_cast<Char*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    resParcel->ReadChar(*addr);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Byte: {
                    Byte* addr = reinterpret_cast<Byte*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    resParcel->ReadByte(*addr);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Short: {
                    Short* addr = reinterpret_cast<Short*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    resParcel->ReadShort(*addr);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Integer: {
                    Integer* addr = reinterpret_cast<Integer*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    resParcel->ReadInteger(*addr);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Long: {
                    Long* addr = reinterpret_cast<Long*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    resParcel->ReadLong(*addr);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Float: {
                    Float* addr = reinterpret_cast<Float*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    resParcel->ReadFloat(*addr);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Double: {
                    Double* addr = reinterpret_cast<Double*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    resParcel->ReadDouble(*addr);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Boolean: {
                    Boolean* addr = reinterpret_cast<Boolean*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    resParcel->ReadBoolean(*addr);
                    intParamIndex++;
                    break;
                }
                case TypeKind::String: {
                    String* addr = reinterpret_cast<String*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    resParcel->ReadString(*addr);
                    intParamIndex++;
                    break;
                }
                case TypeKind::ECode: {
                    ECode* addr = reinterpret_cast<ECode*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    resParcel->ReadECode(*addr);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Enum: {
                    Integer* addr = reinterpret_cast<Integer*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    resParcel->ReadEnumeration(*addr);
                    intParamIndex++;
                    break;
                }
                case TypeKind::Array: {
                    HANDLE addr = GetHANDLEValue(regs, intParamIndex, fpParamIndex);
                    resParcel->ReadArray(reinterpret_cast<Triple*>(addr));
                    intParamIndex++;
                    break;
                }
                case TypeKind::Interface: {
                    AutoPtr<IInterface>* intf = reinterpret_cast<AutoPtr<IInterface>*>(
                            GetHANDLEValue(regs, intParamIndex, fpParamIndex));
                    resParcel->ReadInterface(*intf);
                    intParamIndex++;
                    break;
                }
                case TypeKind::CoclassID:
                case TypeKind::ComponentID:
                case TypeKind::InterfaceID:
                case TypeKind::HANDLE:
                case TypeKind::Triple:
                default:
                    Logger::E("CProxy", "Invalid [in, out] or [out] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        else if (ioAttr == IOAttribute::OUT_CALLEE) {
            switch (kind) {
                case TypeKind::Array: {
                    HANDLE addr = GetHANDLEValue(regs, intParamIndex, fpParamIndex);
                    resParcel->ReadArray(reinterpret_cast<Triple*>(addr));
                    intParamIndex++;
                    break;
                }
                case TypeKind::Char:
                case TypeKind::Byte:
                case TypeKind::Short:
                case TypeKind::Integer:
                case TypeKind::Long:
                case TypeKind::Float:
                case TypeKind::Double:
                case TypeKind::Boolean:
                case TypeKind::String:
                case TypeKind::ECode:
                case TypeKind::Enum:
                case TypeKind::Interface:
                case TypeKind::CoclassID:
                case TypeKind::ComponentID:
                case TypeKind::InterfaceID:
                case TypeKind::Triple:
                default:
                    Logger::E("CProxy", "Invalid [out, callee] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
    }

    return NOERROR;
}

Integer InterfaceProxy::GetIntegerValue(
    /* [in] */ Registers& regs,
    /* [in] */ Integer intParamIndex,
    /* [in] */ Integer fpParamIndex)
{
    switch (intParamIndex) {
        case 0:
            return regs.rdi.iVal;
        case 1:
            return regs.rsi.iVal;
        case 2:
            return regs.rdx.iVal;
        case 3:
            return regs.rcx.iVal;
        case 4:
            return regs.r8.iVal;
        case 5:
            return regs.r9.iVal;
        default: {
            Integer value, offset;
            offset = fpParamIndex <= 7
                    ? intParamIndex - 6
                    : intParamIndex - 6 + fpParamIndex - 8;
            offset += regs.paramStartOffset;
            offset *= 8;
            GET_STACK_INTEGER(regs.rbp, offset, value);
            return value;
        }
    }
}

Long InterfaceProxy::GetLongValue(
    /* [in] */ Registers& regs,
    /* [in] */ Integer intParamIndex,
    /* [in] */ Integer fpParamIndex)
{
    switch (intParamIndex) {
        case 0:
            return regs.rdi.lVal;
        case 1:
            return regs.rsi.lVal;
        case 2:
            return regs.rdx.lVal;
        case 3:
            return regs.rcx.lVal;
        case 4:
            return regs.r8.lVal;
        case 5:
            return regs.r9.lVal;
        default: {
            Integer offset;
            offset = fpParamIndex <= 7
                    ? intParamIndex - 6
                    : intParamIndex - 6 + fpParamIndex - 8;
            offset += regs.paramStartOffset;
            offset *= 8;
            Long value;
            GET_STACK_LONG(regs.rbp, offset, value);
            return value;
        }
    }
}

Float InterfaceProxy::GetFloatValue(
    /* [in] */ Registers& regs,
    /* [in] */ Integer intParamIndex,
    /* [in] */ Integer fpParamIndex)
{
    switch (fpParamIndex) {
        case 0:
            return regs.xmm0.fVal;
        case 1:
            return regs.xmm1.fVal;
        case 2:
            return regs.xmm2.fVal;
        case 3:
            return regs.xmm3.fVal;
        case 4:
            return regs.xmm4.fVal;
        case 5:
            return regs.xmm5.fVal;
        case 6:
            return regs.xmm6.fVal;
        case 7:
            return regs.xmm7.fVal;
        default: {
            Integer offset = intParamIndex <= 5
                    ? fpParamIndex - 8
                    : fpParamIndex - 8 + intParamIndex - 6;
            offset += regs.paramStartOffset;
            offset *= 8;
            Float value;
            GET_STACK_FLOAT(regs.rbp, offset, value);
            return value;
        }
    }
}

Double InterfaceProxy::GetDoubleValue(
    /* [in] */ Registers& regs,
    /* [in] */ Integer intParamIndex,
    /* [in] */ Integer fpParamIndex)
{
    switch (fpParamIndex) {
        case 0:
            return regs.xmm0.dVal;
        case 1:
            return regs.xmm1.dVal;
        case 2:
            return regs.xmm2.dVal;
        case 3:
            return regs.xmm3.dVal;
        case 4:
            return regs.xmm4.dVal;
        case 5:
            return regs.xmm5.dVal;
        case 6:
            return regs.xmm6.dVal;
        case 7:
            return regs.xmm7.dVal;
        default: {
            Integer offset = intParamIndex <= 5
                    ? fpParamIndex - 8
                    : fpParamIndex - 8 + intParamIndex - 6;
            offset += regs.paramStartOffset;
            offset *= 8;
            Double value;
            GET_STACK_DOUBLE(regs.rbp, offset, value);
            return value;
        }
    }
}

HANDLE InterfaceProxy::GetHANDLEValue(
    /* [in] */ Registers& regs,
    /* [in] */ Integer intParamIndex,
    /* [in] */ Integer fpParamIndex)
{
    return (HANDLE)GetLongValue(regs, intParamIndex, fpParamIndex);
}

ECode InterfaceProxy::ProxyEntry(
    /* [in] */ HANDLE args)
{
    InterfaceProxy* thisObj;
    Integer methodIndex;
    Integer offset;

    offset = 0;
    GET_STACK_INTEGER(args, offset, methodIndex);

    offset = 8;
    GET_STACK_LONG(args, offset, thisObj);

    Registers regs;
    regs.rbp.reg = args + 16;
    regs.paramStartOffset = 2;
    GET_REG(rdi, regs.rdi.reg);
    GET_REG(rsi, regs.rsi.reg);
    GET_REG(rdx, regs.rdx.reg);
    GET_REG(rcx, regs.rcx.reg);
    GET_REG(r8, regs.r8.reg);
    GET_REG(r9, regs.r9.reg);

    GET_REG(xmm0, regs.xmm0.reg);
    GET_REG(xmm1, regs.xmm1.reg);
    GET_REG(xmm2, regs.xmm2.reg);
    GET_REG(xmm3, regs.xmm3.reg);
    GET_REG(xmm4, regs.xmm4.reg);
    GET_REG(xmm5, regs.xmm5.reg);
    GET_REG(xmm6, regs.xmm6.reg);
    GET_REG(xmm7, regs.xmm7.reg);

    if (DEBUG) {
        String name, ns;
        thisObj->mTargetMetadata->GetName(name);
        thisObj->mTargetMetadata->GetNamespace(ns);
        Logger::D("CProxy", "Call ProxyEntry with interface \"%s::%s\"",
                ns.string(), name.string());
    }

    AutoPtr<IMetaMethod> method;
    thisObj->mTargetMetadata->GetMethod(methodIndex + 4, method);

    if (DEBUG) {
        String name, signature;
        method->GetName(name);
        method->GetSignature(signature);
        Logger::D("CProxy", "Call ProxyEntry with method \"%s(%s)\"",
                name.string(), signature.string());
    }

    RPCType type;
    thisObj->mOwner->mChannel->GetRPCType(type);
    AutoPtr<IParcel> inParcel, outParcel;
    CoCreateParcel(type, inParcel);
    inParcel->WriteInteger(RPC_MAGIC_NUMBER);
    inParcel->WriteInteger(thisObj->mIndex);
    inParcel->WriteInteger(methodIndex + 4);
    ECode ec = thisObj->MarshalArguments(regs, method, inParcel);
    if (FAILED(ec)) {
        goto ProxyExit;
    }

    ec = thisObj->mOwner->mChannel->Invoke(method, inParcel, outParcel);
    if (FAILED(ec)) {
        goto ProxyExit;
    }

    ec = thisObj->UnmarshalResults(regs, method, outParcel);

ProxyExit:
    if (DEBUG) {
        Logger::D("CProxy", "Exit ProxyEntry with ec(0x%x)", ec);
    }

    return ec;
}

//----------------------------------------------------------------------

const CoclassID CID_CProxy =
        {{0x228c4e6a,0x1df5,0x4130,0xb46e,{0xd0,0x32,0x2b,0x67,0x69,0x76}}, &CID_COMORuntime};

COMO_OBJECT_IMPL(CProxy);

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
    else if (IID_IProxy == iid) {
        return (IProxy*)this;
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
    /* [out] */ InterfaceID& iid)
{
    if (object == (IObject*)this) {
        iid = IID_IObject;
        return NOERROR;
    }
    for (Integer i = 0; i < mInterfaces.GetLength(); i++) {
        InterfaceProxy* iproxy = mInterfaces[i];
        if ((IInterface*)iproxy == object) {
            iid = iproxy->mIid;
            return NOERROR;
        }
    }
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CProxy::GetTargetCoclass(
    /* [out] */ AutoPtr<IMetaCoclass>& target)
{
    target = mTargetMetadata;
    return NOERROR;
}

ECode CProxy::IsStubAlive(
    /* [out] */ Boolean& alive)
{
    return mChannel->IsPeerAlive(alive);
}

ECode CProxy::LinkToDeath(
    /* [in] */ IDeathRecipient* recipient,
    /* [in] */ HANDLE cookie,
    /* [in] */ Integer flags)
{
    return mChannel->LinkToDeath(this, recipient, cookie, flags);
}

ECode CProxy::UnlinkToDeath(
    /* [in] */ IDeathRecipient* recipient,
    /* [in] */ HANDLE cookie,
    /* [in] */ Integer flags,
    /* [out] */ AutoPtr<IDeathRecipient>* outRecipient)
{
    return mChannel->UnlinkToDeath(this, recipient, cookie, flags, outRecipient);
}

AutoPtr<IRPCChannel> CProxy::GetChannel()
{
    return mChannel;
}

CoclassID CProxy::GetTargetCoclassID()
{
    return mCid;
}

ECode CProxy::CreateObject(
    /* [in] */ const CoclassID& cid,
    /* [in] */ IRPCChannel* channel,
    /* [in] */ IClassLoader* loader,
    /* [out] */ AutoPtr<IProxy>& proxy)
{
    proxy = nullptr;

    if (loader == nullptr) {
        loader = CBootClassLoader::GetInstance();
    }

    AutoPtr<IMetaCoclass> mc;
    loader->LoadCoclass(cid, mc);
    if (mc == nullptr) {
        Array<Byte> metadata;
        channel->GetComponentMetadata(cid, metadata);
        AutoPtr<IMetaComponent> component;
        loader->LoadMetadata(metadata, component);
        loader->LoadCoclass(cid, mc);
        if (mc == nullptr) {
            Logger::E("CProxy", "Get IMetaCoclass failed.");
            return E_CLASS_NOT_FOUND_EXCEPTION;
        }
    }

    AutoPtr<CProxy> proxyObj = new CProxy();
    mc->GetCoclassID(proxyObj->mCid);
    proxyObj->mTargetMetadata = mc;
    proxyObj->mChannel = channel;

    Integer interfaceNumber;
    mc->GetInterfaceNumber(interfaceNumber);
    Array<IMetaInterface*> interfaces(interfaceNumber);
    mc->GetAllInterfaces(interfaces);
    proxyObj->mInterfaces = Array<InterfaceProxy*>(interfaceNumber);
    for (Integer i = 0; i < interfaceNumber; i++) {
        AutoPtr<InterfaceProxy> iproxy = new InterfaceProxy();
        iproxy->mIndex = i;
        iproxy->mOwner = proxyObj;
        iproxy->mTargetMetadata = interfaces[i];
        iproxy->mTargetMetadata->GetInterfaceID(iproxy->mIid);
        iproxy->mVtable = sProxyVtable;
        iproxy->mProxyEntry = reinterpret_cast<HANDLE>(&InterfaceProxy::ProxyEntry);
        proxyObj->mInterfaces[i] = iproxy;
    }

    proxy = proxyObj;
    return NOERROR;
}

} // namespace como
