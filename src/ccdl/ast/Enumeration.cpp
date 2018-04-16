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

#include "Enumeration.h"
#include "Namespace.h"
#include "../util/StringBuilder.h"

namespace ccdl {
namespace ast {

Enumeration::Enumerator::Enumerator(
    /* [in] */ const String& name,
    /* [in] */ int value)
    : mName(name)
    , mValue(value)
{}

void Enumeration::SetNamespace(
    /* [in] */ Namespace* ns)
{
    Type::SetNamespace(ns);
    mNamespace->AddEnumeration(this);
}

bool Enumeration::IsEnumerationType()
{
    return true;
}

bool Enumeration::AddEnumerator(
    /* [in] */ const String& name,
    /* [in] */ int value)
{
    if (name.IsNullOrEmpty()) return true;

    Enumerator* e = new Enumerator(name, value);
    if (!mEnumerators.Add(e)) {
        delete e;
        return false;
    }
    return true;
}

bool Enumeration::Contains(
    /* [in] */ const String& name)
{
    for (int i = 0; i < mEnumerators.GetSize(); i++) {
        if (mEnumerators.Get(i)->mName.Equals(name)) {
            return true;
        }
    }
    return false;
}

String Enumeration::Signature()
{
    StringBuilder builder;

    builder.Append("L");
    builder.Append(mNamespace->ToString().Replace("::", "/"));
    builder.Append(mName);
    builder.Append(";");
    return builder.ToString();
}

String Enumeration::Dump(
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append("enum ").Append(mName).Append("\n");
    for (int i = 0; i < mEnumerators.GetSize(); i++) {
        Enumerator* e = mEnumerators.Get(i);
        builder.Append(prefix).Append("  "). Append(e->mName).Append("\n");
    }

    return builder.ToString();
}

}
}