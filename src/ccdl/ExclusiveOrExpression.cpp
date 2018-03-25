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

#include "ExclusiveOrExpression.h"

namespace ccm {
namespace ccdl {

int ExclusiveOrExpression::EvaluateIntegerValue()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->EvaluateIntegerValue() ^
                mRightOperand->EvaluateIntegerValue();
    }
    else {
        return mRightOperand->EvaluateIntegerValue();
    }
}

long long int ExclusiveOrExpression::EvaluateLongValue()
{
    if (mLeftOperand != nullptr) {
        long long int leftValue = mLeftOperand->GetType()->GetName().Equals("Integer") ?
                mLeftOperand->EvaluateIntegerValue() : mLeftOperand->EvaluateLongValue();
        long long int rightValue = mRightOperand->GetType()->GetName().Equals("Integer") ?
                mRightOperand->EvaluateIntegerValue() : mRightOperand->EvaluateLongValue();
        return leftValue ^ rightValue;
    }
    else {
        return mRightOperand->EvaluateLongValue();
    }
}

float ExclusiveOrExpression::EvaluateFloatValue()
{
    return mRightOperand->EvaluateFloatValue();
}

double ExclusiveOrExpression::EvaluateDoubleValue()
{
    return mRightOperand->EvaluateDoubleValue();
}

char ExclusiveOrExpression::EvaluateCharacterValue()
{
    return mRightOperand->EvaluateCharacterValue();
}

bool ExclusiveOrExpression::EvaluateBooleanValue()
{
    return mRightOperand->EvaluateBooleanValue();
}

String ExclusiveOrExpression::EvaluateStringValue()
{
    return mRightOperand->EvaluateStringValue();
}

String ExclusiveOrExpression::EvaluateEnumeratorName()
{
    return mRightOperand->EvaluateEnumeratorName();
}

}
}
