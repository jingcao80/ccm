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

#include "../ccmuuid.h"

namespace ccm {
namespace metadata {

#define CCM_MAGIC   0x12E20FD

enum class CcdlType
{
    Char = 1,
    Byte,
    Short,
    Integer,
    Long,
    Float,
    Double,
    Boolean,
    String,
    HANDLE,
    Enum,
    Array,
    Interface,
};

struct MetaCoclass;
struct MetaConstant;
struct MetaEnumeration;
struct MetaEnumerator;
struct MetaInterface;
struct MetaMethod;
struct MetaNamespace;
struct MetaParameter;
struct MetaType;

struct MetaComponent
{
    int                 mMagic;
    int                 mSize;
    Uuid                mUuid;
    char*               mName;
    char*               mUrl;
    int                 mNamespaceNumber;
    int                 mCoclassNumber;
    int                 mEnumerationNumber;
    int                 mInterfaceNumber;
    int                 mTypeNumber;
    MetaNamespace**     mNamespaces;
    MetaCoclass**       mCoclasses;
    MetaEnumeration**   mEnumerations;
    MetaInterface**     mInterfaces;
    MetaType**          mTypes;
    char*               mStringPool;
};

struct MetaNamespace
{
    char*               mName;
    int                 mCoclassNumber;
    int                 mEnumerationNumber;
    int                 mInterfaceNumber;
    int*                mCoclassIndexes;
    int*                mEnumerationIndexes;
    int*                mInterfaceIndexes;
};

struct MetaCoclass
{
    Uuid                mUuid;
    char*               mName;
    char*               mNamespace;
    int                 mInterfaceNumber;
    int*                mInterfaceIndexes;
};

struct MetaEnumeration
{
    char*               mName;
    char*               mNamespace;
    int                 mEnumeratorNumber;
    MetaEnumerator**    mEnumerators;
};

struct MetaEnumerator
{
    char*               mName;
    int                 mValue;
};

struct MetaInterface
{
    Uuid                mUuid;
    char*               mName;
    char*               mNamespace;
    int                 mBaseInterfaceIndex;
    int                 mConstantNumber;
    int                 mMethodNumber;
    MetaConstant**      mConstants;
    MetaMethod**        mMethods;
};

struct MetaConstant
{
    char*               mName;
    int                 mTypeIndex;
    union {
        bool            mBoolean;
        int             mInteger;
        long long int   mLong;
        float           mFloat;
        double          mDouble;
        char*           mString;
    }                   mValue;
    unsigned char       mRadix;
};

struct MetaMethod
{
    char*               mName;
    char*               mSignature;
    int                 mParameterNumber;
    MetaParameter**     mParameters;
};

struct MetaParameter
{
    char*               mName;
    int                 mAttribute;
    int                 mTypeIndex;
};

struct MetaType
{
    CcdlType            mKind;
    int                 mIndex;
    int                 mNestedTypeIndex;
    unsigned char       mPointerNumber;
};

}
}

#endif // __CCM_METADATA_COMPONENT_H__
