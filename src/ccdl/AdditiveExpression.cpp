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

int AdditiveExpression::IntegerValue()
{
    if (mLeftOperand != nullptr) {
        switch (mOperator) {
            case PLUS:
                return mLeftOperand->IntegerValue() +
                        mRightOperand->IntegerValue();
            case MINUS:
                return mLeftOperand->IntegerValue() -
                        mRightOperand->IntegerValue();
            default:
                return 0;
        }
    }
    else {
        return mRightOperand->IntegerValue();
    }
}

long long int AdditiveExpression::LongValue()
{
    if (mLeftOperand != nullptr) {
        long long int leftValue = mLeftOperand->GetType()->GetName().Equals("Integer") ?
                mLeftOperand->IntegerValue() : mLeftOperand->LongValue();
        long long int rightValue = mRightOperand->GetType()->GetName().Equals("Integer") ?
                mRightOperand->IntegerValue() : mRightOperand->LongValue();
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
        return mRightOperand->LongValue();
    }
}

float AdditiveExpression::FloatValue()
{
    if (mLeftOperand != nullptr) {
        float leftValue, rightValue;
        String typeName = mLeftOperand->GetType()->GetName();
        if (typeName.Equals("Integer")) {
            leftValue = mLeftOperand->IntegerValue();
        }
        else if (typeName.Equals("Long")) {
            leftValue = mLeftOperand->LongValue();
        }
        else {
            leftValue = mLeftOperand->FloatValue();
        }
        typeName = mRightOperand->GetType()->GetName();
        if (typeName.Equals("Integer")) {
            rightValue = mRightOperand->IntegerValue();
        }
        else if (typeName.Equals("Long")) {
            rightValue = mRightOperand->LongValue();
        }
        else {
            rightValue = mRightOperand->FloatValue();
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
        return mRightOperand->FloatValue();
    }
}

double AdditiveExpression::DoubleValue()
{
    if (mLeftOperand != nullptr) {
        double leftValue, rightValue;
        String typeName = mLeftOperand->GetType()->GetName();
        if (typeName.Equals("Integer")) {
            leftValue = mLeftOperand->IntegerValue();
        }
        else if (typeName.Equals("Long")) {
            leftValue = mLeftOperand->LongValue();
        }
        else if (typeName.Equals("Float")) {
            leftValue = mLeftOperand->FloatValue();
        }
        else leftValue = mLeftOperand->DoubleValue();
        typeName = mRightOperand->GetType()->GetName();
        if (typeName.Equals("Integer")) {
            rightValue = mRightOperand->IntegerValue();
        }
        else if (typeName.Equals("Long")) {
            rightValue = mRightOperand->LongValue();
        }
        else if (typeName.Equals("Float")) {
            rightValue = mRightOperand->FloatValue();
        }
        else rightValue = mRightOperand->DoubleValue();
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
        return mRightOperand->DoubleValue();
    }
}

char AdditiveExpression::CharacterValue()
{
    return mRightOperand->CharacterValue();
}

bool AdditiveExpression::BooleanValue()
{
    return mRightOperand->BooleanValue();
}

String AdditiveExpression::StringValue()
{
    return mRightOperand->StringValue();
}

String AdditiveExpression::EnumeratorValue()
{
    return mRightOperand->EnumeratorValue();
}

}
}
