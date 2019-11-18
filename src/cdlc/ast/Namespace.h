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

#ifndef __CDLC_NAMESPACE_H__
#define __CDLC_NAMESPACE_H__

#include "ast/Node.h"
#include "ast/Type.h"
#include "metadata/Metadata.h"
#include "util/AutoPtr.h"
#include "util/String.h"
#include <vector>

namespace cdlc {

class CoclassType;
class Constant;
class EnumerationType;
class InterfaceType;
class Module;

class Namespace
    : public Node
{
    friend class Module;

public:
    Namespace(
        /* [in] */ const String& name,
        /* [in] */ Module* module);

    Namespace(
        /* [in] */ InterfaceType* interfaceWrapped,
        /* [in] */ Module* module);

    inline AutoPtr<Namespace> GetParent();

    inline void SetParent(
        /* [in] */ Namespace* parent);

    void AddNamespace(
        /* [in] */ Namespace* ns);

    AutoPtr<Namespace> GetNamespace(
        /* [in] */ int i);

    AutoPtr<Namespace> FindNamespace(
        /* [in] */ const String& nsString);

    inline int GetNamespaceNumber();

    void AddConstant(
        /* [in] */ Constant* constant);

    AutoPtr<Constant> GetConstant(
        /* [in] */ int i);

    AutoPtr<Constant> FindConstant(
        /* [in] */ const String& constantName);

    inline int GetConstantNumber();

    void AddEnumerationType(
        /* [in] */ EnumerationType* enumeration);

    AutoPtr<EnumerationType> GetEnumeration(
        /* [in] */ int i);

    inline int GetEnumerationNumber();

    int GetExternalEnumerationNumber();

    void AddInterfaceType(
        /* [in] */ InterfaceType* interface);

    AutoPtr<InterfaceType> GetInterface(
        /* [in] */ int i);

    inline int GetInterfaceNumber();

    int GetExternalInterfaceNumber();

    void AddCoclassType(
        /* [in] */ CoclassType* klass);

    AutoPtr<CoclassType> GetCoclass(
        /* [in] */ int i);

    inline int GetCoclassNumber();

    void AddType(
        /* [in] */ Type* type);

    inline bool IsGlobal();

    inline bool IsInterfaceWrapper();

    AutoPtr<InterfaceType> GetInterfaceWrapped();

    inline String ToShortString();

    inline bool NeedResolve();

    inline void SetMetadata(
        /* [in] */ como::MetaNamespace* metadata);

    String ToString() override;

    String Dump(
        /* [in] */ const String& prefix) override;

private:
    void AddTypeInternal(
        /* [in] */ Type* type);

public:
    static const String GLOBAL_NAME;

private:
    // mName does not include "::"
    String mName;
    Namespace* mParent = nullptr;
    std::vector<AutoPtr<Namespace>> mChildren;
    std::vector<AutoPtr<Constant>> mConstants;
    std::vector<AutoPtr<EnumerationType>> mEnumerations;
    std::vector<AutoPtr<InterfaceType>> mInterfaces;
    std::vector<AutoPtr<CoclassType>> mKlasses;
    std::vector<AutoPtr<Type>> mTypes;
    bool mIsWrapper = false;
    InterfaceType* mInterfaceWrapped = nullptr;

    Module* mModule = nullptr;
    bool mResolved = true;
    como::MetaNamespace* mMetadata = nullptr;
};

AutoPtr<Namespace> Namespace::GetParent()
{
    return mParent;
}

void Namespace::SetParent(
    /* [in] */ Namespace* parent)
{
    mParent = parent;
}

int Namespace::GetNamespaceNumber()
{
    return mChildren.size();
}

int Namespace::GetConstantNumber()
{
    return mConstants.size();
}

int Namespace::GetEnumerationNumber()
{
    return mEnumerations.size();
}

int Namespace::GetInterfaceNumber()
{
    return mInterfaces.size();
}

int Namespace::GetCoclassNumber()
{
    return mKlasses.size();
}

bool Namespace::IsGlobal()
{
    return mName.Equals(GLOBAL_NAME);
}

bool Namespace::IsInterfaceWrapper()
{
    return mIsWrapper;
}

String Namespace::ToShortString()
{
    return mName;
}

bool Namespace::NeedResolve()
{
    return mMetadata != nullptr;
}

void Namespace::SetMetadata(
    /* [in] */ como::MetaNamespace* metadata)
{
    mMetadata = metadata;
}

}

#endif // __CDLC_NAMESPACE_H__
