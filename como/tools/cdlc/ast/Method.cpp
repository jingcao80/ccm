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
#include "ast/Module.h"
#include "ast/Namespace.h"
#include "util/Properties.h"
#include "util/StringBuilder.h"

namespace cdlc {

AutoPtr<Parameter> Method::GetParameter(
    /* [in] */ int i)
{
    if (i >= 0 && i < mParameters.size()) {
        return mParameters[i];
    }
    return nullptr;
}

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
    StringBuilder builder;

    builder.Append(prefix).Append("Method[");
    builder.AppendFormat("name:%s, ", mName.string());
    builder.AppendFormat("signature:%s", mSignature.string());
    builder.Append("]\n");
    builder.Append(prefix + Properties::INDENT).Append("Return[");
    builder.AppendFormat("type:%s]\n", mReturnType->ToString().string());
    for (AutoPtr<Parameter> parameter : mParameters) {
        String parameterInfo = parameter->Dump(prefix + Properties::INDENT);
        builder.AppendFormat("%s\n", parameterInfo.string());
    }

    return builder.ToString();
}

AutoPtr<Node> Method::Clone(
    /* [in] */ Module* module,
    /* [in] */ bool deepCopy)
{
    AutoPtr<Method> clone = new Method();
    clone->mName = mName;
    clone->mSignature = mSignature;
    if (!deepCopy) {
        clone->mReturnType = mReturnType;
        clone->mParameters = mParameters;
    }
    else {
        AutoPtr<Type> returnType = module->FindType(mReturnType->ToString());
        if (returnType == nullptr) {
            returnType = mReturnType->Clone(module, false);
        }
        clone->mReturnType = returnType;
        for (int i = 0; i < mParameters.size(); i++) {
            AutoPtr<Parameter> parameter = mParameters[i]->Clone(module, true);
            clone->AddParameter(parameter);
        }
    }
    clone->mDeleted = mDeleted;
    return clone;
}

}
