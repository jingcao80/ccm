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

#ifndef __CDLC_COCLASSTYPE_H__
#define __CDLC_COCLASSTYPE_H__

#include "ast/Attributes.h"
#include "ast/InterfaceType.h"
#include "ast/Method.h"
#include "ast/Type.h"
#include "util/AutoPtr.h"
#include "util/String.h"
#include "util/UUID.h"

namespace cdlc {

class CoclassType
    : public Type
{
public:
    void SetAttributes(
        /* [in] */ const Attributes& attrs);

    inline void AddConstructor(
        /* [in] */ Method* constructor);

    AutoPtr<Method> FindConstructor(
        /* [in] */ const String& name,
        /* [in] */ const String& signature);

    inline void AddInterface(
        /* [in] */ InterfaceType* interface);

    String GetSignature() override;

private:
    AutoPtr<UUID> mUuid;
    String mVersion;
    String mDescription;
    std::vector<AutoPtr<Method>> mConstructors;
    std::vector<AutoPtr<InterfaceType>> mInterfaces;
};

void CoclassType::AddConstructor(
    /* [in] */ Method* constructor)
{
    if (constructor != nullptr) {
        mConstructors.push_back(constructor);
    }
}

void CoclassType::AddInterface(
    /* [in] */ InterfaceType* interface)
{
    if (interface != nullptr) {
        mInterfaces.push_back(interface);
    }
}

}

#endif // __CDLC_COCLASSTYPE_H__
