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

#include "ccmuuid.h"
#include "ccmtypes.h"

namespace ccm {

COM_PUBLIC extern const Uuid UUID_ZERO =
        {0x00000000,0x0000,0x0000,0x0000,{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}};

String DumpUuid(
    /* [in] */ const Uuid& id)
{
    String uuidStr = String::Format("%08x-%04x-%04x-%04x-%x%x%x%x%x%x%x%x%x%x%x%x",
            id.mData1, id.mData2, id.mData3, id.mData4,
            id.mData5[0], id.mData5[1], id.mData5[2], id.mData5[3],
            id.mData5[4], id.mData5[5], id.mData5[6], id.mData5[7],
            id.mData5[8], id.mData5[9], id.mData5[10], id.mData5[11]);
    return uuidStr;
}

Integer HashUuid(
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
