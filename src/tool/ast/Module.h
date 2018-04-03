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

#ifndef __CCDL_AST_MODULE_H__
#define __CCDL_AST_MODULE_H__

#include "Node.h"
#include "BooleanType.h"
#include "ByteType.h"
#include "CharType.h"
#include "CoclassIDType.h"
#include "DoubleType.h"
#include "Enumeration.h"
#include "FloatType.h"
#include "HANDLEType.h"
#include "IntegerType.h"
#include "Interface.h"
#include "InterfaceIDType.h"
#include "LongType.h"
#include "Namespace.h"
#include "Pool.h"
#include "ShortType.h"
#include "StringType.h"
#include "Type.h"
#include "../util/StringMap.h"

namespace ccdl {
namespace ast {

class Module : public Node, public Pool
{
public:
    Module();

    ~Module();

    inline String GetName()
    { return mName; }

    inline void SetName(
        /* [in] */ const String& name)
    { mName = name; }

    void SetAttribute(
        /* [in] */ const Attribute& attr);

    inline Uuid& GetUuid()
    { return mUuid; }

    inline String GetUrl()
    { return mUrl; }

    String Dump(
        /* [in] */ const String& prefix) override;

private:
    String mName;
    Uuid mUuid;
    String mVersion;
    String mDescription;
    String mUrl;

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
    CoclassIDType* mCoclassIDType;
    InterfaceIDType* mInterfaceIDType;
};

}
}

#endif // __CCDL_AST_MODULE_H__
