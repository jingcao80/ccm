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
#include <initializer_list>

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

    Array(
        /* [in] */ std::initializer_list<T> list);

    ~Array();

    Long GetLength() const;

    T* GetPayload() const;

    Boolean IsNull() const;

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

    inline Boolean Equals(
        /* [in] */ const Array<T>& other) const;

    inline Boolean operator==(
        /* [in] */ const Array<T>& other) const;

    Array& operator=(
        /* [in] */ const Array<T>& other);

    Array& operator=(
        /* [in] */ std::initializer_list<T> list);

    inline T& operator[](
        /* [in] */ Long index);

    inline const T& operator[](
        /* [in] */ Long index) const;

    T* begin();

    const T* begin() const;

    T* end();

    const T* end() const;

    void Clear();

    Array Clone() const;

    static Array Allocate(
        /* [in] */ Long size);

    static Array Null();

    operator Array<IInterface*>();

private:
    Boolean Alloc(
        /* [in] */ Long size);
};

template<class T>
Boolean Array<T>::Alloc(
    /* [in] */ Long size)
{
    if (size < 0 || size > SIZE_MAX) {
        Logger::E("Array", "Invalid array size %lld", size);
        mData = nullptr;
        mSize = 0;
        return false;
    }

    Long byteSize = sizeof(T) * size;
    SharedBuffer* buf = SharedBuffer::Alloc(byteSize);
    if (buf == nullptr) {
        Logger::E("Array", "Malloc array which size is %lld failed.", byteSize);
        mData = nullptr;
        mSize = 0;
        return false;
    }
    void* data = buf->GetData();
    memset(data, 0, byteSize);

    mData = reinterpret_cast<T*>(data);
    mSize = size;
    return true;
}

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
    mType = Type2Kind<T>::Kind();
    Alloc(size);
}

template<class T>
Array<T>::Array(
    /* [in] */ const Array<T>& other)
{
    if (other.mData != nullptr) {
        SharedBuffer::GetBufferFromData(other.mData)->AddRef();
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
Array<T>::Array(
    /* [in] */ std::initializer_list<T> list)
    : Array(list.size())
{
    mType = Type2Kind<T>::Kind();
    if (Alloc(list.size())) {
        Long i;
        typename std::initializer_list<T>::const_iterator it;
        for (it = list.begin(), i = 0; it != list.end(); ++it, ++i) {
            Set(i, *it);
        }
    }
}

template<class T>
Array<T>::~Array()
{
    if (mData != nullptr) {
        SharedBuffer* sb = SharedBuffer::GetBufferFromData(mData);
        if (sb->OnlyOwner()) {
            DeleteFunc<T> deleteF;
            for (Long i = 0; i < mSize; i++) {
                deleteF(&static_cast<T*>(mData)[i], this);
            }
        }
        sb->Release();
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
Boolean Array<T>::IsNull() const
{
    return mData == nullptr;
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
Boolean Array<T>::Equals(
    /* [in] */ const Array<T>& other) const
{
    return mData == other.mData && mSize == other.mSize &&
            mType == other.mType;
}

template<class T>
Boolean Array<T>::operator==(
    /* [in] */ const Array<T>& other) const
{
    return mData == other.mData && mSize == other.mSize &&
            mType == other.mType;
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
        SharedBuffer* sb = SharedBuffer::GetBufferFromData(mData);
        if (sb->OnlyOwner()) {
            DeleteFunc<T> deleteF;
            for (Long i = 0; i < mSize; i++) {
                deleteF(&static_cast<T*>(mData)[i], this);
            }
        }
        sb->Release();
    }
    mData = other.mData;
    mSize = other.mSize;
    return *this;
}

template<class T>
Array<T>& Array<T>::operator=(
    /* [in] */ std::initializer_list<T> list)
{
    if (mData != nullptr) {
        SharedBuffer* sb = SharedBuffer::GetBufferFromData(mData);
        if (sb->OnlyOwner()) {
            DeleteFunc<T> deleteF;
            for (Long i = 0; i < mSize; i++) {
                deleteF(&static_cast<T*>(mData)[i], this);
            }
        }
        sb->Release();
        mData = nullptr;
        mSize = 0;
    }

    if (Alloc(list.size())) {
        Long i;
        typename std::initializer_list<T>::const_iterator it;
        for (it = list.begin(), i = 0; it != list.end(); ++it, ++i) {
            Set(i, *it);
        }
    }
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
T* Array<T>::begin()
{
    return static_cast<T*>(mData);
}

template<class T>
const T* Array<T>::begin() const
{
    return static_cast<T*>(mData);
}

template<class T>
T* Array<T>::end()
{
    return static_cast<T*>(mData) + mSize;
}

template<class T>
const T* Array<T>::end() const
{
    return static_cast<T*>(mData) + mSize;
}

template<class T>
void Array<T>::Clear()
{
    if (mData != nullptr) {
        SharedBuffer* sb = SharedBuffer::GetBufferFromData(mData);
        if (sb->OnlyOwner()) {
            DeleteFunc<T> deleteF;
            for (Long i = 0; i < mSize; i++) {
                deleteF(&static_cast<T*>(mData)[i], this);
            }
        }
        sb->Release();
        mData = nullptr;
    }
    mSize = 0;
}

template<class T>
Array<T> Array<T>::Clone() const
{
    Array<T> newArray;

    if (!newArray.Alloc(mSize)) {
        return newArray;
    }

    T* src = static_cast<T*>(mData);
    T* des = static_cast<T*>(newArray.mData);
    for (Long i = 0; i < mSize; i++) {
        AssignFunc<T> assignF;
        assignF(&des[i], src[i], des);
    }

    return newArray;
}

template<class T>
Array<T> Array<T>::Allocate(
    /* [in] */ Long size)
{
    return Array<T>(size);
}

template<class T>
Array<T> Array<T>::Null()
{
    return Array<T>();
}

template<class T, Boolean isIInterfaceSubclass>
struct ConvertImpl
{
    Array<IInterface*> operator()(
        /* [in] */ Array<T>* lvalue)
    {
        return Array<IInterface*>::Null();
    }
};

template<class T>
struct ConvertImpl<T, true>
{
    Array<IInterface*> operator()(
        /* [in] */ Array<T>* lvalue)
    {
        return *reinterpret_cast<Array<IInterface*>*>(lvalue);
    }
};

template<class T>
Array<T>::operator Array<IInterface*>()
{
    typedef typename TypeTraits<T>::BareType BareType;
    ConvertImpl<T, SUPERSUBCLASS(IInterface, BareType)> impl;
    return impl(this);
}

}

#endif //__CCM_ARRAY_H__
