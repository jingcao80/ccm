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

#ifndef __COMO_CMETAINTERFACE_H__
#define __COMO_CMETAINTERFACE_H__

#include "comoref.h"
#include "Component.h"

namespace como {

class CMetaComponent;

class CMetaInterface
    : public LightRefBase
    , public IMetaInterface
{
public:
    CMetaInterface();

    CMetaInterface(
        /* [in] */ CMetaComponent* mcObj,
        /* [in] */ MetaComponent* mc,
        /* [in] */ MetaInterface* mi);

    ~CMetaInterface();

    COMO_INTERFACE_DECL();

    ECode GetComponent(
        /* [out] */ AutoPtr<IMetaComponent>& metaComp) override;

    ECode GetName(
        /* [out] */ String& name) override;

    ECode GetNamespace(
        /* [out] */ String& ns) override;

    ECode GetInterfaceID(
        /* [out] */ InterfaceID& iid) override;

    ECode GetBaseInterface(
        /* [out] */ AutoPtr<IMetaInterface>& baseIntf) override;

    ECode GetConstantNumber(
        /* [out] */ Integer& number) override;

    ECode GetAllConstants(
        /* [out] */ Array<IMetaConstant*>& consts) override;

    ECode GetConstant(
        /* [in] */ const String& name,
        /* [out] */ AutoPtr<IMetaConstant>& constt) override;

    ECode GetConstant(
        /* [in] */ Integer index,
        /* [out] */ AutoPtr<IMetaConstant>& constt) override;

    ECode GetMethodNumber(
        /* [out] */ Integer& number) override;

    ECode GetAllMethods(
        /* [out] */ Array<IMetaMethod*>& methods) override;

    ECode GetMethod(
        /* [in] */ const String& name,
        /* [in] */ const String& signature,
        /* [out] */ AutoPtr<IMetaMethod>& method) override;

    ECode GetMethod(
        /* [in] */ Integer index,
        /* [out] */ AutoPtr<IMetaMethod>& method) override;

private:
    Integer CalculateMethodNumber();

    void BuildBaseInterface();

    void BuildAllConstants();

    void BuildAllMethods();

    Integer BuildInterfaceMethod(
        /* [in] */ MetaInterface* mi);

public:
    MetaInterface* mMetadata;
    CMetaComponent* mOwner;
    InterfaceID mIid;
    String mName;
    String mNamespace;
    CMetaInterface* mBaseInterface;
    Array<IMetaConstant*> mMetaConstants;
    Array<IMetaMethod*> mMetaMethods;
};

} // namespace como

#endif // __COMO_CMETAINTERFACE_H__
