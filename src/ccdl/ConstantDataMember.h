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

#ifndef __CCM_CCDL_CONSTANTDATAMEMBER_H__
#define __CCM_CCDL_CONSTANTDATAMEMBER_H__

#include "ASTElement.h"
#include "Expression.h"
#include "Type.h"
#include "../util/String.h"

namespace ccm {
namespace ccdl {

class ConstantDataMember : public ASTElement
{
public:
    ConstantDataMember()
        : mType(nullptr)
    {}

    inline ConstantDataMember& SetName(
        /* [in] */ const String& name)
    { mName = name; return *this; }

    inline ConstantDataMember& SetType(
        /* [in] */ Type* type)
    { mType = type; return *this; }

    inline ConstantDataMember& SetValue(
        /* [in] */ Expression* value)
    { mValue = value; return *this; }

private:
    Type* mType;
    String mName;
    Expression* mValue;
};

}
}

#endif // __CCM_CCDL_CONSTANTDATAMEMBER_H__
