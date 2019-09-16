//=========================================================================
// Copyright (C) 2018 The C++ Component Model(COMO) Open Source Project
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

#ifndef __CDLC_PARAMETER_H__
#define __CDLC_PARAMETER_H__

#include "ast/Expression.h"
#include "ast/Node.h"
#include "ast/Type.h"
#include "util/AutoPtr.h"

namespace cdlc {

class Parameter
    : public Node
{
public:
    inline String GetName();

    inline void SetName(
        /* [in] */ const String& name);

    inline AutoPtr<Type> GetType();

    inline void SetType(
        /* [in] */ Type* type);

    inline AutoPtr<Expression> GetDefaultValue();

    inline bool HasDefaultValue();

    inline void SetDefaultValue(
        /* [in] */ Expression* expr);

    inline bool IsCallee();

    inline bool IsIn();

    inline bool IsOut();

    inline void SetAttributes(
        /* [in] */ int attrs);

    String ToString() override;

    String Dump(
        /* [in] */ const String& prefix) override;

public:
    static constexpr int IN = 0x01;
    static constexpr int OUT = 0x02;
    static constexpr int CALLEE = 0x04;

private:
    String mName;
    AutoPtr<Type> mType;
    AutoPtr<Expression> mValue;
    int mAttributes = 0;
};

String Parameter::GetName()
{
    return mName;
}

void Parameter::SetName(
    /* [in] */ const String& name)
{
    mName = name;
}

AutoPtr<Type> Parameter::GetType()
{
    return mType;
}

void Parameter::SetType(
    /* [in] */ Type* type)
{
    mType = type;
}

AutoPtr<Expression> Parameter::GetDefaultValue()
{
    return mValue;
}

bool Parameter::HasDefaultValue()
{
    return mValue != nullptr;
}

void Parameter::SetDefaultValue(
    /* [in] */ Expression* expr)
{
    mValue = expr;
}

bool Parameter::IsCallee()
{
    return mAttributes & CALLEE == CALLEE;
}

bool Parameter::IsIn()
{
    return mAttributes & IN == IN;
}

bool Parameter::IsOut()
{
    return mAttributes & OUT == OUT;
}

void Parameter::SetAttributes(
    /* [in] */ int attrs)
{
    mAttributes |= attrs;
}

}

#endif // __CDLC_PARAMETER_H__
