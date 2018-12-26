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

#ifndef __CCDL_AST_COCLASS_H__
#define __CCDL_AST_COCLASS_H__

#include "Attribute.h"
#include "Interface.h"
#include "Method.h"
#include "Type.h"
#include "../util/ArrayList.h"

namespace ccdl {
namespace ast {

class Coclass : public Type
{
public:
    Coclass();

    void SetNamespace(
        /* [in] */ Namespace* ns) override;

    inline Uuid& GetUuid();

    void SetAttribute(
        /* [in] */ const Attribute& attr);

    bool IsCoclassType() override;

    bool AddConstructor(
        /* [in] */ Method* constructor);

    bool RemoveConstructor(
        /* [in] */ Method* constructor);

    inline int GetConstructorNumber();

    inline Method* GetConstructor(
        /* [in] */ int index);

    Method* FindConstructor(
        /* [in] */ const String& name,
        /* [in] */ const String& signature);

    inline bool HasDefaultConstructor();

    inline void SetConstructorDefault(
        /* [in] */ bool isDefault);

    inline bool IsConstructorDeleted();

    inline void SetConstructorDeleted(
        /* [in] */ bool deleted);

    bool AddInterface(
        /* [in] */ Interface* interface);

    inline int GetInterfaceNumber();

    inline Interface* GetInterface(
        /* [in] */ int index);

    String Signature() override;

    String Dump(
        /* [in] */ const String& prefix) override;

private:
    Uuid mUuid;
    String mVersion;
    String mDescription;
    bool mConstructorDefault;
    bool mConstructorDeleted;
    ArrayList<Method*> mConstructors;
    ArrayList<Interface*> mInterfaces;
};

Uuid& Coclass::GetUuid()
{
    return mUuid;
}

int Coclass::GetConstructorNumber()
{
    return mConstructors.GetSize();
}

Method* Coclass::GetConstructor(
    /* [in] */ int index)
{
    return mConstructors.Get(index);
}

bool Coclass::HasDefaultConstructor()
{
    return mConstructorDefault;
}

void Coclass::SetConstructorDefault(
    /* [in] */ bool isDefault)
{
    mConstructorDefault = isDefault;
}

bool Coclass::IsConstructorDeleted()
{
    return mConstructorDeleted;
}

void Coclass::SetConstructorDeleted(
    /* [in] */ bool deleted)
{
    mConstructorDeleted = deleted;
}

int Coclass::GetInterfaceNumber()
{
    return mInterfaces.GetSize();
}

Interface* Coclass::GetInterface(
    /* [in] */ int index)
{
    return mInterfaces.Get(index);
}

}
}

#endif // __CCDL_AST_COCLASS_H__
