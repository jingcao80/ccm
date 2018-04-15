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

class Interface : public Type
{
public:
    Interface();

    void SetNamespace(
        /* [in] */ Namespace* ns) override;

    inline bool IsInterfaceType() override
    { return true; }

    inline Interface* GetBaseInterface()
    { return mBaseInterface; }

    void SetBaseInterface(
        /* [in] */ Interface* baseItf);

    inline bool IsSystemPreDeclared()
    { return mSystemPreDeclared; }

    inline void SetSystemPreDeclared(
        /* [in] */ bool preDeclared)
    { mSystemPreDeclared = preDeclared; }

    inline bool IsDeclared()
    { return mDeclared; }

    inline void SetDeclared(
        /* [in] */ bool declared)
    { mDeclared = declared; }

    inline Uuid& GetUuid()
    { return mUuid; }

    void SetAttribute(
        /* [in] */ const Attribute& attr);

    bool AddConstant(
        /* [in] */ Constant* constant);

    inline int GetConstantNumber()
    { return mConstants.GetSize(); }

    inline Constant* GetConstant(
        /* [in] */ int index)
    { return mConstants.Get(index); }

    bool AddMethod(
        /* [in] */ Method* method);

    inline int GetMethodNumber()
    { return mMethods.GetSize(); }

    inline Method* GetMethod(
        /* [in] */ int index)
    { return mMethods.Get(index); }

    Method* FindMethod(
        /* [in] */ const String& name,
        /* [in] */ const String& signature);

    String Signature() override;

    String Dump(
        /* [in] */ const String& prefix) override;

private:
    bool mSystemPreDeclared;
    bool mDeclared;
    Interface* mBaseInterface;
    Uuid mUuid;
    String mVersion;
    String mDescription;
    ArrayList<Constant*> mConstants;
    ArrayList<Method*> mMethods;
};

}
}

#endif // __CCDL_AST_INTERFACE_H__
