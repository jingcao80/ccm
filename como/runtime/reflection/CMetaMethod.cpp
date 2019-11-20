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
#include "reflection/CMetaComponent.h"
#include "reflection/CMetaInterface.h"
#include "reflection/CMetaMethod.h"
#include "reflection/CMetaParameter.h"
#include "reflection/CMetaType.h"

namespace como {

EXTERN_C ECode invoke(
    /* [in] */ HANDLE func,
    /* [in] */ Long* intData,
    /* [in] */ Integer intDataSize,
    /* [in] */ Double* fpData,
    /* [in] */ Integer fpDataSize,
    /* [in] */ Long* stkData,
    /* [in] */ Integer stkDataSize);

COMO_INTERFACE_IMPL_LIGHT_1(CMetaMethod, LightRefBase, IMetaMethod);

CMetaMethod::CMetaMethod()
    : mMetadata(nullptr)
    , mOwner(nullptr)
    , mIndex(0)
    , mHasOutArguments(false)
{}

CMetaMethod::CMetaMethod(
    /* [in] */ MetaComponent* mc,
    /* [in] */ CMetaInterface* miObj,
    /* [in] */ Integer index,
    /* [in] */ MetaMethod* mm)
    : mMetadata(mm)
    , mOwner(miObj)
    , mIndex(index)
    , mName(mm->mName)
    , mSignature(mm->mSignature)
    , mParameters(mMetadata->mParameterNumber)
    , mHasOutArguments(false)
{
    mReturnType = new CMetaType(mc,
            mc->mTypes[mm->mReturnTypeIndex]);
}

ECode CMetaMethod::GetInterface(
    /* [out] */ AutoPtr<IMetaInterface>& intf)
{
    intf = mOwner;
    return NOERROR;
}

ECode CMetaMethod::GetName(
    /* [out] */ String& name)
{
    name = mName;
    return NOERROR;
}

ECode CMetaMethod::GetSignature(
    /* [out] */ String& signature)
{
    signature = mSignature;
    return NOERROR;
}

ECode CMetaMethod::GetParameterNumber(
    /* [out] */ Integer& number)
{
    number = mParameters.GetLength();
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
    /* [out] */ AutoPtr<IMetaParameter>& param)
{
    if (index < 0 || index >= mMetadata->mParameterNumber) {
        param = nullptr;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    BuildAllParameters();

    param = mParameters[index];
    return NOERROR;
}

ECode CMetaMethod::GetParameter(
    /* [in] */ const String& name,
    /* [out] */ AutoPtr<IMetaParameter>& param)
{
    if (name.IsEmpty() || mParameters.IsEmpty()) {
        param = nullptr;
        return NOERROR;
    }

    BuildAllParameters();

    for (Integer i = 0; i < mParameters.GetLength(); i++) {
        IMetaParameter* mpObj = mParameters[i];
        String mpName;
        mpObj->GetName(mpName);
        if (name.Equals(mpName)) {
            param = mpObj;
            return NOERROR;
        }
    }
    param = nullptr;
    return NOERROR;
}

ECode CMetaMethod::HasOutArguments(
    /* [out] */ Boolean& outArgs)
{
    BuildAllParameters();

    outArgs = mHasOutArguments;
    return NOERROR;
}

ECode CMetaMethod::CreateArgumentList(
    /* [out] */ AutoPtr<IArgumentList>& argList)
{
    argList = new CArgumentList(
            mOwner->mOwner->mMetadata, mMetadata);
    return NOERROR;
}

ECode CMetaMethod::Invoke(
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
    Long* intData = args->GetIntegerData(intDataNum);
    Double* fpData = args->GetFPData(fpDataNum);
    Long* stkData =  args->GetStackData(stkDataNum);
    VObject* vobj = reinterpret_cast<VObject*>(thisObject->Probe(mOwner->mIid));
    intData[0] = reinterpret_cast<Long>(vobj);
    HANDLE methodAddr = vobj->mVtab->mMethods[mIndex];
    return invoke(methodAddr, intData, intDataNum, fpData, fpDataNum, stkData, stkDataNum);
}

void CMetaMethod::BuildAllParameters()
{
    if (mParameters[0] == nullptr) {
        Mutex::AutoLock lock(mParametersLock);
        if (mParameters[0] == nullptr) {
            for (Integer i = 0; i < mMetadata->mParameterNumber; i++) {
                AutoPtr<CMetaParameter> mpObj = new CMetaParameter(
                        mOwner->mOwner->mMetadata, this,
                        mMetadata->mParameters[i], i);
                mParameters.Set(i, mpObj);
                if (!mHasOutArguments && (mpObj->mIOAttr == IOAttribute::OUT ||
                        mpObj->mIOAttr == IOAttribute::IN_OUT)) {
                    mHasOutArguments = true;
                }
            }
        }
    }
}

} // namespace como
