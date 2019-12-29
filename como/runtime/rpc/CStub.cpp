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

#include "comorpc.h"
#include "CStub.h"
#include "registry.h"

namespace como {

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

ECode InterfaceStub::UnmarshalArguments(
    /* [in] */ IMetaMethod* method,
    /* [in] */ IParcel* argParcel,
    /* [out] */ AutoPtr<IArgumentList>& argList)
{
    method->CreateArgumentList(argList);

    Integer N;
    method->GetParameterNumber(N);
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
                    Char value;
                    argParcel->ReadChar(value);
                    argList->SetInputArgumentOfChar(i, value);
                    break;
                }
                case TypeKind::Byte: {
                    Byte value;
                    argParcel->ReadByte(value);
                    argList->SetInputArgumentOfByte(i, value);
                    break;
                }
                case TypeKind::Short: {
                    Short value;
                    argParcel->ReadShort(value);
                    argList->SetInputArgumentOfShort(i, value);
                    break;
                }
                case TypeKind::Integer: {
                    Integer value;
                    argParcel->ReadInteger(value);
                    argList->SetInputArgumentOfInteger(i, value);
                    break;
                }
                case TypeKind::Long: {
                    Long value;
                    argParcel->ReadLong(value);
                    argList->SetInputArgumentOfLong(i, value);
                    break;
                }
                case TypeKind::Float: {
                    Float value;
                    argParcel->ReadFloat(value);
                    argList->SetInputArgumentOfFloat(i, value);
                    break;
                }
                case TypeKind::Double: {
                    Double value;
                    argParcel->ReadDouble(value);
                    argList->SetInputArgumentOfDouble(i, value);
                    break;
                }
                case TypeKind::Boolean: {
                    Boolean value;
                    argParcel->ReadBoolean(value);
                    argList->SetInputArgumentOfBoolean(i, value);
                    break;
                }
                case TypeKind::String: {
                    String* value = new String();
                    argParcel->ReadString(*value);
                    argList->SetInputArgumentOfString(i, *value);
                    break;
                }
                case TypeKind::ECode: {
                    ECode value;
                    argParcel->ReadECode(value);
                    argList->SetInputArgumentOfECode(i, value);
                    break;
                }
                case TypeKind::Enum: {
                    Integer value;
                    argParcel->ReadEnumeration(value);
                    argList->SetInputArgumentOfEnumeration(i, value);
                    break;
                }
                case TypeKind::Array: {
                    Triple* t = new Triple();
                    argParcel->ReadArray(t);
                    argList->SetInputArgumentOfArray(i, *t);
                    break;
                }
                case TypeKind::Interface: {
                    AutoPtr<IInterface> value;
                    argParcel->ReadInterface(value);
                    argList->SetInputArgumentOfInterface(i, value);
                    REFCOUNT_ADD(value);
                    break;
                }
                case TypeKind::CoclassID:
                case TypeKind::ComponentID:
                case TypeKind::InterfaceID:
                case TypeKind::HANDLE:
                case TypeKind::Triple:
                default:
                    Logger::E("CStub", "Invalid [in] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        else if (ioAttr == IOAttribute::IN_OUT || ioAttr == IOAttribute::OUT) {
            switch (kind) {
                case TypeKind::Char: {
                    Char* value = new Char;
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadChar(*value);
                    }
                    argList->SetOutputArgumentOfChar(i, reinterpret_cast<HANDLE>(value));
                    break;
                }
                case TypeKind::Byte: {
                    Byte* value = new Byte;
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadByte(*value);
                    }
                    argList->SetOutputArgumentOfByte(i, reinterpret_cast<HANDLE>(value));
                    break;
                }
                case TypeKind::Short: {
                    Short* value = new Short;
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadShort(*value);
                    }
                    argList->SetOutputArgumentOfShort(i, reinterpret_cast<HANDLE>(value));
                    break;
                }
                case TypeKind::Integer: {
                    Integer* value = new Integer;
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadInteger(*value);
                    }
                    argList->SetOutputArgumentOfInteger(i, reinterpret_cast<HANDLE>(value));
                    break;
                }
                case TypeKind::Long: {
                    Long* value = new Long;
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadLong(*value);
                    }
                    argList->SetOutputArgumentOfLong(i, reinterpret_cast<HANDLE>(value));
                    break;
                }
                case TypeKind::Float: {
                    Float* value = new Float;
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadFloat(*value);
                    }
                    argList->SetOutputArgumentOfFloat(i, reinterpret_cast<HANDLE>(value));
                    break;
                }
                case TypeKind::Double: {
                    Double* value = new Double;
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadDouble(*value);
                    }
                    argList->SetOutputArgumentOfDouble(i, reinterpret_cast<HANDLE>(value));
                    break;
                }
                case TypeKind::Boolean: {
                    Boolean* value = new Boolean;
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadBoolean(*value);
                    }
                    argList->SetOutputArgumentOfBoolean(i, reinterpret_cast<HANDLE>(value));
                    break;
                }
                case TypeKind::String: {
                    String* value = new String();
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadString(*value);
                    }
                    argList->SetOutputArgumentOfString(i, reinterpret_cast<HANDLE>(value));
                    break;
                }
                case TypeKind::ECode: {
                    ECode* value = new ECode;
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadECode(*value);
                    }
                    argList->SetOutputArgumentOfECode(i, reinterpret_cast<HANDLE>(value));
                    break;
                }
                case TypeKind::Enum: {
                    Integer* value = new Integer;
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadEnumeration(*value);
                    }
                    argList->SetOutputArgumentOfEnumeration(i, reinterpret_cast<HANDLE>(value));
                    break;
                }
                case TypeKind::Array: {
                    Triple* t = new Triple();
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadArray(t);
                    }
                    argList->SetOutputArgumentOfArray(i, reinterpret_cast<HANDLE>(t));
                    break;
                }
                case TypeKind::Interface: {
                    IInterface** intf = new IInterface*;
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadInterface(*reinterpret_cast<AutoPtr<IInterface>*>(intf));
                    }
                    argList->SetOutputArgumentOfInterface(i, reinterpret_cast<HANDLE>(intf));
                    break;
                }
                case TypeKind::CoclassID:
                case TypeKind::ComponentID:
                case TypeKind::InterfaceID:
                case TypeKind::HANDLE:
                case TypeKind::Triple:
                default:
                    Logger::E("CStub", "Invalid [in, out] or [out] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        else if (ioAttr == IOAttribute::OUT_CALLEE) {
            switch (kind) {
                case TypeKind::Array: {
                    Triple* t = new Triple();
                    argList->SetOutputArgumentOfArray(i, reinterpret_cast<HANDLE>(t));
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
                    Logger::E("CStub", "Invalid [out, callee] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
    }

    return NOERROR;
}

ECode InterfaceStub::MarshalResults(
    /* [in] */ IMetaMethod* method,
    /* [in] */ IArgumentList* argList,
    /* [out] */ AutoPtr<IParcel>& resParcel)
{
    RPCType type;
    mOwner->mChannel->GetRPCType(type);
    CoCreateParcel(type, resParcel);

    Integer N;
    method->GetParameterNumber(N);
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
                case TypeKind::Float:
                case TypeKind::Double:
                case TypeKind::Boolean:
                case TypeKind::ECode:
                case TypeKind::Enum:
                    break;
                case TypeKind::String: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, addr);
                    String* value = reinterpret_cast<String*>(addr);
                    delete value;
                    break;
                }
                case TypeKind::Array: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, addr);
                    Triple* t = reinterpret_cast<Triple*>(addr);
                    t->FreeData();
                    delete t;
                    break;
                }
                case TypeKind::Interface: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, addr);
                    IInterface* intf = reinterpret_cast<IInterface*>(addr);
                    REFCOUNT_RELEASE(intf);
                    break;
                }
                case TypeKind::CoclassID:
                case TypeKind::ComponentID:
                case TypeKind::InterfaceID:
                case TypeKind::HANDLE:
                case TypeKind::Triple:
                default:
                    Logger::E("CStub", "Invalid [in] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        else if (ioAttr == IOAttribute::OUT || ioAttr == IOAttribute::IN_OUT) {
            switch (kind) {
                case TypeKind::Char: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, addr);
                    Char* value = reinterpret_cast<Char*>(addr);
                    resParcel->WriteChar(*value);
                    delete value;
                    break;
                }
                case TypeKind::Byte: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, addr);
                    Byte* value = reinterpret_cast<Byte*>(addr);
                    resParcel->WriteByte(*value);
                    delete value;
                    break;
                }
                case TypeKind::Short: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, addr);
                    Short* value = reinterpret_cast<Short*>(addr);
                    resParcel->WriteShort(*value);
                    delete value;
                    break;
                }
                case TypeKind::Integer: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, addr);
                    Integer* value = reinterpret_cast<Integer*>(addr);
                    resParcel->WriteInteger(*value);
                    delete value;
                    break;
                }
                case TypeKind::Long: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, addr);
                    Long* value = reinterpret_cast<Long*>(addr);
                    resParcel->WriteLong(*value);
                    delete value;
                    break;
                }
                case TypeKind::Float: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, addr);
                    Float* value = reinterpret_cast<Float*>(addr);
                    resParcel->WriteFloat(*value);
                    delete value;
                    break;
                }
                case TypeKind::Double: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, addr);
                    Double* value = reinterpret_cast<Double*>(addr);
                    resParcel->WriteDouble(*value);
                    delete value;
                    break;
                }
                case TypeKind::Boolean: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, addr);
                    Boolean* value = reinterpret_cast<Boolean*>(addr);
                    resParcel->WriteBoolean(*value);
                    delete value;
                    break;
                }
                case TypeKind::String: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, addr);
                    String* value = reinterpret_cast<String*>(addr);
                    resParcel->WriteString(*value);
                    delete value;
                    break;
                }
                case TypeKind::ECode: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, addr);
                    ECode* value = reinterpret_cast<ECode*>(addr);
                    resParcel->WriteECode(*value);
                    delete value;
                    break;
                }
                case TypeKind::Enum: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, addr);
                    Integer* value = reinterpret_cast<Integer*>(addr);
                    resParcel->WriteEnumeration(*value);
                    delete value;
                    break;
                }
                case TypeKind::Array: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, addr);
                    Triple* t = reinterpret_cast<Triple*>(addr);
                    resParcel->WriteArray(*t);
                    delete t;
                    break;
                }
                case TypeKind::Interface: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, addr);
                    IInterface** intf = reinterpret_cast<IInterface**>(addr);
                    resParcel->WriteInterface(*intf);
                    REFCOUNT_RELEASE(*intf);
                    delete intf;
                    break;
                }
                case TypeKind::CoclassID:
                case TypeKind::ComponentID:
                case TypeKind::InterfaceID:
                case TypeKind::HANDLE:
                case TypeKind::Triple:
                default:
                    Logger::E("CStub", "Invalid [in, out] or [out] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        else if (ioAttr == IOAttribute::OUT_CALLEE) {
            switch (kind) {
                case TypeKind::Array: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, addr);
                    Triple* t = reinterpret_cast<Triple*>(addr);
                    resParcel->WriteArray(*t);
                    delete t;
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
                    Logger::E("CStub", "Invalid [out, callee] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
    }

    return NOERROR;
}

