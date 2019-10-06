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

#include "comoref.h"
#include "Component.h"

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

    ~CMetaConstructor();

    COMO_INTERFACE_DECL();

    ECode GetInterface(
        /* [out] */ AutoPtr<IMetaInterface>& intf);

    ECode GetName(
        /* [out] */ String& name);

    ECode GetSignature(
        /* [out] */ String& signature);

    ECode GetParameterNumber(
        /* [out] */ Integer& number);

    ECode GetAllParameters(
        /* [out] */ Array<IMetaParameter*>& params);

    ECode GetParameter(
        /* [in] */ Integer index,
        /* [out] */ AutoPtr<IMetaParameter>& param);

    ECode GetParameter(
        /* [in] */ const String& name,
        /* [out] */ AutoPtr<IMetaParameter>& param);

    ECode HasOutArguments(
        /* [out] */ Boolean& outArgs);

    ECode CreateArgumentList(
        /* [out] */ AutoPtr<IArgumentList>& argList);

    ECode Invoke(
        /* [in] */ IInterface* thisObject,
        /* [in] */ IArgumentList* argList);

    ECode GetCoclass(
        /* [out] */ AutoPtr<IMetaCoclass>& klass);

    ECode IsDefault(
        /* [out] */ Boolean& isDefault);

    ECode CreateObject(
        /* [in] */ IArgumentList* argList,
        /* [out] */ AutoPtr<IInterface>& object);

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
};

} // namespace como

#endif // __COMO_CMETACONSTRUCTOR_H__
