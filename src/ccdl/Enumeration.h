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

#ifndef __CCM_CCDL_ENUMERATION_H__
#define __CCM_CCDL_ENUMERATION_H__

#include "Type.h"
#include "Namespace.h"
#include "../util/String.h"

namespace ccm {
namespace ccdl {

class Enumeration : public Type
{
private:
    struct Enumerator
    {
        Enumerator(
            /* [in] */ const String& name,
            /* [in] */ int value)
            : mName(name)
            , mValue(value)
        {}

        String mName;
        int mValue;
    };

public:
    Enumeration()
        : mEnumCapacity(0)
        , mEnumIndex(0)
        , mEnumerators(nullptr)
    {}

    ~Enumeration();

    bool IsEnumeration() override { return true; }

    inline Enumeration& SetName(
        /* [in] */ const String& name) { mName = name; return *this; }
    inline Enumeration& SetNamespace(
        /* [in] */ Namespace* ns) { mNamespace = ns; return *this; }

    Enumeration& AddEnumerator(
        /* [in] */ const String& name,
        /* [in] */ int value);

    String ToString();
    inline String ToShortString() { return mName; }

    String Dump(
        /* [in] */ const String& prefix);

private:
    bool EnlargeEnumeratorArray();

private:
    Namespace* mNamespace;
    int mEnumCapacity;
    int mEnumIndex;
    Enumerator** mEnumerators;
};

}
}

#endif // __CCM_CCDL_ENUMERATION_H__
