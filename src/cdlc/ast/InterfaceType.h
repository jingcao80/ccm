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

#ifndef __CDLC_INTERFACETYPE_H__
#define __CDLC_INTERFACETYPE_H__

#include "ast/Attributes.h"
#include "ast/Constant.h"
#include "ast/Method.h"
#include "ast/Type.h"
#include "util/AutoPtr.h"
#include "util/String.h"
#include "util/UUID.h"
#include <vector>

namespace cdlc {

class InterfaceType
    : public Type
{
public:
    void SetAttributes(
        /* [in] */ const Attributes& attrs);

    inline void SetBaseInterface(
        /* [in] */ InterfaceType* interface);

    inline void SetOuterInterface(
        /* [in] */ InterfaceType* interface);

    inline void AddNestedInterface(
        /* [in] */ InterfaceType* interface);

    inline void AddConstant(
        /* [in] */ Constant* constant);

    AutoPtr<Constant> FindConstant(
        /* [in] */ const String& name);

    inline void AddMethod(
        /* [in] */ Method* method);

    AutoPtr<Method> FindMethod(
        /* [in] */ const String& name,
        /* [in] */ const String& signature);

    inline int GetMethodNumber();

    bool IsInterfaceType() override;

    String GetSignature() override;

    String ToString() override;

    String Dump(
        /* [in] */ const String& prefix) override;

    inline static AutoPtr<InterfaceType> CastFrom(
        /* [in] */ Type* type);

public:
    static constexpr int METHOD_MAX_NUMBER = 240 + 4;

private:
    AutoPtr<UUID> mUuid;
    String mVersion;
    String mDescription;
    InterfaceType* mBaseInterface = nullptr;
    InterfaceType* mOuterInterface = nullptr;
    std::vector<AutoPtr<InterfaceType>> mNestedInterfaces;
    std::vector<AutoPtr<Constant>> mConstants;
    std::vector<AutoPtr<Method>> mMethods;
};

void InterfaceType::SetBaseInterface(
    /* [in] */ InterfaceType* interface)
{
    mBaseInterface = interface;
}

void InterfaceType::SetOuterInterface(
    /* [in] */ InterfaceType* interface)
{
    mOuterInterface = interface;
}

void InterfaceType::AddNestedInterface(
    /* [in] */ InterfaceType* interface)
{
    if (interface != nullptr) {
        mNestedInterfaces.push_back(interface);
    }
}

void InterfaceType::AddConstant(
    /* [in] */ Constant* constant)
{
    if (constant != nullptr) {
        mConstants.push_back(constant);
    }
}

void InterfaceType::AddMethod(
    /* [in] */ Method* method)
{
    if (method != nullptr) {
        mMethods.push_back(method);
    }
}

int InterfaceType::GetMethodNumber()
{
    return mMethods.size();
}

AutoPtr<InterfaceType> InterfaceType::CastFrom(
    /* [in] */ Type* type)
{
    return static_cast<InterfaceType*>(type);
}

}

#endif // __CDLC_INTERFACETYPE_H__
