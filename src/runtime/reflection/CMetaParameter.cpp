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

#include "CMetaParameter.h"

namespace ccm {

CCM_INTERFACE_IMPL_LIGHT_1(CMetaParameter, IMetaParameter)

CMetaParameter::CMetaParameter()
{}

CMetaParameter::CMetaParameter(
    /* [in] */ IMetaMethod* mmObj,
    /* [in] */ MetaParameter* mp)
{}

CMetaParameter::~CMetaParameter()
{}

ECode CMetaParameter::GetMethod(
    /* [out] */ IMetaMethod** method)
{
    return NOERROR;
}

ECode CMetaParameter::GetName(
    /* [out] */ String* name)
{
    return NOERROR;
}

ECode CMetaParameter::GetIndex(
    /* [out] */ Integer* index)
{
    return NOERROR;
}

ECode CMetaParameter::GetIOAttribute(
    /* [out] */ IOAttribute* attr)
{
    return NOERROR;
}

ECode CMetaParameter::GetType(
    /* [out] */ IMetaType** type)
{
    return NOERROR;
}

}
