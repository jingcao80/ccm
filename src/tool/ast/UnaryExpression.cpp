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

namespace ccdl {
namespace ast {

int UnaryExpression::IntegerValue()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->IntegerValue();
    }
    else if (mRightOperand != nullptr) {
        switch (mOperator) {
            case POSITIVE:
                return mRightOperand->IntegerValue();
            case NEGATIVE:
                return - mRightOperand->IntegerValue();
            case COMPLIMENT:
                return ~ mRightOperand->IntegerValue();
            case NOT:
                return ! mRightOperand->IntegerValue();
            default:
                return 0;
        }
    }
    return 0;
}

long long int UnaryExpression::LongValue()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->LongValue();
    }
    else if (mRightOperand != nullptr) {
        switch (mOperator) {
            case POSITIVE:
                return mRightOperand->LongValue();
            case NEGATIVE:
                return - mRightOperand->IntegerValue();
            case COMPLIMENT:
                return ~ mRightOperand->IntegerValue();
            case NOT:
                return ! mRightOperand->IntegerValue();
            default:
                return 0;
        }
    }
    return 0;
}

float UnaryExpression::FloatValue()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->FloatValue();
    }
    else if (mRightOperand != nullptr) {
        switch (mOperator) {
            case POSITIVE:
                return + mRightOperand->FloatValue();
            case NEGATIVE:
                return - mRightOperand->FloatValue();
            case NOT:
                return ! mRightOperand->FloatValue();
            case COMPLIMENT:
            default:
                return 0;
        }
    }
    return 0;
}

double UnaryExpression::DoubleValue()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->DoubleValue();
    }
    else if (mRightOperand != nullptr) {
        switch (mOperator) {
            case POSITIVE:
                return + mRightOperand->DoubleValue();
            case NEGATIVE:
                return - mRightOperand->DoubleValue();
            case NOT:
                return ! mRightOperand->DoubleValue();
            case COMPLIMENT:
            default:
                return 0;
        }
    }
    return 0;
}

char UnaryExpression::CharacterValue()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->CharacterValue();
    }
    else if (mRightOperand != nullptr) {
        switch (mOperator) {
            case POSITIVE:
                return + mRightOperand->CharacterValue();
            case NEGATIVE:
                return - mRightOperand->CharacterValue();
            case COMPLIMENT:
                return ~ mRightOperand->CharacterValue();
            case NOT:
                return ! mRightOperand->CharacterValue();
            default:
                return 0;
        }
    }
    return 0;
}

bool UnaryExpression::BooleanValue()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->BooleanValue();
    }
    else if (mRightOperand != nullptr) {
        switch (mOperator) {
            case NOT:
                return ! mRightOperand->BooleanValue();
            case POSITIVE:
            case NEGATIVE:
            case COMPLIMENT:
            default:
                return false;
        }
    }
    return false;
}

String UnaryExpression::StringValue()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->StringValue();
    }
    return String(nullptr);
}

String UnaryExpression::EnumeratorValue()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->EnumeratorValue();
    }
    return String(nullptr);
}

}
}
