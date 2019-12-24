//=========================================================================
// Copyright (C) 2018 The C++ Component Model(COMO) Open Source Project
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

#ifndef __COMO_CMETACOMPONENT_H__
#define __COMO_CMETACOMPONENT_H__

#include "comotypes.h"
#include "component/comocomp.h"
#include "metadata/Component.h"
#include "util/comoref.h"
#include "util/hashmap.h"
#include "util/mutex.h"

namespace como {

class CMetaComponent
    : public LightRefBase
    , public IMetaComponent
{
public:
    CMetaComponent(
        /* [in] */ IClassLoader* loader,
        /* [in] */ ComoComponent* component,
        /* [in] */ MetaComponent* metadata);

    ~CMetaComponent();

    COMO_INTERFACE_DECL();

    ECode GetName(
        /* [ou] */ String& name) override;

    ECode GetComponentID(
        /* [out] */ ComponentID& cid) override;

    ECode GetConstantNumber(
        /* [out] */ Integer& number) override;

    ECode GetAllConstants(
        /* [out] */ Array<IMetaConstant*>& consts) override;

    ECode GetConstant(
        /* [in] */ const String& name,
        /* [out] */ AutoPtr<IMetaConstant>& constt) override;

    ECode GetCoclassNumber(
        /* [out] */ Integer& number) override;

    ECode GetAllCoclasses(
        /* [out] */ Array<IMetaCoclass*>& klasses) override;

    ECode GetCoclass(
        /* [in] */ const String& fullName,
        /* [out] */ AutoPtr<IMetaCoclass>& klass) override;

    ECode GetCoclass(
        /* [in] */ const CoclassID& cid,
        /* [out] */ AutoPtr<IMetaCoclass>& klass) override;

    ECode GetEnumerationNumber(
        /* [out] */ Integer& number) override;

    ECode GetAllEnumerations(
        /* [out] */ Array<IMetaEnumeration*>& enumns) override;

    ECode GetEnumeration(
        /* [in] */ const String& fullName,
        /* [out] */ AutoPtr<IMetaEnumeration>& enumn) override;

    ECode GetInterfaceNumber(
        /* [out] */ Integer& number) override;

    ECode GetAllInterfaces(
        /* [out] */ Array<IMetaInterface*>& intfs) override;

    ECode GetInterface(
        /* [in] */ const String& fullName,
        /* [out] */ AutoPtr<IMetaInterface>& metaIntf) override;

    ECode GetInterface(
        /* [in] */ const InterfaceID& iid,
        /* [out] */ AutoPtr<IMetaInterface>& metaIntf) override;

    ECode GetSerializedMetadata(
        /* [out, callee] */ Array<Byte>& metadata) override;

    ECode IsOnlyMetadata(
        /* [out] */ Boolean& onlyMetadata) override;

    ECode Preload() override;

    ECode CanUnload(
        /* [out] */ Boolean& unload) override;

    ECode Unload() override;

    ECode GetClassObject(
        /* [in] */ const CoclassID& cid,
        /* [out] */ AutoPtr<IClassObject>& object);

    void BuildAllConstants();

    void BuildAllCoclasses();

    void BuildAllEnumerations();

    void BuildAllInterfaces();

    AutoPtr<IMetaInterface> BuildInterface(
        /* [in] */ Integer index);

private:
    void LoadAllClassObjectGetters();

    void BuildIInterface();

    void ReleaseResources();

public:
    IClassLoader* mLoader;
    ComoComponent* mComponent;
    MetaComponent* mMetadata;
    ComponentID mCid;
    String mName;
    String mUri;
    Array<IMetaConstant*> mConstants;
    HashMap<String, IMetaConstant*> mConstantNameMap;
    Boolean mConstantsAlreadyBuilt;
    Mutex mConstantsLock { true };
    Array<IMetaCoclass*> mCoclasses;
    HashMap<String, IMetaCoclass*> mCoclassNameMap;
    HashMap<UUID, IMetaCoclass*> mCoclassIdMap;
    Boolean mCoclassesAlreadyBuilt;
    Mutex mCoclassesLock { true };
    Array<IMetaEnumeration*> mEnumerations;
    HashMap<String, IMetaEnumeration*> mEnumerationNameMap;
    Boolean mEnumerationsAlreadyBuilt;
    Mutex mEnumerationsLock { true };
    Array<IMetaInterface*> mInterfaces;
    HashMap<String, IMetaInterface*> mInterfaceNameMap;
    HashMap<UUID, IMetaInterface*> mInterfaceIdMap;
    Boolean mInterfacesAlreadyBuilt;
    Mutex mInterfacesLock { true };

    AutoPtr<IMetaInterface> mIInterface;
    HashMap<UUID, ClassObjectGetter*> mClassObjects;
};

} // namespace como

#endif // __COMO_CMETACOMPONENT_H__
