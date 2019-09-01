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
    inline InterfaceType(
        /* [in] */ const String& name);

    void SetAttributes(
        /* [in] */ const Attributes& attrs);

    inline void AddMethod(
        /* [in] */ Method* method);

    bool IsInterfaceType() override;

    String Dump(
        /* [in] */ const String& prefix) override;

private:
    AutoPtr<UUID> mUuid;
    String mVersion;
    String mDescription;
    std::vector<AutoPtr<Method>> mMethods;
};

InterfaceType::InterfaceType(
    /* [in] */ const String& name)
{
    mName = name;
}

void InterfaceType::AddMethod(
    /* [in] */ Method* method)
{
    if (method != nullptr) {
        mMethods.push_back(method);
    }
}

}

#endif // __CDLC_INTERFACETYPE_H__
