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

#ifndef __CCDL_AST_PARAMETER_H__
#define __CCDL_AST_PARAMETER_H__

#include "Expression.h"
#include "Node.h"
#include "Type.h"
#include "../util/String.h"

namespace ccdl {
namespace ast {

class Parameter : public Node
{
public:
    Parameter();

    ~Parameter();

    inline int GetAttribute()
    { return mAttribute; }

    inline void SetAttribute(
        /* [in] */ int attr)
    { mAttribute = mAttribute | attr; }

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

    void SetDefaultValue(
        /* [in] */ Expression* expr);

    String Dump(
        /* [in] */ const String& prefix) override;

public:
    static constexpr int IN = 0x1;
    static constexpr int OUT = 0x2;
    static constexpr int CALLEE = 0x4;

private:
    String mName;
    Type* mType;
    Expression* mDefaultValue;
    int mAttribute;
};

}
}

#endif // __CCDL_AST_PARAMETER_H__