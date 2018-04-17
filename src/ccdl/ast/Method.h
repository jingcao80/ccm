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

#ifndef __CCDL_AST_METHOD_H__
#define __CCDL_AST_METHOD_H__

#include "Node.h"
#include "Parameter.h"
#include "Type.h"
#include "../util/ArrayList.h"
#include "../util/String.h"

namespace ccdl {
namespace ast {

class Pool;

class Method : public Node
{
public:
    Method();

    inline String GetName();

    inline void SetName(
        /* [in] */ const String& name);

    inline String GetSignature();

    inline Type* GetReturnType();

    inline void SetReturnType(
        /* [in] */ Type* type);

    bool AddParameter(
        /* [in] */ Parameter* param);

    inline int GetParameterNumber();

    inline Parameter* GetParameter(
        /* [in] */ int index);

    void BuildSignature();

    void DeepCopy(
        /* [in] */ Method* source,
        /* [in] */ Pool* pool);

    String ToString() override;

    String Dump(
        /* [in] */ const String& prefix) override;

private:
    String mName;
    String mSignature;
    Type* mReturnType;
    ArrayList<Parameter*> mParameters;
};

String Method::GetName()
{
    return mName;
}

void Method::SetName(
    /* [in] */ const String& name)
{
    mName = name;
}

String Method::GetSignature()
{
    return mSignature;
}

Type* Method::GetReturnType()
{
    return mReturnType;
}

void Method::SetReturnType(
    /* [in] */ Type* type)
{
    mReturnType = type;
}

int Method::GetParameterNumber()
{
    return mParameters.GetSize();
}

Parameter* Method::GetParameter(
    /* [in] */ int index)
{
    return mParameters.Get(index);
}

}
}

#endif // __CCDL_AST_METHOD_H__
