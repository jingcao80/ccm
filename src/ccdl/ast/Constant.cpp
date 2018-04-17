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

#include "Constant.h"
#include "Pool.h"
#include "../util/StringBuilder.h"

namespace ccdl {
namespace ast {

Constant::Constant()
    : mType(nullptr)
    , mValue(nullptr)
{}

void Constant::DeepCopy(
    /* [in] */ Constant* source,
    /* [in] */ Pool* pool)
{
    mName = source->mName;
    mType = pool->FindType(source->mType->ToString());
    if (mType == nullptr) {
        mType = pool->DeepCopyType(source->mType);
    }
    mValue = source->mValue->Clone();
    mValue->SetType(mType);
}

String Constant::Dump(
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append(mName).Append("[").Append(mType->ToString()).Append("]\n");

    return builder.ToString();
}

}
}
