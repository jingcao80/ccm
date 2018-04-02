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

#include <assert.h>
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
    if (n < *first) return *first;
    if (n > *last) return *last;
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

template<class Key>
int CompareKeyImpl(
    /* [in] */ Key key1,
    /* [in] */ Key key2)
{
    assert(0 && "CompareKeyImpl not implemented.");
    return -1;
}

template<>
int CompareKeyImpl<const char*>(
    /* [in] */ const char* key1,
    /* [in] */ const char* key2)
{
    return strcmp(key1, key2);
}

template<>
int CompareKeyImpl<const Uuid&>(
    /* [in] */ const Uuid& key1,
    /* [in] */ const Uuid& key2)
{
    return key1 == key2 ? 0 : -1;
}

template<class Key>
int HashKeyImpl(
    /* [in] */ Key key)
{
    assert(0 && "HashKeyImpl not implemented.");
    return -1;
}

template<>
int HashKeyImpl<const char*>(
    /* [in] */ const char* key)
{
    if (key == nullptr) return -1;

    // BKDR Hash Function
    int seed = 31; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    const char* string = key;
    if (string) {
        for ( ; *string; ++string) {
            hash = hash * seed + (*string);
        }
    }
    return (hash & 0x7FFFFFFF);
}

template<>
int HashKeyImpl<const Uuid&>(
    /* [in] */ const Uuid& key)
{
    // BKDR Hash Function
    int seed = 31; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    const char* string = reinterpret_cast<const char*>(&key);
    for (int i = 0; i < sizeof(Uuid); i++) {
        hash = hash * seed + string[i];
    }
    return (hash & 0x7FFFFFFF);
}

template<class Key>
void DeleteKeyImpl(
    /* [in, out] */ Key* key)
{}

template<class Val>
void DeleteValueImpl(
    /* [in, out] */ Val* value)
{}

template<class Key, class Val>
class HashMap
{
private:
    struct Bucket
    {
        Bucket()
            : mNext(nullptr)
        {}

        ~Bucket()
        {
            DeleteKeyImpl(&mKey);
            DeleteValueImpl(&mValue);
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
        /* [in] */ Key key,
        /* [in] */ Val value)
    {
        int hash = HashKey(key);
        if (hash == -1) return;

        int index = hash % mBucketSize;
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
                if (!CompareKeyImpl(prev->mKey, key)) {
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
        /* [in] */ Key key)
    {
        int hash = HashKey(key);
        if (hash == -1) return false;

        int index = hash % mBucketSize;
        Bucket* curr = mBuckets[index];
        while (curr != nullptr) {
            if (!CompareKeyImpl(curr->mKey, key)) return true;
            curr = curr->mNext;
        }

        return false;
    }

    Val Get(
        /* [in] */ Key key)
    {
        int hash = HashKey(key);
        if (hash == -1) return Val(0);

        int index = hash % mBucketSize;
        Bucket* curr = mBuckets[index];
        while (curr != nullptr) {
            if (!CompareKeyImpl(curr->mKey, key)) return curr->mValue;
            curr = curr->mNext;
        }

        return Val(0);
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
        /* [in] */ Key key)
    {
        return HashKeyImpl<Key>(key);
    }

private:
    int mBucketSize;
    Bucket** mBuckets;
};

} // namespace ccm

#endif // __CCM_HASHMAP_H__
