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

#ifndef __CCM_METADATA_COMPONENT_H__
#define __CCM_METADATA_COMPONENT_H__

namespace ccm {
namespace metadata {

typedef struct Uuid
{
    unsigned int    mData1;
    unsigned short  mData2;
    unsigned short  mData3;
    unsigned short  mData4;
    unsigned char   mData5[12];
} Uuid;

typedef struct Coclass      Coclass;
typedef struct Constant     Constant;
typedef struct Enumeration  Enumeration;
typedef struct Enumerator   Enumerator;
typedef struct Interface    Interface;
typedef struct Method       Method;
typedef struct Namespace    Namespace;

typedef struct Component
{
    int             mMagic;
    const char*     mName;
    Uuid            mUuid;
    const char*     mUrl;
    int             mNamespaceNumber;
    Namespace**     mNamespaces;
    int             mCoclassNumber;
    Coclass**       mCoclasses;
    int             mEnumerationNumber;
    Enumeration**   mEnumerations;
    int             mInterfaceNumber;
    Interface**     mInterfaces;
} Component;

typedef struct Namespace
{
    const char*     mName;
    int             mCoclassNumber;
    int*            mCoclassIndexes;
    int             mEnumerationNumber;
    int*            mEnumerationIndexes;
    int             mInterfaceNumber;
    int*            mInterfaceIndexes;
} Namespace;

typedef struct Coclass
{
    const char*     mName;
    int             mNamespaceIndex;
    int             mInterfaceNumber;
    int*            mInterfaceIndexes;
} Coclass;

typedef struct Enumeration
{
    const char*     mName;
    int             mNamespaceIndex;
    int             mEnumeratorNumber;
    Enumerator**    mEnumerators;
} Enumeration;

typedef struct Enumerator
{
    const char*     mName;
    int             mValue;
} Enumerator;

typedef struct Interface
{
    const char*     mName;
    int             mNamespaceIndex;
    int             mConstantNumber;
    Constant**      mConstants;
    int             mMethodNumber;
    Method**        mMethods;
} Interface;

typedef struct Method
{

} Method;

}
}

#endif // __CCM_METADATA_COMPONENT_H__
