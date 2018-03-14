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

#include <stdlib.h>

namespace ccm {

static const int prime_list[11] =
{
    5ul,    11ul,   23ul,   53ul,   97ul,   193ul,
    389ul,  769ul,  1543ul, 3079ul, 6151ul
};

static int get_lower_bound(const int* first, const int* last, int n)
{
    if (n < *first) return *first;
    if (n > *last) return *last;
    for (int i = 0; first + i != last; i++) {
        int l = *(first + i);
        int r = *(first + i + 1);
        if (l < n && n < r) {
            return (n - l) < (r - n)? l : r;
        }
    }
}

static int get_next_prime(int n)
{
    return get_lower_bound(&prime_list[0], &prime_list[10], n);
}

template<class T>
class HashMap
{
public:
    HashMap(
        /* [in] */ int size = 50)
    {
        mBucketSize = get_next_prime(size);
        mBuckets = (Bucket**)calloc(sizeof(Bucket*), mBucketSize);
    }

    ~HashMap()
    {
        for (int i = 0; i < mBucketSize; i++) {
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
        int index = HashString(key) % mBucketSize;
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

        int index = HashString(key) % mBucketSize;
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

        int index = HashString(key) % mBucketSize;
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
    int mBucketSize;
    Bucket** mBuckets;
};

}

#endif // __CCM_HASHMAP_H__
