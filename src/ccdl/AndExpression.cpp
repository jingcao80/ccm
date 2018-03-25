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

#include "AndExpression.h"

namespace ccm {
namespace ccdl {

int AndExpression::EvaluateIntegerValue()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->EvaluateIntegerValue() &
                mRightOperand->EvaluateIntegerValue();
    }
    else {
        return mRightOperand->EvaluateIntegerValue();
    }
}

long long int AndExpression::EvaluateLongValue()
{
    if (mLeftOperand != nullptr) {
        long long int leftValue = mLeftOperand->GetType()->GetName().Equals("Integer") ?
                mLeftOperand->EvaluateIntegerValue() : mLeftOperand->EvaluateLongValue();
        long long int rightValue = mRightOperand->GetType()->GetName().Equals("Integer") ?
                mRightOperand->EvaluateIntegerValue() : mRightOperand->EvaluateLongValue();
        return leftValue & rightValue;
    }
    else {
        return mRightOperand->EvaluateLongValue();
    }
}

float AndExpression::EvaluateFloatValue()
{
    return mRightOperand->EvaluateFloatValue();
}

double AndExpression::EvaluateDoubleValue()
{
    return mRightOperand->EvaluateDoubleValue();
}

char AndExpression::EvaluateCharacterValue()
{
    return mRightOperand->EvaluateCharacterValue();
}

bool AndExpression::EvaluateBooleanValue()
{
    return mRightOperand->EvaluateBooleanValue();
}

String AndExpression::EvaluateStringValue()
{
    return mRightOperand->EvaluateStringValue();
}

String AndExpression::EvaluateEnumeratorName()
{
    return mRightOperand->EvaluateEnumeratorName();
}

}
}
