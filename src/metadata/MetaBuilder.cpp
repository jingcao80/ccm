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
#define ALIGN8(v) (((v) + 7) & ~7)

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

    WriteMetadata(reinterpret_cast<uintptr_t>(metadata));

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
    int ENUMN_NUM = module->GetEnumerationNumber();
    int ITF_NUM = module->GetInterfaceNumber();
    int NS_NUM = module->GetNamespaceNumber();
    int TP_NUM = module->GetTypeNumber();

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
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaEnumeration*) * ENUMN_NUM);
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

    for (int i = 0; i< ENUMN_NUM; i++) {
        CalculateMetaEnumeration(module->GetEnumeration(i));
    }

    for (int i = 0; i < ITF_NUM; i++) {
        CalculateMetaInterface(module->GetInterface(i));
    }

    std::shared_ptr< ArrayList<HashMap<Type*>::Pair*> > types =
            module->GetTypes();
    for (int i = 0; i < TP_NUM; i++) {
        CalculateMetaType(types->Get(i)->mValue);
    }

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
    // add mNamespace to StringPool
    mStringPool.Add(klass->GetNamespace()->ToString());
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
    // add mNamespace to StringPool
    mStringPool.Add(enumn->GetNamespace()->ToString());
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
    // add mNamespace to StringPool
    mStringPool.Add(itf->GetNamespace()->ToString());
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
    else if (constant->GetType()->IsEnumeration()) {
        mStringPool.Add(constant->GetValue()->EnumeratorValue());
    }
    // end address
    mBasePtr = mBasePtr + sizeof(MetaConstant);
}

void MetaBuilder::CalculateMetaMethod(
    /* [in] */ Method* method)
{
    int PARAM_NUM = method->GetParameterNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // add mName to StringPool
    mStringPool.Add(method->GetName());
    // add mSignature to StringPool
    mStringPool.Add(method->GetSignature());
    // mParmeters's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaMethod));
    // end address
    mBasePtr = mBasePtr + sizeof(MetaParameter*) * PARAM_NUM;

    for (int i = 0; i < PARAM_NUM; i++) {
        CalculateMetaParameter(method->GetParameter(i));
    }
}

void MetaBuilder::CalculateMetaNamespace(
    /* [in] */ Namespace* ns)
{
    int CLS_NUM = ns->GetCoclassNumber();
    int ENUMN_NUM = ns->GetEnumerationNumber();
    int ITF_NUM = ns->GetInterfaceNumber();
    int NS_NUM = ns->GetNamespaceNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // add mName to StringPool
    mStringPool.Add(ns->GetName());
    // mCoclassIndexes's address
    mBasePtr = ALIGN4(mBasePtr + sizeof(MetaNamespace));
    // mEnumerationIndexes's address
    mBasePtr = ALIGN4(mBasePtr + sizeof(int) * CLS_NUM);
    // mInterfaceIndexes's address
    mBasePtr = ALIGN4(mBasePtr + sizeof(int) * ENUMN_NUM);
    // mNamespaces's address
    mBasePtr = ALIGN(mBasePtr + sizeof(int) * ITF_NUM);
    // end address
    mBasePtr = mBasePtr + sizeof(MetaNamespace*) * NS_NUM;

    for (int i = 0; i < NS_NUM; i++) {
        CalculateMetaNamespace(ns->GetNamespace(i));
    }
}

void MetaBuilder::CalculateMetaParameter(
    /* [in] */ Parameter* param)
{
    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // add mName to StringPool
    mStringPool.Add(param->GetName());
    // end address
    mBasePtr = mBasePtr + sizeof(MetaParameter);
}

void MetaBuilder::CalculateMetaType(
    /* [in] */ Type* type)
{
    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // end address
    mBasePtr = mBasePtr + sizeof(MetaType);
}

void MetaBuilder::CalculateStringPool()
{
    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // end address
    mBasePtr = mBasePtr + mStringPool.GetSize();
}

void MetaBuilder::WriteMetadata(
    /* [in] */ uintptr_t base)
{
    mBasePtr = base;
    WriteMetaComponent(mModule.get());
}

