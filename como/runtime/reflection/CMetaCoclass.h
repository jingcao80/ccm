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

#ifndef __COMO_CMETACOCLASS_H__
#define __COMO_CMETACOCLASS_H__

#include "metadata/Component.h"
#include "util/comoref.h"
#include "util/mutex.h"

namespace como {

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

    COMO_INTERFACE_DECL();

    ECode GetComponent(
        /* [out] */ AutoPtr<IMetaComponent>& comp) override;

    ECode GetName(
        /* [out] */ String& name) override;

    ECode GetNamespace(
        /* [out] */ String& ns) override;

    ECode GetCoclassID(
        /* [out] */ CoclassID& cid) override;

    ECode GetClassLoader(
        /* [out] */ AutoPtr<IClassLoader>& loader) override;

    ECode GetConstructorNumber(
        /* [out] */ Integer& number) override;

    ECode GetAllConstructors(
        /* [out] */ Array<IMetaConstructor*>& constrs) override;

    ECode GetConstructor(
        /* [in] */ const String& paramNumber,
        /* [out] */ AutoPtr<IMetaConstructor>& constr) override;

    ECode GetInterfaceNumber(
        /* [out] */ Integer& number) override;

    ECode GetAllInterfaces(
        /* [out] */ Array<IMetaInterface*>& intfs) override;

    ECode GetInterface(
        /* [in] */ const String& fullName,
        /* [out] */ AutoPtr<IMetaInterface>& intf) override;

    ECode ContainsInterface(
        /* [in] */ const String& fullName,
        /* [out] */ Boolean& result) override;

    ECode GetMethodNumber(
        /* [out] */ Integer& number) override;

    ECode GetAllMethods(
        /* [out] */ Array<IMetaMethod*>& methods) override;

    ECode GetMethod(
        /* [in] */ const String& name,
        /* [in] */ const String& signature,
        /* [out] */ AutoPtr<IMetaMethod>& method) override;

    ECode CreateObject(
        /* [in] */ const InterfaceID& iid,
        /* [out] */ IInterface** object) override;

private:
    void BuildAllConstructors();

    void BuildAllInterfaces();

    void BuildAllMethods();

    void BuildInterfaceMethodLocked(
        /* [in] */ IMetaInterface* miObj,
        /* [in, out] */ Integer& index);

public:
    MetaCoclass* mMetadata;
    CMetaComponent* mOwner;
    CoclassID mCid;
    String mName;
    String mNamespace;
    Array<IMetaConstructor*> mConstructors;
    Mutex mConstructorsLock;
    Array<IMetaMethod*> mMethods;
    Mutex mMethodsLock { true };
    Array<IMetaInterface*> mInterfaces;
    Mutex mInterfacesLock;
};

} // namespace como

#endif // __COMO_CMETACOCLASS_H__
