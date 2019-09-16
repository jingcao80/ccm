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

#include "CArgumentList.h"
#include "CMetaCoclass.h"
#include "CMetaComponent.h"
#include "CMetaConstructor.h"
#include "CMetaInterface.h"
#include "CMetaParameter.h"
#include "ccmobjectapi.h"

using ccm::metadata::MetaParameter;

namespace ccm {

EXTERN_C ECode invoke(
    /* [in] */ HANDLE func,
    /* [in] */ Long* intData,
    /* [in] */ Integer intDataSize,
    /* [in] */ Double* fpData,
    /* [in] */ Integer fpDataSize,
    /* [in] */ Long* stkData,
    /* [in] */ Integer stkDataSize);

CCM_INTERFACE_IMPL_LIGHT_1(CMetaConstructor, LightRefBase, IMetaConstructor);

CMetaConstructor::CMetaConstructor(
    /* [in] */ CMetaCoclass* mcObj,
    /* [in] */ MetaInterface* mi,
    /* [in] */ Integer index,
    /* [in] */ MetaMethod* mm)
    : mMetadata(mm)
    , mOwner(mcObj)
    , mClassObjectInterface(mi)
    , mIndex(index)
    , mName("Constructor")
    , mSignature(mm->mSignature)
    , mIsDefault(mcObj->mMetadata->mConstructorDefault)
    , mParameters(mm->mParameterNumber)
{}

CMetaConstructor::~CMetaConstructor()
{
    mMetadata = nullptr;
    mOwner = nullptr;
}

ECode CMetaConstructor::GetInterface(
    /* [out] */ IMetaInterface** intf)
{
    VALIDATE_NOT_NULL(intf);

    *intf = nullptr;
    return NOERROR;
}

ECode CMetaConstructor::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mName;
    return NOERROR;
}

ECode CMetaConstructor::GetSignature(
    /* [out] */ String* signature)
{
    VALIDATE_NOT_NULL(signature);

    *signature = mSignature;
    return NOERROR;
}

ECode CMetaConstructor::GetParameterNumber(
    /* [out] */ Integer* number)
{
    VALIDATE_NOT_NULL(number);

    *number = mMetadata->mParameterNumber;
    return NOERROR;
}

ECode CMetaConstructor::GetAllParameters(
    /* [out] */ Array<IMetaParameter*>& params)
{
    BuildAllParameters();

    Integer N = MIN(mParameters.GetLength(), params.GetLength());
    for (Integer i = 0; i < N; i++) {
        params.Set(i, mParameters[i]);
    }
    return NOERROR;
}

ECode CMetaConstructor::GetParameter(
    /* [in] */ Integer index,
    /* [out] */ IMetaParameter** param)
{
    VALIDATE_NOT_NULL(param);

    if (index < 0 || index > mParameters.GetLength()) {
        *param = nullptr;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *param = mParameters[index];
    REFCOUNT_ADD(*param);
    return NOERROR;
}

ECode CMetaConstructor::GetParameter(
    /* [in] */ const String& name,
    /* [out] */ IMetaParameter** param)
{
    VALIDATE_NOT_NULL(param);

    if (name.IsNullOrEmpty()) {
        *param = nullptr;
        return NOERROR;
    }

    for (Integer i = 0; i < mParameters.GetLength(); i++) {
        String mpName;
        mParameters[i]->GetName(&mpName);
        if (mpName.Equals(name)) {
            *param = mParameters[i];
            REFCOUNT_ADD(*param);
            return NOERROR;
        }
    }
    *param = nullptr;
    return NOERROR;
}

ECode CMetaConstructor::HasOutArguments(
    /* [out] */ Boolean* outArgs)
{
    VALIDATE_NOT_NULL(outArgs);

    *outArgs = false;
    return NOERROR;
}

ECode CMetaConstructor::CreateArgumentList(
    /* [out] */ IArgumentList** argList)
{
    VALIDATE_NOT_NULL(argList);

    AutoPtr<IArgumentList> args = new CArgumentList(
            mOwner->mOwner->mMetadata, mMetadata);
    args.MoveTo(argList);
    return NOERROR;
}

ECode CMetaConstructor::Invoke(
    /* [in] */ IInterface* thisObject,
    /* [in] */ IArgumentList* argList)
{
    struct VTable
    {
        HANDLE mMethods[0];
    };

    struct VObject
    {
        VTable* mVtab;
    };

    CArgumentList* args = (CArgumentList*)argList;
    Integer intDataNum, fpDataNum, stkDataNum;
    Long* intData = args->GetIntegerData(&intDataNum);
    Double* fpData = args->GetFPData(&fpDataNum);
    Long* stkData =  args->GetStackData(&stkDataNum);
    VObject* vobj = reinterpret_cast<VObject*>(thisObject->Probe(
            *reinterpret_cast<InterfaceID*>(&mClassObjectInterface->mUuid)));
    intData[0] = reinterpret_cast<Long>(vobj);
    HANDLE methodAddr = vobj->mVtab->mMethods[mIndex];
    return invoke(methodAddr, intData, intDataNum, fpData, fpDataNum, stkData, stkDataNum);
}

ECode CMetaConstructor::GetCoclass(
    /* [out] */ IMetaCoclass** klass)
{
    VALIDATE_NOT_NULL(klass);

    *klass = (IMetaCoclass*)mOwner;
    REFCOUNT_ADD(*klass);
    return NOERROR;
}

ECode CMetaConstructor::IsDefault(
    /* [out] */ Boolean* isDefault)
{
    VALIDATE_NOT_NULL(isDefault);

    *isDefault = mIsDefault;
    return NOERROR;
}

ECode CMetaConstructor::CreateObject(
    /* [in] */ IArgumentList* argList,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    *object = nullptr;

    AutoPtr<IClassObject> clsObj;
    ECode ec = CoAcquireClassFactory(mOwner->mCid,
            mOwner->mOwner->mLoader, &clsObj);
    if (FAILED(ec)) return ec;
    argList->SetInputArgumentOfInterfaceID(
            mParameters.GetLength(), IID_IInterface);
    argList->SetOutputArgumentOfInterface(
            mParameters.GetLength() + 1, reinterpret_cast<HANDLE>(object));
    return Invoke(clsObj, argList);
}

void CMetaConstructor::BuildAllParameters()
{
    if (mParameters[0] == nullptr) {
        for (Integer i = 0; i < mMetadata->mParameterNumber; i++) {
            MetaParameter* mp = mMetadata->mParameters[i];
            CMetaParameter* mpObj = new CMetaParameter(
                    mOwner->mOwner->mMetadata, this, mp, i);
            mParameters.Set(i, mpObj);
        }
    }
}

}
