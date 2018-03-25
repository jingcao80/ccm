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

#ifndef __CCM_CCDL_UNARYEXPRESSION_H__
#define __CCM_CCDL_UNARYEXPRESSION_H__

#include "Expression.h"
#include "PostfixExpression.h"

namespace ccm {
namespace ccdl {

class UnaryExpression : public Expression
{
public:
    int EvaluateIntegerValue() override;

    long long int EvaluateLongValue() override;

    float EvaluateFloatValue() override;

    double EvaluateDoubleValue() override;

    char EvaluateCharacterValue() override;

    bool EvaluateBooleanValue() override;

    String EvaluateStringValue() override;

    String EvaluateEnumeratorName() override;

public:
    static constexpr int POSITIVE = 0;
    static constexpr int NEGATIVE = 1;
    static constexpr int COMPLIMENT = 2;
    static constexpr int NOT = 3;
};

}
}

#endif // __CCM_CCDL_UNARYEXPRESSION_H__
