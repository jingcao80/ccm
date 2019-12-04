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

#include "reflection/CArgumentList.h"
#include "reflection/CMetaCoclass.h"
#include "reflection/CMetaComponent.h"
#include "reflection/CMetaConstructor.h"
#include "reflection/CMetaInterface.h"
#include "reflection/CMetaParameter.h"
#include "component/comoobjapi.h"

namespace como {

EXTERN_C ECode invoke(
    /* [in] */ HANDLE func,
    /* [in] */ Byte* params,
    /* [in] */ Integer paramNum,
    /* [in] */ Integer stackParamNum,
    /* [in] */ struct ParameterInfo* paramInfos);

COMO_INTERFACE_IMPL_LIGHT_1(CMetaConstructor, LightRefBase, IMetaConstructor);

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
    , mIsDefault(mcObj->mMetadata->mProperties & COCLASS_CONSTRUCTOR_DEFAULT)
    , mParameters(mm->mParameterNumber)
{}

ECode CMetaConstructor::GetInterface(
    /* [out] */ AutoPtr<IMetaInterface>& intf)
{
    intf = nullptr;
    return NOERROR;
}

ECode CMetaConstructor::GetName(
    /* [out] */ String& name)
{
    name = mName;
    return NOERROR;
}

ECode CMetaConstructor::GetSignature(
    /* [out] */ String& signature)
{
    signature = mSignature;
    return NOERROR;
}

ECode CMetaConstructor::GetParameterNumber(
    /* [out] */ Integer& number)
{
    number = mParameters.GetLength();
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
    /* [out] */ AutoPtr<IMetaParameter>& param)
{
    if (index < 0 || index > mParameters.GetLength()) {
        param = nullptr;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    BuildAllParameters();

    param = mParameters[index];
    return NOERROR;
}

ECode CMetaConstructor::GetParameter(
    /* [in] */ const String& name,
    /* [out] */ AutoPtr<IMetaParameter>& param)
{
    if (name.IsEmpty()) {
        param = nullptr;
        return NOERROR;
    }

    BuildAllParameters();

    for (Integer i = 0; i < mParameters.GetLength(); i++) {
        String mpName;
        mParameters[i]->GetName(mpName);
        if (mpName.Equals(name)) {
            param = mParameters[i];
            return NOERROR;
        }
    }
    param = nullptr;
    return NOERROR;
}

ECode CMetaConstructor::HasOutArguments(
    /* [out] */ Boolean& outArgs)
{
    outArgs = false;
    return NOERROR;
}

ECode CMetaConstructor::CreateArgumentList(
    /* [out] */ AutoPtr<IArgumentList>& argList)
{
    BuildAllParameters();

    argList = new CArgumentList(mParameters);
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

    Byte* params = args->GetParameterBuffer();
    Integer paramNum = args->GetParameterNumber();
    ParameterInfo* paramInfos = args->GetParameterInfos();
    Integer intParamNum = 1, fpParamNum = 0;
    for (Integer i = 0; i < paramNum; i++) {
        if (paramInfos[i].mNumberType == NUMBER_TYPE_INTEGER) {
            intParamNum++;
        }
        else {
            fpParamNum++;
        }
    }
#if defined(__aarch64__)
    Integer stackParamNum = (intParamNum > 8 ? intParamNum - 8 : 0) +
                            (fpParamNum > 8 ? fpParamNum - 8 : 0);
#elif defined(__x86_64__)
    Integer stackParamNum = (intParamNum > 6 ? intParamNum - 6 : 0) +
                            (fpParamNum > 8 ? fpParamNum - 8 : 0);
#endif
    VObject* vobj = reinterpret_cast<VObject*>(thisObject->Probe(
            *reinterpret_cast<InterfaceID*>(&mClassObjectInterface->mUuid)));
    reinterpret_cast<HANDLE*>(params)[0] = reinterpret_cast<HANDLE>(vobj);
    HANDLE methodAddr = vobj->mVtab->mMethods[mIndex];
    return invoke(methodAddr, params, paramNum + 1, stackParamNum, paramInfos);
}

ECode CMetaConstructor::GetCoclass(
    /* [out] */ AutoPtr<IMetaCoclass>& klass)
{
    klass = (IMetaCoclass*)mOwner;
    return NOERROR;
}

ECode CMetaConstructor::IsDefault(
    /* [out] */ Boolean& isDefault)
{
    isDefault = mIsDefault;
    return NOERROR;
}

ECode CMetaConstructor::CreateObject(
    /* [in] */ IArgumentList* argList,
    /* [out] */ AutoPtr<IInterface>& object)
{
    AutoPtr<IClassObject> clsObj;
    ECode ec = CoAcquireClassFactory(mOwner->mCid,
            mOwner->mOwner->mLoader, clsObj);
    if (FAILED(ec)) {
        object = nullptr;
        return ec;
    }
    argList->SetInputArgumentOfInterfaceID(
            mParameters.GetLength(), IID_IInterface);
    argList->SetOutputArgumentOfInterface(
            mParameters.GetLength() + 1, reinterpret_cast<HANDLE>(&object));
    return Invoke(clsObj, argList);
}

void CMetaConstructor::BuildAllParameters()
{
    if (mParameters[0] == nullptr) {
        Mutex::AutoLock lock(mParametersLock);
        if (mParameters[0] == nullptr) {
            for (Integer i = 0; i < mMetadata->mParameterNumber; i++) {
                MetaParameter* mp = mMetadata->mParameters[i];
                AutoPtr<CMetaParameter> mpObj = new CMetaParameter(
                        mOwner->mOwner->mMetadata, this, mp, i);
                mParameters.Set(i, mpObj);
            }
        }
    }
}

} // namespace como
