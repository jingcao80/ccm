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

#include "ast/InclusiveOrExpression.h"
#include "ast/Namespace.h"

namespace cdlc {

int InclusiveOrExpression::IntegerValue()
{
    if (mLeftOperand != nullptr) {
        return mLeftOperand->IntegerValue()
                | mRightOperand->IntegerValue();
    }
    else {
        return mRightOperand->IntegerValue();
    }
}

long long int InclusiveOrExpression::LongValue()
{
    if (mLeftOperand != nullptr) {
        long long int leftValue = mLeftOperand->GetType()->IsIntegerType()
                ? mLeftOperand->IntegerValue() : mLeftOperand->LongValue();
        long long int rightValue = mRightOperand->GetType()->IsIntegerType()
                ? mRightOperand->IntegerValue() : mRightOperand->LongValue();
        return leftValue | rightValue;
    }
    else {
        return mRightOperand->LongValue();
    }
}

float InclusiveOrExpression::FloatValue()
{
    return mRightOperand->FloatValue();
}

double InclusiveOrExpression::DoubleValue()
{
    return mRightOperand->DoubleValue();
}

}
