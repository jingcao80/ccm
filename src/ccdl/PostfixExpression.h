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

#ifndef __CCM_CCDL_POSTFIXEXPRESSION_H__
#define __CCM_CCDL_POSTFIXEXPRESSION_H__

#include "Expression.h"
#include "Type.h"

namespace ccm {
namespace ccdl {

class PostfixExpression : public Expression
{
public:
    PostfixExpression()
        : mType(nullptr)
        , mCharValue(0)
        , mBooleanValue(false)
        , mIntegerValue(0)
        , mLongValue(0)
        , mFloatValue(0)
        , mDoubleValue(0)
        , mExpression(nullptr)
    {}

    inline PostfixExpression& SetType(
        /* [in] */ Type* type)
    { mType = type; return *this; }

    inline PostfixExpression& SetEnumerator(
        /* [in] */ const String& enumName)
    { mEnumeratorName = enumName; return *this; }

    inline PostfixExpression& SetExpression(
        /* [in] */ Expression* expression)
    { mExpression = expression; return *this; }

private:
    Type* mType;
    char mCharValue;
    bool mBooleanValue;
    int mIntegerValue;
    long long int mLongValue;
    float mFloatValue;
    double mDoubleValue;
    String mStringValue;
    String mEnumeratorName;
    Expression* mExpression;
};

}
}

#endif // __CCM_CCDL_POSTFIXEXPRESSION_H__
