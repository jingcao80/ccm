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

#include "CMetaCoclass.h"

namespace ccm {

CCM_INTERFACE_IMPL_LIGHT_1(CMetaCoclass, IMetaCoclass);

ECode CMetaCoclass::GetName(
    /* [out] */ String* name)
{
    return NOERROR;
}

ECode CMetaCoclass::GetNamespace(
    /* [out] */ String* ns)
{
    return NOERROR;
}

ECode CMetaCoclass::GetCoclassID(
    /* [out] */ CoclassID* cid)
{
    return NOERROR;
}

ECode CMetaCoclass::GetInterfaceNumber(
    /* [out] */ Integer* number)
{
    return NOERROR;
}

ECode CMetaCoclass::GetAllInterfaces(
    /* [out] */ Array<IMetaInterface*>& intfs)
{
    return NOERROR;
}

ECode CMetaCoclass::CreateObject(
    /* [out] */ IInterface** object)
{
    return NOERROR;
}

}
