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
#include "ast/Module.h"
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
    if (mValue != nullptr) {
        if (mType->IsBooleanType()) {
            builder.AppendFormat("default:%s, ", mValue->BooleanValue() ? "true" : "false");
        }
        else if (mType->IsCharType()) {
            builder.AppendFormat("default:%c, ", mValue->CharacterValue());
        }
        else if (mType->IsIntegerType()) {
            builder.AppendFormat("default:%d, ", mValue->IntegerValue());
        }
        else if (mType->IsLongType() || mType->IsHANDLEType()) {
            builder.AppendFormat("default:%lld, ", mValue->LongValue());
        }
        else if (mType->IsFloatType()) {
            builder.AppendFormat("default:%f, ", mValue->FloatValue());
        }
        else if (mType->IsDoubleType()) {
            builder.AppendFormat("default:%f, ", mValue->DoubleValue());
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

AutoPtr<Node> Parameter::Clone(
    /* [in] */ Module* module,
    /* [in] */ bool deepCopy)
{
    AutoPtr<Parameter> clone = new Parameter();
    clone->mName = mName;
    if (!deepCopy) {
        clone->mType = mType;
    }
    else {
        AutoPtr<Type> type = module->FindType(mType->ToString());
        if (type == nullptr) {
            type = mType->Clone(module, false);
        }
        clone->mType = type;
    }
    clone->mValue = mValue;
    clone->mAttributes = mAttributes;
    return clone;
}

}

