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

#include "ast/EnumerationType.h"
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
public:
    inline Module();

    inline void SetName(
        /* [in] */ const String& name);

    inline void AddNamespace(
        /* [in] */ Namespace* ns);

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
    friend class Namespace;

    String mName;
    AutoPtr<Namespace> mGlobalNamespace;

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

}

#endif // __CDLC_MODULE_H__
