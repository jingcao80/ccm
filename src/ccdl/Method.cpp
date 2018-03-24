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
#include "../util/StringBuilder.h"

namespace ccm {
namespace ccdl {

Method& Method::AddParameter(
    /* [in] */ Parameter* param)
{
    if (param == nullptr) return *this;

    mParameters.Add(param);
    return *this;
}

String Method::Dump(
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append(mName);
    builder.Append("[");
    for (int i = 0; i < mParameters.GetSize(); i++) {
        if (i > 0) builder.Append(", ");
        Parameter* p = mParameters.Get(i);
        builder.Append(p->Dump(String("")));
    }
    builder.Append("]\n");

    return builder.ToString();
}

}
}
