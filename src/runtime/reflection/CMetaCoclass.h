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

#ifndef __CCM_CMETACOCLASS_H__
#define __CCM_CMETACOCLASS_H__

#include "ccmtypes.h"
#include "ccmrefbase.h"
#include "Component.h"

using ccm::metadata::MetaCoclass;
using ccm::metadata::MetaComponent;

namespace ccm {

class CMetaComponent;

class CMetaCoclass
    : public LightRefBase
    , public IMetaCoclass
{
public:
    CMetaCoclass(
        /* [in] */ CMetaComponent* mcObj,
        /* [in] */ MetaComponent* mc,
        /* [in] */ MetaCoclass* mk);

    ~CMetaCoclass();

    CCM_INTERFACE_DECL();

    ECode GetComponent(
        /* [out] */ IMetaComponent** metaComp) override;

    ECode GetName(
        /* [out] */ String* name) override;

    ECode GetNamespace(
        /* [out] */ String* ns) override;

    ECode GetCoclassID(
        /* [out] */ CoclassID* cid) override;

    ECode GetConstructorNumber(
        /* [out] */ Integer* number) override;

    ECode GetAllConstructors(
        /* [out] */ Array<IMetaConstructor*>& constrs) override;

    ECode GetConstructor(
        /* [in] */ const String& paramNumber,
        /* [out] */ IMetaConstructor** constr) override;

    ECode GetInterfaceNumber(
        /* [out] */ Integer* number) override;

    ECode GetAllInterfaces(
        /* [out] */ Array<IMetaInterface*>& intfs) override;

    ECode GetInterface(
        /* [in] */ const String& fullName,
        /* [out] */ IMetaInterface** intf) override;

    ECode GetMethodNumber(
        /* [out] */ Integer* number) override;

    ECode GetMethods(
        /* [out] */ Array<IMetaMethod*>& methods) override;

    ECode GetMethod(
        /* [in] */ const String& name,
        /* [in] */ const String& signature,
        /* [out] */ IMetaMethod** method) override;

    ECode CreateObject(
        /* [in] */ const InterfaceID& iid,
        /* [out] */ IInterface** object) override;

private:
    void BuildAllInterfaces();

public:
    MetaCoclass* mMetadata;
    CMetaComponent* mMetaComponent;
    CoclassID mCid;
    String mName;
    String mNamespace;
    Array<IMetaInterface*> mMetaInterfaces;
};

}

#endif // __CCM_CMETACOCLASS_H__
