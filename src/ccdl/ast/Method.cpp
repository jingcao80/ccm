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

#include "Method.h"
#include "Pool.h"
#include "../util/StringBuilder.h"

namespace ccdl {
namespace ast {

Method::Method()
    : mReturnType(nullptr)
{}

bool Method::AddParameter(
    /* [in] */ Parameter* param)
{
    if (param == nullptr) return true;

    return mParameters.Add(param);
}

void Method::BuildSignature()
{
    StringBuilder builder;

    builder.Append("(");
    for (int i = 0; i < mParameters.GetSize(); i++) {
        builder.Append(mParameters.Get(i)->GetType()->Signature());
    }
    builder.Append(")");
    if (mReturnType == nullptr) {
        builder.Append("E");
    }
    else {
        builder.Append(mReturnType->Signature());
    }
    mSignature = builder.ToString();
}

void Method::DeepCopy(
    /* [in] */ Method* source,
    /* [in] */ Pool* pool)
{
    mName = source->mName;
    mSignature = source->mSignature;
    if (source->mReturnType != nullptr) {
        mReturnType = pool->FindType(source->mReturnType->ToString());
        if (mReturnType == nullptr) {
            mReturnType = pool->DeepCopyType(source->mReturnType);
        }
    }
    for (int i = 0; i < source->GetParameterNumber(); i++) {
        Parameter* desParam = new Parameter();
        desParam->DeepCopy(source->GetParameter(i), pool);
        AddParameter(desParam);
    }
}

String Method::ToString()
{
    return Dump(String(""));
}

String Method::Dump(
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append(mName);
    builder.Append("[SIG:").Append(mSignature);
    for (int i = 0; i < mParameters.GetSize(); i++) {
        Parameter* p = mParameters.Get(i);
        builder.Append(", ").Append(p->Dump(String("")));
    }
    builder.Append("]\n");

    return builder.ToString();
}

}
}
