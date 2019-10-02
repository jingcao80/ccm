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

#include "util/UUID.h"
#include <cctype>
#include <cstdlib>

namespace cdlc {

bool UUID::IsValid(
    /* [in] */ const String& uuidStr)
{
    if (uuidStr.IsEmpty() || uuidStr.GetLength() != 36) {
        return false;
    }

    for (int i = 0; i < 36; i++) {
        char c = uuidStr.GetChar(i);
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            if (c != '-') {
                return false;
            }
        }
        else if (!isxdigit(c)) {
            return false;
        }
    }
    return true;
}

AutoPtr<UUID> UUID::Parse(
    /* [in] */ const String& uuidStr)
{
    if (!IsValid(uuidStr)) {
        return nullptr;
    }

    AutoPtr<UUID> uuid = new UUID();

    uuid->mData1 = strtol(uuidStr.Substring(0, 8).string(), nullptr, 16);
    uuid->mData2 = strtol(uuidStr.Substring(9, 13).string(), nullptr, 16);
    uuid->mData3 = strtol(uuidStr.Substring(14, 18).string(), nullptr, 16);
    uuid->mData4 = strtol(uuidStr.Substring(19, 23).string(), nullptr, 16);
    for (int i = 0; i < 12; i++) {
        char c = uuidStr.GetChar(i + 24);
        if (isdigit(c)) {
            uuid->mData5[i] = c - '0';
        }
        else if (islower(c)) {
            uuid->mData5[i] = c - 'a' + 10;
        }
        else {
            uuid->mData5[i] = c - 'A' + 10;
        }
    }

    return uuid;
}

AutoPtr<UUID> UUID::Parse(
    /* [in] */ const como::UUID& source)
{
    AutoPtr<UUID> uuid = new UUID();
    memcpy(&uuid->mData1, &source, sizeof(como::UUID));
    return uuid;
}

como::UUID UUID::ToComoUUID()
{
    como::UUID uuid;
    memcpy(&uuid, &mData1, sizeof(como::UUID));
    return uuid;
}

String UUID::ToString()
{
    String uuidStr = String::Format("{0x%08x,0x%04x,0x%04x,0x%04x,"
            "{0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x}}",
            mData1, mData2, mData3, mData4, mData5[0], mData5[1], mData5[2], mData5[3],
            mData5[4], mData5[5], mData5[6], mData5[7], mData5[8], mData5[9], mData5[10], mData5[11]);
    return uuidStr;
}

String UUID::Dump()
{
    String uuidStr = String::Format("%08x-%04x-%04x-%04x-%x%x%x%x%x%x%x%x%x%x%x%x",
            mData1, mData2, mData3, mData4, mData5[0], mData5[1], mData5[2], mData5[3],
            mData5[4], mData5[5], mData5[6], mData5[7], mData5[8], mData5[9], mData5[10], mData5[11]);
    return uuidStr;
}

}
