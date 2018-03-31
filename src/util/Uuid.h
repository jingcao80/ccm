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

#ifndef __CCM_UUID_H__
#define __CCM_UUID_H__

#include "String.h"

namespace ccm {

class Uuid
{
public:
    Uuid();

    bool Parse(
        /* [in] */ const String& uuidStr);

    Uuid& operator=(
        /* [in] */ const Uuid& other);

    String ToString();

    String Dump();

private:
    bool IsValid(
        /* [in] */ const String& uuidStr);

public:
    unsigned int mData1;
    unsigned short mData2;
    unsigned short mData3;
    unsigned short mData4;
    unsigned char mData5[12];
};

}

#endif // __CCM_UUID_H__
