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

#ifndef __CCM_CCDL_EXCLUSIVEOREXPRESSION_H__
#define __CCM_CCDL_EXCLUSIVEOREXPRESSION_H__

#include "Expression.h"
#include "AndExpression.h"

namespace ccm {
namespace ccdl {

class ExclusiveOrExpression : public Expression
{
public:
    ExclusiveOrExpression()
        : mLeftOperand(nullptr)
        , mRightOperand(nullptr)
    {}

    ~ExclusiveOrExpression();

    inline ExclusiveOrExpression& SetLeftOperand(
        /* [in] */ ExclusiveOrExpression* leftOperand)
    { mLeftOperand = leftOperand; return *this; }
    inline ExclusiveOrExpression& SetRightOperand(
        /* [in] */ AndExpression* rightOperand)
    { mRightOperand = rightOperand; return *this; }

private:
    ExclusiveOrExpression* mLeftOperand;
    AndExpression* mRightOperand;
};

}
}

#endif // __CCM_CCDL_EXCLUSIVEOREXPRESSION_H__
