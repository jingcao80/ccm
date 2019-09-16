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

#ifndef __CCM_CMETACONSTANT_H__
#define __CCM_CMETACONSTANT_H__

#include "ccmautoptr.h"
#include "ccmrefbase.h"
#include "Component.h"

using ccm::metadata::MetaComponent;
using ccm::metadata::MetaConstant;
using ccm::metadata::MetaType;

namespace ccm {

class CMetaConstant
    : public LightRefBase
    , public IMetaConstant
{
public:
    CMetaConstant(
        /* [in] */ MetaComponent* mc,
        /* [in] */ MetaConstant* mk);

    ~CMetaConstant();

    CCM_INTERFACE_DECL();

    ECode GetName(
        /* [out] */ String* name);

    ECode GetType(
        /* [out] */ IMetaType** type);

    ECode GetValue(
        /* [out] */ IMetaValue** value);

private:
    AutoPtr<IMetaValue> BuildValue(
        /* [in] */ IMetaType* type);

public:
    MetaConstant* mMetadata;
    String mName;
    AutoPtr<IMetaType> mType;
    AutoPtr<IMetaValue> mValue;
};

}

#endif // __CCM_CMETACONSTANT_H__
