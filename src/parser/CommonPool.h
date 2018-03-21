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

#ifndef __CCM_COMMONPOOL_H__
#define __CCM_COMMONPOOL_H__

#include "../ccdl/BooleanType.h"
#include "../ccdl/ByteType.h"
#include "../ccdl/CharType.h"
#include "../ccdl/DoubleType.h"
#include "../ccdl/Enumeration.h"
#include "../ccdl/FloatType.h"
#include "../ccdl/HANDLEType.h"
#include "../ccdl/IntegerType.h"
#include "../ccdl/Interface.h"
#include "../ccdl/LongType.h"
#include "../ccdl/Namespace.h"
#include "../ccdl/ShortType.h"
#include "../ccdl/StringType.h"
#include "../ccdl/Type.h"
#include "../util/HashMap.h"

using ccm::ccdl::BooleanType;
using ccm::ccdl::ByteType;
using ccm::ccdl::CharType;
using ccm::ccdl::DoubleType;
using ccm::ccdl::Enumeration;
using ccm::ccdl::FloatType;
using ccm::ccdl::HANDLEType;
using ccm::ccdl::IntegerType;
using ccm::ccdl::Interface;
using ccm::ccdl::LongType;
using ccm::ccdl::Namespace;
using ccm::ccdl::ShortType;
using ccm::ccdl::StringType;
using ccm::ccdl::Type;

namespace ccm {

class CommonPool
{
public:
    CommonPool();

    ~CommonPool();

    inline void SetRootFile(
        /* [in] */ const String& rootFile)
    { mRootFile = rootFile; }

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
    String mRootFile;

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

#endif // __CCM_COMMONPOOL_H__
