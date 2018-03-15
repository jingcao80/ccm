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

#include "Uuid.h"

#include <stdlib.h>

namespace ccm {

Uuid::Uuid()
{
    mData1 = mData2 = mData3 = mData4 = 0;
    memset(mData5, 0, 12);
}

bool Uuid::Parse(
    /* [in] */ const String& uuidStr)
{
    if (!IsValid(uuidStr)) return false;

    mData1 = strtol(uuidStr.Substring(0, 7).string(), nullptr, 16);
    mData2 = strtol(uuidStr.Substring(9, 12).string(), nullptr, 16);
    mData3 = strtol(uuidStr.Substring(14, 17).string(), nullptr, 16);
    mData4 = strtol(uuidStr.Substring(19, 22).string(), nullptr, 16);
    for (int i = 0; i < 12; i++) {
        char c = uuidStr.GetChar(i + 24);
        if ('0' <= c && c <= '9') mData5[i] = c - '0';
        else if ('a' <= c && c <= 'z') mData5[i] = c - 'a' + 10;
        else if ('A' <= c && c <= 'Z') mData5[i] = c - 'A' + 10;
    }
    return true;
}

String Uuid::Dump()
{
    String uuidStr = String::Format("%08x-%04x-%04x-%04x-%x%x%x%x%x%x%x%x%x%x%x%x",
            mData1, mData2, mData3, mData4, mData5[0], mData5[1], mData5[2], mData5[3],
            mData5[4], mData5[5], mData5[6], mData5[7], mData5[8], mData5[9], mData5[10], mData5[11]);
    return uuidStr;
}

bool Uuid::IsValid(
    /* [in] */ const String& uuidStr)
{
    if (uuidStr.IsNull() || uuidStr.GetLength() != 36) return false;

    for (int i = 0; i < 36; i++) {
        char c = uuidStr.GetChar(i);
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            if (c != '-') return false;
        }
        else if ((c < '0' || c > '9') && (c < 'a' || c > 'z') &&
                (c < 'A' || c > 'Z')) return false;
    }
    return true;
}

}
