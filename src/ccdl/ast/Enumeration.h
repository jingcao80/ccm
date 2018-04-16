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

#ifndef __CCDL_AST_ENUMERATION_H__
#define __CCDL_AST_ENUMERATION_H__

#include "Type.h"
#include "../util/ArrayList.h"
#include "../util/String.h"

namespace ccdl {
namespace ast {

class Enumeration : public Type
{
public:
    struct Enumerator
    {
        Enumerator(
            /* [in] */ const String& name,
            /* [in] */ int value);

        String mName;
        int mValue;
    };

public:
    void SetNamespace(
        /* [in] */ Namespace* ns) override;

    bool IsEnumerationType() override;

    bool AddEnumerator(
        /* [in] */ const String& name,
        /* [in] */ int value);

    inline int GetEnumeratorNumber();

    inline Enumerator* GetEnumerator(
        /* [in] */ int index);

    bool Contains(
        /* [in] */ const String& name);

    String Signature() override;

    String Dump(
        /* [in] */ const String& prefix) override;

private:
    ArrayList<Enumerator*> mEnumerators;
};

int Enumeration::GetEnumeratorNumber()
{
    return mEnumerators.GetSize();
}

Enumeration::Enumerator* Enumeration::GetEnumerator(
    /* [in] */ int index)
{
    return mEnumerators.Get(index);
}

}
}

#endif // __CCDL_AST_ENUMERATION_H__
