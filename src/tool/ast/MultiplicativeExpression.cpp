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

#include "MultiplicativeExpression.h"

namespace ccdl {
namespace ast {

int MultiplicativeExpression::IntegerValue()
{
    if (mLeftOperand != nullptr) {
        switch (mOperator) {
            case MULTIPLE:
                return mLeftOperand->IntegerValue() *
                        mRightOperand->IntegerValue();
            case DIVIDE: {
                long long int divisor = mRightOperand->IntegerValue();
                return divisor != 0 ? mLeftOperand->IntegerValue() / divisor : 0;
            }
            case MODULO: {
                long long int divisor = mRightOperand->IntegerValue();
                return divisor != 0 ? mLeftOperand->IntegerValue() % divisor : 0;
            }
            default:
                return 0;
        }
    }
    else {
        return mRightOperand->IntegerValue();
    }
}

long long int MultiplicativeExpression::LongValue()
{
    if (mLeftOperand != nullptr) {
        long long int leftValue = mLeftOperand->GetType()->IsIntegerType() ?
                mLeftOperand->IntegerValue() : mLeftOperand->LongValue();
        long long int rightValue = mRightOperand->GetType()->IsIntegerType() ?
                mRightOperand->IntegerValue() : mRightOperand->LongValue();
        switch (mOperator) {
            case MULTIPLE:
                return leftValue * rightValue;
            case DIVIDE:
                return rightValue != 0 ? leftValue / rightValue : 0;
            case MODULO:
                return rightValue != 0 ? leftValue % rightValue : 0;
            default:
                return 0;
        }
    }
    else {
        return mRightOperand->LongValue();
    }
}

float MultiplicativeExpression::FloatValue()
{
    if (mLeftOperand != nullptr) {
        float leftValue, rightValue;
        Type* type = mLeftOperand->GetType();
        if (type->IsIntegerType()) {
            leftValue = mLeftOperand->IntegerValue();
        }
        else if (type->IsLongType()) {
            leftValue = mLeftOperand->LongValue();
        }
        else {
            leftValue = mLeftOperand->FloatValue();
        }
        type = mRightOperand->GetType();
        if (type->IsIntegerType()) {
            rightValue = mRightOperand->IntegerValue();
        }
        else if (type->IsLongType()) {
            rightValue = mRightOperand->LongValue();
        }
        else {
            rightValue = mRightOperand->FloatValue();
        }
        switch (mOperator) {
            case MULTIPLE:
                return leftValue * rightValue;
            case DIVIDE:
                return rightValue != 0 ? leftValue / rightValue : 0;
            case MODULO:
            default:
                return 0;
        }
    }
    else {
        return mRightOperand->FloatValue();
    }
}

double MultiplicativeExpression::DoubleValue()
{
    if (mLeftOperand != nullptr) {
        double leftValue, rightValue;
        Type* type = mLeftOperand->GetType();
        if (type->IsIntegerType()) {
            leftValue = mLeftOperand->IntegerValue();
        }
        else if (type->IsLongType()) {
            leftValue = mLeftOperand->LongValue();
        }
        else if (type->IsFloatType()) {
            leftValue = mLeftOperand->FloatValue();
        }
        else leftValue = mLeftOperand->DoubleValue();
        type = mRightOperand->GetType();
        if (type->IsIntegerType()) {
            rightValue = mRightOperand->IntegerValue();
        }
        else if (type->IsLongType()) {
            rightValue = mRightOperand->LongValue();
        }
        else if (type->IsFloatType()) {
            rightValue = mRightOperand->FloatValue();
        }
        else rightValue = mRightOperand->DoubleValue();
        switch (mOperator) {
            case MULTIPLE:
                return leftValue * rightValue;
            case DIVIDE:
                return rightValue != 0 ? leftValue / rightValue : 0;
            case MODULO:
            default:
                return 0;
        }
    }
    else {
        return mRightOperand->DoubleValue();
    }
}

char MultiplicativeExpression::CharacterValue()
{
    return mRightOperand->CharacterValue();
}

bool MultiplicativeExpression::BooleanValue()
{
    return mRightOperand->BooleanValue();
}

String MultiplicativeExpression::StringValue()
{
    return mRightOperand->StringValue();
}

String MultiplicativeExpression::EnumeratorValue()
{
    return mRightOperand->EnumeratorValue();
}

}
}
