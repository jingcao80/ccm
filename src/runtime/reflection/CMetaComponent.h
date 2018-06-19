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

#include "ccmautoptr.h"
#include "ccmrefbase.h"
#include "hashmap.h"
#include "component/ccmcomponent.h"
#include "metadata/Component.h"

using ccm::metadata::MetaComponent;

namespace ccm {

class CMetaComponent
    : public LightRefBase
    , public IMetaComponent
{
public:
    CMetaComponent(
        /* [in] */ IClassLoader* loader,
        /* [in] */ CcmComponent* component,
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

    ECode GetCoclass(
        /* [in] */ const CoclassID& cid,
        /* [out] */ IMetaCoclass** metaKls) override;

    ECode GetEnumerationNumber(
        /* [out] */ Integer* number) override;

    ECode GetAllEnumerations(
        /* [out] */ Array<IMetaEnumeration*>& enumns) override;

    ECode GetEnumeration(
        /* [in] */ const String& fullName,
        /* [out] */ IMetaEnumeration** enumn) override;

    ECode GetInterfaceNumber(
        /* [out] */ Integer* number) override;

    ECode GetAllInterfaces(
        /* [out] */ Array<IMetaInterface*>& intfs) override;

    ECode GetInterface(
        /* [in] */ const String& fullName,
        /* [out] */ IMetaInterface** metaIntf) override;

    ECode GetInterface(
        /* [in] */ const InterfaceID& iid,
        /* [out] */ IMetaInterface** metaIntf) override;

    ECode CanUnload(
        /* [out] */ Boolean* unload);

    ECode Unload();

    ECode GetClassObject(
        /* [in] */ const CoclassID& cid,
        /* [out] */ IClassObject** object);

    void BuildAllCoclasses();

    AutoPtr<IMetaCoclass> BuildCoclass(
        /* [in] */ Integer index);

    void BuildAllEnumerations();

    AutoPtr<IMetaEnumeration> BuildEnumeration(
        /* [in] */ Integer index);

    void BuildAllInterfaces();

    AutoPtr<IMetaInterface> BuildInterface(
        /* [in] */ Integer index);

private:
    void LoadAllClassObjectGetters();

    void BuildIInterface();

    void ReleaseResources();

public:
    IClassLoader* mLoader;
    CcmComponent* mComponent;
    MetaComponent* mMetadata;
    ComponentID mCid;
    String mName;
    String mUrl;
    Array<IMetaCoclass*> mMetaCoclasses;
    HashMap<String, IMetaCoclass*> mMetaCoclassNameMap;
    HashMap<Uuid, IMetaCoclass*> mMetaCoclassIdMap;
    Boolean mMetaCoclassesAllBuilt;
    Array<IMetaEnumeration*> mMetaEnumerations;
    HashMap<String, IMetaEnumeration*> mMetaEnumerationMap;
    Boolean mMetaEnumerationsAllBuilt;
    Array<IMetaInterface*> mMetaInterfaces;
    HashMap<String, IMetaInterface*> mMetaInterfaceNameMap;
    HashMap<Uuid, IMetaInterface*> mMetaInterfaceIdMap;
    Boolean mMetaInterfacesAllBuilt;
    AutoPtr<IMetaInterface> mIInterface;
    HashMap<Uuid, ClassObjectGetter*> mClassObjects;
};

}

#endif // __CCM_CMETACOMPONENT_H__
