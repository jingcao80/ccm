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

#ifndef __CCM_CCDL_INCLUSIVEOREXPRESSION_H__
#define __CCM_CCDL_INCLUSIVEOREXPRESSION_H__

#include "Expression.h"

namespace ccm {
namespace ccdl {

class InclusiveOrExpression : public Expression
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
};

}
}

#endif // __CCM_CCDL_INCLUSIVEOREXPRESSION_H__