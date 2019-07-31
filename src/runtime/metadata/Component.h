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

#include "../type/ccmtypekind.h"
#include "../type/ccmuuid.h"

namespace ccm {
namespace metadata {

#define CCM_MAGIC   0x12E20FD

struct MetaCoclass;
struct MetaConstant;
struct MetaEnumeration;
struct MetaEnumerator;
struct MetaInterface;
struct MetaMethod;
struct MetaNamespace;
struct MetaParameter;
struct MetaType;
struct MetaValue;

struct MetaComponent
{
    int                 mMagic;
    int                 mSize;
    Uuid                mUuid;
    char*               mName;
    char*               mUrl;
    int                 mNamespaceNumber;
    int                 mConstantNumber;
    int                 mCoclassNumber;
    int                 mEnumerationNumber;
    int                 mExternalEnumerationNumber;
    int                 mInterfaceNumber;
    int                 mExternalInterfaceNumber;
    int                 mTypeNumber;
    MetaNamespace**     mNamespaces;
    MetaConstant**      mConstants;
    MetaCoclass**       mCoclasses;
    MetaEnumeration**   mEnumerations;
    MetaInterface**     mInterfaces;
    MetaType**          mTypes;
    char*               mStringPool;
};

struct MetaNamespace
{
    char*               mName;
    int                 mInterfaceWrappedIndex;
    int                 mConstantNumber;
    int                 mCoclassNumber;
    int                 mEnumerationNumber;
    int                 mExternalEnumerationNumber;
    int                 mInterfaceNumber;
    int                 mExternalInterfaceNumber;
    int*                mConstantIndexes;
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
    bool                mConstructorDefault;
    bool                mConstructorDeleted;
};

struct MetaEnumeration
{
    char*               mName;
    char*               mNamespace;
    int                 mEnumeratorNumber;
    MetaEnumerator**    mEnumerators;
    bool                mExternal;
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
    int                 mOuterInterfaceIndex;
    int                 mNestedInterfaceNumber;
    int                 mConstantNumber;
    int                 mMethodNumber;
    int*                mNestedInterfaceIndexes;
    MetaConstant**      mConstants;
    MetaMethod**        mMethods;
    bool                mExternal;
};

#define RADIX_MASK                  0x0f
#define SCIENTIFIC_NOTATION_MASK    0x10
#define POSITIVE_INFINITY_MASK      0x20
#define NEGATIVE_INFINITY_MASK      0x40
#define NAN_MASK                    0x80
#define FP_MASK                     0xE0

struct MetaValue
{
    union {
        bool            mBoolean;
        int             mInteger;
        long long int   mLong;
        float           mFloat;
        double          mDouble;
        char*           mString;
    };
    unsigned char       mAttributes;
};

struct MetaConstant
{
    char*               mName;
    int                 mTypeIndex;
    MetaValue           mValue;
};

struct MetaMethod
{
    char*               mName;
    char*               mSignature;
    int                 mReturnTypeIndex;
    int                 mParameterNumber;
    MetaParameter**     mParameters;
    bool                mDeleted;
    bool                mReference;
};

struct MetaParameter
{
    char*               mName;
    int                 mAttribute;
    int                 mTypeIndex;
    bool                mHasDefaultValue;
    MetaValue           mDefaultValue;
};

struct MetaType
{
    CcmTypeKind         mKind;
    int                 mIndex;
    int                 mNestedTypeIndex;
    unsigned char       mPointerNumber;
    bool                mReference;
};

}
}

#endif // __CCM_METADATA_COMPONENT_H__
