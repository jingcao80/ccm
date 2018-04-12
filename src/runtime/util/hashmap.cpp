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

#include "ccmtypes.h"
#include "hashmap.h"

namespace ccm {

template<>
void AssignImpl<String>(
    /* [in] */ String* target,
    /* [in] */ const String& value)
{
    *target = value;
}

template<>
void AssignImpl<Uuid>(
    /* [in] */ Uuid* target,
    /* [in] */ const Uuid& value)
{
    *target = value;
}

template<>
int CompareKeyImpl<String>(
    /* [in] */ const String& key1,
    /* [in] */ const String& key2)
{
    return key1.Compare(key2);
}

template<>
int CompareKeyImpl<Uuid>(
    /* [in] */ const Uuid& key1,
    /* [in] */ const Uuid& key2)
{
    return key1 == key2 ? 0 : -1;
}

template<>
int HashKeyImpl<String>(
    /* [in] */ const String& key)
{
    if (key.IsNull()) return -1;

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

template<>
int HashKeyImpl<Uuid>(
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

}