ECode InterfaceStub::Invoke(
    /* [in] */ IParcel* argParcel,
    /* [out] */ AutoPtr<IParcel>& resParcel)
{
    Integer methodIndex, methodNumber;
    argParcel->ReadInteger(methodIndex);
    mTargetMetadata->GetMethodNumber(methodNumber);
    if (methodIndex < 0 || methodIndex >= methodNumber) {
        Logger::E("CStub", "MethodIndex %d is invalid.", methodIndex);
        return E_RUNTIME_EXCEPTION;
    }
    AutoPtr<IMetaMethod> mm;
    mTargetMetadata->GetMethod(methodIndex, mm);
    AutoPtr<IArgumentList> argList;
    ECode ec = UnmarshalArguments(mm, argParcel, argList);
    if (FAILED(ec)) {
        Logger::E("CStub", "UnmarshalArguments failed with ec is 0x%x.", ec);
        return ec;
    }

    ECode ret = mm->Invoke(mObject, argList);
    ec = MarshalResults(mm, argList, resParcel);
    if (FAILED(ec)) {
        Logger::E("CStub", "MarshalResults failed with ec is 0x%x.", ec);
    }

    return ret;
}

//----------------------------------------------------------------------

const CoclassID CID_CStub =
        {{0x52068014,0xe347,0x453f,0x87a9,{0x0b,0xec,0xfb,0x69,0xd8,0xed}}, &CID_COMORuntime};

