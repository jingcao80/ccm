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

#ifndef __CCM_CCDL_CONSTANT_H__
#define __CCM_CCDL_CONSTANT_H__

#include "ASTNode.h"
#include "Expression.h"
#include "Type.h"
#include "../util/String.h"

namespace ccm {
namespace ccdl {

class Constant : public ASTNode
{
public:
    Constant()
        : mType(nullptr)
        , mValue(nullptr)
    {}

    inline String GetName()
    { return mName; }

    inline void SetName(
        /* [in] */ const String& name)
    { mName = name; }

    inline Type* GetType()
    { return mType; }

    inline void SetType(
        /* [in] */ Type* type)
    { mType = type; }

    inline Expression* GetValue()
    { return mValue; }

    inline void SetValue(
        /* [in] */ Expression* value)
    { mValue = value; }

    String Dump(
        /* [in] */ const String& prefix) override;

private:
    String mName;
    Type* mType;
    Expression* mValue;
};

}
}

#endif // __CCM_CCDL_CONSTANT_H__
