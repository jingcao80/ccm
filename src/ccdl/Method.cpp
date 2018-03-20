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

#include <stdlib.h>

namespace ccm {
namespace ccdl {

Method::~Method()
{
    for (int i = 0; i < mParamIndex; i++) {
        Parameter* p = mParameters[i];
        delete p;
    }
    if (mParameters != nullptr) {
        free(mParameters);
        mParameters = nullptr;
    }
}

Method& Method::AddParameter(
    /* [in] */ Parameter* param)
{
    if (param == nullptr) return *this;

    if (mParamIndex >= mParamCapacity) {
        if (!EnlargeParameterArray()) return *this;
    }

    mParameters[mParamIndex++] = param;
    return *this;
}

bool Method::EnlargeParameterArray()
{
    int newSize = mParamCapacity == 0 ? 5 : mParamCapacity + 5;
    Parameter** newArray = (Parameter**)calloc(sizeof(Parameter*), newSize);
    if (newArray == nullptr) return false;

    if (mParameters != nullptr) {
        memcpy(newArray, mParameters, mParamCapacity);
        free(mParameters);
    }
    mParamCapacity = newSize;
    mParameters = newArray;
    return true;
}

String Method::Dump(
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append(mName).Append("\n");

    return builder.ToString();
}

}
}
