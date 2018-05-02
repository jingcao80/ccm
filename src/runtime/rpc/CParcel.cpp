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

#include "CParcel.h"
#include <stdlib.h>

namespace ccm {

const CoclassID CID_CParcel =
        {{0x28208469,0x8814,0x49af,0x80f7,{0x8,0xb,0x1,0x4,0x7,0x3,0x9,0x1,0xf,0xb,0x9,0x4}}, &CID_CCMRuntime};

CCM_INTERFACE_IMPL_1(CParcel, Object, IParcel);

CCM_OBJECT_IMPL(CParcel);

CParcel::CParcel()
    : mError(NOERROR)
    , mData(nullptr)
    , mDataSize(0)
    , mDataCapacity(0)
    , mDataPos(0)
{}

CParcel::~CParcel()
{
    if (mData != nullptr) {
        free(mData);
    }
}

ECode CParcel::ReadChar(
    /* [out] */ Char* value)
{
    VALIDATE_NOT_NULL(value);

    Integer i;
    ECode ec = ReadInteger(&i);
    *value = (Char)i;
    return ec;
}

ECode CParcel::WriteChar(
    /* [in] */ Char value)
{
    return WriteInteger((Integer)value);
}

ECode CParcel::ReadByte(
    /* [out] */ Byte* value)
{
    VALIDATE_NOT_NULL(value);

    Integer i;
    ECode ec = ReadInteger(&i);
    *value = (Byte)i;
    return ec;
}

ECode CParcel::WriteByte(
    /* [in] */ Byte value)
{
    return WriteInteger((Integer)value);
}

ECode CParcel::ReadShort(
    /* [out] */ Short* value)
{
    VALIDATE_NOT_NULL(value);

    Integer i;
    ECode ec = ReadInteger(&i);
    *value = (Short)i;
    return ec;
}

ECode CParcel::WriteShort(
    /* [in] */ Short value)
{
    return WriteInteger((Integer)value);
}

ECode CParcel::ReadInteger(
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    return ReadAligned<Integer>(value);
}

ECode CParcel::WriteInteger(
    /* [in] */ Integer value)
{
    return WriteAligned<Integer>(value);
}

ECode CParcel::ReadLong(
    /* [out] */ Long* value)
{
    VALIDATE_NOT_NULL(value);

    return ReadAligned<Long>(value);
}

ECode CParcel::WriteLong(
    /* [in] */ Long value)
{
    return WriteAligned<Long>(value);
}

ECode CParcel::ReadFloat(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    return ReadAligned<Float>(value);
}

ECode CParcel::WriteFloat(
    /* [in] */ Float value)
{
    return WriteAligned<Float>(value);
}

ECode CParcel::ReadDouble(
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);

    return ReadAligned<Double>(value);
}

ECode CParcel::WriteDouble(
    /* [in] */ Double value)
{
    return WriteAligned<Double>(value);
}

ECode CParcel::ReadBoolean(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    Integer i;
    ECode ec = ReadInteger(&i);
    *value = (Boolean)i;
    return ec;
}

ECode CParcel::WriteBoolean(
    /* [in] */ Boolean value)
{
    return WriteInteger((Integer)value);
}

ECode CParcel::ReadString(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = nullptr;

    Integer size;
    ECode ec = ReadInteger(&size);
    if (FAILED(ec)) {
        return ec;
    }

    if (size < 0) {
        return E_RUNTIME_EXCEPTION;
    }

    if (size == 0) {
        *value = String();
        return NOERROR;
    }
    const char* str = (const char*)ReadInplace(size + 1);
    if (str == nullptr) {
        return E_RUNTIME_EXCEPTION;
    }
    *value = str;
    return NOERROR;
}

ECode CParcel::WriteString(
    /* [in] */ const String& value)
{
    ECode ec = WriteInteger(value.GetByteLength());
    if (value.GetByteLength() > 0 && SUCCEEDED(ec)) {
        ec = Write(value.string(), value.GetByteLength() + 1);
    }
    return ec;
}

ECode CParcel::ReadCoclassID(
    /* [out] */ CoclassID* value)
{
    VALIDATE_NOT_NULL(value);

    ECode ec = Read((void*)value, sizeof(CoclassID));
    if (FAILED(ec)) {
        return ec;
    }

    Integer tag;
    ec = ReadInteger(&tag);
    if (FAILED(ec)) {
        return ec;
    }

    if (tag == TAG_NULL) {
        value->mCid = nullptr;
        return NOERROR;
    }

    ComponentID _cid;
    ec = Read((void*)&_cid, sizeof(ComponentID));
    if (FAILED(ec)) {
        return ec;
    }

    Integer size;
    ec = ReadInteger(&size);
    if (FAILED(ec)) {
        return ec;
    }

    if (size < 0) {
        return E_RUNTIME_EXCEPTION;
    }

    ComponentID* cid = nullptr;
    if (size == 0) {
        cid = (ComponentID*)malloc(sizeof(ComponentID));
        if (cid == nullptr) {
            return E_OUT_OF_MEMORY_ERROR;
        }
        cid->mUuid = _cid.mUuid;
        cid->mUrl = nullptr;
    }
    else {
        const char* str = (const char*)ReadInplace(size + 1);
        if (str == nullptr) {
            return E_RUNTIME_EXCEPTION;
        }
        cid = (ComponentID*)malloc(sizeof(ComponentID) + size + 1);
        if (cid == nullptr) {
            return E_OUT_OF_MEMORY_ERROR;
        }
        cid->mUuid = _cid.mUuid;
        cid->mUrl = (const char*)((uintptr_t)cid + sizeof(ComponentID));
        memcpy(const_cast<char*>(cid->mUrl), str, size + 1);
    }
    value->mCid = cid;
    return NOERROR;
}

