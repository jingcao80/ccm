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

struct Uuid
{
    unsigned int    mData1;
    unsigned short  mData2;
    unsigned short  mData3;
    unsigned short  mData4;
    unsigned char   mData5[12];
};

struct MetaCoclass;
struct MetaConstant;
struct MetaEnumeration;
struct MetaEnumerator;
struct MetaInterface;
struct MetaMethod;
struct MetaNamespace;
struct MetaType;

struct MetaComponent
{
    int                 mMagic;
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
    char*               mName;
    int                 mNamespaceIndex;
    int                 mInterfaceNumber;
    int*                mInterfaceIndexes;
};

struct MetaEnumeration
{
    char*               mName;
    int                 mNamespaceIndex;
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
    char*               mName;
    int                 mNamespaceIndex;
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
        int             mCharacter;
        int             mInteger;
        long long int   mLong;
        float           mFloat;
        double          mDouble;
        int             mEnumerator;
        char*           mString;
    }                   mValue;
    unsigned char       mRadix;
};

struct MetaMethod
{

};

struct MetaType
{

};

}
}

#endif // __CCM_METADATA_COMPONENT_H__
