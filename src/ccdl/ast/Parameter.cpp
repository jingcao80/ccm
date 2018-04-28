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

#include "Parameter.h"
#include "Pool.h"
#include "../util/StringBuilder.h"

namespace ccdl {
namespace ast {

Parameter::Parameter()
    : mType(nullptr)
    , mDefaultValue(nullptr)
    , mAttribute(0)
{}

Parameter::~Parameter()
{
    if (mDefaultValue != nullptr) {
        delete mDefaultValue;
        mDefaultValue = nullptr;
    }
}

void Parameter::SetDefaultValue(
    /* [in] */ Expression* expr)
{
    if (mDefaultValue != nullptr) {
        delete mDefaultValue;
    }
    mDefaultValue = expr;
}

void Parameter::ShallowCopy(
    /* [in] */ Parameter* source,
    /* [in] */ Pool* pool)
{
    mName = source->mName;
    mType = pool->FindType(source->mType->ToString());
    if (mType == nullptr) {
        mType = pool->ShallowCopyType(source->mType);
    }
    if (source->mDefaultValue != nullptr) {
        mDefaultValue = source->mDefaultValue->Clone();
    }
    mAttribute = source->mAttribute;
}

String Parameter::Dump(
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append(mName);
    builder.Append(":").Append(mType->ToString());
    return builder.ToString();
}

}
}