COMO_OBJECT_IMPL(CStub);

Integer CStub::AddRef(
    /* [in] */ HANDLE id)
{
    return Object::AddRef(id);
}

Integer CStub::Release(
    /* [in] */ HANDLE id)
{
    return Object::Release(id);
}

IInterface* CStub::Probe(
    /* [in] */ const InterfaceID& iid)
{
    if (iid == IID_IInterface) {
        return (IInterface*)(IStub*)this;
    }
    else if (iid == IID_IStub) {
        return (IStub*)this;
    }
    return Object::Probe(iid);
}

ECode CStub::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID& iid)
{
    if (object == (IInterface*)(IStub*)this) {
        iid = IID_IStub;
        return NOERROR;
    }
    return Object::GetInterfaceID(object, iid);
}

void CStub::OnLastStrongRef(
    /* [in] */ const void* id)
{
    UnregisterExportObject(RPCType::Local, mTarget);
    Object::OnLastStrongRef(id);
}

ECode CStub::Match(
    /* [in] */ IInterfacePack* ipack,
    /* [out] */ Boolean& matched)
{
    return mChannel->Match(ipack, matched);
}

ECode CStub::Invoke(
    /* [in] */ IParcel* argParcel,
    /* [out] */ AutoPtr<IParcel>& resParcel)
{
    Integer magic;
    argParcel->ReadInteger(magic);
    if (magic != RPC_MAGIC_NUMBER) {
        Logger::E("CStub", "Magic number 0x%x is invalid.", magic);
        return E_RUNTIME_EXCEPTION;
    }

    Integer interfaceIndex, methodIndex;
    argParcel->ReadInteger(interfaceIndex);
    if (interfaceIndex < 0 || interfaceIndex >= mInterfaces.GetLength()) {
        Logger::E("CStub", "InterfaceIndex %d is invalid.", interfaceIndex);
        return E_RUNTIME_EXCEPTION;
    }

    return mInterfaces[interfaceIndex]->Invoke(argParcel, resParcel);
}

