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

#ifndef __CCM_CCDL_COMPONENT_H__
#define __CCM_CCDL_COMPONENT_H__

#include "BooleanType.h"
#include "ByteType.h"
#include "CharType.h"
#include "DoubleType.h"
#include "Enumeration.h"
#include "FloatType.h"
#include "HANDLEType.h"
#include "IntegerType.h"
#include "Interface.h"
#include "LongType.h"
#include "Namespace.h"
#include "ShortType.h"
#include "StringType.h"
#include "Type.h"
#include "../util/HashMap.h"

namespace ccm {
namespace ccdl {

class Component
{
public:
    Component(
        /* [in] */ const String& cdlFile);

    ~Component();

    bool AddEnumeration(
        /* [in] */ Enumeration* enumeration);

    Enumeration* FindEnumeration(
        /* [in] */ const String& enumFullName);

    bool AddInterface(
        /* [in] */ Interface* interface);

    Interface* FindInterface(
        /* [in] */ const String& itfFullName);

    bool AddNamespace(
        /* [in] */ Namespace* ns);

    Namespace* ParseNamespace(
        /* [in] */ const String& nsString);

    Namespace* FindNamespace(
        /* [in] */ const String& nsString);

    bool AddTemporaryType(
        /* [in] */ Type* type);

    Type* FindType(
        /* [in] */ const String& typeName);

    String Dump(
        /* [in] */ const String& prefix);

private:
    bool EnlargeEnumerationArray();

    bool EnlargeInterfaceArray();

    bool EnlargeNamespaceArray();

    bool EnlargeTempTypeArray();

private:
    String mCdlFile;

    int mEnumCapacity;
    int mEnumIndex;
    Enumeration** mEnumerations;

    int mItfCapacity;
    int mItfIndex;
    Interface** mInterfaces;

    int mNSCapacity;
    int mNSIndex;
    Namespace** mNamespaces;

    int mTempTypeCapacity;
    int mTempTypeIndex;
    Type** mTempTypes;

    ByteType* mByteType;
    ShortType* mShortType;
    IntegerType* mIntegerType;
    LongType* mLongType;
    CharType* mCharType;
    FloatType* mFloatType;
    DoubleType* mDoubleType;
    BooleanType* mBooleanType;
    StringType* mStringType;
    HANDLEType* mHANDLEType;
    HashMap<Type*> mTypes;
};

}
}

#endif // __CCM_CCDL_COMPONENT_H__
