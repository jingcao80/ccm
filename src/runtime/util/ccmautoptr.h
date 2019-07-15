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

#ifndef __CCM_AUTOPTR_H__
#define __CCM_AUTOPTR_H__

#include "ccmtypes.h"
#include <utility>

namespace ccm {

template<class T>
class AutoPtr
{
public:
    inline AutoPtr()
        : mPtr(nullptr)
    {}

    AutoPtr(
        /* [in] */ T* other);

    AutoPtr(
        /* [in] */ const AutoPtr<T>& other);

    AutoPtr(
        /* [in] */ AutoPtr<T>&& other);

    ~AutoPtr();

    AutoPtr& operator=(
        /* [in] */ T* other);

    AutoPtr& operator=(
        /* [in] */ const AutoPtr<T>& other);

    AutoPtr& operator=(
        /* [in] */ AutoPtr<T>&& other);

    template<class U>
    AutoPtr& operator=(
        /* [in] */ AutoPtr<U>&& other);

    void MoveTo(
        /* [out] */ T** other);

    template<class U>
    void MoveTo(
        /* [out] */ U** other);

    inline operator T*() const;

    inline T** operator&();

    inline T* operator->() const;

    inline T& operator*() const;

    inline T* Get() const;

    inline Boolean operator==(
        /* [in] */ T* other) const;

    inline Boolean operator==(
        /* [in] */ const AutoPtr<T>& other) const;

    inline Boolean operator!=(
        /* [in] */ T* other) const;

    inline Boolean operator!=(
        /* [in] */ const AutoPtr<T>& other) const;

    inline Boolean operator>(
        /* [in] */ T* other) const;

    inline Boolean operator>(
        /* [in] */ const AutoPtr<T>& other) const;

    inline Boolean operator<(
        /* [in] */ T* other) const;

    inline Boolean operator<(
        /* [in] */ const AutoPtr<T>& other) const;

    inline Boolean operator<=(
        /* [in] */ T* other) const;

    inline Boolean operator<=(
        /* [in] */ const AutoPtr<T>& other) const;

    inline Boolean operator>=(
        /* [in] */ T* other) const;

    inline Boolean operator>=(
        /* [in] */ const AutoPtr<T>& other) const;

private:
    template<class U, class Y, Boolean isSuperSubclass> friend struct MoveAssignImpl;
    template<class U, class Y, Boolean isSuperSubclass> friend struct MoveToImpl;

