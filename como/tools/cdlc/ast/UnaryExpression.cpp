//=========================================================================
// Copyright (C) 2018 The C++ Component Model(COMO) Open Source Project
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

#include "ast/UnaryExpression.h"
#include "ast/Namespace.h"

namespace cdlc {

bool UnaryExpression::BooleanValue()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->BooleanValue();
    }
    else if (mRightOperand != nullptr) {
        switch (mOperator) {
            case OPERATOR_NOT:
                return ! mRightOperand->BooleanValue();
            case OPERATOR_POSITIVE:
            case OPERATOR_NEGATIVE:
            case OPERATOR_COMPLIMENT:
            default:
                return false;
        }
    }
    return false;
}

char UnaryExpression::CharacterValue()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->CharacterValue();
    }
    else if (mRightOperand != nullptr) {
        switch (mOperator) {
            case OPERATOR_POSITIVE:
                return + mRightOperand->CharacterValue();
            case OPERATOR_NEGATIVE:
                return - mRightOperand->CharacterValue();
            case OPERATOR_COMPLIMENT:
                return ~ mRightOperand->CharacterValue();
            case OPERATOR_NOT:
                return ! mRightOperand->CharacterValue();
            default:
                return 0;
        }
    }
    return 0;
}

int UnaryExpression::IntegerValue()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->IntegerValue();
    }
    else if (mRightOperand != nullptr) {
        switch (mOperator) {
            case OPERATOR_POSITIVE: {
                return mRightOperand->IntegerValue();
            }
            case OPERATOR_NEGATIVE: {
                return -mRightOperand->IntegerValue();
            }
            case OPERATOR_COMPLIMENT: {
                return ~mRightOperand->IntegerValue();
            }
            case OPERATOR_NOT: {
                return !mRightOperand->IntegerValue();
            }
            default: {
                return 0;
            }
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
            case OPERATOR_POSITIVE: {
                return mRightOperand->LongValue();
            }
            case OPERATOR_NEGATIVE: {
                return -mRightOperand->IntegerValue();
            }
            case OPERATOR_COMPLIMENT: {
                return ~mRightOperand->IntegerValue();
            }
            case OPERATOR_NOT: {
                return !mRightOperand->IntegerValue();
            }
            default: {
                return 0;
            }
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
            case OPERATOR_POSITIVE: {
                return mRightOperand->FloatValue();
            }
            case OPERATOR_NEGATIVE: {
                return -mRightOperand->FloatValue();
            }
            case OPERATOR_NOT: {
                return !mRightOperand->FloatValue();
            }
            case OPERATOR_COMPLIMENT:
            default: {
                return 0;
            }
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
            case OPERATOR_POSITIVE: {
                return mRightOperand->DoubleValue();
            }
            case OPERATOR_NEGATIVE: {
                return -mRightOperand->DoubleValue();
            }
            case OPERATOR_NOT: {
                return !mRightOperand->DoubleValue();
            }
            case OPERATOR_COMPLIMENT:
            default: {
                return 0;
            }
        }
    }
    return 0;
}

String UnaryExpression::StringValue()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->StringValue();
    }
    return nullptr;
}

String UnaryExpression::EnumeratorValue()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->EnumeratorValue();
    }
    return nullptr;
}

bool UnaryExpression::IsPositiveInfinity()
{
    return false;
}

bool UnaryExpression::IsNegativeInfinity()
{
    return false;
}

bool UnaryExpression::IsNaN()
{
    return false;
}

}
