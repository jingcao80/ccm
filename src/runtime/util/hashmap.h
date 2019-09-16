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

#ifndef __CCM_HASHMAP_H__
#define __CCM_HASHMAP_H__

#include "type/ccmarray.h"
#include <stdlib.h>
#include <string.h>

namespace ccm {

static const int prime_list[11] =
{
    5ul,    11ul,   23ul,   53ul,   97ul,   193ul,
    389ul,  769ul,  1543ul, 3079ul, 6151ul
};

static int get_lower_bound(const int* first, const int* last, int n)
{
    if (n <= *first) return *first;
    if (n >= *last) return *last;
    for (int i = 0; first + i != last; i++) {
        int l = *(first + i);
        int r = *(first + i + 1);
        if (l <= n && n < r) {
            return (n - l) < (r - n) ? l : r;
        }
    }
}

inline int get_next_prime(int n)
{
    return get_lower_bound(&prime_list[0], &prime_list[10], n);
}

template<class Key, class Val>
class HashMap
{
private:
    struct Bucket
    {
        Bucket()
            : mNext(nullptr)
        {
            InitFunc<Key> initKeyF;
            InitFunc<Val> initValF;
            initKeyF(&mKey, this);
            initValF(&mValue, this);
        }

        ~Bucket()
        {
            DeleteFunc<Key> deleteKeyF;
            DeleteFunc<Val> deleteValF;
            deleteKeyF(&mKey, this);
            deleteValF(&mValue, this);
            mNext = nullptr;
        }

        int mHash;
        Key mKey;
        Val mValue;
        struct Bucket* mNext;
    };

public:
    HashMap(
        /* [in] */ int size = 50)
        : mCount(0)
    {
        mBucketSize = get_next_prime(size);
        mBuckets = (Bucket**)calloc(sizeof(Bucket*), mBucketSize);
        mThreshold = mBucketSize * LOAD_FACTOR;
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
                mBuckets[i] = nullptr;
            }
        }
        free(mBuckets);
    }

    void Put(
        /* [in] */ const Key& key,
        /* [in] */ Val value)
    {
        CompareFunc<Key> compareF;
        AssignFunc<Key> assignKeyF;
        AssignFunc<Val> assignValF;

        if (mCount >= mThreshold) {
            Rehash();
        }

        int hash = HashKey(key);
        if (hash == -1) return;

        int index = (unsigned int)hash % mBucketSize;
        if (mBuckets[index] == nullptr) {
            Bucket* b = new Bucket();
            b->mHash = hash;
            assignKeyF(&b->mKey, key, this);
            assignValF(&b->mValue, value, this);
            mBuckets[index] = b;
            mCount++;
            return;
        }
        else {
            Bucket* prev = mBuckets[index];
            while (prev != nullptr) {
                if (!compareF(prev->mKey, key)) {
                    assignValF(&prev->mValue, value, this);
                    return;
                }
                else if (prev->mNext == nullptr) {
                    break;
                }
                prev = prev->mNext;
            }
            Bucket* b = new Bucket();
            b->mHash = hash;
            assignKeyF(&b->mKey, key, this);
            assignValF(&b->mValue, value, this);
            prev->mNext = b;
            mCount++;
            return;
        }
    }

    bool ContainsKey(
        /* [in] */ const Key& key)
    {
        CompareFunc<Key> compareF;

        int hash = HashKey(key);
        if (hash == -1) return false;

        int index = (unsigned int)hash % mBucketSize;
        Bucket* curr = mBuckets[index];
        while (curr != nullptr) {
            if (curr->mHash == hash && !compareF(curr->mKey, key)) return true;
            curr = curr->mNext;
        }

        return false;
    }

    Val Get(
        /* [in] */ const Key& key)
    {
        CompareFunc<Key> compareF;

        int hash = HashKey(key);
        if (hash == -1) return Val(0);

        int index = (unsigned int)hash % mBucketSize;
        Bucket* curr = mBuckets[index];
        while (curr != nullptr) {
            if (curr->mHash == hash && !compareF(curr->mKey, key)) return curr->mValue;
            curr = curr->mNext;
        }

        return Val(0);
    }

    void Remove(
        /* [in] */ const Key& key)
    {
        CompareFunc<Key> compareF;

        int hash = HashKey(key);
        if (hash == -1) return;

        int index = (unsigned int)hash % mBucketSize;
        Bucket* curr = mBuckets[index];
        Bucket* prev = curr;
        while (curr != nullptr) {
            if (curr->mHash == hash && !compareF(curr->mKey, key)) {
                if (curr == mBuckets[index]) {
                    mBuckets[index] = curr->mNext;
                }
                else {
                    prev->mNext = curr->mNext;
                }
                delete curr;
                return;
            }
            prev = curr;
            curr = prev->mNext;
        }

        return;
    }

    void Clear()
    {
        for (int i = 0; i < mBucketSize; i++) {
            if (mBuckets[i] != nullptr) {
                Bucket* curr = mBuckets[i];
                while (curr != nullptr) {
                    Bucket* next = curr->mNext;
                    delete curr;
                    curr = next;
                }
                mBuckets[i] = nullptr;
            }
        }
    }

    Array<Val> GetValues()
    {
        Long N = 0;
        for (int i = 0; i < mBucketSize; i++) {
            if (mBuckets[i] != nullptr) {
                Bucket* curr = mBuckets[i];
                while (curr != nullptr) {
                    N++;
                    curr = curr->mNext;
                }
            }
        }

        Array<Val> values(N);
        if (N > 0) {
            Long idx = 0;
            for (int i = 0; i < mBucketSize; i++) {
                if (mBuckets[i] != nullptr) {
                    Bucket* curr = mBuckets[i];
                    while (curr != nullptr) {
                        values.Set(idx, curr->mValue);
                        idx++;
                        curr = curr->mNext;
                    }
                }
            }
        }
        return values;
    }

