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

#ifndef __COMO_METACOMPONENT_H__
#define __COMO_METACOMPONENT_H__

#include "../type/comotypekind.h"
#include "../type/comouuid.h"

namespace como {

#define COMO_MAGIC   0x12E20FD

#define VALUE_RADIX_MASK            0x0f
#define VALUE_SCIENTIFIC_NOTATION   0x10
#define VALUE_POSITIVE_INFINITY     0x20
#define VALUE_NEGATIVE_INFINITY     0x40
#define VALUE_NAN                   0x80
// #define VALUE_FLOATING_POINT        0xE0

struct MetaValue
{
    union {
        bool            mBooleanValue;
        long long int   mIntegralValue;
        double          mFloatingPointValue;
        char*           mStringValue;
    };
    unsigned char       mProperties;
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
    size_t              mSize;
    UUID                mUuid;
    char*               mName;
    char*               mUri;
    int                 mNamespaceNumber;
    int                 mConstantNumber;
    int                 mCoclassNumber;
    int                 mEnumerationNumber;
    int                 mExternalEnumerationNumber;
    int                 mInterfaceNumber;
    int                 mExternalInterfaceNumber;
    int                 mTypeNumber;
    MetaNamespace*      mGlobalNamespace;
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
    int                 mNamespaceNumber;
    int                 mConstantNumber;
    int                 mCoclassNumber;
    int                 mEnumerationNumber;
    int                 mExternalEnumerationNumber;
    int                 mInterfaceNumber;
    int                 mExternalInterfaceNumber;
    MetaNamespace**     mNamespaces;
    int*                mConstantIndexes;
    int*                mCoclassIndexes;
    int*                mEnumerationIndexes;
    int*                mInterfaceIndexes;
};

struct MetaConstant
{
    char*               mName;
    char*               mNamespace;
    int                 mTypeIndex;
    MetaValue           mValue;
};

#define COCLASS_CONSTRUCTOR_DEFAULT 0x01
#define COCLASS_CONSTRUCTOR_DELETED 0x02

struct MetaCoclass
{
    UUID                mUuid;
    char*               mName;
    char*               mNamespace;
    int                 mInterfaceNumber;
    int*                mInterfaceIndexes;
    unsigned char       mProperties;
};

struct MetaEnumeration
{
    char*               mName;
    char*               mNamespace;
    int                 mEnumeratorNumber;
    MetaEnumerator**    mEnumerators;
    // bool                mExternal;
    unsigned char       mProperties;
};

struct MetaEnumerator
{
    char*               mName;
    int                 mValue;
};

struct MetaInterface
{
    UUID                mUuid;
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
    // bool                mExternal;
    unsigned char       mProperties;
};

#define METHOD_DELETED              0x01
// #define METHOD_RETURN_REFERENCE     0x02

struct MetaMethod
{
    char*               mName;
    char*               mSignature;
    int                 mReturnTypeIndex;
    int                 mParameterNumber;
    MetaParameter**     mParameters;
    unsigned char       mProperties;
    // bool                mDeleted;
    // bool                mReference;
};

// must be consistent with Parameter
#define PARAMETER_IN                0x01
#define PARAMETER_OUT               0x02
#define PARAMETER_CALLEE            0x04
#define PARAMETER_VALUE_DEFAULT     0x08

/*
 * If the parameter has default value, then it's MetaParameter
 * will be followed by a MetaValue.
 */
struct MetaParameter
{
    char*               mName;
    int                 mTypeIndex;
    unsigned char       mProperties;
    // bool                mHasDefaultValue;
    // MetaValue*          mDefaultValue;
};

#define TYPE_NUMBER_MASK            0x03
#define TYPE_POINTER                0x01
#define TYPE_REFERENCE              0x02
#define TYPE_EXTERNAL               0x80

/*
 * If the type is in other module, then it's MetaType will
 * be followed by the name of the module.
 */
struct MetaType
{
    TypeKind            mKind;
    int                 mIndex;
    // int                 mNestedTypeIndex;
    unsigned char       mProperties;
};

}

#endif // __COMO_METACOMPONENT_H__
