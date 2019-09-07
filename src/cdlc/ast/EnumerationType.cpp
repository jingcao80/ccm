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

#include "ast/EnumerationType.h"
#include "ast/Namespace.h"
#include "util/Properties.h"
#include "util/StringBuilder.h"

namespace cdlc {

void EnumerationType::AddEnumerator(
    /* [in] */ const String& name,
    /* [in] */ int value)
{
    if (!name.IsEmpty()) {
        mEnumerators.push_back(new Enumerator(name, value));
    }
}

bool EnumerationType::Contains(
    /* [in] */ const String& name)
{
    for (AutoPtr<Enumerator> enumerator : mEnumerators) {
        if (enumerator->mName.Equals(name)) {
            return true;
        }
    }
    return false;
}

bool EnumerationType::IsEnumerationType()
{
    return true;
}

String EnumerationType::GetSignature()
{
    StringBuilder builder;

    builder.Append("L");
    if (!mNamespace->IsGlobal()) {
        builder.Append(mNamespace->ToString().Replace("::", "/"));
    }
    builder.Append("/");
    builder.Append(mName);
    builder.Append(";");
    return builder.ToString();
}

String EnumerationType::ToString()
{
    return mNamespace->ToString() + "::" + mName;
}

String EnumerationType::Dump(
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append("Enumeration[");
    builder.AppendFormat("name:%s, ", mName.string());
    builder.AppendFormat("namespace:%s", mNamespace->ToString().string());
    for (AutoPtr<Enumerator> enumerator : mEnumerators) {
        builder.Append("\n");
        builder.Append(prefix + Properties::INDENT).Append("Enumerator[");
        builder.AppendFormat("name:%s, ", enumerator->mName.string());
        builder.AppendFormat("value:%d]", enumerator->mValue);
    }
    builder.Append("]\n");
    return builder.ToString();
}

}
