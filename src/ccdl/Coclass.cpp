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

#include "Coclass.h"
#include "Namespace.h"
#include "../util/StringBuilder.h"

namespace ccm {
namespace ccdl {

void Coclass::SetNamespace(
    /* [in] */ Namespace* ns)
{
    Type::SetNamespace(ns);
    mNamespace->AddCoclass(this);
}

void Coclass::SetAttribute(
    /* [in] */ const Attribute& attr)
{
    mUuid.Parse(attr.mUuid);
    mVersion = attr.mVersion;
    mDescription = attr.mDescription;
}

bool Coclass::AddConstructor(
    /* [in] */ Method* constructor)
{
    if (constructor == nullptr) return true;

    return mConstructors.Add(constructor);
}

bool Coclass::AddInterface(
    /* [in] */ Interface* interface)
{
    if (interface == nullptr) return true;

    return mInterfaces.Add(interface);
}

String Coclass::Dump(
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append("coclass ").Append(mName).Append("[");
    builder.Append("uuid:").Append(mUuid.Dump());
    if (!mVersion.IsNullOrEmpty()) {
        builder.Append(", version:").Append(mVersion);
    }
    if (!mDescription.IsNullOrEmpty()) {
        builder.Append(", description:").Append(mDescription);
    }
    builder.Append("]\n");
    for (int i = 0; i < mConstructors.GetSize(); i++) {
        String constructorStr = mConstructors.Get(i)->Dump(String("  "));
        builder.Append(prefix).Append(constructorStr);
    }
    for (int i = 0; i < mInterfaces.GetSize(); i++) {
        String itfStr = String::Format("interface %s\n", mInterfaces.Get(i)->GetName().string());
        builder.Append(prefix).Append("  ").Append(itfStr);
    }

    return builder.ToString();
}

}
}
