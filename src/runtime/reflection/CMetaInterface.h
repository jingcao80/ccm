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

#ifndef __CCM_CMETAINTERFACE_H__
#define __CCM_CMETAINTERFACE_H__

#include "ccmtypes.h"
#include "ccmrefbase.h"
#include "Component.h"

using ccm::metadata::MetaComponent;
using ccm::metadata::MetaInterface;

namespace ccm {

class CMetaComponent;

class CMetaInterface
    : public LightRefBase
    , public IMetaInterface
{
public:
    CMetaInterface(
        /* [in] */ CMetaComponent* mcObj,
        /* [in] */ MetaComponent* mc,
        /* [in] */ MetaInterface* mi);

    ~CMetaInterface();

    CCM_INTERFACE_DECL();

    ECode GetMetaComponent(
        /* [out] */ IMetaComponent** metaComp) override;

    ECode GetName(
        /* [out] */ String* name) override;

    ECode GetNamespace(
        /* [out] */ String* ns) override;

    ECode GetInterfaceID(
        /* [out] */ InterfaceID* iid) override;

public:
    MetaInterface* mMetadata;
    CMetaComponent* mMetaComponent;
    InterfaceID mIid;
    String mName;
    String mNamespace;
};

}

#endif // __CCM_CMETAINTERFACE_H__
