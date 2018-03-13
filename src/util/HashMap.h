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

#ifndef __CCM_HASHMAP_H__
#define __CCM_HASHMAP_H__

#include "String.h"

namespace ccm {

template<class T>
class HashMap
{
public:
    HashMap()
    {
        memset(mBuckets, 0, sizeof(Bucket*) * BUCKET_SIZE);
    }

    ~HashMap()
    {
        for (int i = 0; i < BUCKET_SIZE; i++) {
            if (mBuckets[i] != nullptr) {
                Bucket* curr = mBuckets[i];
                while (curr != nullptr) {
                    Bucket* next = curr->mNext;
                    delete curr;
                    curr = next;
                }
            }
        }
    }

    void Put(
        /* [in] */ const String& key,
        /* [in] */ T value)
    {
        if (key.IsNull()) return;

        int index = HashString(key) % BUCKET_SIZE;
        if (mBuckets[index] == nullptr) {
            Bucket* b = new Bucket();
            b->mKey = key;
            b->mValue = value;
            mBuckets[index] = b;
            return;
        }
        else {
            Bucket* prev = mBuckets[index];
            while (prev != nullptr) {
                if (prev->mKey.Equals(key)) {
                    prev->mValue = value;
                    return;
                }
                else if (prev->mNext == nullptr) {
                    break;
                }
                prev = prev->mNext;
            }
            Bucket* b = new Bucket();
            b->mKey = key;
            b->mValue = value;
            prev->mNext = b;
            return;
        }
    }

    bool ContainsKey(
        /* [in] */ const String& key)
    {
        if (key.IsNull()) return false;

        int index = HashString(key) % BUCKET_SIZE;
        Bucket* curr = mBuckets[index];
        while (curr != nullptr) {
            if (curr->mKey.Equals(key)) return true;
            curr = curr->mNext;
        }

        return false;
    }

    T Get(
        /* [in] */ const String& key)
    {
        if (key.IsNull()) {
            return T(0);
        }

        int index = HashString(key) % BUCKET_SIZE;
        Bucket* curr = mBuckets[index];
        while (curr != nullptr) {
            if (curr->mKey.Equals(key)) return curr->mValue;
            curr = curr->mNext;
        }

        return T(0);
    }

private:
    int HashString(
        /* [in] */ const String& key)
    {
        // BKDR Hash Function
        int seed = 31; // 31 131 1313 13131 131313 etc..
        unsigned int hash = 0;

        const char* string = key.string();
        if (string) {
            for ( ; *string; ++string) {
                hash = hash * seed + (*string);
            }
        }
        return (hash & 0x7FFFFFFF);
    }

    struct Bucket
    {
        Bucket()
            : mNext(nullptr)
        {}

        ~Bucket()
        {
            mNext = nullptr;
        }

        String mKey;
        T mValue;
        struct Bucket* mNext;
    };

private:
    static constexpr int BUCKET_SIZE = 23;
    Bucket* mBuckets[BUCKET_SIZE];
};

}

#endif // __CCM_HASHMAP_H__
