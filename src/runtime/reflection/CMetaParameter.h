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

#ifndef __CCM_CMETAPARAMETER_H__
#define __CCM_CMETAPARAMETER_H__

#include "ccmautoptr.h"
#include "ccmrefbase.h"
#include "Component.h"

using ccm::metadata::MetaComponent;
using ccm::metadata::MetaParameter;
using ccm::metadata::MetaType;

namespace ccm {

class CMetaParameter
    : public LightRefBase
    , public IMetaParameter
{
public:
    CMetaParameter();

    CMetaParameter(
        /* [in] */ MetaComponent* mc,
        /* [in] */ IMetaMethod* mmObj,
        /* [in] */ MetaParameter* mp,
        /* [in] */ Integer index);

    ~CMetaParameter();

    CCM_INTERFACE_DECL();

    ECode GetMethod(
        /* [out] */ IMetaMethod** method);

    ECode GetName(
        /* [out] */ String* name);

    ECode GetIndex(
        /* [out] */ Integer* index);

    ECode GetIOAttribute(
        /* [out] */ IOAttribute* attr);

    ECode GetType(
        /* [out] */ IMetaType** type);

private:
    IOAttribute BuildIOAttribute(
        /* [in] */ Integer attr);

private:
    static constexpr int IN = 0x1;
    static constexpr int OUT = 0x2;
    static constexpr int CALLEE = 0x4;

public:
    MetaParameter* mMetadata;
    IMetaMethod* mOwner;
    String mName;
    Integer mIndex;
    IOAttribute mIOAttr;
    AutoPtr<IMetaType> mType;
};

}

#endif // __CCM_CMETAPARAMETER_H__
