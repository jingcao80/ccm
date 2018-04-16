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

#ifndef __CCDL_AST_EXPRESSION_H__
#define __CCDL_AST_EXPRESSION_H__

#include "Node.h"
#include "Type.h"

namespace ccdl {
namespace ast {

class Expression : public Node
{
public:
    Expression();

    ~Expression();

    inline void SetLeftOperand(
        /* [in] */ Expression* leftOperand);

    inline void SetRightOperand(
        /* [in] */ Expression* rightOperand);

    inline void SetOperator(
        /* [in] */ int addOperator);

    inline Type* GetType();

    inline void SetType(
        /* [in] */ Type* type);

    inline int GetRadix();

    inline void SetRadix(
        /* [in] */ int radix);

    virtual int IntegerValue() = 0;

    virtual long long int LongValue() = 0;

    virtual float FloatValue() = 0;

    virtual double DoubleValue() = 0;

    virtual char CharacterValue() = 0;

    virtual bool BooleanValue() = 0;

    virtual String StringValue() = 0;

    virtual String EnumeratorValue() = 0;

protected:
    Expression* mLeftOperand;
    Expression* mRightOperand;
    int mOperator;
    Type* mType;
    int mRadix;
};

void Expression::SetLeftOperand(
    /* [in] */ Expression* leftOperand)
{
    mLeftOperand = leftOperand;
}

void Expression::SetRightOperand(
    /* [in] */ Expression* rightOperand)
{
    mRightOperand = rightOperand;
}

void Expression::SetOperator(
    /* [in] */ int addOperator)
{
    mOperator = addOperator;
}

Type* Expression::GetType()
{
    return mType;
}

void Expression::SetType(
    /* [in] */ Type* type)
{
    mType = type;
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

}
}

#endif // __CCDL_AST_EXPRESSION_H__
