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

#ifndef __CCM_CMETAMETHOD_H__
#define __CCM_CMETAMETHOD_H__

#include "ccmtypes.h"
#include "ccmrefbase.h"
#include "Component.h"

namespace ccm {

#include "ccmtypes.h"
#include "ccmrefbase.h"
#include "Component.h"

class CMetaMethod
    : public LightRefBase
    , public IMetaMethod
{
public:
    CMetaMethod();

    ~CMetaMethod();

    CCM_INTERFACE_DECL();

    ECode GetInterface(
        /* [out] */ IMetaInterface** intf);

    ECode GetName(
        /* [out] */ String* name);

    ECode GetSignature(
        /* [out] */ String* signature);

    ECode GetParameterNumber(
        /* [out] */ Integer* number);

    ECode GetAllParameters(
        /* [out] */ Array<IMetaParameter*>& params);

    ECode GetParameter(
        /* [in] */ Integer index,
        /* [out] */ IMetaParameter* param);

    ECode GetParameter(
        /* [in] */ const String& name,
        /* [out] */ IMetaParameter* param);

    ECode CreateArgumentList(
        /* [out] */ IArgumentList** argList);

    ECode Invoke(
        /* [in] */ IInterface* thisObject,
        /* [in] */ IArgumentList* argList);
};

}

#endif // __CCM_CMETAMETHOD_H__