AutoPtr<IObject> CStub::GetTarget()
{
    return mTarget;
}

AutoPtr<IRPCChannel> CStub::GetChannel()
{
    return mChannel;
}

CoclassID CStub::GetTargetCoclassID()
{
    return mCid;
}

ECode CStub::CreateObject(
    /* [in] */ IInterface* object,
    /* [in] */ IRPCChannel* channel,
    /* [out] */ AutoPtr<IStub>& stub)
{
    stub = nullptr;

    IObject* obj = IObject::Probe(object);
    if (obj == nullptr) {
        Logger::E("CStub", "Object does not have \"IObject\" interface.");
        return E_INTERFACE_NOT_FOUND_EXCEPTION;
    }

    AutoPtr<IMetaCoclass> mc;
    obj->GetCoclass(mc);
    if (mc == nullptr) {
        Logger::E("CStub", "Fail to get object's Coclass.");
        return E_NOT_FOUND_EXCEPTION;
    }

    CoclassID cid;
    mc->GetCoclassID(cid);

    if (DEBUG) {
        Logger::D("CStub", "Object's CoclassID is %s",
                DumpUUID(cid.mUuid).string());
    }

    AutoPtr<CStub> stubObj = new CStub();
    stubObj->mTarget = obj;
    stubObj->mCid = cid;
    stubObj->mTargetMetadata = mc;
    stubObj->mChannel = channel;

    Integer interfaceNumber;
    mc->GetInterfaceNumber(interfaceNumber);
    Array<IMetaInterface*> interfaces(interfaceNumber);
    mc->GetAllInterfaces(interfaces);
    stubObj->mInterfaces = Array<InterfaceStub*>(interfaceNumber);
    for (Integer i = 0; i < interfaceNumber; i++) {
        AutoPtr<InterfaceStub> istub = new InterfaceStub();
        istub->mOwner = stubObj;
        istub->mTargetMetadata = interfaces[i];
        istub->mTargetMetadata->GetInterfaceID(istub->mIid);
        istub->mObject = object->Probe(istub->mIid);
        if (istub->mObject == nullptr) {
            String name, ns;
            interfaces[i]->GetNamespace(ns);
            interfaces[i]->GetName(name);
            Logger::E("CStub", "Object does not have \"%s::%s\" interface.",
                    ns.string(), name.string());
            return E_INTERFACE_NOT_FOUND_EXCEPTION;
        }
        stubObj->mInterfaces[i] = istub;
    }

    ECode ec = channel->StartListening(stubObj);
    if (FAILED(ec)) {
        Logger::E("CStub", "Channel start listening failed with ec is 0x%x", ec);
        return ec;
    }

    stub = stubObj;
    return NOERROR;
}

} // namespace como
