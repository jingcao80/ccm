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

#ifndef __CCM_CSTUB_H__
#define __CCM_CSTUB_H__

#include "util/ccmobject.h"

namespace ccm {

extern const CoclassID CID_CStub;

COCLASS_ID(52068014-e347-453f-87a9-0becfb69d8ed)
class CStub
    : public Object
    , public IStub
{
public:
    CCM_INTERFACE_DECL();

    CCM_OBJECT_DECL();

    ECode Invoke(
        /* [in] */ IMetaMethod* method,
        /* [in] */ IArgumentList* args);

    static ECode CreateObject(
        /* [in] */ IInterface* object,
        /* [in] */ IStub** stub);
};

}

#endif // __CCM_CSTUB_H__
