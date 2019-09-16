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

#ifndef __CCM_CMETATYPE_H__
#define __CCM_CMETATYPE_H__

#include "ccmautoptr.h"
#include "ccmrefbase.h"
#include "Component.h"

using ccm::metadata::MetaComponent;
using ccm::metadata::MetaType;

namespace ccm {

class CMetaType
    : public LightRefBase
    , public IMetaType
{
public:
    CMetaType();

    CMetaType(
        /* [in] */ MetaComponent* mc,
        /* [in] */ MetaType* mt);

    ~CMetaType();

    CCM_INTERFACE_DECL();

    ECode GetName(
        /* [out] */ String* name);

    ECode GetTypeKind(
        /* [out] */ Integer* kind);

    ECode GetElementType(
        /* [out] */ IMetaType** elemType);

    ECode GetPointerNumber(
        /* [out] */ Integer* number);

private:
    static String BuildName(
        /* [in] */ MetaComponent* mc,
        /* [in] */ MetaType* mt);

public:
    MetaType* mMetadata;
    CcmTypeKind mKind;
    String mName;
    AutoPtr<IMetaType> mElementType;
    Integer mPointerNumber;
};

}

#endif // __CCM_CMETATYPE_H__
