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

#include "rpc/binder/CBinderParcel.h"
#include "component/comoobjapi.h"
#include "rpc/comorpc.h"
#include "rpc/binder/InterfacePack.h"
#include "util/comolog.h"

namespace como {

const CoclassID CID_CBinderParcel =
        {{0x6d46b393,0x621b,0x4f0c,0x853b,{0x16,0x22,0x88,0x5e,0xd7,0x8d}}, &CID_COMORuntime};

COMO_INTERFACE_IMPL_1(CBinderParcel, Object, IParcel);

COMO_OBJECT_IMPL(CBinderParcel);

CBinderParcel::CBinderParcel()
    : mData(new android::Parcel())
{}

CBinderParcel::CBinderParcel(
    /* [in] */ android::Parcel* parcel,
    /* [in] */ Boolean releaseWhenDestruct)
    : mData(parcel)
    , mReleaseData(releaseWhenDestruct)
{}

CBinderParcel::~CBinderParcel()
{
    if (mReleaseData) {
        delete mData;
    }
}

ECode CBinderParcel::ReadChar(
    /* [out] */ Char& value)
{
    value = (Char)mData->readInt32();
    return NOERROR;
}

ECode CBinderParcel::WriteChar(
    /* [in] */ Char value)
{
    mData->writeInt32(value);
    return NOERROR;
}

ECode CBinderParcel::ReadByte(
    /* [out] */ Byte& value)
{
    value = (Byte)mData->readInt32();
    return NOERROR;
}

ECode CBinderParcel::WriteByte(
    /* [in] */ Byte value)
{
    mData->writeInt32(value);
    return NOERROR;
}

ECode CBinderParcel::ReadShort(
    /* [out] */ Short& value)
{
    value = (Short)mData->readInt32();
    return NOERROR;
}

ECode CBinderParcel::WriteShort(
    /* [in] */ Short value)
{
    mData->writeInt32(value);
    return NOERROR;
}

ECode CBinderParcel::ReadInteger(
    /* [out] */ Integer& value)
{
    value = mData->readInt32();
    return NOERROR;
}

ECode CBinderParcel::WriteInteger(
    /* [in] */ Integer value)
{
    mData->writeInt32(value);
    return NOERROR;
}

ECode CBinderParcel::ReadLong(
    /* [out] */ Long& value)
{
    value = mData->readInt64();
    return NOERROR;
}

ECode CBinderParcel::WriteLong(
    /* [in] */ Long value)
{
    mData->writeInt64(value);
    return NOERROR;
}

ECode CBinderParcel::ReadFloat(
    /* [out] */ Float& value)
{
    value = mData->readFloat();
    return NOERROR;
}

ECode CBinderParcel::WriteFloat(
    /* [in] */ Float value)
{
    mData->writeFloat(value);
    return NOERROR;
}

ECode CBinderParcel::ReadDouble(
    /* [out] */ Double& value)
{
    value = mData->readDouble();
    return NOERROR;
}

ECode CBinderParcel::WriteDouble(
    /* [in] */ Double value)
{
    mData->writeDouble(value);
    return NOERROR;
}

ECode CBinderParcel::ReadBoolean(
    /* [out] */ Boolean& value)
{
    value = (Boolean)mData->readInt32();
    return NOERROR;
}

ECode CBinderParcel::WriteBoolean(
    /* [in] */ Boolean value)
{
    mData->writeInt32(value);
    return NOERROR;
}

ECode CBinderParcel::ReadString(
    /* [out] */ String& value)
{
    Integer tag = mData->readInt32();
    if (tag == TAG_NOT_NULL) {
        value = mData->readCString();
    }
    else {
        value = nullptr;
    }
    return NOERROR;
}

ECode CBinderParcel::WriteString(
    /* [in] */ const String& value)
{
    if (value.IsNull()) {
        mData->writeInt32(TAG_NULL);
    }
    else {
        mData->writeInt32(TAG_NOT_NULL);
        mData->writeCString(value.string());
    }
    return NOERROR;
}

ECode CBinderParcel::ReadCoclassID(
    /* [out] */ CoclassID& value)
{
    android::status_t st = mData->read((void*)&value, sizeof(CoclassID));
    if (st != android::NO_ERROR) {
        return E_RUNTIME_EXCEPTION;
    }

    Integer tag = mData->readInt32();
    if (tag == TAG_NULL) {
        return NOERROR;
    }

    ComponentID* cid = (ComponentID*)malloc(sizeof(ComponentID));
    if (cid == nullptr) {
        return E_OUT_OF_MEMORY_ERROR;
    }
    value.mCid = cid;
    return ReadComponentID(*cid);
}

ECode CBinderParcel::WriteCoclassID(
    /* [in] */ const CoclassID& value)
{
    CoclassID* cid = (CoclassID*)mData->writeInplace(sizeof(CoclassID));
    if (cid == nullptr) {
        return E_RUNTIME_EXCEPTION;
    }
    memcpy(cid, &value, sizeof(CoclassID));
    cid->mCid = nullptr;

    if (value.mCid == nullptr) {
        mData->writeInt32(TAG_NULL);
        return NOERROR;
    }

    mData->writeInt32(TAG_NOT_NULL);
    return WriteComponentID(*value.mCid);
}

ECode CBinderParcel::ReadComponentID(
    /* [out] */ ComponentID& value)
{
    android::status_t st = mData->read((void*)&value, sizeof(ComponentID));
    if (st != android::NO_ERROR) {
        return E_RUNTIME_EXCEPTION;
    }

    Integer tag = mData->readInt32();
    if (tag == TAG_NOT_NULL) {
        const char* uri = mData->readCString();
        Integer size = strlen(uri);
        value.mUri = (const char*)malloc(size + 1);
        if (value.mUri != nullptr) {
            memcpy(const_cast<char*>(value.mUri), uri, size + 1);
        }
    }
    return NOERROR;
}

ECode CBinderParcel::WriteComponentID(
    /* [in] */ const ComponentID& value)
{
    ComponentID* cid = (ComponentID*)mData->writeInplace(sizeof(ComponentID));
    if (cid == nullptr) {
        return E_RUNTIME_EXCEPTION;
    }
    memcpy(cid, &value, sizeof(ComponentID));
    cid->mUri = nullptr;

    if (value.mUri != nullptr) {
        mData->writeInt32(TAG_NOT_NULL);
        mData->writeCString(value.mUri);
    }
    else {
        mData->writeInt32(TAG_NULL);
    }
    return NOERROR;
}

ECode CBinderParcel::ReadInterfaceID(
    /* [out] */ InterfaceID& value)
{
    android::status_t st = mData->read((void*)&value, sizeof(InterfaceID));
    if (st != android::NO_ERROR) {
        return E_RUNTIME_EXCEPTION;
    }

    Integer tag = mData->readInt32();
    if (tag == TAG_NULL) {
        return NOERROR;
    }

    ComponentID* cid = (ComponentID*)malloc(sizeof(ComponentID));
    if (cid == nullptr) {
        return E_OUT_OF_MEMORY_ERROR;
    }
    value.mCid = cid;
    return ReadComponentID(*cid);
}

ECode CBinderParcel::WriteInterfaceID(
    /* [in] */ const InterfaceID& value)
{
    InterfaceID* iid = (InterfaceID*)mData->writeInplace(sizeof(InterfaceID));
    if (iid == nullptr) {
        return E_RUNTIME_EXCEPTION;
    }
    memcpy(iid, &value, sizeof(InterfaceID));
    iid->mCid = nullptr;

    if (value.mCid == nullptr) {
        mData->writeInt32(TAG_NULL);
        return NOERROR;
    }

    mData->writeInt32(TAG_NOT_NULL);
    return WriteComponentID(*value.mCid);
}

ECode CBinderParcel::ReadECode(
    /* [out] */ ECode& value)
{
    value = mData->readInt32();
    return NOERROR;
}

ECode CBinderParcel::WriteECode(
    /* [in] */ ECode value)
{
    mData->writeInt32(value);
    return NOERROR;
}

ECode CBinderParcel::ReadEnumeration(
    /* [out] */ Integer& value)
{
    value = mData->readInt32();
    return NOERROR;
}

ECode CBinderParcel::WriteEnumeration(
    /* [in] */ Integer value)
{
    mData->writeInt32(value);
    return NOERROR;
}

ECode CBinderParcel::ReadArray(
    /* [out] */ Triple* array)
{
    Triple* t = array;
    VALIDATE_NOT_NULL(t);

    TypeKind kind = (TypeKind)mData->readInt32();
    Long size = mData->readInt64();
    if (size <= 0) {
        t->mData = nullptr;
        t->mSize = 0;
        t->mType = kind;
        return size < 0 ? E_RUNTIME_EXCEPTION : NOERROR;
    }

    android::status_t st = android::NO_ERROR;
    switch (kind) {
        case TypeKind::Char: {
            Triple tt;
            tt.mSize = size;
            tt.mType = TypeKind::Char;
            tt.AllocData(sizeof(Char) * size);
            if (tt.mData != nullptr) {
                st = mData->read(tt.mData, sizeof(Char) * size);
            }
            else {
                st = android::NO_MEMORY;
            }
            *t = std::move(tt);
            break;
        }
        case TypeKind::Byte: {
            Triple tt;
            tt.mSize = size;
            tt.mType = TypeKind::Byte;
            tt.AllocData(sizeof(Byte) * size);
            if (tt.mData != nullptr) {
                st = mData->read(tt.mData, sizeof(Byte) * size);
            }
            else {
                st = android::NO_MEMORY;
            }
            *t = std::move(tt);
            break;
        }
        case TypeKind::Short: {
            Triple tt;
            tt.mSize = size;
            tt.mType = TypeKind::Short;
            tt.AllocData(sizeof(Short) * size);
            if (tt.mData != nullptr) {
                st = mData->read(tt.mData, sizeof(Short) * size);
            }
            else {
                st = android::NO_MEMORY;
            }
            *t = std::move(tt);
            break;
        }
        case TypeKind::Integer: {
            Triple tt;
            tt.mSize = size;
            tt.mType = TypeKind::Integer;
            tt.AllocData(sizeof(Integer) * size);
            if (tt.mData != nullptr) {
                st = mData->read(tt.mData, sizeof(Integer) * size);
            }
            else {
                st = android::NO_MEMORY;
            }
            *t = std::move(tt);
            break;
        }
        case TypeKind::Long: {
            Triple tt;
            tt.mSize = size;
            tt.mType = TypeKind::Long;
            tt.AllocData(sizeof(Long) * size);
            if (tt.mData != nullptr) {
                st = mData->read(tt.mData, sizeof(Long) * size);
            }
            else {
                st = android::NO_MEMORY;
            }
            *t = std::move(tt);
            break;
        }
        case TypeKind::Float: {
            Triple tt;
            tt.mSize = size;
            tt.mType = TypeKind::Float;
            tt.AllocData(sizeof(Float) * size);
            if (tt.mData != nullptr) {
                st = mData->read(tt.mData, sizeof(Float) * size);
            }
            else {
                st = android::NO_MEMORY;
            }
            *t = std::move(tt);
            break;
        }
        case TypeKind::Double: {
            Triple tt;
            tt.mSize = size;
            tt.mType = TypeKind::Double;
            tt.AllocData(sizeof(Double) * size);
            if (tt.mData != nullptr) {
                st = mData->read(tt.mData, sizeof(Double) * size);
            }
            else {
                st = android::NO_MEMORY;
            }
            *t = std::move(tt);
            break;
        }
        case TypeKind::Boolean: {
            Triple tt;
            tt.mSize = size;
            tt.mType = TypeKind::Boolean;
            tt.AllocData(sizeof(Boolean) * size);
            if (tt.mData != nullptr) {
                st = mData->read(tt.mData, sizeof(Boolean) * size);
            }
            else {
                st = android::NO_MEMORY;
            }
            *t = std::move(tt);
            break;
        }
        case TypeKind::String: {
            Array<String> strArray(size);
            for (Long i = 0; i < size; i++) {
                strArray[i] = mData->readCString();
            }
            *t = strArray;
            break;
        }
        case TypeKind::CoclassID: {
            Array<CoclassID> cidArray(size);
            for (Long i = 0; i < size; i++) {
                CoclassID& cid = cidArray[i];
                ECode ec = ReadCoclassID(cid);
                if (FAILED(ec)) {
                    t->mData = nullptr;
                    t->mSize = 0;
                    t->mType = kind;
                    return ec;
                }
            }
            *t = cidArray;
            break;
        }
        case TypeKind::ComponentID: {
            Array<ComponentID> cidArray(size);
            for (Long i = 0; i < size; i++) {
                ComponentID& cid = cidArray[i];
                ECode ec = ReadComponentID(cid);
                if (FAILED(ec)) {
                    t->mData = nullptr;
                    t->mSize = 0;
                    t->mType = kind;
                    return ec;
                }
            }
            *t = cidArray;
            break;
        }
        case TypeKind::InterfaceID: {
            Array<InterfaceID> iidArray(size);
            for (Long i = 0; i < size; i++) {
                InterfaceID& iid = iidArray[i];
                ECode ec = ReadInterfaceID(iid);
                if (FAILED(ec)) {
                    t->mData = nullptr;
                    t->mSize = 0;
                    t->mType = kind;
                    return ec;
                }
            }
            *t = iidArray;
            break;
        }
        case TypeKind::ECode: {
            Triple tt;
            tt.mSize = size;
            tt.mType = TypeKind::ECode;
            tt.AllocData(sizeof(ECode) * size);
            if (tt.mData != nullptr) {
                st = mData->read(tt.mData, sizeof(ECode) * size);
            }
            else {
                st = android::NO_MEMORY;
            }
            *t = std::move(tt);
            break;
        }
        case TypeKind::Enum: {
            Triple tt;
            tt.mSize = size;
            tt.mType = TypeKind::Enum;
            tt.AllocData(sizeof(Integer) * size);
            if (tt.mData != nullptr) {
                st = mData->read(tt.mData, sizeof(Integer) * size);
            }
            else {
                st = android::NO_MEMORY;
            }
            *t = std::move(tt);
            break;
        }
        case TypeKind::Array: {
            Array<Triple> triArray(size);
            for (Long i = 0; i < size; i++) {
                Triple tt;
                ECode ec = ReadArray(&tt);
                if (FAILED(ec)) {
                    t->mData = nullptr;
                    t->mSize = 0;
                    t->mType = kind;
                    return ec;
                }
                triArray[i] = std::move(tt);
            }
            *t = triArray;
            break;
        }
        case TypeKind::Interface: {
            Array<IInterface*> intfArray(size);
            for (Long i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                ECode ec = ReadInterface(obj);
                if (FAILED(ec)) {
                    t->mData = nullptr;
                    t->mSize = 0;
                    t->mType = kind;
                    return ec;
                }
                intfArray.Set(i, obj);
            }
            *t = intfArray;
            break;
        }
        default:
            Logger::E("CBinderParcel", "Cannot read array with %d type from parcel", t->mType);
            break;
    }

    return st == android::NO_ERROR ? NOERROR : E_RUNTIME_EXCEPTION;
}

ECode CBinderParcel::WriteArray(
    /* [in] */ const Triple& array)
{
    const Triple* t = &array;
    VALIDATE_NOT_NULL(t);

    mData->writeInt32((Integer)t->mType);
    mData->writeInt64(t->mSize);

    if (t->mSize == 0) {
        return NOERROR;
    }

    android::status_t st = android::NO_ERROR;
    switch (t->mType) {
        case TypeKind::Char:
            st = mData->write(t->mData, sizeof(Char) * t->mSize);
            break;
        case TypeKind::Byte:
            st = mData->write(t->mData, sizeof(Byte) * t->mSize);
            break;
        case TypeKind::Short:
            st = mData->write(t->mData, sizeof(Short) * t->mSize);
            break;
        case TypeKind::Integer:
            st = mData->write(t->mData, sizeof(Integer) * t->mSize);
            break;
        case TypeKind::Long:
            st = mData->write(t->mData, sizeof(Long) * t->mSize);
            break;
        case TypeKind::Float:
            st = mData->write(t->mData, sizeof(Float) * t->mSize);
            break;
        case TypeKind::Double:
            st = mData->write(t->mData, sizeof(Double) * t->mSize);
            break;
        case TypeKind::Boolean:
            st = mData->write(t->mData, sizeof(Boolean) * t->mSize);
            break;
        case TypeKind::String: {
            for (Long i = 0; i < t->mSize; i++) {
                const String& str = reinterpret_cast<String*>(t->mData)[i];
                st = mData->writeCString(str.string());
                if (st != android::NO_ERROR) {
                    break;
                }
            }
            break;
        }
        case TypeKind::CoclassID: {
            for (Long i = 0; i < t->mSize; i++) {
                const CoclassID& cid = reinterpret_cast<CoclassID*>(t->mData)[i];
                ECode ec = WriteCoclassID(cid);
                if (FAILED(ec)) {
                    break;
                }
            }
            break;
        }
        case TypeKind::ComponentID: {
            for (Long i = 0; i < t->mSize; i++) {
                const ComponentID& cid = reinterpret_cast<ComponentID*>(t->mData)[i];
                ECode ec = WriteComponentID(cid);
                if (FAILED(ec)) {
                    break;
                }
            }
            break;
        }
        case TypeKind::InterfaceID: {
            for (Long i = 0; i < t->mSize; i++) {
                const InterfaceID& iid = reinterpret_cast<InterfaceID*>(t->mData)[i];
                ECode ec = WriteInterfaceID(iid);
                if (FAILED(ec)) {
                    break;
                }
            }
            break;
        }
        case TypeKind::ECode:
            st = mData->write(t->mData, sizeof(ECode) * t->mSize);
            break;
        case TypeKind::Enum:
            st = mData->write(t->mData, sizeof(Integer) * t->mSize);
            break;
        case TypeKind::Array: {
            for (Long i = 0; i < t->mSize; i++) {
                const Triple& tt = reinterpret_cast<Triple*>(t->mData)[i];
                ECode ec = WriteArray(tt);
                if (FAILED(ec)) {
                    break;
                }
            }
            break;
        }
        case TypeKind::Interface: {
            for (Long i = 0; i < t->mSize; i++) {
                IInterface* intf = reinterpret_cast<IInterface**>(t->mData)[i];
                ECode ec = WriteInterface(intf);
                if (FAILED(ec)) {
                    break;
                }
            }
            break;
        }
        default:
            Logger::E("CBinderParcel", "Cannot write array with %d type into parcel", t->mType);
            break;
    }

    return st == android::NO_ERROR ? NOERROR : E_RUNTIME_EXCEPTION;
}

ECode CBinderParcel::ReadInterface(
    /* [out] */ AutoPtr<IInterface>& value)
{
    Integer tag = mData->readInt32();
    if (tag == TAG_NOT_NULL) {
        AutoPtr<IInterfacePack> ipack;
        CoCreateInterfacePack(RPCType::Local, ipack);
        IParcelable::Probe(ipack)->ReadFromParcel(this);

        ECode ec = CoUnmarshalInterface(ipack, RPCType::Local, value);
        if (FAILED(ec)) {
            Logger::E("CBinderParcel", "Unmarshal the interface in ReadInterface failed.");
            return ec;
        }

        Boolean parcelable;
        ipack->IsParcelable(parcelable);
        if (parcelable) {
            IParcelable::Probe(value)->ReadFromParcel(this);
        }
    }
    else {
        value = nullptr;
    }
    return NOERROR;
}

ECode CBinderParcel::WriteInterface(
    /* [in] */ IInterface* value)
{
    mData->writeInt32(value != nullptr ? TAG_NOT_NULL : TAG_NULL);

    if (value != nullptr) {
        AutoPtr<IInterfacePack> ipack;
        ECode ec = CoMarshalInterface(value, RPCType::Local, ipack);
        if (FAILED(ec)) {
            Logger::E("CBinderParcel", "Marshal the interface in WriteInterface failed.");
            return ec;
        }

        IParcelable::Probe(ipack)->WriteToParcel(this);

        IParcelable* p = IParcelable::Probe(value);
        if (p != nullptr) {
            p->WriteToParcel(this);
        }
    }

    return NOERROR;
}

ECode CBinderParcel::GetData(
    /* [out] */ HANDLE& data)
{
    data = reinterpret_cast<HANDLE>(mData->data());
    return NOERROR;
}

ECode CBinderParcel::GetDataSize(
    /* [out] */ Long& size)
{
    size = mData->dataSize();
    return NOERROR;
}

ECode CBinderParcel::SetData(
    /* [in] */ HANDLE data,
    /* [in] */ Long size)
{
    if (size <= 0) {
        return NOERROR;
    }

    mData->setData(reinterpret_cast<uint8_t*>(data), size);
    return NOERROR;
}

ECode CBinderParcel::GetDataPosition(
    /* [out] */ Long& pos)
{
    pos = mData->dataPosition();
    return NOERROR;
}

ECode CBinderParcel::SetDataPosition(
    /* [in] */ Long pos)
{
    if (pos < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mData->setDataPosition(pos);
    return NOERROR;
}

ECode CBinderParcel::GetPayload(
    /* [out] */ HANDLE& payload)
{
    payload = reinterpret_cast<HANDLE>(mData);
    return NOERROR;
}

ECode CBinderParcel::SetPayload(
    /* [in] */ HANDLE payload,
    /* [in] */ Boolean release)
{
    if (payload == 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mReleaseData && mData != nullptr) {
        delete mData;
    }

    mData = reinterpret_cast<android::Parcel*>(payload);
    mReleaseData = release;
    return NOERROR;
}

}
