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

#ifndef __CCDL_AST_CONSTANT_H__
#define __CCDL_AST_CONSTANT_H__

#include "Node.h"
#include "Expression.h"
#include "Type.h"
#include "../util/String.h"

namespace ccdl {
namespace ast {

class Namespace;
class Pool;

class Constant : public Node
{
public:
    Constant();

    inline String GetName();

    inline void SetName(
        /* [in] */ const String& name);

    inline Type* GetType();

    inline void SetType(
        /* [in] */ Type* type);

    inline Expression* GetValue();

    inline void SetValue(
        /* [in] */ Expression* value);

    inline Namespace* GetNamespace();

    void SetNamespace(
        /* [in] */ Namespace* ns);

    void DeepCopy(
        /* [in] */ Constant* source,
        /* [in] */ Pool* pool);

    String Dump(
        /* [in] */ const String& prefix) override;

private:
    String mName;
    Type* mType;
    Expression* mValue;
    Namespace* mNamespace;
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

Type* Constant::GetType()
{
    return mType;
}

void Constant::SetType(
    /* [in] */ Type* type)
{
    mType = type;
}

Expression* Constant::GetValue()
{
    return mValue;
}

void Constant::SetValue(
    /* [in] */ Expression* value)
{
    mValue = value;
}

Namespace* Constant::GetNamespace()
{
    return mNamespace;
}

}
}

#endif // __CCDL_AST_CONSTANT_H__
