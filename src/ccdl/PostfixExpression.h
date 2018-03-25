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
        : mBooleanValue(false)
        , mIntegralValue(0)
        , mFloatingPointValue(0)
        , mExpression(nullptr)
    {}

    inline PostfixExpression& SetEnumerator(
        /* [in] */ const String& enumName)
    { mEnumeratorName = enumName; return *this; }

    inline PostfixExpression& SetExpression(
        /* [in] */ Expression* expression)
    { mExpression = expression; return *this; }

    inline PostfixExpression& SetBooleanValue(
        /* [in] */ bool value)
    { mBooleanValue = value; return *this; }

    inline PostfixExpression& SetIntegralValue(
        /* [in] */ long long int value)
    { mIntegralValue = value; return *this; }

    inline PostfixExpression& SetFloatingPointValue(
        /* [in] */ double value)
    { mFloatingPointValue = value; return *this; }

    inline PostfixExpression& SetStringValue(
        /* [in] */ const String& value)
    { mStringValue = value; return *this; }

    int EvaluateIntegerValue() override;

    long long int EvaluateLongValue() override;

    float EvaluateFloatValue() override;

    double EvaluateDoubleValue() override;

    char EvaluateCharacterValue() override;

    bool EvaluateBooleanValue() override;

    String EvaluateStringValue() override;

    String EvaluateEnumeratorName() override;

private:
    bool mBooleanValue;
    long long int mIntegralValue;
    double mFloatingPointValue;
    String mStringValue;
    String mEnumeratorName;
    Expression* mExpression;
};

}
}

#endif // __CCM_CCDL_POSTFIXEXPRESSION_H__
