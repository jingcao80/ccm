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

#ifndef __CDLC_CONSTANT_H__
#define __CDLC_CONSTANT_H__

#include "ast/Expression.h"
#include "ast/Node.h"
#include "ast/Type.h"
#include "util/String.h"

namespace cdlc {

class Namespace;

class Constant
    : public Node
{
public:
    inline String GetName();

    inline void SetName(
        /* [in] */ const String& name);

    inline AutoPtr<Type> GetType();

    inline void SetType(
        /* [in] */ Type* type);

    inline AutoPtr<Expression> GetValue();

    inline void SetValue(
        /* [in] */ Expression* value);

    inline AutoPtr<Namespace> GetNamespace();

    inline void SetNamespace(
        /* [in] */ Namespace* ns);

    String ToString() override;

    String Dump(
        /* [in] */ const String& prefix) override;

    AutoPtr<Node> Clone(
        /* [in] */ Module* module,
        /* [in] */ bool deepCopy) override;

private:
    String mName;
    AutoPtr<Type> mType;
    AutoPtr<Expression> mValue;
    Namespace* mNamespace = nullptr;
};

String Constant::GetName()
{
    return mName;
}

void Constant::SetName(
    /* [in] */ const String& name)
{
    mName = name;
}

AutoPtr<Type> Constant::GetType()
{
    return mType;
}

void Constant::SetType(
    /* [in] */ Type* type)
{
    mType = type;
}

AutoPtr<Expression> Constant::GetValue()
{
    return mValue;
}

void Constant::SetValue(
    /* [in] */ Expression* value)
{
    mValue = value;
}

AutoPtr<Namespace> Constant::GetNamespace()
{
    return mNamespace;
}

void Constant::SetNamespace(
    /* [in] */ Namespace* ns)
{
    mNamespace = ns;
}

}

#endif // __CDLC_CONSTANT_H__
