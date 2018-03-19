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

#ifndef __CCM_CCDL_ADDITIVEEXPRESSION_H__
#define __CCM_CCDL_ADDITIVEEXPRESSION_H__

#include "Expression.h"
#include "MultiplicativeExpression.h"

namespace ccm {
namespace ccdl {

class AdditiveExpression : public Expression
{
public:
    AdditiveExpression()
        : mLeftOperand(nullptr)
        , mRightOperand(nullptr)
        , mOperator(-1)
    {}

    ~AdditiveExpression();

    inline AdditiveExpression& SetLeftOperand(
        /* [in] */ AdditiveExpression* leftOperand)
    { mLeftOperand = leftOperand; return *this; }
    inline AdditiveExpression& SetRightOperand(
        /* [in] */ MultiplicativeExpression* rightOperand)
    { mRightOperand = rightOperand; return *this; }
    inline AdditiveExpression& SetOperator(
        /* [in] */ int addOperator)
    { mOperator = addOperator; return *this; }

public:
    static constexpr int PLUS = 0;
    static constexpr int MINUS = 1;

private:
    AdditiveExpression* mLeftOperand;
    MultiplicativeExpression* mRightOperand;
    int mOperator;
};

}
}


#endif // __CCM_CCDL_ADDITIVEEXPRESSION_H__