ECode CParcel::WriteCoclassID(
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

ECode CParcel::ReadComponentID(
    /* [out] */ ComponentID* value)
{
    VALIDATE_NOT_NULL(value);

    ECode ec = Read((void*)value, sizeof(ComponentID));
    if (FAILED(ec)) {
        return ec;
    }

    Integer size;
    ec = ReadInteger(&size);
    if (FAILED(ec)) {
        return ec;
    }

    if (size < 0) {
        return E_RUNTIME_EXCEPTION;
    }

    if (size == 0) {
        value->mUrl = nullptr;
        return NOERROR;
    }
    const char* str = (const char*)ReadInplace(size + 1);
    if (str == nullptr) {
        return E_RUNTIME_EXCEPTION;
    }
    value->mUrl = (const char*)malloc(size + 1);
    if (value->mUrl == nullptr) {
        return E_OUT_OF_MEMORY_ERROR;
    }
    memcpy(const_cast<char*>(value->mUrl), str, size + 1);
    return NOERROR;
}

ECode CParcel::WriteComponentID(
    /* [in] */ const ComponentID& value)
{
    ECode ec = Write((void*)&value, sizeof(ComponentID));
    if (FAILED(ec)) {
        return ec;
    }

    Integer size = value.mUrl == nullptr ? 0 : strlen(value.mUrl);
    ec = WriteInteger(size);
    if (size > 0 && SUCCEEDED(ec)) {
        ec = Write(value.mUrl, size + 1);
    }
    return ec;
}

ECode CParcel::ReadInterfaceID(
    /* [out] */ InterfaceID* value)
{
    VALIDATE_NOT_NULL(value);

    ECode ec = Read((void*)value, sizeof(InterfaceID));
    if (FAILED(ec)) {
        return ec;
    }

    Integer tag;
    ec = ReadInteger(&tag);
    if (FAILED(ec)) {
        return ec;
    }

    if (tag == TAG_NULL) {
        value->mCid = nullptr;
        return NOERROR;
    }

    ComponentID _cid;
    ec = Read((void*)&_cid, sizeof(ComponentID));
    if (FAILED(ec)) {
        return ec;
    }

    Integer size;
    ec = ReadInteger(&size);
    if (FAILED(ec)) {
        return ec;
    }

    if (size < 0) {
        return E_RUNTIME_EXCEPTION;
    }

    ComponentID* cid = nullptr;
    if (size == 0) {
        cid = (ComponentID*)malloc(sizeof(ComponentID));
        if (cid == nullptr) {
            return E_OUT_OF_MEMORY_ERROR;
        }
        cid->mUuid = _cid.mUuid;
        cid->mUrl = nullptr;
    }
    else {
        const char* str = (const char*)ReadInplace(size + 1);
        if (str == nullptr) {
            return E_RUNTIME_EXCEPTION;
        }
        cid = (ComponentID*)malloc(sizeof(ComponentID) + size + 1);
        if (cid == nullptr) {
            return E_OUT_OF_MEMORY_ERROR;
        }
        cid->mUuid = _cid.mUuid;
        cid->mUrl = (const char*)((uintptr_t)cid + sizeof(ComponentID));
        memcpy(const_cast<char*>(cid->mUrl), str, size + 1);
    }
    value->mCid = cid;
    return NOERROR;
}

ECode CParcel::WriteInterfaceID(
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

ECode CParcel::ReadECode(
    /* [out] */ ECode* value)
{
    return ReadInteger(value);
}

ECode CParcel::WriteECode(
    /* [in] */ ECode value)
{
    return WriteInteger(value);
}

ECode CParcel::ReadEnumeration(
    /* [out] */ Integer* value)
{
    return ReadInteger(value);
}

ECode CParcel::WriteEnumeration(
    /* [in] */ Integer value)
{
    return WriteInteger(value);
}

ECode CParcel::ReadArray(
    /* [out] */ HANDLE* array)
{
    return NOERROR;
}

ECode CParcel::WriteArray(
    /* [in] */ HANDLE array)
{
    return NOERROR;
}

ECode CParcel::ReadInterface(
    /* [out] */ IInterface** value)
{
    return NOERROR;
}

ECode CParcel::WriteInterface(
    /* [in] */ IInterface* value)
{
    return NOERROR;
}

ECode CParcel::SetDataPosition(
    /* [in] */ Long pos)
{
    if (pos < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mDataPos = pos;
    return NOERROR;
}

ECode CParcel::Read(
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

const void* CParcel::ReadInplace(
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

ECode CParcel::Write(
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

void* CParcel::WriteInplace(
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

ECode CParcel::FinishWrite(
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

ECode CParcel::GrowData(
    /* [in] */ Long len)
{
    if (len < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Long newSize = ((mDataSize + len) * 3) / 2;
    return (newSize <= mDataSize) ?
            E_OUT_OF_MEMORY_ERROR : ContinueWrite(newSize);
}

ECode CParcel::ContinueWrite(
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
ECode CParcel::ReadAligned(
    /* [out] */ T* value) const
{
    if (sizeof(value) == 8) {
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
ECode CParcel::WriteAligned(
    /* [in] */ T value)
{
    Long oldDataPos = mDataPos;
    if (sizeof(value) == 8) {
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

ECode CParcel::CreateObject(
    /* [out] */ IParcel** parcel)
{
    VALIDATE_NOT_NULL(parcel);

    *parcel = new CParcel();
    REFCOUNT_ADD(*parcel);
    return NOERROR;
}

}
