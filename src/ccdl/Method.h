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

#ifndef __CCM_CCDL_METHOD_H__
#define __CCM_CCDL_METHOD_H__

#include "ASTElement.h"
#include "Parameter.h"
#include "../util/String.h"

namespace ccm {
namespace ccdl {

class Method : public ASTElement
{
public:
    Method()
        : mParamCapacity(0)
        , mParamIndex(0)
        , mParameters(nullptr)
    {}

    ~Method();

    inline Method& SetName(
        /* [in] */ const String& name)
    { mName = name; return *this; }

    Method& AddParameter(
        /* [in] */ Parameter* param);

    String Dump(
        /* [in] */ const String& prefix) override;

private:
    bool EnlargeParameterArray();

private:
    String mName;

    int mParamCapacity;
    int mParamIndex;
    Parameter** mParameters;
};

}
}

#endif // __CCM_CCDL_METHOD_H__
