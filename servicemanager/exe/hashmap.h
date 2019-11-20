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

#ifndef __PISCES_HASHMAP_H__
#define __PISCES_HASHMAP_H__

#include <cstdlib>
#include <cstring>

namespace pisces {

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
        if (l < n && n < r) {
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

        Key mKey;
        Val mValue;
        struct Bucket* mNext;
    };

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

        int hash = HashKey(key);
        if (hash == -1) return;

        int index = hash % mBucketSize;
        if (mBuckets[index] == nullptr) {
            Bucket* b = new Bucket();
            assignKeyF(&b->mKey, key, this);
            assignValF(&b->mValue, value, this);
            mBuckets[index] = b;
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
            assignKeyF(&b->mKey, key, this);
            assignValF(&b->mValue, value, this);
            prev->mNext = b;
            return;
        }
    }

    bool ContainsKey(
        /* [in] */ const Key& key)
    {
        CompareFunc<Key> compareF;

        int hash = HashKey(key);
        if (hash == -1) return false;

        int index = hash % mBucketSize;
        Bucket* curr = mBuckets[index];
        while (curr != nullptr) {
            if (!compareF(curr->mKey, key)) return true;
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

        int index = hash % mBucketSize;
        Bucket* curr = mBuckets[index];
        while (curr != nullptr) {
            if (!compareF(curr->mKey, key)) return curr->mValue;
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

        int index = hash % mBucketSize;
        Bucket* curr = mBuckets[index];
        Bucket* prev = curr;
        while (curr != nullptr) {
            if (!compareF(curr->mKey, key)) {
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

private:
    int HashKey(
        /* [in] */ const Key& key)
    {
        HashFunc<Key> hashF;
        return hashF(key);
    }

private:
    int mBucketSize;
    Bucket** mBuckets;
};

} // namespace pisces

#endif // __PISCES_HASHMAP_H__
