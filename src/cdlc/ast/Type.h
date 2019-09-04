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

#ifndef __CDLC_TYPE_H__
#define __CDLC_TYPE_H__

#include "ast/Node.h"
#include "util/AutoPtr.h"
#include "util/String.h"

namespace cdlc {

class Module;
class Namespace;

class Type
    : public Node
{
public:
    inline void SetModule(
        /* [in] */ Module* module);

    inline String GetName();

    inline void SetName(
        /* [in] */ const String& name);

    inline AutoPtr<Namespace> GetNamespace();

    void SetNamespace(
        /* [in] */ Namespace* ns);

    inline bool IsForwardDeclared();

    inline void SetForwardDeclared(
        /* [in] */ bool forwardDeclared);

    virtual bool IsBooleanType();

    virtual bool IsIntegerType();

    virtual bool IsLongType();

    virtual bool IsFloatType();

    virtual bool IsDoubleType();

    virtual bool IsIntegralType();

    virtual bool IsFloatingPointType();

    virtual bool IsNumericType();

    virtual bool IsHANDLEType();

    virtual bool IsStringType();

    virtual bool IsEnumerationType();

    virtual bool IsInterfaceType();

    virtual bool IsPointerType();

    virtual String GetSignature() = 0;

    String ToString() override;

    String Dump(
        /* [in] */ const String& prefix) override;

    static AutoPtr<Type> Choose(
        /* [in] */ Type* type1,
        /* [in] */ Type* type2);

protected:
    Module* mModule = nullptr;

    String mName;
    Namespace* mNamespace = nullptr;
    bool mForwardDeclared = false;
};

void Type::SetModule(
    /* [in] */ Module* module)
{
    mModule = module;
}

String Type::GetName()
{
    return mName;
}

void Type::SetName(
    /* [in] */ const String& name)
{
    mName = name;
}

AutoPtr<Namespace> Type::GetNamespace()
{
    return mNamespace;
}

bool Type::IsForwardDeclared()
{
    return mForwardDeclared;
}

void Type::SetForwardDeclared(
    /* [in] */ bool forwardDeclared)
{
    mForwardDeclared = forwardDeclared;
}

}

#endif // __CDLC_TYPE_H__
