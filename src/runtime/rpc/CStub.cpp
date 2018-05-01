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

//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
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

#include "CStub.h"

namespace ccm {

//----------------------------------------------------------------------

const CoclassID CID_CStub =
        {{0x52068014,0xe347,0x453f,0x87a9,{0x0,0xb,0xe,0xc,0xf,0xb,0x6,0x9,0xd,0x8,0xe,0xd}}, &CID_CCMRuntime};

CCM_INTERFACE_IMPL_1(CStub, Object, IStub);

CCM_OBJECT_IMPL(CStub);

ECode CStub::Invoke(
    /* [in] */ IMetaMethod* method,
    /* [in] */ IArgumentList* args)
{
    return NOERROR;
}

ECode CStub::CreateObject(
    /* [in] */ IInterface* object,
    /* [in] */ IStub** stub)
{
    VALIDATE_NOT_NULL(stub);
}

}