void MetaBuilder::WriteMetaComponent(
    /* [in] */ Module* module)
{
    int CLS_NUM = module->GetCoclassNumber();
    int ENUMN_NUM = module->GetEnumerationNumber();
    int ITF_NUM = module->GetInterfaceNumber();
    int NS_NUM = module->GetNamespaceNumber();
    int TP_NUM = module->GetTypeNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    MetaComponent* mc = reinterpret_cast<MetaComponent*>(mBasePtr);
    mc->mMagic = CCM_MAGIC;
    mc->mUuid = module->GetUuid();
    mc->mName = mStringPool.FindAddress(module->GetName());
    mc->mUrl = mStringPool.FindAddress(module->GetUrl());
    mc->mNamespaceNumber = NS_NUM;
    mc->mCoclassNumber = CLS_NUM;
    mc->mEnumerationNumber = ENUMN_NUM;
    mc->mInterfaceNumber = ITF_NUM;
    mc->mTypeNumber = TP_NUM;
    // mNamespaces's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaComponent));
    mc->mNamespaces = reinterpret_cast<MetaNamespace**>(mBasePtr);
    // mCoclasses's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaNamespace*) * NS_NUM);
    mc->mCoclasses = reinterpret_cast<MetaCoclass**>(mBasePtr);
    // mEnumerations's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaCoclass*) * CLS_NUM);
    mc->mEnumerations = reinterpret_cast<MetaEnumeration**>(mBasePtr);
    // mInterfaces's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaEnumeration*) * ENUMN_NUM);
    mc->mInterfaces = reinterpret_cast<MetaInterface**>(mBasePtr);
    // mTypes's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaInterface*) * ITF_NUM);
    mc->mTypes = reinterpret_cast<MetaType**>(mBasePtr);
    // end address
    mBasePtr = mBasePtr + sizeof(MetaType*) * TP_NUM;

    for (int i = 0; i < NS_NUM; i++) {
        mc->mNamespaces[i] = WriteMetaNamespace(module->GetNamespace(i));
    }

    for (int i = 0; i < CLS_NUM; i++) {
        mc->mCoclasses[i] = WriteMetaCoclass(module->GetCoclass(i));
    }

    for (int i = 0; i < ENUMN_NUM; i++) {
        mc->mEnumerations[i] = WriteMetaEnumeration(module->GetEnumeration(i));
    }

    for (int i = 0; i < ITF_NUM; i++) {
        mc->mInterfaces[i] = WriteMetaInterface(module->GetInterface(i));
    }
}

MetaCoclass* MetaBuilder::WriteMetaCoclass(
    /* [in] */ Coclass* klass)
{
    int ITF_NUM = klass->GetInterfaceNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    MetaCoclass* mc = reinterpret_cast<MetaCoclass*>(mBasePtr);
    mc->mName = mStringPool.FindAddress(klass->GetName());
    mc->mNamespace = mStringPool.FindAddress(klass->GetNamespace()->ToString());
    mc->mInterfaceNumber = ITF_NUM;
    // mInterfaceIndexes's address
    mBasePtr = ALIGN4(mBasePtr + sizeof(MetaCoclass));
    mc->mInterfaceIndexes = reinterpret_cast<int*>(mBasePtr);
    // end address
    mBasePtr = mBasePtr + sizeof(int) * ITF_NUM;

    for (int i = 0; i < ITF_NUM; i++) {
        Interface* itf = klass->GetInterface(i);
        mc->mInterfaceIndexes[i] = mModule->IndexOf(itf);
    }

    return mc;
}

MetaConstant* MetaBuilder::WriteMetaConstant(
    /* [in] */ Constant* constant)
{
    // begin address
    mBasePtr = ALIGN(mBasePtr);
    MetaConstant* mc = reinterpret_cast<MetaConstant*>(mBasePtr);
    mc->mName = mStringPool.FindAddress(constant->GetName());
    Type* type = constant->GetType();
    mc->mTypeIndex = mModule->IndexOf(type);
    if (type->IsIntegralType()) {
        if (type->GetName().Equals("Byte")) {
            mc->mValue.mInteger = constant->GetValue()->IntegerValue();
        }
        else if (type->GetName().Equals("Short")) {
            mc->mValue.mInteger = constant->GetValue()->IntegerValue();
        }
        else if (type->GetName().Equals("Integer")) {
            mc->mValue.mInteger = constant->GetValue()->IntegerValue();
        }
        else if (type->GetName().Equals("Long")) {
            mc->mValue.mLong = constant->GetValue()->LongValue();
        }
        else {
            mc->mValue.mInteger = constant->GetValue()->CharacterValue();
        }
    }
    else if (type->IsFloatingPointType()) {
        if (type->GetName().Equals("Float")) {
            mc->mValue.mFloat = constant->GetValue()->FloatValue();
        }
        else {
            mc->mValue.mDouble = constant->GetValue()->DoubleValue();
        }
    }
    else if (type->IsPrimitiveType()) {
        if (type->GetName().Equals("Boolean")) {
            mc->mValue.mBoolean = constant->GetValue()->BooleanValue();
        }
        else if (type->GetName().Equals("String")) {
            mc->mValue.mString = mStringPool.FindAddress(
                    constant->GetValue()->StringValue());
        }
    }
    else if (type->IsEnumeration()) {
        mc->mValue.mString = mStringPool.FindAddress(
                    constant->GetValue()->EnumeratorValue());
    }
    mc->mRadix = constant->GetValue()->GetRadix();
    // end address
    mBasePtr = mBasePtr + sizeof(MetaConstant);

    return mc;
}

