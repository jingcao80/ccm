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

#include "ShiftExpression.h"

namespace ccdl {
namespace ast {

int ShiftExpression::IntegerValue()
{
    if (mLeftOperand != nullptr) {
        switch (mOperator) {
            case LEFT:
                return mLeftOperand->IntegerValue() <<
                        mRightOperand->IntegerValue();
            case RIGHT:
                return mLeftOperand->IntegerValue() >>
                        mRightOperand->IntegerValue();
            case RIGHT_UNSIGNED:
                return ((unsigned int)mLeftOperand->IntegerValue()) >>
                        mRightOperand->IntegerValue();
            default:
                return 0;
        }
    }
    else {
        return mRightOperand->IntegerValue();
    }
}

long long int ShiftExpression::LongValue()
{
    if (mLeftOperand != nullptr) {
        long long int leftValue = mLeftOperand->GetType()->IsIntegerType() ?
                mLeftOperand->IntegerValue() : mLeftOperand->LongValue();
        long long int rightValue = mRightOperand->GetType()->IsIntegerType() ?
                mRightOperand->IntegerValue() : mRightOperand->LongValue();
        switch (mOperator) {
            case LEFT:
                return leftValue << rightValue;
            case RIGHT:
                return leftValue >> rightValue;
            case RIGHT_UNSIGNED:
                return ((unsigned long long int)leftValue) >> rightValue;
            default:
                return 0;
        }
    }
    else {
        return mRightOperand->LongValue();
    }
}

float ShiftExpression::FloatValue()
{
    return mRightOperand->FloatValue();
}

double ShiftExpression::DoubleValue()
{
    return mRightOperand->DoubleValue();
}

char ShiftExpression::CharacterValue()
{
    return mRightOperand->CharacterValue();
}

bool ShiftExpression::BooleanValue()
{
    return mRightOperand->BooleanValue();
}

String ShiftExpression::StringValue()
{
    return mRightOperand->StringValue();
}

String ShiftExpression::EnumeratorValue()
{
    return mRightOperand->EnumeratorValue();
}

bool ShiftExpression::IsPositiveInfinity()
{
    if (mLeftOperand != nullptr || mRightOperand == nullptr) {
        return false;
    }
    return mRightOperand->IsPositiveInfinity();
}

bool ShiftExpression::IsNegativeInfinity()
{
    if (mLeftOperand != nullptr || mRightOperand == nullptr) {
        return false;
    }
    return mRightOperand->IsNegativeInfinity();
}

bool ShiftExpression::IsNaN()
{
    if (mLeftOperand != nullptr || mRightOperand == nullptr) {
        return false;
    }
    return mRightOperand->IsNaN();
}

Expression* ShiftExpression::Clone()
{
    ShiftExpression* newExpr = new ShiftExpression();
    if (mLeftOperand != nullptr) {
        newExpr->mLeftOperand = mLeftOperand->Clone();
    }
    if (mRightOperand != nullptr) {
        newExpr->mRightOperand = mRightOperand->Clone();
    }
    newExpr->mOperator = mOperator;
    newExpr->mType = mType;
    newExpr->mRadix = mRadix;
    newExpr->mScientificNotation = mScientificNotation;
    return newExpr;
}

}
}
