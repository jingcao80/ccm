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

#ifndef __CCDL_AST_INTERFACE_H__
#define __CCDL_AST_INTERFACE_H__

#include "Type.h"
#include "Attribute.h"
#include "Constant.h"
#include "Method.h"
#include "../util/ArrayList.h"
#include "../util/Uuid.h"

namespace ccdl {
class Parser;
}

namespace ccdl {
namespace ast {

class Pool;

class Interface : public Type
{
public:
    Interface();

    void SetDeclared() override;

    void SetNamespace(
        /* [in] */ Namespace* ns) override;

    bool IsInterfaceType() override;

    inline Interface* GetBaseInterface();

    void SetBaseInterface(
        /* [in] */ Interface* baseItf);

    inline Uuid& GetUuid();

    void SetAttribute(
        /* [in] */ const Attribute& attr);

    void SetOuterInterface(
        /* [in] */ Interface* outer);

    inline Interface* GetOuterInterface();

    bool AddNestedInterface(
        /* [in] */ Interface* interface);

    inline int GetNestedInterfaceNumber();

    inline Interface* GetNestedInterface(
        /* [in] */ int index);

    bool AddConstant(
        /* [in] */ Constant* constant);

    inline int GetConstantNumber();

    inline Constant* GetConstant(
        /* [in] */ int index);

    Constant* FindConstant(
        /* [in] */ const String& name);

    bool AddMethod(
        /* [in] */ Method* method);

    inline int GetMethodNumber();

    inline Method* GetMethod(
        /* [in] */ int index);

    Method* FindMethod(
        /* [in] */ const String& name,
        /* [in] */ const String& signature);

    String Signature() override;

    void DeepCopy(
        /* [in] */ Interface* source,
        /* [in] */ Pool* pool);

    void ShallowCopy(
        /* [in] */ Interface* source,
        /* [in] */ Pool* pool);

    void Specialize();

    String Dump(
        /* [in] */ const String& prefix) override;

private:
    void SpecializeInternal(
        /* [in] */ Interface* source,
        /* [in] */ Pool* pool);

public:
    static constexpr int METHOD_MAX_NUMBER = 240 + 4;

private:
    Interface* mBaseInterface;
    Uuid mUuid;
    String mVersion;
    String mDescription;
    Interface* mOuterInterface;
    ArrayList<Interface*> mInterfaces;
    ArrayList<Constant*> mConstants;
    ArrayList<Method*> mMethods;
};

Interface* Interface::GetBaseInterface()
{
    return mBaseInterface;
}

Uuid& Interface::GetUuid()
{
    return mUuid;
}

Interface* Interface::GetOuterInterface()
{
    return mOuterInterface;
}

int Interface::GetNestedInterfaceNumber()
{
    return mInterfaces.GetSize();
}

Interface* Interface::GetNestedInterface(
    /* [in] */ int index)
{
    return mInterfaces.Get(index);
}

int Interface::GetConstantNumber()
{
    return mConstants.GetSize();
}

Constant* Interface::GetConstant(
    /* [in] */ int index)
{
    return mConstants.Get(index);
}

int Interface::GetMethodNumber()
{
    return mMethods.GetSize();
}

Method* Interface::GetMethod(
    /* [in] */ int index)
{
    return mMethods.Get(index);
}

}
}

#endif // __CCDL_AST_INTERFACE_H__
