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

#ifndef __CCM_CCDL_EXPRESSION_H__
#define __CCM_CCDL_EXPRESSION_H__

#include "ASTElement.h"

namespace ccm {
namespace ccdl {

class Expression : public ASTElement
{
public:
    Expression()
        : mLeftOperand(nullptr)
        , mRightOperand(nullptr)
        , mOperator(-1)
    {}

    virtual ~Expression();

    inline Expression& SetLeftOperand(
        /* [in] */ Expression* leftOperand)
    { mLeftOperand = leftOperand; return *this; }

    inline Expression& SetRightOperand(
        /* [in] */ Expression* rightOperand)
    { mRightOperand = rightOperand; return *this; }

    inline Expression& SetOperator(
        /* [in] */ int addOperator)
    { mOperator = addOperator; return *this; }

protected:
    Expression* mLeftOperand;
    Expression* mRightOperand;
    int mOperator;
};

}
}

#endif // __CCM_CCDL_EXPRESSION_H__
