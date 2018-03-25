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

#include "UnaryExpression.h"

namespace ccm {
namespace ccdl {

int UnaryExpression::EvaluateIntegerValue()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->EvaluateIntegerValue();
    }
    else if (mRightOperand != nullptr) {
        switch (mOperator) {
            case POSITIVE:
                return mRightOperand->EvaluateIntegerValue();
            case NEGATIVE:
                return - mRightOperand->EvaluateIntegerValue();
            case COMPLIMENT:
                return ~ mRightOperand->EvaluateIntegerValue();
            case NOT:
                return ! mRightOperand->EvaluateIntegerValue();
            default:
                return 0;
        }
    }
    return 0;
}

long long int UnaryExpression::EvaluateLongValue()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->EvaluateLongValue();
    }
    else if (mRightOperand != nullptr) {
        switch (mOperator) {
            case POSITIVE:
                return mRightOperand->EvaluateLongValue();
            case NEGATIVE:
                return - mRightOperand->EvaluateIntegerValue();
            case COMPLIMENT:
                return ~ mRightOperand->EvaluateIntegerValue();
            case NOT:
                return ! mRightOperand->EvaluateIntegerValue();
            default:
                return 0;
        }
    }
    return 0;
}

float UnaryExpression::EvaluateFloatValue()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->EvaluateFloatValue();
    }
    else if (mRightOperand != nullptr) {
        switch (mOperator) {
            case POSITIVE:
                return + mRightOperand->EvaluateFloatValue();
            case NEGATIVE:
                return - mRightOperand->EvaluateFloatValue();
            case NOT:
                return ! mRightOperand->EvaluateFloatValue();
            case COMPLIMENT:
            default:
                return 0;
        }
    }
    return 0;
}

double UnaryExpression::EvaluateDoubleValue()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->EvaluateDoubleValue();
    }
    else if (mRightOperand != nullptr) {
        switch (mOperator) {
            case POSITIVE:
                return + mRightOperand->EvaluateDoubleValue();
            case NEGATIVE:
                return - mRightOperand->EvaluateDoubleValue();
            case NOT:
                return ! mRightOperand->EvaluateDoubleValue();
            case COMPLIMENT:
            default:
                return 0;
        }
    }
    return 0;
}

char UnaryExpression::EvaluateCharacterValue()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->EvaluateCharacterValue();
    }
    else if (mRightOperand != nullptr) {
        switch (mOperator) {
            case POSITIVE:
                return + mRightOperand->EvaluateCharacterValue();
            case NEGATIVE:
                return - mRightOperand->EvaluateCharacterValue();
            case COMPLIMENT:
                return ~ mRightOperand->EvaluateCharacterValue();
            case NOT:
                return ! mRightOperand->EvaluateCharacterValue();
            default:
                return 0;
        }
    }
    return 0;
}

bool UnaryExpression::EvaluateBooleanValue()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->EvaluateBooleanValue();
    }
    else if (mRightOperand != nullptr) {
        switch (mOperator) {
            case NOT:
                return ! mRightOperand->EvaluateBooleanValue();
            case POSITIVE:
            case NEGATIVE:
            case COMPLIMENT:
            default:
                return false;
        }
    }
    return false;
}

String UnaryExpression::EvaluateStringValue()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->EvaluateStringValue();
    }
    return String(nullptr);
}

String UnaryExpression::EvaluateEnumeratorName()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->EvaluateEnumeratorName();
    }
    return String(nullptr);
}

}
}
