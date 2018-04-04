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

#ifndef __CCM_CCMTYPE_H__
#define __CCM_CCMTYPE_H__

#include "ccmdef.h"
#include "ccmuuid.h"

namespace ccm {

typedef unsigned char   Byte;
typedef short           Short;
typedef int             Integer;
typedef long long int   Long;
typedef float           Float;
typedef double          Double;
typedef int             Char;
typedef bool            Boolean;
typedef uintptr_t       HANDLE;
typedef int             ECode;

typedef Uuid            InterfaceID;
struct ComponentID;
struct CoclassID
{
    Uuid                mUuid;
    ComponentID*        mCid;
};
struct ComponentID
{
    Uuid                mUuid;
    const char*         mUrl;
};

inline bool operator==(
    /* [in] */ const CoclassID& cid1,
    /* [in] */ const CoclassID& cid2)
{
    return !memcmp(&cid1.mUuid, &cid2.mUuid, sizeof(Uuid));
}

inline bool operator==(
    /* [in] */ const ComponentID& cid1,
    /* [in] */ const ComponentID& cid2)
{
    return !memcmp(&cid1.mUuid, &cid2.mUuid, sizeof(Uuid));
}

}

#include "ccmintfs.h"
#include "ccmerror.h"

#endif // __CCM_CCMTYPE_H__
