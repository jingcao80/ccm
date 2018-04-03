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

namespace ccm {

template<class T>
class AutoPtr
{
public:
    inline AutoPtr()
        : mPtr(nullptr)
    {}

    AutoPtr(
        /* [in] */ T* other,
        /* [in] */ const void* id = nullptr);

    AutoPtr(
        /* [in] */ const AutoPtr<T>& other,
        /* [in] */ const void* id = nullptr);

    AutoPtr(
        /* [in] */ AutoPtr<T>&& other);

    inline T* operator->() const
    { return mPtr; }

private:
    T* mPtr;
};

template<class T>
AutoPtr<T>::AutoPtr(
    /* [in] */ T* other,
    /* [in] */ const void* id)
    : mPtr(other)
{
    if (mPtr != nullptr) {
        mPtr->AddRef(id);
    }
}

template<class T>
AutoPtr<T>::AutoPtr(
    /* [in] */ const AutoPtr<T>& other,
    /* [in] */ const void* id)
    : mPtr(other.mPtr)
{
    if (mPtr != nullptr) {
        mPtr->AddRef(id);
    }
}

template<class T>
AutoPtr<T>::AutoPtr(
    /* [in] */ AutoPtr<T>&& other)
    : mPtr(other.mPtr)
{
    other.mPtr = nullptr;
}

}

#endif //__CCM_AUTOPTR_H__
