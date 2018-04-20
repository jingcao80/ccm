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

#include "CMetaComponent.h"
#include "CMetaInterface.h"
#include "CMetaMethod.h"
#include "CMetaParameter.h"
#include "CMetaType.h"

namespace ccm {

CCM_INTERFACE_IMPL_LIGHT_1(CMetaMethod, IMetaMethod);

CMetaMethod::CMetaMethod()
    : mMetadata(nullptr)
    , mOwner(nullptr)
{}

CMetaMethod::CMetaMethod(
    /* [in] */ MetaComponent* mc,
    /* [in] */ CMetaInterface* miObj,
    /* [in] */ MetaMethod* mm)
    : mMetadata(mm)
    , mOwner(miObj)
    , mName(mm->mName)
    , mSignature(mm->mSignature)
    , mParameters(mMetadata->mParameterNumber)
{
    mReturnType = new CMetaType(mc,
            mc->mTypes[mm->mReturnTypeIndex]);
}

CMetaMethod::~CMetaMethod()
{
    mMetadata = nullptr;
    mOwner = nullptr;
}

ECode CMetaMethod::GetInterface(
    /* [out] */ IMetaInterface** intf)
{
    VALIDATE_NOT_NULL(intf);

    *intf = (IMetaInterface*)mOwner;
    REFCOUNT_ADD(*intf);
    return NOERROR;
}

ECode CMetaMethod::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mName;
    return NOERROR;
}

ECode CMetaMethod::GetSignature(
    /* [out] */ String* signature)
{
    VALIDATE_NOT_NULL(signature);

    *signature = mSignature;
    return NOERROR;
}

ECode CMetaMethod::GetParameterNumber(
    /* [out] */ Integer* number)
{
    VALIDATE_NOT_NULL(number);

    *number = mParameters.GetLength();
    return NOERROR;
}

ECode CMetaMethod::GetAllParameters(
    /* [out] */ Array<IMetaParameter*>& params)
{
    BuildAllParameters();

    Integer N = MIN(mParameters.GetLength(), params.GetLength());
    for (Integer i = 0; i < N; i++) {
        params.Set(i, mParameters[i]);
    }
    return NOERROR;
}

ECode CMetaMethod::GetParameter(
    /* [in] */ Integer index,
    /* [out] */ IMetaParameter** param)
{
    VALIDATE_NOT_NULL(param);

    if (index < 0 || index >= mMetadata->mParameterNumber) {
        *param = nullptr;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    BuildAllParameters();

    *param = mParameters[index];
    REFCOUNT_ADD(*param);
    return NOERROR;
}

ECode CMetaMethod::GetParameter(
    /* [in] */ const String& name,
    /* [out] */ IMetaParameter** param)
{
    VALIDATE_NOT_NULL(param);

    if (name.IsNullOrEmpty()) {
        *param = nullptr;
        return NOERROR;
    }

    for (Integer i = 0; i < mParameters.GetLength(); i++) {
        IMetaParameter* mpObj = mParameters[i];
        String mpName;
        mpObj->GetName(&mpName);
        if (name.Equals(mpName)) {
            *param = mpObj;
            REFCOUNT_ADD(*param);
            return NOERROR;
        }
    }
    *param = nullptr;
    return NOERROR;
}

ECode CMetaMethod::CreateArgumentList(
    /* [out] */ IArgumentList** argList)
{
    return NOERROR;
}

ECode CMetaMethod::Invoke(
    /* [in] */ IInterface* thisObject,
    /* [in] */ IArgumentList* argList)
{
    return NOERROR;
}

void CMetaMethod::BuildAllParameters()
{
    if (mParameters[0] == nullptr) {
        for (Integer i = 0; i < mMetadata->mParameterNumber; i++) {
            CMetaParameter* mpObj = new CMetaParameter(
                    mOwner->mOwner->mMetadata, this,
                    mMetadata->mParameters[i], i);
            mParameters.Set(i, mpObj);
        }
    }
}

}
