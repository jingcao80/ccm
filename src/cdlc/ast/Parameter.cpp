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

#include "ast/Parameter.h"
#include "ast/Namespace.h"
#include "util/StringBuilder.h"

namespace cdlc {

String Parameter::ToString()
{
    return mName;
}

String Parameter::Dump(
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append("Parameter[");
    builder.AppendFormat("name:%s, ", mName.string());
    builder.AppendFormat("type:%s, ", mType->ToString().string());
    if (mDefaultValue != nullptr) {
        if (mType->IsBooleanType()) {
            builder.AppendFormat("default:%s, ", mDefaultValue->BooleanValue() ? "true" : "false");
        }
        else if (mType->IsIntegerType()) {
            builder.AppendFormat("default:%d, ", mDefaultValue->IntegerValue());
        }
        else if (mType->IsLongType()) {
            builder.AppendFormat("default:%lld, ", mDefaultValue->LongValue());
        }

    }
    builder.Append("attribute:");
    String separator = "";
    if (mAttributes & IN) {
        builder.Append("in");
        separator = "|";
    }
    if (mAttributes & OUT) {
        builder.Append(separator).Append("OUT");
        separator = "|";
    }
    if (mAttributes & CALLEE) {
        builder.Append(separator).Append("CALLEE");
    }
    builder.Append("]");

    return builder.ToString();
}

}

