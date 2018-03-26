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

#ifndef __CCM_CCDL_INTERFACE_H__
#define __CCM_CCDL_INTERFACE_H__

#include "Type.h"
#include "Attribute.h"
#include "Constant.h"
#include "Method.h"
#include "Namespace.h"
#include "../util/ArrayList.h"
#include "../util/Uuid.h"

namespace ccm {
class Parser;
}

namespace ccm {
namespace ccdl {

class Interface : public Type
{
public:
    Interface()
        : mDeclared(false)
        , mBaseInterface(nullptr)
        , mConstants(10)
        , mMethods(20)
    {}

    ~Interface();

    bool IsInterface() override
    { return true; }

    Interface& SetBaseInterface(
        /* [in] */ Interface* baseItf);

    inline bool IsDeclared()
    { return mDeclared; }

    Interface& SetAttribute(
        /* [in] */ const Attribute& attr);

    Interface& AddConstant(
        /* [in] */ Constant* constant);

    inline int GetConstantNumber()
    { return mConstants.GetSize(); }

    inline Constant* GetConstant(
        /* [in] */ int index)
    { return mConstants.Get(index); }

    Interface& AddMethod(
        /* [in] */ Method* method);

    inline int GetMethodNumber()
    { return mMethods.GetSize(); }

    inline Method* GetMethod(
        /* [in] */ int index)
    { return mMethods.Get(index); }

    String Dump(
        /* [in] */ const String& prefix) override;

private:
    friend class ccm::Parser;

    inline Interface& SetDeclared(
        /* [in] */ bool declared)
    { mDeclared = declared; return *this; }

private:
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

#endif // __CCM_CCDL_INTERFACE_H__
