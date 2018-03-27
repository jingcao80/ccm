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

#ifndef __CCM_CCDL_POOL_H__
#define __CCM_CCDL_POOL_H__

#include "Coclass.h"
#include "Enumeration.h"
#include "Interface.h"
#include "Namespace.h"
#include "../util/ArrayList.h"
#include "../util/HashMap.h"
#include "../util/String.h"

namespace ccm {
namespace ccdl {

class Pool
{
public:
    Pool();

    bool AddEnumeration(
        /* [in] */ Enumeration* enumeration);

    inline int GetEnumerationNumber()
    { return mEnumerations.GetSize(); }

    Enumeration* GetEnumeration(
        /* [in] */ int index)
    { return mEnumerations.Get(index); }

    Enumeration* FindEnumeration(
        /* [in] */ const String& enumFullName);

    inline int IndexOf(
        /* [in] */ Enumeration* enumn)
    { return mEnumerations.IndexOf(enumn); }

    bool AddInterface(
        /* [in] */ Interface* interface);

    inline int GetInterfaceNumber()
    { return mInterfaces.GetSize(); }

    inline Interface* GetInterface(
        /* [in] */ int index)
    { return mInterfaces.Get(index); }

    Interface* FindInterface(
        /* [in] */ const String& itfFullName);

    inline int IndexOf(
        /* [in] */ Interface* interface)
    { return mInterfaces.IndexOf(interface); }

    bool AddCoclass(
        /* [in] */ Coclass* klass);

    inline int GetCoclassNumber()
    { return mCoclasses.GetSize(); }

    Coclass* GetCoclass(
        /* [in] */ int index)
    { return mCoclasses.Get(index); }

    Coclass* FindClass(
        /* [in] */ const String& klassName);

    inline int IndexOf(
        /* [in] */ Coclass* klass)
    { return mCoclasses.IndexOf(klass); }

    bool AddNamespace(
        /* [in] */ Namespace* ns);

    inline int GetNamespaceNumber()
    { return mNamespaces.GetSize(); }

    Namespace* GetNamespace(
        /* [in] */ int index)
    { return mNamespaces.Get(index); }

    Namespace* FindNamespace(
        /* [in] */ const String& nsString);

    Namespace* ParseNamespace(
        /* [in] */ const String& nsString);

    bool AddTemporaryType(
        /* [in] */ Type* type);

    inline int GetTypeNumber()
    { return mTypes.GetSize(); }

    Type* FindType(
        /* [in] */ const String& typeName);

    inline std::shared_ptr< ArrayList<HashMap<Type*>::Pair*> >
    GetTypes()
    { return mTypes.GetKeyValues(); }

    int IndexOf(
        /* [in] */ Type* type);

    virtual String Dump(
        /* [in] */ const String& prefix);

protected:
    ArrayList<Coclass*> mCoclasses;
    ArrayList<Enumeration*> mEnumerations;
    ArrayList<Interface*> mInterfaces;
    ArrayList<Namespace*> mNamespaces;
    ArrayList<Type*> mTempTypes;
    HashMap<Type*> mTypes;
};

}
}

#endif // __CCM_CCDL_POOL_H__
