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

#include "ast/Method.h"
#include "ast/Namespace.h"
#include "util/StringBuilder.h"

namespace cdlc {

void Method::BuildSignature()
{
    StringBuilder builder;

    builder.Append("(");
    for (AutoPtr<Parameter> param : mParameters) {
        builder.Append(param->GetType()->GetSignature());
    }
    builder.Append(")");
    builder.Append(mReturnType != nullptr ? mReturnType->GetSignature() : "E");
    mSignature = builder.ToString();
}

String Method::ToString()
{
    return mName;
}

String Method::Dump(
    /* [in] */ const String& prefix)
{
    return prefix + ToString();
}

}
