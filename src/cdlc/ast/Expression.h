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

#ifndef __CDLC_EXPRESSION_H__
#define __CDLC_EXPRESSION_H__

#include "ast/Node.h"
#include "ast/Type.h"
#include "util/AutoPtr.h"

namespace cdlc {

class Expression
    : public Node
{
public:
    inline AutoPtr<Type> GetType();

    inline void SetType(
        /* [in] */ Type* type);

    inline void SetOperator(
        /* [in] */ int exprOperator);

    inline void SetLeftOperand(
        /* [in] */ Expression* expr);

    inline void SetRightOperand(
        /* [in] */ Expression* expr);

    inline int GetRadix();

    inline void SetRadix(
        /* [in] */ int radix);

    inline bool IsScientificNotation();

    inline void SetScientificNotation(
        /* [in] */ bool scientificNotation);

    virtual int IntegerValue() = 0;

    virtual long long int LongValue() = 0;

    virtual float FloatValue() = 0;

    virtual double DoubleValue() = 0;

public:
    static const int OPERATOR_NONE = 0;
    static const int OPERATOR_POSITIVE = 1;
    static const int OPERATOR_NEGATIVE = 2;
    static const int OPERATOR_COMPLIMENT = 3;
    static const int OPERATOR_NOT = 4;
    static const int OPERATOR_MULTIPLE = 5;
    static const int OPERATOR_DIVIDE = 6;
    static const int OPERATOR_MODULO = 7;
    static const int OPERATOR_PLUS = 8;
    static const int OPERATOR_MINUS = 9;
    static const int OPERATOR_LEFT_SHIFT = 10;
    static const int OPERATOR_RIGHT_SHIFT = 11;
    static const int OPERATOR_UNSIGNED_RIGHT_SHIFT = 12;
    static const int OPERATOR_BITWISE_AND = 13;
    static const int OPERATOR_BITWISE_XOR = 14;
    static const int OPERATOR_BITWISE_OR = 15;

protected:
    AutoPtr<Type> mType;
    int mOperator = OPERATOR_NONE;
    AutoPtr<Expression> mLeftOperand;
    AutoPtr<Expression> mRightOperand;
    int mRadix = 10;
    bool mScientificNotation = false;
};

AutoPtr<Type> Expression::GetType()
{
    return mType;
}

void Expression::SetType(
    /* [in] */ Type* type)
{
    mType = type;
}

void Expression::SetOperator(
    /* [in] */ int exprOperator)
{
    mOperator = exprOperator;
}

void Expression::SetLeftOperand(
    /* [in] */ Expression* expr)
{
    mLeftOperand = expr;
}

void Expression::SetRightOperand(
    /* [in] */ Expression* expr)
{
    mRightOperand = expr;
}

int Expression::GetRadix()
{
    return mRadix;
}

void Expression::SetRadix(
    /* [in] */ int radix)
{
    mRadix = radix;
}

bool Expression::IsScientificNotation()
{
    return mScientificNotation;
}

void Expression::SetScientificNotation(
    /* [in] */ bool scientificNotation)
{
    mScientificNotation = scientificNotation;
}

}

#endif // __CDLC_EXPRESSION_H__
