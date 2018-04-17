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

#ifndef __CCDL_AST_POOL_H__
#define __CCDL_AST_POOL_H__

#include "Coclass.h"
#include "Enumeration.h"
#include "Interface.h"
#include "Namespace.h"
#include "../util/ArrayList.h"
#include "../util/StringMap.h"
#include "../util/String.h"

namespace ccdl {
namespace ast {

class Pool
{
public:
    Pool();

    bool AddEnumeration(
        /* [in] */ Enumeration* enumeration);

    inline int GetEnumerationNumber();

    inline Enumeration* GetEnumeration(
        /* [in] */ int index);

    Enumeration* FindEnumeration(
        /* [in] */ const String& enumFullName);

    inline int IndexOf(
        /* [in] */ Enumeration* enumn);

    bool AddInterface(
        /* [in] */ Interface* interface);

    inline int GetInterfaceNumber();

    inline Interface* GetInterface(
        /* [in] */ int index);

    Interface* FindInterface(
        /* [in] */ const String& itfFullName);

    inline int IndexOf(
        /* [in] */ Interface* interface);

    bool AddCoclass(
        /* [in] */ Coclass* klass);

    inline int GetCoclassNumber();

    inline Coclass* GetCoclass(
        /* [in] */ int index);

    Coclass* FindClass(
        /* [in] */ const String& klassName);

    inline int IndexOf(
        /* [in] */ Coclass* klass);

    bool AddNamespace(
        /* [in] */ Namespace* ns);

    inline int GetNamespaceNumber();

    inline Namespace* GetNamespace(
        /* [in] */ int index);

    Namespace* FindNamespace(
        /* [in] */ const String& nsString);

    Namespace* ParseNamespace(
        /* [in] */ const String& nsString);

    bool AddTemporaryType(
        /* [in] */ Type* type);

    inline int GetTypeNumber();

    Type* GetType(
        /* [in] */ int index);

    Type* FindType(
        /* [in] */ const String& typeName);

    inline std::shared_ptr< ArrayList<StringMap<Type*>::Pair*> >
    GetTypes();

    int IndexOf(
        /* [in] */ Type* type);

    Type* DeepCopyType(
        /* [in] */ Type* type);

    Type* ShallowCopyType(
        /* [in] */ Type* type);

    virtual Type* ResolveType(
        /* [in] */ const String& fullName) = 0;

    virtual String Dump(
        /* [in] */ const String& prefix);

protected:
    ArrayList<Coclass*> mCoclasses;
    ArrayList<Enumeration*> mEnumerations;
    ArrayList<Interface*> mInterfaces;
    ArrayList<Namespace*> mNamespaces;
    ArrayList<Type*> mTempTypes;
    StringMap<Type*> mTypes;
};

int Pool::GetEnumerationNumber()
{
    return mEnumerations.GetSize();
}

Enumeration* Pool::GetEnumeration(
    /* [in] */ int index)
{
    return mEnumerations.Get(index);
}

int Pool::IndexOf(
    /* [in] */ Enumeration* enumn)
{
    return mEnumerations.IndexOf(enumn);
}

int Pool::GetInterfaceNumber()
{
    return mInterfaces.GetSize();
}

Interface* Pool::GetInterface(
    /* [in] */ int index)
{
    return mInterfaces.Get(index);
}

int Pool::IndexOf(
    /* [in] */ Interface* interface)
{
    return mInterfaces.IndexOf(interface);
}

int Pool::GetCoclassNumber()
{
    return mCoclasses.GetSize();
}

Coclass* Pool::GetCoclass(
    /* [in] */ int index)
{
    return mCoclasses.Get(index);
}

int Pool::IndexOf(
    /* [in] */ Coclass* klass)
{
    return mCoclasses.IndexOf(klass);
}

int Pool::GetNamespaceNumber()
{
    return mNamespaces.GetSize();
}

Namespace* Pool::GetNamespace(
    /* [in] */ int index)
{
    return mNamespaces.Get(index);
}

int Pool::GetTypeNumber()
{
    return mTypes.GetSize();
}

std::shared_ptr< ArrayList<StringMap<Type*>::Pair*> >
Pool::GetTypes()
{
    return mTypes.GetKeyValues();
}

}
}

#endif // __CCDL_AST_POOL_H__
