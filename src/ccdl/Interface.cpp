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

#include "Interface.h"
#include "../util/StringBuilder.h"

namespace ccm {
namespace ccdl {

Interface::~Interface()
{
    mNamespace = nullptr;
}

Interface& Interface::SetBaseInterface(
    /* [in] */ Interface* baseItf)
{
    if (baseItf == nullptr) return *this;

    mBaseInterface = baseItf;
    return *this;
}

Interface& Interface::SetAttribute(
    /* [in] */ const Attribute& attr)
{
    mUuid.Parse(attr.mUuid);
    mVersion = attr.mVersion;
    mDescription = attr.mDescription;
    return *this;
}

Interface& Interface::AddMethod(
    /* [in] */ Method* method)
{
    if (method == nullptr) return *this;

    mMethods.Add(method);
    return *this;
}

Interface& Interface::AddConstant(
    /* [in] */ Constant* constant)
{
    if (constant == nullptr) return *this;

    mConstants.Add(constant);
    return *this;
}

String Interface::Dump(
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append("interface ").Append(mName).Append("[");
    builder.Append("uuid:").Append(mUuid.Dump());
    if (!mVersion.IsNullOrEmpty()) {
        builder.Append(", version:").Append(mVersion);
    }
    if (!mDescription.IsNullOrEmpty()) {
        builder.Append(", description:").Append(mDescription);
    }
    builder.Append("]\n");
    for (int i = 0; i < mConstants.GetSize(); i++) {
        String constantStr = mConstants.Get(i)->Dump(String("  "));
        builder.Append(prefix).Append(constantStr);
    }
    for (int i = 0; i < mMethods.GetSize(); i++) {
        String methodStr = mMethods.Get(i)->Dump(String("  "));
        builder.Append(prefix).Append(methodStr);
    }

    return builder.ToString();
}

}
}
