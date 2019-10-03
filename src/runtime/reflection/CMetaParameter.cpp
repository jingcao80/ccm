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

#include "CMetaParameter.h"
#include "CMetaType.h"

namespace ccm {

COMO_INTERFACE_IMPL_LIGHT_1(CMetaParameter, LightRefBase, IMetaParameter)

CMetaParameter::CMetaParameter()
    : mMetadata(nullptr)
    , mOwner(nullptr)
    , mIndex(-1)
    , mIOAttr(IOAttribute::UNKNOWN)
{}

CMetaParameter::CMetaParameter(
    /* [in] */ MetaComponent* mc,
    /* [in] */ IMetaMethod* mmObj,
    /* [in] */ MetaParameter* mp,
    /* [in] */ Integer index)
    : mMetadata(mp)
    , mOwner(mmObj)
    , mName(mp->mName)
    , mIndex(index)
{
    mIOAttr = BuildIOAttribute(mp->mAttribute);
    mType = new CMetaType(mc, mc->mTypes[mp->mTypeIndex]);
}

CMetaParameter::~CMetaParameter()
{
    mMetadata = nullptr;
    mOwner = nullptr;
}

ECode CMetaParameter::GetMethod(
    /* [out] */ IMetaMethod** method)
{
    VALIDATE_NOT_NULL(method);

    *method = mOwner;
    REFCOUNT_ADD(*method);
    return NOERROR;
}

ECode CMetaParameter::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mName;
    return NOERROR;
}

ECode CMetaParameter::GetIndex(
    /* [out] */ Integer* index)
{
    VALIDATE_NOT_NULL(index);

    *index = mIndex;
    return NOERROR;
}

ECode CMetaParameter::GetIOAttribute(
    /* [out] */ IOAttribute* attr)
{
    VALIDATE_NOT_NULL(attr);

    *attr = mIOAttr;
    return NOERROR;
}

ECode CMetaParameter::GetType(
    /* [out] */ IMetaType** type)
{
    VALIDATE_NOT_NULL(type);

    *type = mType;
    REFCOUNT_ADD(*type);
    return NOERROR;
}

IOAttribute CMetaParameter::BuildIOAttribute(
    /* [in] */ Integer attr)
{
    if (attr == IN) {
        return IOAttribute::IN;
    }
    else if (attr == OUT) {
        return IOAttribute::OUT;
    }
    else if (attr == IN | OUT) {
        return IOAttribute::IN_OUT;
    }
    else if (attr == OUT | CALLEE) {
        return IOAttribute::OUT_CALLEE;
    }
    return IOAttribute::UNKNOWN;
}

}