private:
    int HashKey(
        /* [in] */ const Key& key)
    {
        HashFunc<Key> hashF;
        return hashF(key);
    }

    void Rehash()
    {
        if (mBucketSize == MAX_BUCKET_SIZE) {
            return;
        }
        int oldBucketSize = mBucketSize;
        mBucketSize = oldBucketSize * 2 + 1;
        if (mBucketSize > MAX_BUCKET_SIZE || mBucketSize < 0) {
            mBucketSize = MAX_BUCKET_SIZE;
        }
        Bucket** newBuckets = (Bucket**)calloc(sizeof(Bucket*), mBucketSize);
        if (newBuckets == nullptr) {
            return;
        }
        for (int i = 0; i < oldBucketSize; i++) {
            Bucket* curr = mBuckets[i];
            while (curr != nullptr) {
                Bucket* next = curr->mNext;
                curr->mNext = nullptr;
                PutBucket(newBuckets, mBucketSize, curr);
                curr = next;
            }
        }
        free(mBuckets);
        mBuckets = newBuckets;
        mThreshold = mBucketSize * LOAD_FACTOR;
    }

    void PutBucket(
        /* [in] */ Bucket** buckets,
        /* [in] */ int bucketSize,
        /* [in] */ Bucket* bucket)
    {
        int index = bucket->mHash % bucketSize;
        if (buckets[index] == nullptr) {
            buckets[index] = bucket;
            return;
        }
        else {
            Bucket* prev = buckets[index];
            while (prev->mNext != nullptr) {
                prev = prev->mNext;
            }
            prev->mNext = bucket;
            return;
        }
    }

private:
    static constexpr float LOAD_FACTOR = 0.75;
    static constexpr int MAX_BUCKET_SIZE = INT32_MAX - 8;
    int mThreshold;
    int mCount;
    int mBucketSize;
    Bucket** mBuckets;
};

} // namespace ccm

#endif // __CCM_HASHMAP_H__
