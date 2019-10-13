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

/*
 * Copyright (C) 2005 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "CDBusParcel.h"
#include "util/comosp.h"
#include "util/comolog.h"
#include <stdlib.h>

namespace como {

const CoclassID CID_CDBusParcel =
        {{0x28208469,0x8814,0x49af,0x80f7,{0x8,0xb,0x1,0x4,0x7,0x3,0x9,0x1,0xf,0xb,0x9,0x4}}, &CID_COMORuntime};

COMO_INTERFACE_IMPL_1(CDBusParcel, Object, IParcel);

COMO_OBJECT_IMPL(CDBusParcel);

CDBusParcel::CDBusParcel()
    : mError(NOERROR)
    , mData(nullptr)
    , mDataSize(0)
    , mDataCapacity(0)
    , mDataPos(0)
{}

CDBusParcel::~CDBusParcel()
{
    if (mData != nullptr) {
        free(mData);
    }
}

ECode CDBusParcel::ReadChar(
    /* [out] */ Char& value)
{
    Integer i;
    ECode ec = ReadInteger(i);
    value = (Char)i;
    return ec;
}

ECode CDBusParcel::WriteChar(
    /* [in] */ Char value)
{
    return WriteInteger((Integer)value);
}

ECode CDBusParcel::ReadByte(
    /* [out] */ Byte& value)
{
    Integer i;
    ECode ec = ReadInteger(i);
    value = (Byte)i;
    return ec;
}

ECode CDBusParcel::WriteByte(
    /* [in] */ Byte value)
{
    return WriteInteger((Integer)value);
}

ECode CDBusParcel::ReadShort(
    /* [out] */ Short& value)
{
    Integer i;
    ECode ec = ReadInteger(i);
    value = (Short)i;
    return ec;
}

ECode CDBusParcel::WriteShort(
    /* [in] */ Short value)
{
    return WriteInteger((Integer)value);
}

ECode CDBusParcel::ReadInteger(
    /* [out] */ Integer& value)
{
    return ReadAligned<Integer>(&value);
}

ECode CDBusParcel::WriteInteger(
    /* [in] */ Integer value)
{
    return WriteAligned<Integer>(value);
}

ECode CDBusParcel::ReadLong(
    /* [out] */ Long& value)
{
    return ReadAligned<Long>(&value);
}

ECode CDBusParcel::WriteLong(
    /* [in] */ Long value)
{
    return WriteAligned<Long>(value);
}

ECode CDBusParcel::ReadFloat(
    /* [out] */ Float& value)
{
    return ReadAligned<Float>(&value);
}

ECode CDBusParcel::WriteFloat(
    /* [in] */ Float value)
{
    return WriteAligned<Float>(value);
}

ECode CDBusParcel::ReadDouble(
    /* [out] */ Double& value)
{
    return ReadAligned<Double>(&value);
}

ECode CDBusParcel::WriteDouble(
    /* [in] */ Double value)
{
    return WriteAligned<Double>(value);
}

ECode CDBusParcel::ReadBoolean(
    /* [out] */ Boolean& value)
{
    Integer i;
    ECode ec = ReadInteger(i);
    value = (Boolean)i;
    return ec;
}

ECode CDBusParcel::WriteBoolean(
    /* [in] */ Boolean value)
{
    return WriteInteger((Integer)value);
}

ECode CDBusParcel::ReadString(
    /* [out] */ String& value)
{
    value = nullptr;

    Integer size;
    ECode ec = ReadInteger(size);
    if (FAILED(ec)) {
        return ec;
    }

    if (size < 0) {
        return E_RUNTIME_EXCEPTION;
    }

    if (size == 0) {
        return NOERROR;
    }
    const char* str = (const char*)ReadInplace(size + 1);
    if (str == nullptr) {
        return E_RUNTIME_EXCEPTION;
    }
    value = str;
    return NOERROR;
}

ECode CDBusParcel::WriteString(
    /* [in] */ const String& value)
{
    ECode ec = WriteInteger(value.GetByteLength());
    if (value.GetByteLength() > 0 && SUCCEEDED(ec)) {
        ec = Write(value.string(), value.GetByteLength() + 1);
    }
    return ec;
}

