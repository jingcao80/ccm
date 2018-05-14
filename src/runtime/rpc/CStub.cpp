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

#include "ccmrpc.h"
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

ECode InterfaceStub::UnmarshalArguments(
    /* [in] */ IMetaMethod* method,
    /* [in] */ IParcel* argParcel,
    /* [out] */ IArgumentList** argList)
{
    AutoPtr<IArgumentList> args;
    method->CreateArgumentList((IArgumentList**)&args);

    Integer N;
    method->GetParameterNumber(&N);
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
                    Char value;
                    argParcel->ReadChar(&value);
                    args->SetInputArgumentOfChar(i, value);
                    break;
                }
                case CcmTypeKind::Byte: {
                    Byte value;
                    argParcel->ReadByte(&value);
                    args->SetInputArgumentOfByte(i, value);
                    break;
                }
                case CcmTypeKind::Short: {
                    Short value;
                    argParcel->ReadShort(&value);
                    args->SetInputArgumentOfShort(i, value);
                    break;
                }
                case CcmTypeKind::Integer: {
                    Integer value;
                    argParcel->ReadInteger(&value);
                    args->SetInputArgumentOfInteger(i, value);
                    break;
                }
                case CcmTypeKind::Long: {
                    Long value;
                    argParcel->ReadLong(&value);
                    args->SetInputArgumentOfLong(i, value);
                    break;
                }
                case CcmTypeKind::Float: {
                    Float value;
                    argParcel->ReadFloat(&value);
                    args->SetInputArgumentOfFloat(i, value);
                    break;
                }
                case CcmTypeKind::Double: {
                    Double value;
                    argParcel->ReadDouble(&value);
                    args->SetInputArgumentOfDouble(i, value);
                    break;
                }
                case CcmTypeKind::Boolean: {
                    Boolean value;
                    argParcel->ReadBoolean(&value);
                    args->SetInputArgumentOfBoolean(i, value);
                    break;
                }
                case CcmTypeKind::String: {
                    String* value = new String();
                    argParcel->ReadString(value);
                    args->SetInputArgumentOfString(i, *value);
                    break;
                }
                case CcmTypeKind::ECode: {
                    ECode value;
                    argParcel->ReadECode(&value);
                    args->SetInputArgumentOfECode(i, value);
                    break;
                }
                case CcmTypeKind::Enum: {
                    Integer value;
                    argParcel->ReadEnumeration(&value);
                    args->SetInputArgumentOfEnumeration(i, value);
                    break;
                }
                case CcmTypeKind::Array: {
                    Triple* t = new Triple();
                    argParcel->ReadArray(reinterpret_cast<HANDLE>(t));
                    args->SetInputArgumentOfArray(i, reinterpret_cast<HANDLE>(t));
                    break;
                }
                case CcmTypeKind::Interface: {
                    AutoPtr<IInterface> value;
                    argParcel->ReadInterface((IInterface**)&value);
                    args->SetInputArgumentOfInterface(i, value);
                    REFCOUNT_ADD(value);
                    break;
                }
                case CcmTypeKind::CoclassID:
                case CcmTypeKind::ComponentID:
                case CcmTypeKind::InterfaceID:
                case CcmTypeKind::HANDLE:
                default:
                    Logger::E("CStub", "Invalid [in] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        else if (ioAttr == IOAttribute::IN_OUT || ioAttr == IOAttribute::OUT) {
            switch (kind) {
                case CcmTypeKind::Char: {
                    Char* value = new Char;
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadChar(value);
                    }
                    args->SetOutputArgumentOfChar(i, reinterpret_cast<HANDLE>(value));
                    break;
                }
                case CcmTypeKind::Byte: {
                    Byte* value = new Byte;
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadByte(value);
                    }
                    args->SetOutputArgumentOfByte(i, reinterpret_cast<HANDLE>(value));
                }
                case CcmTypeKind::Short: {
                    Short* value = new Short;
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadShort(value);
                    }
                    args->SetOutputArgumentOfShort(i, reinterpret_cast<HANDLE>(value));
                }
                case CcmTypeKind::Integer: {
                    Integer* value = new Integer;
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadInteger(value);
                    }
                    args->SetOutputArgumentOfInteger(i, reinterpret_cast<HANDLE>(value));
                }
                case CcmTypeKind::Long: {
                    Long* value = new Long;
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadLong(value);
                    }
                    args->SetOutputArgumentOfLong(i, reinterpret_cast<HANDLE>(value));
                }
                case CcmTypeKind::Float: {
                    Float* value = new Float;
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadFloat(value);
                    }
                    args->SetOutputArgumentOfFloat(i, reinterpret_cast<HANDLE>(value));
                }
                case CcmTypeKind::Double: {
                    Double* value = new Double;
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadDouble(value);
                    }
                    args->SetOutputArgumentOfDouble(i, reinterpret_cast<HANDLE>(value));
                }
                case CcmTypeKind::Boolean: {
                    Boolean* value = new Boolean;
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadBoolean(value);
                    }
                    args->SetOutputArgumentOfBoolean(i, reinterpret_cast<HANDLE>(value));
                }
                case CcmTypeKind::String: {
                    String* value = new String();
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadString(value);
                    }
                    args->SetOutputArgumentOfString(i, reinterpret_cast<HANDLE>(value));
                }
                case CcmTypeKind::ECode: {
                    ECode* value = new ECode;
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadECode(value);
                    }
                    args->SetOutputArgumentOfECode(i, reinterpret_cast<HANDLE>(value));
                }
                case CcmTypeKind::Enum: {
                    Integer* value = new Integer;
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadEnumeration(value);
                    }
                    args->SetOutputArgumentOfEnumeration(i, reinterpret_cast<HANDLE>(value));
                }
                case CcmTypeKind::Array: {
                    Triple* t = new Triple();
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadArray(reinterpret_cast<HANDLE>(t));
                    }
                    args->SetOutputArgumentOfArray(i, reinterpret_cast<HANDLE>(t));
                }
                case CcmTypeKind::Interface: {
                    IInterface** intf = new IInterface*;
                    if (ioAttr == IOAttribute::IN_OUT) {
                        argParcel->ReadInterface(intf);
                    }
                    args->SetOutputArgumentOfInterface(i, reinterpret_cast<HANDLE>(intf));
                }
                case CcmTypeKind::CoclassID:
                case CcmTypeKind::ComponentID:
                case CcmTypeKind::InterfaceID:
                case CcmTypeKind::HANDLE:
                default:
                    Logger::E("CStub", "Invalid [in, out] or [out] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        else if (ioAttr == IOAttribute::OUT_CALLEE) {
            switch (kind) {
                case CcmTypeKind::Array: {
                    Triple* t = new Triple();
                    args->SetOutputArgumentOfArray(i, reinterpret_cast<HANDLE>(t));
                    break;
                }
                case CcmTypeKind::Char:
                case CcmTypeKind::Byte:
                case CcmTypeKind::Short:
                case CcmTypeKind::Integer:
                case CcmTypeKind::Long:
                case CcmTypeKind::Float:
                case CcmTypeKind::Double:
                case CcmTypeKind::Boolean:
                case CcmTypeKind::String:
                case CcmTypeKind::ECode:
                case CcmTypeKind::Enum:
                case CcmTypeKind::Interface:
                case CcmTypeKind::CoclassID:
                case CcmTypeKind::ComponentID:
                case CcmTypeKind::InterfaceID:
                default:
                    Logger::E("CStub", "Invalid [out, callee] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
    }

    *argList = args;
    REFCOUNT_ADD(*argList);
    return NOERROR;
}

ECode InterfaceStub::MarshalResults(
    /* [in] */ IMetaMethod* method,
    /* [in] */ ECode ec,
    /* [in] */ IArgumentList* argList,
    /* [out] */ IParcel** resParcel)
{
    AutoPtr<IParcel> outParcel;
    mOwner->mChannel->CreateParcel((IParcel**)&outParcel);

    outParcel->WriteECode(ec);
    Integer N;
    method->GetParameterNumber(&N);
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
                case CcmTypeKind::Char:
                case CcmTypeKind::Byte:
                case CcmTypeKind::Short:
                case CcmTypeKind::Integer:
                case CcmTypeKind::Long:
                case CcmTypeKind::Float:
                case CcmTypeKind::Double:
                case CcmTypeKind::Boolean:
                case CcmTypeKind::ECode:
                case CcmTypeKind::Enum:
                    break;
                case CcmTypeKind::String: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, &addr);
                    String* value = reinterpret_cast<String*>(addr);
                    delete value;
                    break;
                }
                case CcmTypeKind::Array: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, &addr);
                    Triple* t = reinterpret_cast<Triple*>(addr);
                    t->FreeData();
                    delete t;
                    break;
                }
                case CcmTypeKind::Interface: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, &addr);
                    IInterface* intf = reinterpret_cast<IInterface*>(addr);
                    REFCOUNT_RELEASE(intf);
                    break;
                }
                case CcmTypeKind::CoclassID:
                case CcmTypeKind::ComponentID:
                case CcmTypeKind::InterfaceID:
                case CcmTypeKind::HANDLE:
                default:
                    Logger::E("CStub", "Invalid [in] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        else if (ioAttr == IOAttribute::OUT || ioAttr == IOAttribute::IN_OUT) {
            switch (kind) {
                case CcmTypeKind::Char: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, &addr);
                    Char* value = reinterpret_cast<Char*>(addr);
                    outParcel->WriteChar(*value);
                    delete value;
                    break;
                }
                case CcmTypeKind::Byte: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, &addr);
                    Byte* value = reinterpret_cast<Byte*>(addr);
                    outParcel->WriteByte(*value);
                    delete value;
                    break;
                }
                case CcmTypeKind::Short: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, &addr);
                    Short* value = reinterpret_cast<Short*>(addr);
                    outParcel->WriteShort(*value);
                    delete value;
                    break;
                }
                case CcmTypeKind::Integer: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, &addr);
                    Integer* value = reinterpret_cast<Integer*>(addr);
                    outParcel->WriteInteger(*value);
                    delete value;
                    break;
                }
                case CcmTypeKind::Long: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, &addr);
                    Long* value = reinterpret_cast<Long*>(addr);
                    outParcel->WriteLong(*value);
                    delete value;
                    break;
                }
                case CcmTypeKind::Float: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, &addr);
                    Float* value = reinterpret_cast<Float*>(addr);
                    outParcel->WriteFloat(*value);
                    delete value;
                    break;
                }
                case CcmTypeKind::Double: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, &addr);
                    Double* value = reinterpret_cast<Double*>(addr);
                    outParcel->WriteDouble(*value);
                    delete value;
                    break;
                }
                case CcmTypeKind::Boolean: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, &addr);
                    Boolean* value = reinterpret_cast<Boolean*>(addr);
                    outParcel->WriteBoolean(*value);
                    delete value;
                    break;
                }
                case CcmTypeKind::String: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, &addr);
                    String* value = reinterpret_cast<String*>(addr);
                    outParcel->WriteString(*value);
                    delete value;
                    break;
                }
                case CcmTypeKind::ECode: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, &addr);
                    ECode* value = reinterpret_cast<ECode*>(addr);
                    outParcel->WriteECode(*value);
                    delete value;
                    break;
                }
                case CcmTypeKind::Enum: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, &addr);
                    Integer* value = reinterpret_cast<Integer*>(addr);
                    outParcel->WriteEnumeration(*value);
                    delete value;
                    break;
                }
                case CcmTypeKind::Array: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, &addr);
                    Triple* t = reinterpret_cast<Triple*>(addr);
                    outParcel->WriteArray(reinterpret_cast<HANDLE>(t));
                    delete t;
                    break;
                }
                case CcmTypeKind::Interface: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, &addr);
                    IInterface** intf = reinterpret_cast<IInterface**>(addr);
                    REFCOUNT_RELEASE(*intf);
                    delete intf;
                    break;
                }
                case CcmTypeKind::CoclassID:
                case CcmTypeKind::ComponentID:
                case CcmTypeKind::InterfaceID:
                case CcmTypeKind::HANDLE:
                default:
                    Logger::E("CStub", "Invalid [in, out] or [out] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        else if (ioAttr == IOAttribute::OUT_CALLEE) {
            switch (kind) {
                case CcmTypeKind::Array: {
                    HANDLE addr;
                    argList->GetArgumentAddress(i, &addr);
                    Triple* t = reinterpret_cast<Triple*>(addr);
                    outParcel->WriteArray(reinterpret_cast<HANDLE>(t));
                    delete t;
                    break;
                }
                case CcmTypeKind::Char:
                case CcmTypeKind::Byte:
                case CcmTypeKind::Short:
                case CcmTypeKind::Integer:
                case CcmTypeKind::Long:
                case CcmTypeKind::Float:
                case CcmTypeKind::Double:
                case CcmTypeKind::Boolean:
                case CcmTypeKind::String:
                case CcmTypeKind::ECode:
                case CcmTypeKind::Enum:
                case CcmTypeKind::Interface:
                case CcmTypeKind::CoclassID:
                case CcmTypeKind::ComponentID:
                case CcmTypeKind::InterfaceID:
                default:
                    Logger::E("CStub", "Invalid [out, callee] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
    }

    *resParcel = outParcel;
    REFCOUNT_ADD(*resParcel);
    return NOERROR;
}

ECode InterfaceStub::Invoke(
    /* [in] */ IParcel* argParcel,
    /* [out] */ IParcel** resParcel)
{
    Integer methodIndex, methodNum;
    argParcel->ReadInteger(&methodIndex);
    mTargetMetadata->GetMethodNumber(&methodNum);
    if (methodIndex < 0 || methodIndex >= methodNum) {
        Logger::E("CStub", "MethodIndex %d is invalid.", methodIndex);
        return E_RUNTIME_EXCEPTION;
    }
    AutoPtr<IMetaMethod> mm;
    mTargetMetadata->GetMethod(methodIndex, (IMetaMethod**)&mm);
    AutoPtr<IArgumentList> argList;
    ECode ec = UnmarshalArguments(mm, argParcel, (IArgumentList**)&argList);
    if (FAILED(ec)) {
        Logger::E("CStub", "UnmarshalArguments failed with ec is 0x%x.", ec);
        return ec;
    }

    ECode ret = mm->Invoke(mObject, argList);
    ec = MarshalResults(mm, ret, argList, resParcel);
    if (FAILED(ec)) {
        Logger::E("CStub", "MarshalResults failed with ec is 0x%x.", ec);
    }

    return ret;
}

//----------------------------------------------------------------------

const CoclassID CID_CStub =
        {{0x52068014,0xe347,0x453f,0x87a9,{0x0,0xb,0xe,0xc,0xf,0xb,0x6,0x9,0xd,0x8,0xe,0xd}}, &CID_CCMRuntime};

CCM_OBJECT_IMPL(CStub);

Integer CStub::AddRef(
    /* [in] */ HANDLE id)
{
    return Object::AddRef(id);
}

Integer CStub::Release(
    /* [in] */ HANDLE id)
{
    Integer ref = Object::Release(id);
    if (ref == 1) {
        ECode ec = UnregisterExportObject(RPCType::Local, mTarget);
        if (FAILED(ec)) {
            Logger::E("CStub", "Unregister export object failed with ec is 0x%x.", ec);
        }
    }
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
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid);

    if (object == (IInterface*)(IStub*)this) {
        *iid = IID_IStub;
        return NOERROR;
    }
    return Object::GetInterfaceID(object, iid);
}

ECode CStub::Match(
    /* [in] */ IInterfacePack* ipack,
    /* [out] */ Boolean* matched)
{
    return mChannel->Match(ipack, matched);
}

ECode CStub::Invoke(
    /* [in] */ IParcel* argParcel,
    /* [out] */ IParcel** resParcel)
{
    VALIDATE_NOT_NULL(resParcel);

    Integer magic;
    argParcel->ReadInteger(&magic);
    if (magic != RPC_MAGIC_NUMBER) {
        Logger::E("CStub", "Magic number 0x%x is invalid.", magic);
        return E_RUNTIME_EXCEPTION;
    }

    Integer interfaceIndex, methodIndex;
    argParcel->ReadInteger(&interfaceIndex);
    if (interfaceIndex < 0 || interfaceIndex >= mInterfaces.GetLength()) {
        Logger::E("CStub", "InterfaceIndex %d is invalid.", interfaceIndex);
        return E_RUNTIME_EXCEPTION;
    }

    return mInterfaces[interfaceIndex]->Invoke(argParcel, resParcel);
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
        istub->mOwner = stubObj;
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

    ec = channel->StartListening(stubObj);
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
