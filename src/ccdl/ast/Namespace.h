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

#ifndef __CCDL_AST_NAMESPACE_H__
#define __CCDL_AST_NAMESPACE_H__

#include "Node.h"
#include "Coclass.h"
#include "Enumeration.h"
#include "Interface.h"
#include "../util/ArrayList.h"
#include "../util/String.h"

namespace ccdl {
namespace ast {

class Namespace : public Node
{
public:
    Namespace(
        /* [in] */ const String& nsStr);

    ~Namespace();

    inline String GetName();

    inline Namespace* GetOuterNamespace();

    inline void SetOuterNamespace(
        /* [in] */ Namespace* outerNS);

    bool AddNamespace(
        /* [in] */ Namespace* innerNS);

    inline int GetNamespaceNumber();

    inline Namespace* GetNamespace(
        /* [in] */ int index);

    Namespace* FindNamespace(
        /* [in] */ const String& nsString);

    bool AddCoclass(
        /* [in] */ Coclass* klass);

    inline int GetCoclassNumber();

    inline Coclass* GetCoclass(
        /* [in] */ int index);

    bool AddEnumeration(
        /* [in] */ Enumeration* enumn);

    inline int GetEnumerationNumber();

    inline Enumeration* GetEnumeration(
        /* [in] */ int index);

    bool AddInterface(
        /* [in] */ Interface* itf);

    inline int GetInterfaceNumber();

    int GetSystemPreDeclaredInterfaceNumber();

    inline Interface* GetInterface(
        /* [in] */ int index);

    String ToString() override;

    inline String ToShortString();

private:
    String mName;
    Namespace* mOuterNamespace;
    ArrayList<Namespace*> mNamespaces;
    ArrayList<Coclass*> mCoclasses;
    ArrayList<Enumeration*> mEnumerations;
    ArrayList<Interface*> mInterfaces;
};

String Namespace::GetName()
{
    return mName;
}

Namespace* Namespace::GetOuterNamespace()
{
    return mOuterNamespace;
}

void Namespace::SetOuterNamespace(
    /* [in] */ Namespace* outerNS)
{
    mOuterNamespace = outerNS;
}

int Namespace::GetNamespaceNumber()
{
    return mNamespaces.GetSize();
}

Namespace* Namespace::GetNamespace(
    /* [in] */ int index)
{
    return mNamespaces.Get(index);
}

int Namespace::GetCoclassNumber()
{
    return mCoclasses.GetSize();
}

Coclass* Namespace::GetCoclass(
    /* [in] */ int index)
{
    return mCoclasses.Get(index);
}

int Namespace::GetEnumerationNumber()
{
    return mEnumerations.GetSize();
}

Enumeration* Namespace::GetEnumeration(
    /* [in] */ int index)
{
    return mEnumerations.Get(index);
}

int Namespace::GetInterfaceNumber()
{
    return mInterfaces.GetSize();
}

Interface* Namespace::GetInterface(
    /* [in] */ int index)
{
    return mInterfaces.Get(index);
}

String Namespace::ToShortString()
{
    return mName;
}

}
}

#endif // __CCDL_AST_NAMESPACE_H__