ECode CDBusParcel::ReadCoclassID(
    /* [out] */ CoclassID& value)
{
    ECode ec = Read((void*)&value, sizeof(CoclassID));
    if (FAILED(ec)) {
        return ec;
    }

    Integer tag;
    ec = ReadInteger(tag);
    if (FAILED(ec)) {
        return ec;
    }

    if (tag == TAG_NULL) {
        value.mCid = nullptr;
        return NOERROR;
    }

    ComponentID* cid = (ComponentID*)malloc(sizeof(ComponentID));
    if (cid == nullptr) {
        return E_OUT_OF_MEMORY_ERROR;
    }

    ec = Read((void*)cid, sizeof(ComponentID));
    if (FAILED(ec)) {
        return ec;
    }

    Integer size;
    ec = ReadInteger(size);
    if (FAILED(ec)) {
        return ec;
    }

    if (size < 0) {
        return E_RUNTIME_EXCEPTION;
    }

    if (size == 0) {
        cid->mUri = nullptr;
    }
    else {
        const char* str = (const char*)ReadInplace(size + 1);
        if (str == nullptr) {
            return E_RUNTIME_EXCEPTION;
        }
        cid->mUri = (const char*)malloc(size + 1);
        if (cid->mUri == nullptr) {
            return E_OUT_OF_MEMORY_ERROR;
        }
        memcpy(const_cast<char*>(cid->mUri), str, size + 1);
    }
    value.mCid = cid;
    return NOERROR;
}

ECode CDBusParcel::WriteCoclassID(
    /* [in] */ const CoclassID& value)
{
    ECode ec = Write((void*)&value, sizeof(CoclassID));
    if (FAILED(ec)) {
        return ec;
    }

    if (value.mCid == nullptr) {
        return WriteInteger(TAG_NULL);
    }

    ec = WriteInteger(TAG_NOT_NULL);
    if (SUCCEEDED(ec)) {
        ec = WriteComponentID(*value.mCid);
    }
    return ec;
}

ECode CDBusParcel::ReadComponentID(
    /* [out] */ ComponentID& value)
{
    ECode ec = Read((void*)&value, sizeof(ComponentID));
    if (FAILED(ec)) {
        return ec;
    }

    Integer size;
    ec = ReadInteger(size);
    if (FAILED(ec)) {
        return ec;
    }

    if (size < 0) {
        return E_RUNTIME_EXCEPTION;
    }

    if (size == 0) {
        value.mUri = nullptr;
        return NOERROR;
    }
    const char* str = (const char*)ReadInplace(size + 1);
    if (str == nullptr) {
        return E_RUNTIME_EXCEPTION;
    }
    value.mUri = (const char*)malloc(size + 1);
    if (value.mUri == nullptr) {
        return E_OUT_OF_MEMORY_ERROR;
    }
    memcpy(const_cast<char*>(value.mUri), str, size + 1);
    return NOERROR;
}

ECode CDBusParcel::WriteComponentID(
    /* [in] */ const ComponentID& value)
{
    ECode ec = Write((void*)&value, sizeof(ComponentID));
    if (FAILED(ec)) {
        return ec;
    }

    Integer size = value.mUri == nullptr ? 0 : strlen(value.mUri);
    ec = WriteInteger(size);
    if (size > 0 && SUCCEEDED(ec)) {
        ec = Write(value.mUri, size + 1);
    }
    return ec;
}

ECode CDBusParcel::ReadInterfaceID(
    /* [out] */ InterfaceID& value)
{
    ECode ec = Read((void*)&value, sizeof(InterfaceID));
    if (FAILED(ec)) {
        return ec;
    }

    Integer tag;
    ec = ReadInteger(tag);
    if (FAILED(ec)) {
        return ec;
    }

    if (tag == TAG_NULL) {
        value.mCid = nullptr;
        return NOERROR;
    }

    ComponentID* cid = (ComponentID*)malloc(sizeof(ComponentID));
    if (cid == nullptr) {
        return E_OUT_OF_MEMORY_ERROR;
    }

    ec = Read((void*)cid, sizeof(ComponentID));
    if (FAILED(ec)) {
        return ec;
    }

    Integer size;
    ec = ReadInteger(size);
    if (FAILED(ec)) {
        return ec;
    }

    if (size < 0) {
        return E_RUNTIME_EXCEPTION;
    }

    if (size == 0) {
        cid->mUri = nullptr;
    }
    else {
        const char* str = (const char*)ReadInplace(size + 1);
        if (str == nullptr) {
            return E_RUNTIME_EXCEPTION;
        }
        cid->mUri = (const char*)malloc(size + 1);
        if (cid->mUri == nullptr) {
            return E_OUT_OF_MEMORY_ERROR;
        }
        memcpy(const_cast<char*>(cid->mUri), str, size + 1);
    }
    value.mCid = cid;
    return NOERROR;
}

