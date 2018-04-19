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

#include "CMetaMethod.h"

namespace ccm {

CCM_INTERFACE_IMPL_LIGHT_1(CMetaMethod, IMetaMethod);

CMetaMethod::CMetaMethod()
{}

CMetaMethod::~CMetaMethod()
{}

ECode CMetaMethod::GetInterface(
    /* [out] */ IMetaInterface** intf)
{
    return NOERROR;
}

ECode CMetaMethod::GetName(
    /* [out] */ String* name)
{
    return NOERROR;
}

ECode CMetaMethod::GetSignature(
    /* [out] */ String* signature)
{
    return NOERROR;
}

ECode CMetaMethod::GetParameterNumber(
    /* [out] */ Integer* number)
{
    return NOERROR;
}

ECode CMetaMethod::GetAllParameters(
    /* [out] */ Array<IMetaParameter*>& params)
{
    return NOERROR;
}

ECode CMetaMethod::GetParameter(
    /* [in] */ Integer index,
    /* [out] */ IMetaParameter** param)
{
    return NOERROR;
}

ECode CMetaMethod::GetParameter(
    /* [in] */ const String& name,
    /* [out] */ IMetaParameter** param)
{
    return NOERROR;
}

ECode CMetaMethod::CreateArgumentList(
    /* [out] */ IArgumentList** argList)
{
    return NOERROR;
}

ECode CMetaMethod::Invoke(
    /* [in] */ IInterface* thisObject,
    /* [in] */ IArgumentList* argList)
{
    return NOERROR;
}

}
