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

namespace ccm {
namespace ccdl {

int ShiftExpression::EvaluateIntegerValue()
{
    if (mLeftOperand != nullptr) {
        switch (mOperator) {
            case LEFT:
                return mLeftOperand->EvaluateIntegerValue() <<
                        mRightOperand->EvaluateIntegerValue();
            case RIGHT:
                return mLeftOperand->EvaluateIntegerValue() >>
                        mRightOperand->EvaluateIntegerValue();
            case RIGHT_UNSIGNED:
                return ((unsigned int)mLeftOperand->EvaluateIntegerValue()) >>
                        mRightOperand->EvaluateIntegerValue();
            default:
                return 0;
        }
    }
    else {
        return mRightOperand->EvaluateIntegerValue();
    }
}

long long int ShiftExpression::EvaluateLongValue()
{
    if (mLeftOperand != nullptr) {
        long long int leftValue = mLeftOperand->GetType()->GetName().Equals("Integer") ?
                mLeftOperand->EvaluateIntegerValue() : mLeftOperand->EvaluateLongValue();
        long long int rightValue = mRightOperand->GetType()->GetName().Equals("Integer") ?
                mRightOperand->EvaluateIntegerValue() : mRightOperand->EvaluateLongValue();
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
        return mRightOperand->EvaluateLongValue();
    }
}

float ShiftExpression::EvaluateFloatValue()
{
    return mRightOperand->EvaluateFloatValue();
}

double ShiftExpression::EvaluateDoubleValue()
{
    return mRightOperand->EvaluateDoubleValue();
}

char ShiftExpression::EvaluateCharacterValue()
{
    return mRightOperand->EvaluateCharacterValue();
}

bool ShiftExpression::EvaluateBooleanValue()
{
    return mRightOperand->EvaluateBooleanValue();
}

String ShiftExpression::EvaluateStringValue()
{
    return mRightOperand->EvaluateStringValue();
}

String ShiftExpression::EvaluateEnumeratorName()
{
    return mRightOperand->EvaluateEnumeratorName();
}

}
}
