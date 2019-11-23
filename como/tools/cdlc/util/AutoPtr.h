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

#ifndef __CDLC_AUTOPTR_H__
#define __CDLC_AUTOPTR_H__

namespace cdlc {

template<class T>
class AutoPtr
{
public:
    AutoPtr();

    AutoPtr(
        /* [in] */ T* other);

    AutoPtr(
        /* [in] */ const AutoPtr<T>& other);

    template<class U>
    AutoPtr(
        /* [in] */ const AutoPtr<U>& other);

    AutoPtr(
        /* [in] */ AutoPtr<T>&& other);

    template<class U>
    AutoPtr(
        /* [in] */ AutoPtr<U>&& other);

    ~AutoPtr();

    AutoPtr& operator=(
        /* [in] */ T* other);

    AutoPtr& operator=(
        /* [in] */ const AutoPtr<T>& other);

    template<class U>
    AutoPtr& operator=(
        /* [in] */ const AutoPtr<U>& other);

    AutoPtr& operator=(
        /* [in] */ AutoPtr<T>&& other);

    template<class U>
    AutoPtr& operator=(
        /* [in] */ AutoPtr<U>&& other);

    inline operator T*() const;

    inline T* operator->() const;

    inline T& operator*() const;

    inline T* Get() const;

    inline bool operator==(
        /* [in] */ T* other) const;

    inline bool operator==(
        /* [in] */ const AutoPtr<T>& other) const;

    template<class U>
    inline bool operator==(
        /* [in] */ const AutoPtr<U>& other) const;

    inline bool operator!=(
        /* [in] */ T* other) const;

    inline bool operator!=(
        /* [in] */ const AutoPtr<T>& other) const;

    template<class U>
    inline bool operator!=(
        /* [in] */ const AutoPtr<U>& other) const;

private:
    template<class U> friend class AutoPtr;

    T* mPtr = nullptr;
};

template<class T>
AutoPtr<T>::AutoPtr()
{}

template<class T>
AutoPtr<T>::AutoPtr(
    /* [in] */ T* other)
    : mPtr(other)
{
    if (mPtr != nullptr) {
        mPtr->AddRef();
    }
}

template<class T>
AutoPtr<T>::AutoPtr(
    /* [in] */ const AutoPtr<T>& other)
    : mPtr(other.mPtr)
{
    if (mPtr != nullptr) {
        mPtr->AddRef();
    }
}

template<class T> template<class U>
AutoPtr<T>::AutoPtr(
    /* [in] */ const AutoPtr<U>& other)
    : mPtr((T*)other.mPtr)
{
    if (mPtr != nullptr) {
        mPtr->AddRef();
    }
}

template<class T>
AutoPtr<T>::AutoPtr(
    /* [in] */ AutoPtr<T>&& other)
    : mPtr(other.mPtr)
{
    other.mPtr = nullptr;
}

template<class T> template<class U>
AutoPtr<T>::AutoPtr(
    /* [in] */ AutoPtr<U>&& other)
    : mPtr((T*)other.mPtr)
{
    other.mPtr = nullptr;
}

template<class T>
AutoPtr<T>::~AutoPtr()
{
    if (mPtr != nullptr) {
        mPtr->Release();
    }
}

template<class T>
AutoPtr<T>& AutoPtr<T>::operator=(
    /* [in] */ T* other)
{
    if (mPtr == other) {
        return *this;
    }

    if (other != nullptr) {
        other->AddRef();
    }
    if (mPtr != nullptr) {
        mPtr->Release();
    }
    mPtr = other;
    return *this;
}

template<class T>
AutoPtr<T>& AutoPtr<T>::operator=(
    /* [in] */ const AutoPtr<T>& other)
{
    if (mPtr == other.mPtr) {
        return *this;
    }

    if (other.mPtr != nullptr) {
        other.mPtr->AddRef();
    }
    if (mPtr != nullptr) {
        mPtr->Release();
    }
    mPtr = other.mPtr;
    return *this;
}

template<class T> template<class U>
AutoPtr<T>& AutoPtr<T>::operator=(
    /* [in] */ const AutoPtr<U>& other)
{
    if (mPtr == (T*)other.mPtr) {
        return *this;
    }

    if (other.mPtr != nullptr) {
        other.mPtr->AddRef();
    }
    if (mPtr != nullptr) {
        mPtr->Release();
    }
    mPtr = (T*)other.mPtr;
    return *this;
}

template<class T>
AutoPtr<T>& AutoPtr<T>::operator=(
    /* [in] */ AutoPtr<T>&& other)
{
    if (mPtr != nullptr) {
        mPtr->Release();
    }
    mPtr = other.mPtr;
    other.mPtr = nullptr;
    return *this;
}

template<class T> template<class U>
AutoPtr<T>& AutoPtr<T>::operator=(
    /* [in] */ AutoPtr<U>&& other)
{
    if (mPtr != nullptr) {
        mPtr->Release();
    }
    mPtr = (T*)other.mPtr;
    other.mPtr = nullptr;
    return *this;
}

template<class T>
AutoPtr<T>::operator T*() const
{
    return mPtr;
}

template<class T>
T* AutoPtr<T>::operator->() const
{
    return mPtr;
}

template<class T>
T& AutoPtr<T>::operator*() const
{
    return *mPtr;
}

template<class T>
T* AutoPtr<T>::Get() const
{
    return mPtr;
}

template<class T>
bool AutoPtr<T>::operator==(
    /* [in] */ T* other) const
{
    return mPtr == other;
}

template<class T>
bool AutoPtr<T>::operator==(
    /* [in] */ const AutoPtr<T>& other) const
{
    return mPtr == other.mPtr;
}

template<class T> template<class U>
bool AutoPtr<T>::operator==(
    /* [in] */ const AutoPtr<U>& other) const
{
    return mPtr == (T*)other.mPtr;
}

template<class T>
bool AutoPtr<T>::operator!=(
    /* [in] */ T* other) const
{
    return mPtr != other;
}

template<class T>
bool AutoPtr<T>::operator!=(
    /* [in] */ const AutoPtr<T>& other) const
{
    return mPtr != other.mPtr;
}

template<class T> template<class U>
bool AutoPtr<T>::operator!=(
    /* [in] */ const AutoPtr<U>& other) const
{
    return mPtr != (T*)other.mPtr;
}

}

#endif // __CDLC_AUTOPTR_H__