ECode CDBusParcel::WriteInterfaceID(
    /* [in] */ const InterfaceID& value)
{
    ECode ec = Write((void*)&value, sizeof(InterfaceID));
    if (FAILED(ec)) {
        return ec;
    }

    if (value.mCid == nullptr) {
        return WriteInteger(TAG_NULL);
    }

    ec = WriteInteger(TAG_NOT_NULL);
    if (SUCCEEDED(ec)) {
        ec = WriteComponentID(*value.mCid);
    }
    return ec;
}

ECode CDBusParcel::ReadECode(
    /* [out] */ ECode& value)
{
    return ReadInteger(value);
}

ECode CDBusParcel::WriteECode(
    /* [in] */ ECode value)
{
    return WriteInteger(value);
}

ECode CDBusParcel::ReadEnumeration(
    /* [out] */ Integer& value)
{
    return ReadInteger(value);
}

ECode CDBusParcel::WriteEnumeration(
    /* [in] */ Integer value)
{
    return WriteInteger(value);
}

ECode CDBusParcel::ReadArray(
    /* [out] */ HANDLE array)
{
    Triple* t = reinterpret_cast<Triple*>(array);
    VALIDATE_NOT_NULL(t);

    Integer value;
    ECode ec = ReadInteger(value);
    if (FAILED(ec)) return ec;

    TypeKind kind = (TypeKind)value;
    Long size;
    ec = ReadLong(size);
    if (size <= 0 || FAILED(ec)) {
        t->mData = nullptr;
        t->mSize = 0;
        t->mType = kind;
        return FAILED(ec) ? ec : size < 0 ?
                E_RUNTIME_EXCEPTION : NOERROR;
    }

    switch (kind) {
        case TypeKind::Char: {
            Triple tt;
            tt.mSize = size;
            tt.mType = TypeKind::Char;
            tt.AllocData(sizeof(Char) * size);
            if (tt.mData != nullptr) {
                ec = Read(tt.mData, sizeof(Char) * size);
            }
            else ec = E_OUT_OF_MEMORY_ERROR;
            *t = std::move(tt);
            break;
        }
        case TypeKind::Byte: {
            Triple tt;
            tt.mSize = size;
            tt.mType = TypeKind::Byte;
            tt.AllocData(sizeof(Byte) * size);
            if (tt.mData != nullptr) {
                ec = Read(tt.mData, sizeof(Byte) * size);
            }
            else ec = E_OUT_OF_MEMORY_ERROR;
            *t = std::move(tt);
            break;
        }
        case TypeKind::Short: {
            Triple tt;
            tt.mSize = size;
            tt.mType = TypeKind::Short;
            tt.AllocData(sizeof(Short) * size);
            if (tt.mData != nullptr) {
                ec = Read(tt.mData, sizeof(Short) * size);
            }
            else ec = E_OUT_OF_MEMORY_ERROR;
            *t = std::move(tt);
            break;
        }
        case TypeKind::Integer: {
            Triple tt;
            tt.mSize = size;
            tt.mType = TypeKind::Integer;
            tt.AllocData(sizeof(Integer) * size);
            if (tt.mData != nullptr) {
                ec = Read(tt.mData, sizeof(Integer) * size);
            }
            else ec = E_OUT_OF_MEMORY_ERROR;
            *t = std::move(tt);
            break;
        }
        case TypeKind::Long: {
            Triple tt;
            tt.mSize = size;
            tt.mType = TypeKind::Long;
            tt.AllocData(sizeof(Long) * size);
            if (tt.mData != nullptr) {
                ec = Read(tt.mData, sizeof(Long) * size);
            }
            else ec = E_OUT_OF_MEMORY_ERROR;
            *t = std::move(tt);
            break;
        }
        case TypeKind::Float: {
            Triple tt;
            tt.mSize = size;
            tt.mType = TypeKind::Float;
            tt.AllocData(sizeof(Float) * size);
            if (tt.mData != nullptr) {
                ec = Read(tt.mData, sizeof(Float) * size);
            }
            else ec = E_OUT_OF_MEMORY_ERROR;
            *t = std::move(tt);
            break;
        }
        case TypeKind::Double: {
            Triple tt;
            tt.mSize = size;
            tt.mType = TypeKind::Double;
            tt.AllocData(sizeof(Double) * size);
            if (tt.mData != nullptr) {
                ec = Read(tt.mData, sizeof(Double) * size);
            }
            else ec = E_OUT_OF_MEMORY_ERROR;
            *t = std::move(tt);
            break;
        }
        case TypeKind::Boolean: {
            Triple tt;
            tt.mSize = size;
            tt.mType = TypeKind::Boolean;
            tt.AllocData(sizeof(Boolean) * size);
            if (tt.mData != nullptr) {
                ec = Read(tt.mData, sizeof(Boolean) * size);
            }
            else ec = E_OUT_OF_MEMORY_ERROR;
            *t = std::move(tt);
            break;
        }
        case TypeKind::String: {
            Array<String> strArray(size);
            for (Long i = 0; i < size; i++) {
                String str;
                ec = ReadString(str);
                if (FAILED(ec)) {
                    t->mData = nullptr;
                    t->mSize = 0;
                    t->mType = kind;
                    return ec;
                }
                strArray[i] = str;
            }
            *t = strArray;
            break;
        }
        case TypeKind::CoclassID: {
            Array<CoclassID> cidArray(size);
            for (Long i = 0; i < size; i++) {
                CoclassID& cid = cidArray[i];
                ec = ReadCoclassID(cid);
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
                ec = ReadComponentID(cid);
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
                ec = ReadInterfaceID(iid);
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
                ec = Read(tt.mData, sizeof(ECode) * size);
            }
            else ec = E_OUT_OF_MEMORY_ERROR;
            *t = std::move(tt);
            break;
        }
        case TypeKind::Enum: {
            Triple tt;
            tt.mSize = size;
            tt.mType = TypeKind::Enum;
            tt.AllocData(sizeof(Integer) * size);
            if (tt.mData != nullptr) {
                ec = Read(tt.mData, sizeof(Integer) * size);
            }
            else ec = E_OUT_OF_MEMORY_ERROR;
            *t = std::move(tt);
            break;
        }
        case TypeKind::Array: {
            Array<Triple> triArray(size);
            for (Long i = 0; i < size; i++) {
                Triple tt;
                ec = ReadArray(reinterpret_cast<HANDLE>(&tt));
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
                ec = ReadInterface(obj);
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
            Logger::E("CDBusParcel", "Cannot read array with %d type from parcel", t->mType);
            break;
    }

    return ec;
}

ECode CDBusParcel::WriteArray(
    /* [in] */ HANDLE array)
{
    Triple* t = reinterpret_cast<Triple*>(array);
    VALIDATE_NOT_NULL(t);

    ECode ec = WriteInteger((Integer)t->mType);
    if (FAILED(ec)) return ec;

    ec = WriteLong(t->mSize);
    if (t->mSize == 0 || FAILED(ec)) return ec;

    switch (t->mType) {
        case TypeKind::Char:
            ec = Write(t->mData, sizeof(Char) * t->mSize);
            break;
        case TypeKind::Byte:
            ec = Write(t->mData, sizeof(Byte) * t->mSize);
            break;
        case TypeKind::Short:
            ec = Write(t->mData, sizeof(Short) * t->mSize);
            break;
        case TypeKind::Integer:
            ec = Write(t->mData, sizeof(Integer) * t->mSize);
            break;
        case TypeKind::Long:
            ec = Write(t->mData, sizeof(Long) * t->mSize);
            break;
        case TypeKind::Float:
            ec = Write(t->mData, sizeof(Float) * t->mSize);
            break;
        case TypeKind::Double:
            ec = Write(t->mData, sizeof(Double) * t->mSize);
            break;
        case TypeKind::Boolean:
            ec = Write(t->mData, sizeof(Boolean) * t->mSize);
            break;
        case TypeKind::String: {
            for (Long i = 0; i < t->mSize; i++) {
                const String& str = reinterpret_cast<String*>(t->mData)[i];
                ec = WriteString(str);
                if (FAILED(ec)) break;
            }
            break;
        }
        case TypeKind::CoclassID: {
            for (Long i = 0; i < t->mSize; i++) {
                const CoclassID& cid = reinterpret_cast<CoclassID*>(t->mData)[i];
                ec = WriteCoclassID(cid);
                if (FAILED(ec)) break;
            }
            break;
        }
        case TypeKind::ComponentID: {
            for (Long i = 0; i < t->mSize; i++) {
                const ComponentID& cid = reinterpret_cast<ComponentID*>(t->mData)[i];
                ec = WriteComponentID(cid);
                if (FAILED(ec)) break;
            }
            break;
        }
        case TypeKind::InterfaceID: {
            for (Long i = 0; i < t->mSize; i++) {
                const InterfaceID& iid = reinterpret_cast<InterfaceID*>(t->mData)[i];
                ec = WriteInterfaceID(iid);
                if (FAILED(ec)) break;
            }
            break;
        }
        case TypeKind::ECode:
            ec = Write(t->mData, sizeof(ECode) * t->mSize);
            break;
        case TypeKind::Enum:
            ec = Write(t->mData, sizeof(Integer) * t->mSize);
            break;
        case TypeKind::Array: {
            for (Long i = 0; i < t->mSize; i++) {
                const Triple& tt = reinterpret_cast<Triple*>(t->mData)[i];
                ec = WriteArray(reinterpret_cast<HANDLE>(&tt));
                if (FAILED(ec)) break;
            }
            break;
        }
        case TypeKind::Interface: {
            for (Long i = 0; i < t->mSize; i++) {
                IInterface* intf = reinterpret_cast<IInterface**>(t->mData)[i];
                ec = WriteInterface(intf);
                if (FAILED(ec)) break;
            }
            break;
        }
        default:
            Logger::E("CDBusParcel", "Cannot write array with %d type into parcel", t->mType);
            break;
    }

    return ec;
}

ECode CDBusParcel::ReadInterface(
    /* [out] */ AutoPtr<IInterface>& value)
{
    return NOERROR;
}

ECode CDBusParcel::WriteInterface(
    /* [in] */ IInterface* value)
{
    return NOERROR;
}

ECode CDBusParcel::GetData(
    /* [out] */ HANDLE& data)
{
    data = reinterpret_cast<HANDLE>(mData);
    return NOERROR;
}

ECode CDBusParcel::SetData(
    /* [in] */ Byte* data,
    /* [in] */ Long size)
{
    if (size <= 0) {
        return NOERROR;
    }

    ECode ec = RestartWrite(size);
    if (SUCCEEDED(ec)) {
        memcpy(mData, data, size);
        mDataSize = size;
    }
    return ec;
}

ECode CDBusParcel::GetDataSize(
    /* [out] */ Long& size)
{
    size = mDataSize;
    return NOERROR;
}

ECode CDBusParcel::SetDataPosition(
    /* [in] */ Long pos)
{
    if (pos < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mDataPos = pos;
    return NOERROR;
}

ECode CDBusParcel::Read(
    /* [in] */ void* outData,
    /* [in] */ Long len) const
{
    if (len < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if ((mDataPos + ALIGN4(len)) >= mDataPos && (mDataPos + ALIGN4(len)) <= mDataSize
            && len <= ALIGN4(len)) {
        memcpy(outData, mData + mDataPos, len);
        mDataPos += ALIGN4(len);
        return NOERROR;
    }
    return E_NOT_FOUND_EXCEPTION;
}

const void* CDBusParcel::ReadInplace(
    /* [in] */ Long len) const
{
    if (len < 0) {
        return nullptr;
    }

    if ((mDataPos + ALIGN4(len)) >= mDataPos && (mDataPos + ALIGN4(len)) <= mDataSize
            && len <= ALIGN4(len)) {
        const void* data = mData + mDataPos;
        mDataPos += ALIGN4(len);
        return data;
    }
    return nullptr;
}

ECode CDBusParcel::Write(
    /* [in] */ const void* data,
    /* [in] */ Long len)
{
    if (len < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    void* const d = WriteInplace(len);
    if (d != nullptr) {
        memcpy(d, data, len);
        return NOERROR;
    }
    return mError;
}

void* CDBusParcel::WriteInplace(
    /* [in] */ Long len)
{
    if (len < 0) {
        return nullptr;
    }

    const Long padded = ALIGN4(len);

    if (mDataPos + padded < mDataPos) {
        return nullptr;
    }

    if ((mDataPos + padded) <= mDataCapacity) {
restart_write:
        Byte* const data = mData + mDataPos;

        if (padded != len) {
#if BYTE_ORDER == BIG_ENDIAN
            static const uint32_t mask[4] = {
                0x00000000, 0xffffff00, 0xffff0000, 0xff000000
            };
#endif
#if BYTE_ORDER == LITTLE_ENDIAN
            static const uint32_t mask[4] = {
                0x00000000, 0x00ffffff, 0x0000ffff, 0x000000ff
            };
#endif
            *reinterpret_cast<uint32_t*>(data + padded - 4) &= mask[padded - len];
        }

        FinishWrite(padded);
        return data;
    }

    ECode ec = GrowData(padded);
    if (SUCCEEDED(ec)) goto restart_write;
    return nullptr;
}

ECode CDBusParcel::FinishWrite(
    /* [in] */ Long len)
{
    if (len < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mDataPos += len;
    if (mDataPos > mDataSize) {
        mDataSize = mDataPos;
    }
    return NOERROR;
}

ECode CDBusParcel::GrowData(
    /* [in] */ Long len)
{
    if (len < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Long newSize = ((mDataSize + len) * 3) / 2;
    return (newSize <= mDataSize) ?
            E_OUT_OF_MEMORY_ERROR : ContinueWrite(newSize);
}

ECode CDBusParcel::RestartWrite(
    /* [in] */ Long desired)
{
    if (desired < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Byte* data = (Byte*)realloc(mData, desired);
    if (data == nullptr && desired > mDataCapacity) {
        mError = E_OUT_OF_MEMORY_ERROR;
        return E_OUT_OF_MEMORY_ERROR;
    }

    if (data != nullptr) {
        mData = data;
        mDataCapacity = desired;
    }

    mDataSize = mDataPos = 0;
    return NOERROR;
}

ECode CDBusParcel::ContinueWrite(
    /* [in] */ Long desired)
{
    if (desired < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mData != nullptr) {
        if (desired > mDataCapacity) {
            Byte* data = (Byte*)realloc(mData, desired);
            if (data != nullptr) {
                mData = data;
                mDataCapacity = desired;
            }
            else {
                mError = E_OUT_OF_MEMORY_ERROR;
                return E_OUT_OF_MEMORY_ERROR;
            }
        }
        else {
            if (mDataSize > desired) {
                mDataSize = desired;
            }
            if (mDataPos > desired) {
                mDataPos = desired;
            }
        }
    }
    else {
        Byte* data = (Byte*)malloc(desired);
        if (data == nullptr) {
            mError = E_OUT_OF_MEMORY_ERROR;
            return E_OUT_OF_MEMORY_ERROR;
        }

        mData = data;
        mDataSize = mDataPos = 0;
        mDataCapacity = desired;
    }

    return NOERROR;
}

template<class T>
ECode CDBusParcel::ReadAligned(
    /* [out] */ T* value) const
{
    if (sizeof(T) == 8) {
        mDataPos = ALIGN8(mDataPos);
    }

    if ((mDataPos + sizeof(T)) <= mDataSize) {
        const void* data = mData + mDataPos;
        mDataPos += sizeof(T);
        *value = *reinterpret_cast<const T*>(data);
        return NOERROR;
    }
    else {
        *value = 0;
        return E_NOT_FOUND_EXCEPTION;
    }
}

template<class T>
ECode CDBusParcel::WriteAligned(
    /* [in] */ T value)
{
    Long oldDataPos = mDataPos;
    if (sizeof(T) == 8) {
        mDataPos = ALIGN8(mDataPos);
    }

    if ((mDataPos + sizeof(value)) <= mDataCapacity) {
restart_write:
        *reinterpret_cast<T*>(mData + mDataPos) = value;
        return FinishWrite(sizeof(value));
    }

    ECode ec = GrowData(mDataPos - oldDataPos + sizeof(value));
    if (SUCCEEDED(ec)) goto restart_write;
    return ec;
}

ECode CDBusParcel::CreateObject(
    /* [out] */ AutoPtr<IParcel>& parcel)
{
    parcel = new CDBusParcel();
    return NOERROR;
}

} // namespace como
