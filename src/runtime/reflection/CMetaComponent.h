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

#ifndef __CCM_CMETACOMPONENT_H__
#define __CCM_CMETACOMPONENT_H__

#include "ccmreflectionintfs.h"
#include "ccmrefbase.h"
#include "Component.h"
#include "hashmap.h"

using ccm::metadata::MetaComponent;

namespace ccm {

class CMetaComponent
    : public LightRefBase
    , public IMetaComponent
{
public:
    CMetaComponent(
        /* [in] */ MetaComponent* metadata);

    ~CMetaComponent();

    CCM_INTERFACE_DECL();

    ECode GetName(
        /* [ou] */ String* name) override;

    ECode GetComponentID(
        /* [out] */ ComponentID* cid) override;

    ECode GetCoclassNumber(
        /* [out] */ Integer* number) override;

    ECode GetAllCoclasses(
        /* [out] */ Array<IMetaCoclass*>& klasses) override;

    ECode GetCoclass(
        /* [in] */ const String& fullName,
        /* [out] */ IMetaCoclass** metaKls) override;

    ECode GetInterfaceNumber(
        /* [out] */ Integer* number) override;

    ECode GetAllInterfaces(
        /* [out] */ Array<IMetaInterface*>& intfs) override;

    ECode GetInterface(
        /* [in] */ const String& fullName,
        /* [out] */ IMetaInterface** metaIntf) override;

    void BuildAllCoclasses();

    void BuildAllInterfaces();

public:
    MetaComponent* mMetadata;
    ComponentID mCid;
    String mName;
    String mUrl;
    Array<IMetaCoclass*> mMetaCoclasses;
    HashMap<String, IMetaCoclass*> mMetaCoclassMap;
    Array<IMetaInterface*> mMetaInterfaces;
    HashMap<String, IMetaInterface*> mMetaInterfaceMap;
};

}

#endif // __CCM_CMETACOMPONENT_H__