MetaEnumeration* MetaBuilder::WriteMetaEnumeration(
    /* [in] */ Enumeration* enumn)
{
    int ENUMR_NUM = enumn->GetEnumeratorNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    MetaEnumeration* me = reinterpret_cast<MetaEnumeration*>(mBasePtr);
    me->mName = mStringPool.FindAddress(enumn->GetName());
    me->mNamespace = mStringPool.FindAddress(enumn->GetNamespace()->ToString());
    me->mEnumeratorNumber = ENUMR_NUM;
    // mEnumerators's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaEnumeration));
    me->mEnumerators = reinterpret_cast<MetaEnumerator**>(mBasePtr);
    // end address
    mBasePtr = mBasePtr + sizeof(MetaEnumerator*) * ENUMR_NUM;

    for (int i = 0; i < ENUMR_NUM; i++) {
        me->mEnumerators[i] = WriteMetaEnumerator(enumn->GetEnumerator(i));
    }

    return me;
}

MetaEnumerator* MetaBuilder::WriteMetaEnumerator(
    /* [in] */ Enumeration::Enumerator* enumr)
{
    // begin address
    mBasePtr = ALIGN(mBasePtr);
    MetaEnumerator* me = reinterpret_cast<MetaEnumerator*>(mBasePtr);
    me->mName = mStringPool.FindAddress(enumr->mName);
    me->mValue = enumr->mValue;
    // end address
    mBasePtr = mBasePtr + sizeof(MetaEnumerator);

    return me;
}

MetaInterface* MetaBuilder::WriteMetaInterface(
    /* [in] */ Interface* itf)
{
    int CONST_NUM = itf->GetConstantNumber();
    int MTH_NUM = itf->GetMethodNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    MetaInterface* mi = reinterpret_cast<MetaInterface*>(mBasePtr);
    mi->mName = mStringPool.FindAddress(itf->GetName());
    mi->mNamespace = mStringPool.FindAddress(itf->GetNamespace()->ToString());
    mi->mConstantNumber = CONST_NUM;
    mi->mMethodNumber = MTH_NUM;
    // mConstants's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaInterface));
    mi->mConstants = reinterpret_cast<MetaConstant**>(mBasePtr);
    // mMethods's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaConstant*) * CONST_NUM);
    mi->mMethods = reinterpret_cast<MetaMethod**>(mBasePtr);
    // end address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaMethod*) * MTH_NUM);

    for (int i = 0; i < CONST_NUM; i++) {
        mi->mConstants[i] = WriteMetaConstant(itf->GetConstant(i));
    }

    for (int i = 0; i < MTH_NUM; i++) {
        mi->mMethods[i] = WriteMetaMethod(itf->GetMethod(i));
    }

    return mi;
}

MetaMethod* MetaBuilder::WriteMetaMethod(
    /* [in] */ Method* method)
{
    return nullptr;
}

MetaNamespace* MetaBuilder::WriteMetaNamespace(
    /* [in] */ Namespace* ns)
{
    int CLS_NUM = ns->GetCoclassNumber();
    int ENUMN_NUM = ns->GetEnumerationNumber();
    int ITF_NUM = ns->GetInterfaceNumber();
    int NS_NUM = ns->GetNamespaceNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    MetaNamespace* mn = reinterpret_cast<MetaNamespace*>(mBasePtr);
    mn->mName = mStringPool.FindAddress(ns->GetName());
    // mCoclassIndexes's address
    mBasePtr = ALIGN4(mBasePtr + sizeof(MetaNamespace));
    mn->mCoclassIndexes = reinterpret_cast<int*>(mBasePtr);
    // mEnumerationIndexes's address
    mBasePtr = ALIGN4(mBasePtr + sizeof(int) * CLS_NUM);
    mn->mEnumerationIndexes = reinterpret_cast<int*>(mBasePtr);
    // mInterfaceIndexes's address
    mBasePtr = ALIGN4(mBasePtr + sizeof(int) * ENUMN_NUM);
    mn->mInterfaceIndexes = reinterpret_cast<int*>(mBasePtr);
    // mNamespaces's address
    mBasePtr = ALIGN(mBasePtr + sizeof(int) * ITF_NUM);
    mn->mNamespaces = reinterpret_cast<MetaNamespace**>(mBasePtr);
    // end address
    mBasePtr = mBasePtr + sizeof(MetaNamespace*) * NS_NUM;

    for (int i = 0; i < CLS_NUM; i++) {
        Coclass* klass = ns->GetCoclass(i);
        mn->mCoclassIndexes[i] = mModule->IndexOf(klass);
    }

    for (int i = 0; i < ENUMN_NUM; i++) {
        Enumeration* enumn = ns->GetEnumeration(i);
        mn->mEnumerationIndexes[i] = mModule->IndexOf(enumn);
    }

    for (int i = 0; i < ITF_NUM; i++) {
        Interface* itf = ns->GetInterface(i);
        mn->mInterfaceIndexes[i] = mModule->IndexOf(itf);
    }

    for (int i = 0; i < NS_NUM; i++) {
        mn->mNamespaces[i] = WriteMetaNamespace(ns->GetNamespace(i));
    }

    return mn;
}

}
}
