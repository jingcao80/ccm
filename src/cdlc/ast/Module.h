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

    inline void AddNamespace(
        /* [in] */ Namespace* ns);

    AutoPtr<Namespace> ParseNamespace(
        /* [in] */ const String& nsString);

    inline AutoPtr<Namespace> FindNamespace(
        /* [in] */ const String& nsString);

    String ToString() override;

    String Dump(
        /* [in] */ const String& prefix) override;

private:
    friend class Namespace;

    Namespace mGlobalNamespace;

    std::unordered_map<String, AutoPtr<Type>, StringHashFunc, StringEqualsFunc> mAllTypeMap;
};

Module::Module()
    : mGlobalNamespace(Namespace::GLOBAL_NAME, this)
{}

void Module::AddNamespace(
    /* [in] */ Namespace* ns)
{
    mGlobalNamespace.AddNamespace(ns);
}

AutoPtr<Namespace> Module::FindNamespace(
    /* [in] */ const String& nsString)
{
    return mGlobalNamespace.FindNamespace(nsString);
}

}

#endif // __CDLC_MODULE_H__
