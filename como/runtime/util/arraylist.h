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

#ifndef __COMO_ARRAYLIST_H__
#define __COMO_ARRAYLIST_H__

#include "comotypes.h"
#include "comolog.h"
#include <cstdlib>

namespace como {

template<class T>
class ArrayList
{
public:
    ArrayList(
        /* [in] */ Long initCap = 10);

    ~ArrayList();

    Boolean Add(
        /* [in] */ const T& data);

    T Get(
        /* [in] */ Long index);

    void Remove(
        /* [in] */ Long index);

    inline Long GetSize();

private:
    Boolean EnsureCapacity();

private:
    Long mCapacity;
    Long mIndex;
    T* mData;
};

template<class T>
ArrayList<T>::ArrayList(
    /* [in] */ Long initCap)
    : mCapacity(initCap)
    , mIndex(0)
    , mData(nullptr)
{
    if (mCapacity <= 0) {
        mCapacity = 10;
    }
    mData = (T*)calloc(sizeof(T), mCapacity);
}

template<class T>
ArrayList<T>::~ArrayList()
{
    DeleteFunc<T> deleteF;
    for (Long i = 0; i < mIndex; i++) {
        deleteF(&mData[i], this);
    }
    free(mData);
}

template<class T>
Boolean ArrayList<T>::Add(
    /* [in] */ const T& data)
{
    if (!EnsureCapacity()) {
        return false;
    }

    AssignFunc<T> assignF;
    assignF(&mData[mIndex], data, this);
    mIndex++;
    return true;
}

template<class T>
T ArrayList<T>::Get(
    /* [in] */ Long index)
{
    if (index < 0 || index >= mIndex) {
        return nullptr;
    }
    return mData[index];
}

template<class T>
void ArrayList<T>::Remove(
    /* [in] */ Long index)
{
    if (index < 0 || index >= mIndex) {
        return;
    }
    DeleteFunc<T> deleteF;
    deleteF(&mData[index], this);
    for (Long i = index; i < mIndex - 1; i++) {
        mData[i] = mData[i + 1];
    }
    mData[mIndex - 1] = nullptr;
    mIndex--;
}

template<class T>
Long ArrayList<T>::GetSize()
{
    return mIndex;
}

template<class T>
Boolean ArrayList<T>::EnsureCapacity()
{
    if (mIndex < mCapacity) {
        return true;
    }

    Long newCap = mCapacity * 2;
    T* newData = (T*)calloc(sizeof(T), newCap);
    if (newData == nullptr) {
        Logger::E("ArrayList", "Enlarge to size %lld failed.", newCap);
        return false;
    }

    memcpy(newData, mData, sizeof(T) * mCapacity);
    free(mData);
    mData = newData;
    mCapacity = newCap;
    return true;
}

} // namespace como

#endif // __COMO_ARRAYLIST_H__
