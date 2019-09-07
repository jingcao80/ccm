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

#include "ast/CoclassType.h"
#include "ast/Namespace.h"
#include "util/StringBuilder.h"

namespace cdlc {

void CoclassType::SetAttributes(
    /* [in] */ const Attributes& attrs)
{
    mUuid = UUID::Parse(attrs.mUuid);
    mVersion = attrs.mVersion;
    mDescription = attrs.mDescription;
}

AutoPtr<Method> CoclassType::FindConstructor(
    /* [in] */ const String& name,
    /* [in] */ const String& signature)
{
    for (AutoPtr<Method> constructor : mConstructors) {
        if (constructor->GetName().Equals(name) &&
                constructor->GetSignature().Equals(signature)) {
            return constructor;
        }
    }
    return nullptr;
}

String CoclassType::GetSignature()
{
    StringBuilder builder;

    builder.Append("L");
    builder.Append(mNamespace->ToString().Replace("::", "/"));
    builder.Append("/");
    builder.Append(mName);
    builder.Append(";");
    return builder.ToString();
}

}
