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

#ifndef __CCDL_AST_MULTIPLICATIVEEXPRESSION_H__
#define __CCDL_AST_MULTIPLICATIVEEXPRESSION_H__

#include "Expression.h"

namespace ccdl {
namespace ast {

class MultiplicativeExpression : public Expression
{
public:
    int IntegerValue() override;

    long long int LongValue() override;

    float FloatValue() override;

    double DoubleValue() override;

    char CharacterValue() override;

    bool BooleanValue() override;

    String StringValue() override;

    String EnumeratorValue() override;

    Expression* Clone() override;

public:
    static constexpr int MULTIPLE = 0;
    static constexpr int DIVIDE = 1;
    static constexpr int MODULO = 2;
};

}
}

#endif // __CCDL_AST_MULTIPLICATIVEEXPRESSION_H__
