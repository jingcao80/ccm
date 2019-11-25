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

#ifndef __COMO_CMETACONSTRUCTOR_H__
#define __COMO_CMETACONSTRUCTOR_H__

#include "metadata/Component.h"
#include "util/comoref.h"
#include "util/mutex.h"

namespace como {

class CMetaCoclass;

class CMetaConstructor
    : public LightRefBase
    , public IMetaConstructor
{
public:
    CMetaConstructor(
        /* [in] */ CMetaCoclass* mcObj,
        /* [in] */ MetaInterface* mi,
        /* [in] */ Integer index,
        /* [in] */ MetaMethod* mm);

    COMO_INTERFACE_DECL();

    ECode GetInterface(
        /* [out] */ AutoPtr<IMetaInterface>& intf) override;

    ECode GetName(
        /* [out] */ String& name) override;

    ECode GetSignature(
        /* [out] */ String& signature) override;

    ECode GetParameterNumber(
        /* [out] */ Integer& number) override;

    ECode GetAllParameters(
        /* [out] */ Array<IMetaParameter*>& params) override;

    ECode GetParameter(
        /* [in] */ Integer index,
        /* [out] */ AutoPtr<IMetaParameter>& param) override;

    ECode GetParameter(
        /* [in] */ const String& name,
        /* [out] */ AutoPtr<IMetaParameter>& param) override;

    ECode HasOutArguments(
        /* [out] */ Boolean& outArgs) override;

    ECode CreateArgumentList(
        /* [out] */ AutoPtr<IArgumentList>& argList) override;

    ECode Invoke(
        /* [in] */ IInterface* thisObject,
        /* [in] */ IArgumentList* argList) override;

    ECode GetCoclass(
        /* [out] */ AutoPtr<IMetaCoclass>& klass) override;

    ECode IsDefault(
        /* [out] */ Boolean& isDefault) override;

    ECode CreateObject(
        /* [in] */ IArgumentList* argList,
        /* [out] */ AutoPtr<IInterface>& object) override;

private:
    void BuildAllParameters();

public:
    MetaMethod* mMetadata;
    CMetaCoclass* mOwner;
    MetaInterface* mClassObjectInterface;
    Integer mIndex;
    String mName;
    String mSignature;
    Boolean mIsDefault;
    Array<IMetaParameter*> mParameters;
    Mutex mParametersLock;
};

} // namespace como

#endif // __COMO_CMETACONSTRUCTOR_H__
