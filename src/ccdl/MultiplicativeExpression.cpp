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

namespace ccm {
namespace ccdl {

int MultiplicativeExpression::EvaluateIntegerValue()
{
    if (mLeftOperand != nullptr) {
        switch (mOperator) {
            case MULTIPLE:
                return mLeftOperand->EvaluateIntegerValue() *
                        mRightOperand->EvaluateIntegerValue();
            case DIVIDE: {
                long long int divisor = mRightOperand->EvaluateIntegerValue();
                return divisor != 0 ? mLeftOperand->EvaluateIntegerValue() / divisor : 0;
            }
            case MODULO: {
                long long int divisor = mRightOperand->EvaluateIntegerValue();
                return divisor != 0 ? mLeftOperand->EvaluateIntegerValue() % divisor : 0;
            }
            default:
                return 0;
        }
    }
    else {
        return mRightOperand->EvaluateIntegerValue();
    }
}

long long int MultiplicativeExpression::EvaluateLongValue()
{
    if (mLeftOperand != nullptr) {
        long long int leftValue = mLeftOperand->GetType()->GetName().Equals("Integer") ?
                mLeftOperand->EvaluateIntegerValue() : mLeftOperand->EvaluateLongValue();
        long long int rightValue = mRightOperand->GetType()->GetName().Equals("Integer") ?
                mRightOperand->EvaluateIntegerValue() : mRightOperand->EvaluateLongValue();
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
        return mRightOperand->EvaluateLongValue();
    }
}

float MultiplicativeExpression::EvaluateFloatValue()
{
    if (mLeftOperand != nullptr) {
        float leftValue, rightValue;
        String typeName = mLeftOperand->GetType()->GetName();
        if (typeName.Equals("Integer")) {
            leftValue = mLeftOperand->EvaluateIntegerValue();
        }
        else if (typeName.Equals("Long")) {
            leftValue = mLeftOperand->EvaluateLongValue();
        }
        else {
            leftValue = mLeftOperand->EvaluateFloatValue();
        }
        typeName = mRightOperand->GetType()->GetName();
        if (typeName.Equals("Integer")) {
            rightValue = mRightOperand->EvaluateIntegerValue();
        }
        else if (typeName.Equals("Long")) {
            rightValue = mRightOperand->EvaluateLongValue();
        }
        else {
            rightValue = mRightOperand->EvaluateFloatValue();
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
        return mRightOperand->EvaluateFloatValue();
    }
}

double MultiplicativeExpression::EvaluateDoubleValue()
{
    if (mLeftOperand != nullptr) {
        double leftValue, rightValue;
        String typeName = mLeftOperand->GetType()->GetName();
        if (typeName.Equals("Integer")) {
            leftValue = mLeftOperand->EvaluateIntegerValue();
        }
        else if (typeName.Equals("Long")) {
            leftValue = mLeftOperand->EvaluateLongValue();
        }
        else if (typeName.Equals("Float")) {
            leftValue = mLeftOperand->EvaluateFloatValue();
        }
        else leftValue = mLeftOperand->EvaluateDoubleValue();
        typeName = mRightOperand->GetType()->GetName();
        if (typeName.Equals("Integer")) {
            rightValue = mRightOperand->EvaluateIntegerValue();
        }
        else if (typeName.Equals("Long")) {
            rightValue = mRightOperand->EvaluateLongValue();
        }
        else if (typeName.Equals("Float")) {
            rightValue = mRightOperand->EvaluateFloatValue();
        }
        else rightValue = mRightOperand->EvaluateDoubleValue();
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
        return mRightOperand->EvaluateDoubleValue();
    }
}

char MultiplicativeExpression::EvaluateCharacterValue()
{
    return mRightOperand->EvaluateCharacterValue();
}

bool MultiplicativeExpression::EvaluateBooleanValue()
{
    return mRightOperand->EvaluateBooleanValue();
}

String MultiplicativeExpression::EvaluateStringValue()
{
    return mRightOperand->EvaluateStringValue();
}

String MultiplicativeExpression::EvaluateEnumeratorName()
{
    return mRightOperand->EvaluateEnumeratorName();
}

}
}
