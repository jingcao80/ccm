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

#ifndef __CDLC_MODULE_H__
#define __CDLC_MODULE_H__

#include "ast/CoclassType.h"
#include "ast/Constant.h"
#include "ast/EnumerationType.h"
#include "ast/InterfaceType.h"
#include "ast/Namespace.h"
#include "ast/Node.h"
#include "ast/Type.h"
#include "util/AutoPtr.h"
#include <unordered_map>
#include <vector>

namespace cdlc {

class Module
    : public Node
{
    friend class Namespace;

public:
    inline Module();

    void SetAttributes(
        /* [in] */ const Attributes& attrs);

    inline void SetName(
        /* [in] */ const String& name);

    AutoPtr<Namespace> ParseNamespace(
        /* [in] */ const String& nsString);

    inline AutoPtr<Namespace> FindNamespace(
        /* [in] */ const String& nsString);

    void AddTemporaryType(
        /* [in] */ Type* type);

    AutoPtr<EnumerationType> FindEnumeration(
        /* [in] */ const String& fullName);

    AutoPtr<Type> FindType(
        /* [in] */ const String& fullName);

    String ToString() override;

    String Dump(
        /* [in] */ const String& prefix) override;

private:
    inline void AddNamespace(
        /* [in] */ Namespace* ns);

    inline void AddConstant(
        /* [in] */ Constant* constant);

    inline void AddEnumerationType(
        /* [in] */ EnumerationType* enumeration);

    inline void AddInterfaceType(
        /* [in] */ InterfaceType* interface);

    inline void AddCoclassType(
        /* [in] */ CoclassType* klass);

    void AddType(
        /* [in] */ Type* type);

private:
    AutoPtr<UUID> mUuid;
    String mVersion;
    String mDescription;
    String mUri;
    String mName;
    AutoPtr<Namespace> mGlobalNamespace;

    std::vector<AutoPtr<Constant>> mConstants;
    std::vector<AutoPtr<EnumerationType>> mEnumerations;
    std::vector<AutoPtr<InterfaceType>> mInterfaces;
    std::vector<AutoPtr<CoclassType>> mKlasses;
    std::unordered_map<String, AutoPtr<Type>, StringHashFunc, StringEqualsFunc> mAllTypeMap;
};

Module::Module()
    : mGlobalNamespace(new Namespace(Namespace::GLOBAL_NAME, this))
{}

void Module::SetName(
    /* [in] */ const String& name)
{
    mName = name;
}

void Module::AddNamespace(
    /* [in] */ Namespace* ns)
{
    mGlobalNamespace->AddNamespace(ns);
}

AutoPtr<Namespace> Module::FindNamespace(
    /* [in] */ const String& nsString)
{
    if (nsString.Equals(Namespace::GLOBAL_NAME)) {
        return mGlobalNamespace;
    }
    else {
        return mGlobalNamespace->FindNamespace(nsString);
    }
}

void Module::AddConstant(
    /* [in] */ Constant* constant)
{
    mConstants.push_back(constant);
    constant->SetModule(this);
}

void Module::AddEnumerationType(
    /* [in] */ EnumerationType* enumeration)
{
    mEnumerations.push_back(enumeration);
    AddType(enumeration);
}

void Module::AddInterfaceType(
    /* [in] */ InterfaceType* interface)
{
    mInterfaces.push_back(interface);
    AddType(interface);
}

void Module::AddCoclassType(
    /* [in] */ CoclassType* klass)
{
    mKlasses.push_back(klass);
    AddType(klass);
}

}

#endif // __CDLC_MODULE_H__
