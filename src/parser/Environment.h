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

#ifndef __CCM_ENVIRONMENT_H__
#define __CCM_ENVIRONMENT_H__

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
#include "../ccdl/Pool.h"
#include "../ccdl/ShortType.h"
#include "../ccdl/StringType.h"
#include "../ccdl/Type.h"
#include "../util/ArrayList.h"
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
using ccm::ccdl::Pool;
using ccm::ccdl::ShortType;
using ccm::ccdl::StringType;
using ccm::ccdl::Type;

namespace ccm {

class Environment : public Pool
{
public:
    Environment();

    ~Environment();

    inline void SetRootFile(
        /* [in] */ const String& rootFile)
    { mRootFile = rootFile; }

    String Dump(
        /* [in] */ const String& prefix) override;

private:
    String mRootFile;

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
};

}

#endif // __CCM_ENVIRONMENT_H__
