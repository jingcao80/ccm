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

#ifndef __CCM_ARRAY_H__
#define __CCM_ARRAY_H__

#include "ccmsharedbuffer.h"
#include "ccmlogger.h"

namespace ccm {

template<class T>
class Array : public Triple
{
public:
    Array();

    Array(
        /* [in] */ Long size);

    Array(
        /* [in] */ const Array<T>& other);

    Array(
        /* [in] */ Array<T>&& other);

    ~Array();

    Long GetLength() const;

    T* GetPayload() const;

    Boolean IsEmpty() const;

    Long Copy(
        /* [in] */ T const* srcData,
        /* [in] */ Long length);

    Long Copy(
        /* [in] */ Long thisPos,
        /* [in] */ T const* srcData,
        /* [in] */ Long length);

    Long Copy(
        /* [in] */ Long thisPos,
        /* [in] */ T const* srcData,
        /* [in] */ Long srcPos,
        /* [in] */ Long length);

    Long Copy(
        /* [in] */ const Array<T>& srcArray);

    Long Copy(
        /* [in] */ const Array<T>& srcArray,
        /* [in] */ Long length);

    Long Copy(
        /* [in] */ const Array<T>& srcArray,
        /* [in] */ Long srcPos,
        /* [in] */ Long length);

    Long Copy(
        /* [in] */ Long thisPos,
        /* [in] */ const Array<T>& srcArray);

    Long Copy(
        /* [in] */ Long thisPos,
        /* [in] */ const Array<T>& srcArray,
        /* [in] */ Long length);

    Long Copy(
        /* [in] */ Long thisPos,
        /* [in] */ const Array<T>& srcArray,
        /* [in] */ Long srcPos,
        /* [in] */ Long length);

    void Set(
        /* [in] */ Long index,
        /* [in] */ T value);

    Array& operator=(
        /* [in] */ const Array<T>& other);

    inline T& operator[](
        /* [in] */ Long index);

    inline const T& operator[](
        /* [in] */ Long index) const;

    void Clear();

