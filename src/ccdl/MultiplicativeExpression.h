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

#ifndef __CCM_CCDL_MULTIPLICATIVEEXPRESSION_H__
#define __CCM_CCDL_MULTIPLICATIVEEXPRESSION_H__

#include "Expression.h"
#include "UnaryExpression.h"

namespace ccm {
namespace ccdl {

class MultiplicativeExpression : public Expression
{
public:
    MultiplicativeExpression()
        : mLeftOperand(nullptr)
        , mRightOperand(nullptr)
        , mOperator(-1)
    {}

    ~MultiplicativeExpression();

    inline MultiplicativeExpression& SetLeftOperand(
        /* [in] */ MultiplicativeExpression* leftOperand)
    { mLeftOperand = leftOperand; return *this; }
    inline MultiplicativeExpression& SetRightOperand(
        /* [in] */ UnaryExpression* rightOperand)
    { mRightOperand = rightOperand; return *this; }
    inline MultiplicativeExpression& SetOperator(
        /* [in] */ int multiOperator)
    { mOperator = multiOperator; return *this; }

public:
    static constexpr int MULTIPLE = 0;
    static constexpr int DIVIDE = 1;
    static constexpr int MODULO = 2;

private:
    MultiplicativeExpression* mLeftOperand;
    UnaryExpression* mRightOperand;
    int mOperator;
};

}
}

#endif // __CCM_CCDL_MULTIPLICATIVEEXPRESSION_H__
