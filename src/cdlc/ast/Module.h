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
#include "metadata/Metadata.h"
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

    inline AutoPtr<UUID> GetUUID();

    inline String GetUri();

    inline String GetName();

    inline void SetName(
        /* [in] */ const String& name);

    AutoPtr<Namespace> ParseNamespace(
        /* [in] */ const String& nsString);

    inline AutoPtr<Namespace> FindNamespace(
        /* [in] */ const String& nsString);

    inline AutoPtr<Namespace> GetGlobalNamespace();

    inline int GetNamespaceNumber();

    AutoPtr<Constant> GetConstant(
        /* [in] */ int i);

    inline int GetConstantNumber();

    int IndexOf(
        /* [in] */ Constant* constant);

    AutoPtr<EnumerationType> FindEnumeration(
        /* [in] */ const String& fullName);

    AutoPtr<EnumerationType> GetEnumeration(
        /* [in] */ int i);

    inline int GetEnumerationNumber();

    int IndexOf(
        /* [in] */ EnumerationType* enumeration);

    AutoPtr<InterfaceType> GetInterface(
        /* [in] */ int i);

    inline int GetInterfaceNumber();

    int IndexOf(
        /* [in] */ InterfaceType* interface);

    AutoPtr<CoclassType> GetCoclass(
        /* [in] */ int i);

    inline int GetCoclassNumber();

    int IndexOf(
        /* [in] */ CoclassType* klass);

    void AddTemporaryType(
        /* [in] */ Type* type);

    AutoPtr<Type> FindType(
        /* [in] */ const String& fullName);

    inline const std::unordered_map<String, AutoPtr<Type>, StringHashFunc, StringEqualsFunc>&
    GetTypes();

    int IndexOf(
        /* [in] */ Type* type);

    inline int GetTypeNumber();

    String ToString() override;

    String Dump(
        /* [in] */ const String& prefix) override;

    static AutoPtr<Module> Resolve(
        /* [in] */ void* metadata);

private:
    Module(
        /* [in] */ como::MetaComponent* component);

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

    void ResolveNamespace(
        /*[in] */ como::MetaNamespace* mn);

    AutoPtr<Type> ResolveType(
        /* [in] */ Namespace* ns,
        /* [in] */ const String& typeName);

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

    std::vector<AutoPtr<Module>> mDependencies;

    como::MetaComponent* mComponent = nullptr;
};

Module::Module()
    : mGlobalNamespace(new Namespace(Namespace::GLOBAL_NAME, this))
{}

AutoPtr<UUID> Module::GetUUID()
{
    return mUuid;
}

String Module::GetUri()
{
    return mUri;
}

String Module::GetName()
{
    return mName;
}

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

AutoPtr<Namespace> Module::GetGlobalNamespace()
{
    return mGlobalNamespace;
}

int Module::GetNamespaceNumber()
{
    return mGlobalNamespace->GetNamespaceNumber();
}

void Module::AddConstant(
    /* [in] */ Constant* constant)
{
    mConstants.push_back(constant);
}

int Module::GetConstantNumber()
{
    return mConstants.size();
}

int Module::GetEnumerationNumber()
{
    return mEnumerations.size();
}

int Module::GetInterfaceNumber()
{
    return mInterfaces.size();
}

int Module::GetCoclassNumber()
{
    return mKlasses.size();
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

const std::unordered_map<String, AutoPtr<Type>, StringHashFunc, StringEqualsFunc>&
Module::GetTypes()
{
    return mAllTypeMap;
}

int Module::GetTypeNumber()
{
    return mAllTypeMap.size();
}

}

#endif // __CDLC_MODULE_H__
