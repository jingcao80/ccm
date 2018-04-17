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

#ifndef __CCDL_AST_POSTFIXEXPRESSION_H__
#define __CCDL_AST_POSTFIXEXPRESSION_H__

#include "Expression.h"

namespace ccdl {
namespace ast {

class PostfixExpression : public Expression
{
public:
    PostfixExpression();

    inline void SetEnumerator(
        /* [in] */ const String& enumName);

    inline void SetExpression(
        /* [in] */ Expression* expression);

    inline void SetBooleanValue(
        /* [in] */ bool value);

    inline void SetIntegralValue(
        /* [in] */ long long int value);

    inline void SetFloatingPointValue(
        /* [in] */ double value);

    inline void SetStringValue(
        /* [in] */ const String& value);

    int IntegerValue() override;

    long long int LongValue() override;

    float FloatValue() override;

    double DoubleValue() override;

    char CharacterValue() override;

    bool BooleanValue() override;

    String StringValue() override;

    String EnumeratorValue() override;

    Expression* Clone() override;

private:
    bool mBooleanValue;
    long long int mIntegralValue;
    double mFloatingPointValue;
    String mStringValue;
    String mEnumeratorName;
    Expression* mExpression;
};

void PostfixExpression::SetEnumerator(
    /* [in] */ const String& enumName)
{
    mEnumeratorName = enumName;
}

void PostfixExpression::SetExpression(
    /* [in] */ Expression* expression)
{
    mExpression = expression;
}

void PostfixExpression::SetBooleanValue(
    /* [in] */ bool value)
{
    mBooleanValue = value;
}

void PostfixExpression::SetIntegralValue(
    /* [in] */ long long int value)
{
    mIntegralValue = value;
}

void PostfixExpression::SetFloatingPointValue(
    /* [in] */ double value)
{
    mFloatingPointValue = value;
}

void PostfixExpression::SetStringValue(
    /* [in] */ const String& value)
{
    mStringValue = value;
}

}
}

#endif // __CCDL_AST_POSTFIXEXPRESSION_H__