    Array Clone() const;
};

template<class T>
Array<T>::Array()
{
    mData = nullptr;
    mSize = 0;
    mType = Type2Kind<T>::Kind();
}

template<class T>
Array<T>::Array(
    /* [in] */ Long size)
{
    if (size < 0 || size > SIZE_MAX) {
        Logger::E("Array", "Invalid array size %lld", size);
        mData = nullptr;
        mSize = 0;
        mType = Type2Kind<T>::Kind();
        return;
    }

    Long byteSize = sizeof(T) * size;
    SharedBuffer* buf = SharedBuffer::Alloc(byteSize);
    if (buf == nullptr) {
        Logger::E("Array", "Malloc array which size is %lld failed.", byteSize);
        mData = nullptr;
        mSize = 0;
        mType = Type2Kind<T>::Kind();
        return;
    }
    void* data = buf->GetData();
    memset(data, 0, byteSize);

    mData = reinterpret_cast<T*>(data);
    mSize = size;
    mType = Type2Kind<T>::Kind();
}

template<class T>
Array<T>::Array(
    /* [in] */ const Array<T>& other)
{
    if (other.mData != nullptr) {
        SharedBuffer::GetBufferFromData(other.mData)->AddRef();
    }
    if (mData != nullptr) {
        SharedBuffer::GetBufferFromData(mData)->Release();
    }
    mData = other.mData;
    mSize = other.mSize;
}

template<class T>
Array<T>::Array(
    /* [in] */ Array<T>&& other)
{
    mData = other.mData;
    mSize = other.mSize;
    other.mData = nullptr;
    other.mSize = 0;
}

template<class T>
Array<T>::~Array()
{
    if (mData != nullptr) {
        DeleteFunc<T> deleteF;
        for (Long i = 0; i < mSize; i++) {
            deleteF(&static_cast<T*>(mData)[i], this);
        }
        SharedBuffer::GetBufferFromData(mData)->Release();
        mData = nullptr;
    }
    mSize = 0;
}

template<class T>
Long Array<T>::GetLength() const
{
    return mSize;
}

template<class T>
T* Array<T>::GetPayload() const
{
    return static_cast<T*>(mData);
}

template<class T>
Boolean Array<T>::IsEmpty() const
{
    return mSize == 0;
}

template<class T>
Long Array<T>::Copy(
    /* [in] */ T const* srcData,
    /* [in] */ Long length)
{
    if (srcData == nullptr) return 0;

    Long N = MIN(mSize, length);
    T* array = static_cast<T*>(mData);
    for (Long i = 0; i < N; i++) {
        AssignFunc<T> assignF;
        assignF(&array[i], srcData[i], mData);
    }
    return N;
}

template<class T>
Long Array<T>::Copy(
    /* [in] */ Long thisPos,
    /* [in] */ T const* srcData,
    /* [in] */ Long length)
{
    if (srcData == nullptr) return 0;

    Long N = MIN(mSize - thisPos, length);
    T* array = static_cast<T*>(mData) + thisPos;
    for (Long i = 0; i < N; i++) {
        AssignFunc<T> assignF;
        assignF(&array[i], srcData[i], mData);
    }
    return N;
}

template<class T>
Long Array<T>::Copy(
    /* [in] */ Long thisPos,
    /* [in] */ T const* srcData,
    /* [in] */ Long srcPos,
    /* [in] */ Long length)
{
    if (srcData == nullptr) return 0;

    Long N = MIN(mSize - thisPos, length);
    T* array = static_cast<T*>(mData) + thisPos;
    srcData += srcPos;
    for (Long i = 0; i < N; i++) {
        AssignFunc<T> assignF;
        assignF(&array[i], srcData[i], mData);
    }
    return N;
}

template<class T>
Long Array<T>::Copy(
    /* [in] */ const Array<T>& srcArray)
{
    if (srcArray.mData == nullptr ||
            srcArray.mSize == 0) {
        return 0;
    }

    Long N = MIN(mSize, srcArray.mSize);
    T* array = static_cast<T*>(mData);
    for (Long i = 0; i < N; i++) {
        AssignFunc<T> assignF;
        assignF(&array[i], srcArray[i], mData);
    }
    return N;
}

template<class T>
Long Array<T>::Copy(
    /* [in] */ const Array<T>& srcArray,
    /* [in] */ Long length)
{
    if (srcArray.mData == nullptr ||
            srcArray.mSize == 0) {
        return 0;
    }

    Long N = MIN(mSize, MIN(srcArray.mSize, length));
    T* array = static_cast<T*>(mData);
    for (Long i = 0; i < N; i++) {
        AssignFunc<T> assignF;
        assignF(&array[i], srcArray[i], mData);
    }
    return N;
}

template<class T>
Long Array<T>::Copy(
    /* [in] */ const Array<T>& srcArray,
    /* [in] */ Long srcPos,
    /* [in] */ Long length)
{
    if (srcArray.mData == nullptr ||
            srcArray.mSize == 0) {
        return 0;
    }

    Long N = MIN(mSize, MIN(srcArray.mSize - srcPos, length));
    T* array = static_cast<T*>(mData);
    for (Long i = 0; i < N; i++) {
        AssignFunc<T> assignF;
        assignF(&array[i], srcArray[srcPos + i], mData);
    }
    return N;
}

template<class T>
Long Array<T>::Copy(
    /* [in] */ Long thisPos,
    /* [in] */ const Array<T>& srcArray)
{
    if (srcArray.mData == nullptr ||
            srcArray.mSize == 0) {
        return 0;
    }

    Long N = MIN(mSize - thisPos, srcArray.mSize);
    T* array = static_cast<T*>(mData) + thisPos;
    for (Long i = 0; i < N; i++) {
        AssignFunc<T> assignF;
        assignF(&array[i], srcArray[i], mData);
    }
    return N;
}

template<class T>
Long Array<T>::Copy(
    /* [in] */ Long thisPos,
    /* [in] */ const Array<T>& srcArray,
    /* [in] */ Long length)
{
    if (srcArray.mData == nullptr ||
            srcArray.mSize == 0) {
        return 0;
    }

    Long N = MIN(mSize - thisPos, MIN(srcArray.mSize, length));
    T* array = static_cast<T*>(mData) + thisPos;
    for (Long i = 0; i < N; i++) {
        AssignFunc<T> assignF;
        assignF(&array[i], srcArray[i], mData);
    }
    return N;
}

template<class T>
Long Array<T>::Copy(
    /* [in] */ Long thisPos,
    /* [in] */ const Array<T>& srcArray,
    /* [in] */ Long srcPos,
    /* [in] */ Long length)
{
    if (srcArray.mData == nullptr ||
            srcArray.mSize == 0) {
        return 0;
    }

    Long N = MIN(mSize - thisPos, MIN(srcArray.mSize - srcPos, length));
    T* array = static_cast<T*>(mData) + thisPos;
    for (Long i = 0; i < N; i++) {
        AssignFunc<T> assignF;
        assignF(&array[i], srcArray[srcPos + i], mData);
    }
    return N;
}

template<class T>
void Array<T>::Set(
    /* [in] */ Long index,
    /* [in] */ T value)
{
    T* array = static_cast<T*>(mData);
    AssignFunc<T> assignF;
    assignF(&array[index], value, mData);
}

template<class T>
Array<T>& Array<T>::operator=(
    /* [in] */ const Array<T>& other)
{
    if (mData == other.mData) {
        return *this;
    }

    if (other.mData != nullptr) {
        SharedBuffer::GetBufferFromData(other.mData)->AddRef();
    }
    if (mData != nullptr) {
        SharedBuffer::GetBufferFromData(mData)->Release();
    }
    mData = other.mData;
    mSize = other.mSize;
    return *this;
}

template<class T>
T& Array<T>::operator[](
    /* [in] */ Long index)
{
    T* array = static_cast<T*>(mData);
    return array[index];
}

template<class T>
const T& Array<T>::operator[](
    /* [in] */ Long index) const
{
    T* array = static_cast<T*>(mData);
    return array[index];
}

template<class T>
void Array<T>::Clear()
{
    if (mData != nullptr) {
        SharedBuffer::GetBufferFromData(mData)->Release();
    }
    mData = nullptr;
    mSize = 0;
    mType = CcmTypeKind::Unknown;
}

template<class T>
Array<T> Array<T>::Clone() const
{
    Array<T> newArray;

    Long byteSize = sizeof(T) * mSize;
    SharedBuffer* buf = SharedBuffer::Alloc(byteSize);
    if (buf == nullptr) {
        Logger::E("Array", "Malloc array which size is %lld failed.", byteSize);
        return newArray;
    }
    void* data = buf->GetData();
    memset(data, 0, byteSize);

    T* src = static_cast<T*>(mData);
    T* des = static_cast<T*>(data);
    for (Long i = 0; i < mSize; i++) {
        AssignFunc<T> assignF;
        assignF(&des[i], src[i], data);
    }

    newArray.mData = data;
    newArray.mSize = mSize;
    newArray.mType = mType;

    return newArray;
}

}

#endif //__CCM_ARRAY_H__