    T* mPtr;
};

template<class U, class Y, Boolean isSuperSubclass>
struct MoveAssignImpl
{
    AutoPtr<U>& operator()(
        /* [in] */ AutoPtr<U>* lvalue,
        /* [in] */ AutoPtr<Y>&& rvalue)
    {
        U* uObj = U::Probe(rvalue.mPtr);
        if (uObj != nullptr) {
            if (lvalue->mPtr != nullptr) {
                lvalue->mPtr->Release(reinterpret_cast<HANDLE>(this));
            }
            lvalue->mPtr = uObj;
            rvalue.mPtr = nullptr;
        }
        return *lvalue;
    }
};

template<class U, class Y>
struct MoveAssignImpl<U, Y, true>
{
    AutoPtr<U>& operator()(
        /* [in] */ AutoPtr<U>* lvalue,
        /* [in] */ AutoPtr<Y>&& rvalue)
    {
        if (lvalue->mPtr != nullptr) {
            lvalue->mPtr->Release(reinterpret_cast<HANDLE>(this));
        }
        lvalue->mPtr = (U*)rvalue.mPtr;
        rvalue.mPtr = nullptr;
        return *lvalue;
    }
};

template<class U, class Y, Boolean isSubSuperclass>
struct MoveToImpl
{
    void operator()(
        /* [in] */ AutoPtr<U>* uObj,
        /* [in] */ Y** other)
    {
        if (other != nullptr) {
            Y* yObj = Y::Probe(uObj->mPtr);
            if (yObj != nullptr) {
                *other = yObj;
                uObj->mPtr = nullptr;
            }
        }
    }
};

template<class U, class Y>
struct  MoveToImpl<U, Y, true>
{
    void operator()(
        /* [in] */ AutoPtr<U>* uObj,
        /* [in] */ Y** other)
    {
        if (other != nullptr) {
            *other = (Y*)uObj->mPtr;
            uObj->mPtr = nullptr;
        }
    }
};

template<class T>
AutoPtr<T>::AutoPtr(
    /* [in] */ T* other)
    : mPtr(other)
{
    if (mPtr != nullptr) {
        mPtr->AddRef(reinterpret_cast<HANDLE>(this));
    }
}

template<class T>
AutoPtr<T>::AutoPtr(
    /* [in] */ const AutoPtr<T>& other)
    : mPtr(other.mPtr)
{
    if (mPtr != nullptr) {
        mPtr->AddRef(reinterpret_cast<HANDLE>(this));
    }
}

template<class T>
AutoPtr<T>::AutoPtr(
    /* [in] */ AutoPtr<T>&& other)
    : mPtr(other.mPtr)
{
    other.mPtr = nullptr;
}

template<class T>
AutoPtr<T>::~AutoPtr()
{
    if (mPtr != nullptr) {
        mPtr->Release(reinterpret_cast<HANDLE>(this));
    }
}

template<class T>
AutoPtr<T>& AutoPtr<T>::operator=(
    /* [in] */ T* other)
{
    if (mPtr == other) return *this;

    if (other != nullptr) {
        other->AddRef(reinterpret_cast<HANDLE>(this));
    }
    if (mPtr != nullptr) {
        mPtr->Release(reinterpret_cast<HANDLE>(this));
    }
    mPtr = other;
    return *this;
}

template<class T>
AutoPtr<T>& AutoPtr<T>::operator=(
    /* [in] */ const AutoPtr<T>& other)
{
    if (mPtr == other.mPtr) return *this;

    if (other.mPtr != nullptr) {
        other.mPtr->AddRef(reinterpret_cast<HANDLE>(this));
    }
    if (mPtr != nullptr) {
        mPtr->Release(reinterpret_cast<HANDLE>(this));
    }
    mPtr = other.mPtr;
    return *this;
}

template<class T>
AutoPtr<T>& AutoPtr<T>::operator=(
    /* [in] */ AutoPtr<T>&& other)
{
    if (mPtr != nullptr) {
        mPtr->Release(reinterpret_cast<HANDLE>(this));
    }
    mPtr = other.mPtr;
    other.mPtr = nullptr;
    return *this;
}

template<class T> template<class U>
AutoPtr<T>& AutoPtr<T>::operator=(
    /* [in] */ AutoPtr<U>&& other)
{
    MoveAssignImpl<T, U, SUPERSUBCLASS(T, U)> impl;
    return impl(this, std::move(other));
}

template<class T>
void AutoPtr<T>::MoveTo(
    /* [out] */ T** other)
{
    if (other != nullptr) {
        *other = mPtr;
        mPtr = nullptr;
    }
}

template<class T> template<class U>
void AutoPtr<T>::MoveTo(
    /* [out] */ U** other)
{
    MoveToImpl<T, U, SUPERSUBCLASS(U, T)> impl;
    return impl(this, other);
}

template<class T>
AutoPtr<T>::operator T*() const
{
    return mPtr;
}

template<class T>
T** AutoPtr<T>::operator&()
{
    return &mPtr;
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
Boolean AutoPtr<T>::operator==(
    /* [in] */ T* other) const
{
    return mPtr == other;
}

template<class T>
Boolean AutoPtr<T>::operator==(
    /* [in] */ const AutoPtr<T>& other) const
{
    return mPtr == other.mPtr;
}

template<class T>
Boolean AutoPtr<T>::operator!=(
    /* [in] */ T* other) const
{
    return mPtr != other;
}

template<class T>
Boolean AutoPtr<T>::operator!=(
    /* [in] */ const AutoPtr<T>& other) const
{
    return mPtr != other.mPtr;
}

template<class T>
Boolean AutoPtr<T>::operator>(
    /* [in] */ T* other) const
{
    return mPtr > other;
}

template<class T>
Boolean AutoPtr<T>::operator>(
    /* [in] */ const AutoPtr<T>& other) const
{
    return mPtr > other.mPtr;
}

template<class T>
Boolean AutoPtr<T>::operator<(
    /* [in] */ T* other) const
{
    return mPtr < other;
}

template<class T>
Boolean AutoPtr<T>::operator<(
    /* [in] */ const AutoPtr<T>& other) const
{
    return mPtr < other.mPtr;
}

template<class T>
Boolean AutoPtr<T>::operator<=(
    /* [in] */ T* other) const
{
    return mPtr <= other;
}

template<class T>
Boolean AutoPtr<T>::operator<=(
    /* [in] */ const AutoPtr<T>& other) const
{
    return mPtr <= other.mPtr;
}

template<class T>
Boolean AutoPtr<T>::operator>=(
    /* [in] */ T* other) const
{
    return mPtr >= other;
}

template<class T>
Boolean AutoPtr<T>::operator>=(
    /* [in] */ const AutoPtr<T>& other) const
{
    return mPtr >= other.mPtr;
}

}

#endif //__CCM_AUTOPTR_H__
