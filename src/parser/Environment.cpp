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

#include "Environment.h"
#include "../util/StringBuilder.h"

namespace ccm {

Environment::Environment()
{
    mByteType = new ByteType();
    mShortType = new ShortType();
    mIntegerType = new IntegerType();
    mLongType = new LongType();
    mCharType = new CharType();
    mFloatType = new FloatType();
    mDoubleType = new DoubleType();
    mBooleanType = new BooleanType();
    mStringType = new StringType();
    mHANDLEType = new HANDLEType();

    mTypes.Put(String("Byte"), mByteType);
    mTypes.Put(String("Short"), mShortType);
    mTypes.Put(String("Integer"), mIntegerType);
    mTypes.Put(String("Long"), mLongType);
    mTypes.Put(String("Char"), mCharType);
    mTypes.Put(String("Float"), mFloatType);
    mTypes.Put(String("Double"), mDoubleType);
    mTypes.Put(String("Boolean"), mBooleanType);
    mTypes.Put(String("String"), mStringType);
    mTypes.Put(String("HANDLE"), mHANDLEType);
}

Environment::~Environment()
{
    delete mByteType;
    delete mShortType;
    delete mIntegerType;
    delete mLongType;
    delete mCharType;
    delete mFloatType;
    delete mDoubleType;
    delete mBooleanType;
    delete mStringType;
    delete mHANDLEType;
}

String Environment::Dump(
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append("Environment[");
    builder.Append("root:").Append(mRootFile);
    builder.Append("]\n");
    builder.Append(Pool::Dump(prefix));
    return builder.ToString();
}

}
