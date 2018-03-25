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

#include "AdditiveExpression.h"

namespace ccm {
namespace ccdl {

int AdditiveExpression::EvaluateIntegerValue()
{
    if (mLeftOperand != nullptr) {
        switch (mOperator) {
            case PLUS:
                return mLeftOperand->EvaluateIntegerValue() +
                        mRightOperand->EvaluateIntegerValue();
            case MINUS:
                return mLeftOperand->EvaluateIntegerValue() -
                        mRightOperand->EvaluateIntegerValue();
            default:
                return 0;
        }
    }
    else {
        return mRightOperand->EvaluateIntegerValue();
    }
}

long long int AdditiveExpression::EvaluateLongValue()
{
    if (mLeftOperand != nullptr) {
        long long int leftValue = mLeftOperand->GetType()->GetName().Equals("Integer") ?
                mLeftOperand->EvaluateIntegerValue() : mLeftOperand->EvaluateLongValue();
        long long int rightValue = mRightOperand->GetType()->GetName().Equals("Integer") ?
                mRightOperand->EvaluateIntegerValue() : mRightOperand->EvaluateLongValue();
        switch (mOperator) {
            case PLUS:
                return leftValue + rightValue;
            case MINUS:
                return leftValue - rightValue;
            default:
                return 0;
        }
    }
    else {
        return mRightOperand->EvaluateLongValue();
    }
}

float AdditiveExpression::EvaluateFloatValue()
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
            case PLUS:
                return leftValue + rightValue;
            case MINUS:
                return leftValue - rightValue;
            default:
                return 0;
        }
    }
    else {
        return mRightOperand->EvaluateFloatValue();
    }
}

double AdditiveExpression::EvaluateDoubleValue()
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
            case PLUS:
                return leftValue + rightValue;
            case MINUS:
                return leftValue - rightValue;
            default:
                return 0;
        }
    }
    else {
        return mRightOperand->EvaluateDoubleValue();
    }
}

char AdditiveExpression::EvaluateCharacterValue()
{
    return mRightOperand->EvaluateCharacterValue();
}

bool AdditiveExpression::EvaluateBooleanValue()
{
    return mRightOperand->EvaluateBooleanValue();
}

String AdditiveExpression::EvaluateStringValue()
{
    return mRightOperand->EvaluateStringValue();
}

String AdditiveExpression::EvaluateEnumeratorName()
{
    return mRightOperand->EvaluateEnumeratorName();
}

}
}
