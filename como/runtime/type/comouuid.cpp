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

#include "comouuid.h"
#include "comotypes.h"

namespace como {

COM_PUBLIC extern const UUID UUID_ZERO =
        {0x00000000,0x0000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00}};

String DumpUUID(
    /* [in] */ const UUID& id)
{
    String uuidStr = String::Format("%08x-%04x-%04x-%04x-%02x%02x%02x%02x%02x%02x",
            id.mData1, id.mData2, id.mData3, id.mData4,
            id.mData5[0], id.mData5[1], id.mData5[2], id.mData5[3], id.mData5[4], id.mData5[5]);
    return uuidStr;
}

Integer HashUUID(
    /* [in] */ const UUID& key)
{
    // BKDR Hash Function
    int seed = 31; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    const char* string = reinterpret_cast<const char*>(&key);
    for (int i = 0; i < sizeof(UUID); i++) {
        hash = hash * seed + string[i];
    }
    return (hash & 0x7FFFFFFF);
}

ComponentID* CloneComponentID(
    /* [in] */ const ComponentID* cid)
{
    if (cid == nullptr) {
        return nullptr;
    }

    ComponentID* clone = (ComponentID*)malloc(sizeof(ComponentID));
    if (clone != nullptr) {
        *clone = *cid;
        if (cid->mUri != nullptr) {
            Integer size = strlen(cid->mUri);
            clone->mUri = (const char*)malloc(size + 1);
            if (clone->mUri != nullptr) {
                memcpy(const_cast<char*>(clone->mUri), cid->mUri, size + 1);
            }
        }
    }

    return clone;
}

void ReleaseComponentID(
    /* [in] */ const ComponentID* cid)
{
    if (cid != nullptr) {
        if (cid->mUri != nullptr) {
            free(const_cast<char*>(cid->mUri));
        }
        free(const_cast<ComponentID*>(cid));
    }
}

} // namespace como
