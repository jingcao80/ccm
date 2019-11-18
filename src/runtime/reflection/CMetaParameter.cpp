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

#include "reflection/CMetaParameter.h"
#include "reflection/CMetaType.h"

namespace como {

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
    mIOAttr = BuildIOAttribute(mp->mProperties);
    mType = new CMetaType(mc, mc->mTypes[mp->mTypeIndex]);
}

ECode CMetaParameter::GetMethod(
    /* [out] */ AutoPtr<IMetaMethod>& method)
{
    method = mOwner;
    return NOERROR;
}

ECode CMetaParameter::GetName(
    /* [out] */ String& name)
{
    name = mName;
    return NOERROR;
}

ECode CMetaParameter::GetIndex(
    /* [out] */ Integer& index)
{
    index = mIndex;
    return NOERROR;
}

ECode CMetaParameter::GetIOAttribute(
    /* [out] */ IOAttribute& attr)
{
    attr = mIOAttr;
    return NOERROR;
}

ECode CMetaParameter::GetType(
    /* [out] */ AutoPtr<IMetaType>& type)
{
    type = mType;
    return NOERROR;
}

IOAttribute CMetaParameter::BuildIOAttribute(
    /* [in] */ unsigned char properties)
{
    if (properties & PARAMETER_IN) {
        if (properties & PARAMETER_OUT) {
            return IOAttribute::IN_OUT;
        }
        else {
            return IOAttribute::IN;
        }
    }
    else if (properties & PARAMETER_OUT) {
        if (properties & PARAMETER_CALLEE) {
            return IOAttribute::OUT_CALLEE;
        }
        else {
            return IOAttribute::OUT;
        }
    }
    return IOAttribute::UNKNOWN;
}

} // namespace como
