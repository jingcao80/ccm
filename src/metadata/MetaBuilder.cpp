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

#include "MetaBuilder.h"
#include "../ccdl/Interface.h"
#include "../util/Logger.h"

using ccm::Logger;
using ccm::ccdl::Interface;

#define ALIGN4(v) (((v) + 3) & ~3)
#define ALIGN8(v) (((v) + 7) & -7)

#if defined(__i386__)
#define ALIGN(v) ALIGN4(v)
#elif defined(__x86_64__)
#define ALIGN(v) ALIGN8(v)
#endif

namespace ccm {
namespace metadata {

const String MetaBuilder::TAG("MetaBuilder");

bool MetaBuilder::IsValidate()
{
    // check if all the interfaces in mModule are declared.
    for (int i = 0; i < mModule->GetInterfaceNumber(); i++) {
        Interface* itf = mModule->GetInterface(i);
        if (!itf->IsDeclared()) return false;
    }

    return true;
}

std::shared_ptr<MetaComponent> MetaBuilder::Build()
{
    if (!IsValidate()) {
        Logger::E(TAG, "The module which used to generate"
                " metadata is not validate.");
    }

    size_t size = CalculateMetadataSize();
    void* metadata = calloc(size, 1);
    if (metadata == nullptr) {
        Logger::E(TAG, "Out of memory.");
    }

    mMetaComponet.reset(
            new(metadata) MetaComponent,
            [](MetaComponent* p){ free(p); });

    return mMetaComponet;
}

size_t MetaBuilder::CalculateMetadataSize()
{
    mBasePtr = 0;
    CalculateMetaComponent(mModule.get());
    return mBasePtr;
}

void MetaBuilder::CalculateMetaComponent(
    /* [in] */ Module* module)
{
    int CLS_NUM = module->GetCoclassNumber();
    int ENUM_NUM = module->GetEnumerationNumber();
    int ITF_NUM = module->GetInterfaceNumber();
    int NS_NUM = module->GetNamespaceNumber();
    int TP_NUM = module->GetTypeSize();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // add mName to StringPool
    mStringPool.Add(module->GetName());
    // add mUrl to StringPool
    mStringPool.Add(module->GetUrl());
    // mNamespaces's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaComponent));
    // mCoclasses's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaNamespace*) * NS_NUM);
    // mEnumerations's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaCoclass*) * CLS_NUM);
    // mInterfaces's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaEnumeration*) * ENUM_NUM);
    // mTypes's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaInterface*) * ITF_NUM);
    // mStringPool's address
    mBasePtr = mBasePtr + sizeof(MetaType*) * TP_NUM;

    for (int i = 0; i < NS_NUM; i++) {
        CalculateMetaNamespace(module->GetNamespace(i));
    }

    for (int i = 0; i < CLS_NUM; i++) {
        CalculateMetaCoclass(module->GetCoclass(i));
    }

    for (int i = 0; i< ENUM_NUM; i++) {
        CalculateMetaEnumeration(module->GetEnumeration(i));
    }

    for (int i = 0; i < ITF_NUM; i++) {
        CalculateMetaInterface(module->GetInterface(i));
    }

    std::shared_ptr< ArrayList<HashMap<Type*>::Pair*> > typeKeys =
            module->GetTypeKeys();
    for (int i = 0; i < TP_NUM; i++) {
        CalculateMetaType(typeKeys->Get(i)->mValue);
    }

    // for (int i = 0; i <)

    // end address
    CalculateStringPool();
}

void MetaBuilder::CalculateMetaCoclass(
    /* [in] */ Coclass* klass)
{
    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // add mName to StringPool
    mStringPool.Add(klass->GetName());
    // mInterfaceIndexes's address
    mBasePtr = ALIGN4(mBasePtr + sizeof(MetaCoclass));
    // end address
    mBasePtr = mBasePtr + sizeof(int) * klass->GetInterfaceNumber();
}

void MetaBuilder::CalculateMetaEnumeration(
    /* [in] */ Enumeration* enumn)
{
    int ENUMR_NUM = enumn->GetEnumeratorNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // add mName to StringPool
    mStringPool.Add(enumn->GetName());
    // mEnumerators's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaEnumeration));
    // end address
    mBasePtr = mBasePtr + sizeof(MetaEnumerator*) * ENUMR_NUM;

    for (int i = 0; i < ENUMR_NUM; i++) {
        CalculateMetaEnumerator(enumn->GetEnumerator(i));
    }
}

void MetaBuilder::CalculateMetaEnumerator(
    /* [in] */ Enumeration::Enumerator* enumr)
{
    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // add mName to StringPool
    mStringPool.Add(enumr->mName);
    // end address
    mBasePtr = mBasePtr + sizeof(MetaEnumerator);
}

void MetaBuilder::CalculateMetaInterface(
    /* [in] */ Interface* itf)
{
    int CONST_NUM = itf->GetConstantNumber();
    int MTH_NUM = itf->GetMethodNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // add mName to StringPool
    mStringPool.Add(itf->GetName());
    // mConstants's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaInterface));
    // mMethods's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaConstant*) * CONST_NUM);
    // end address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaMethod*) * MTH_NUM);

    for (int i = 0; i < CONST_NUM; i++) {
        CalculateMetaConstant(itf->GetConstant(i));
    }

    for (int i = 0; i < MTH_NUM; i++) {
        CalculateMetaMethod(itf->GetMethod(i));
    }
}

void MetaBuilder::CalculateMetaConstant(
    /* [in] */ Constant* constant)
{
    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // add mName to StringPool
    mStringPool.Add(constant->GetName());
    // add mString to StringPool when constant is String
    if (constant->GetType()->GetName().Equals("String")) {
        mStringPool.Add(constant->GetValue()->StringValue());
    }
    // end address
    mBasePtr = mBasePtr + sizeof(MetaConstant);
}

void MetaBuilder::CalculateMetaMethod(
    /* [in] */ Method* method)
{

}

void MetaBuilder::CalculateMetaNamespace(
    /* [in] */ Namespace* ns)
{
    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // mName's address
    mBasePtr = mBasePtr + sizeof(MetaNamespace);
    // mCoclassIndexes's address
    mBasePtr = ALIGN4(mBasePtr + ns->GetName().GetLength() + 1);
    // mEnumerationIndexes's address
    mBasePtr = ALIGN4(mBasePtr + sizeof(int) * ns->GetCoclassNumber());
    // mInterfaceIndexes's address
    mBasePtr = ALIGN4(mBasePtr + sizeof(int) * ns->GetEnumerationNumber());
    // end address
    mBasePtr = mBasePtr + sizeof(int) * ns->GetInterfaceNumber();
}

void MetaBuilder::CalculateMetaType(
    /* [in] */ Type* type)
{

}

void MetaBuilder::CalculateStringPool()
{
    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // end address
    mBasePtr = mBasePtr + mStringPool.GetSize();
}

void MetaBuilder::WriteMetaComponent(
    /* [in] */ uintptr_t base,
    /* [in] */ bool onlyCalc)
{
    mBasePtr = base;

    uintptr_t beginAddr = mBasePtr;
    uintptr_t nameAddr = beginAddr + sizeof(MetaComponent);


    MetaComponent* component = (MetaComponent*)base;
    component->mName = (char*)((uintptr_t)component + sizeof(MetaComponent));
    strcpy(component->mName, mModule->GetName().string());
}


}
}
