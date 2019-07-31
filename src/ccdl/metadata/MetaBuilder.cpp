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
#include "../ast/ArrayType.h"
#include "../ast/Interface.h"
#include "../ast/PointerType.h"
#include "../ast/ReferenceType.h"
#include "../util/Logger.h"
#include "../util/StringBuilder.h"

using ccdl::Logger;
using ccdl::ast::ArrayType;
using ccdl::ast::Interface;
using ccdl::ast::PointerType;
using ccdl::ast::ReferenceType;

#define ALIGN4(v) (((v) + 3) & ~3)
#define ALIGN8(v) (((v) + 7) & ~7)

#if defined(__i386__)
#define ALIGN(v) ALIGN4(v)
#elif defined(__x86_64__)
#define ALIGN(v) ALIGN8(v)
#endif

namespace ccdl {
namespace metadata {

const String MetaBuilder::TAG("MetaBuilder");

MetaBuilder::MetaBuilder(
    /* [in] */ std::shared_ptr<Module> module)
    : mModule(module)
    , mBasePtr(0)
{}

bool MetaBuilder::IsValid()
{
    // check if all the interfaces in mModule are declared.
    for (int i = 0; i < mModule->GetInterfaceNumber(); i++) {
        Interface* itf = mModule->GetInterface(i);
        if (itf->IsPredecl()) {
            Logger::E(TAG, "Interface \"%s\" is not declared",
                    itf->GetName().string());
            return false;
        }
    }

    // check if all the enumerations in mModule are declared.
    for (int i = 0; i < mModule->GetEnumerationNumber(); i++) {
        Enumeration* enumn = mModule->GetEnumeration(i);
        if (enumn->IsPredecl()) {
            Logger::E(TAG, "Enumeration \"%s\" is not declared",
                    enumn->GetName().string());
            return false;
        }
    }

    return true;
}

std::shared_ptr<MetaComponent> MetaBuilder::Build()
{
    if (!IsValid()) {
        Logger::E(TAG, "The module which used to generate"
                " metadata is not validate.");
        return nullptr;
    }

    mSize = (int)CalculateMetadataSize();
    void* metadata = calloc(mSize, 1);
    if (metadata == nullptr) {
        Logger::E(TAG, "Out of memory.");
        return nullptr;
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
    int CONST_NUM = module->GetConstantNumber();
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
    // mConstatns's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaNamespace*) * NS_NUM);
    // mCoclasses's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaConstant*) * CONST_NUM);
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

    for (int i = 0; i < CONST_NUM; i++) {
        CalculateMetaConstant(module->GetConstant(i));
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

    std::shared_ptr< ArrayList<StringMap<Type*>::Pair*> > types =
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
    int NEST_ITF_NUM = itf->GetNestedInterfaceNumber();
    int CONST_NUM = itf->GetConstantNumber();
    int MTH_NUM = itf->GetMethodNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // add mName to StringPool
    mStringPool.Add(itf->GetName());
    // add mNamespace to StringPool
    mStringPool.Add(itf->GetNamespace()->ToString());
    // mNestedInterfaces's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaInterface));
    // mConstants's address
    mBasePtr = ALIGN(mBasePtr + sizeof(int) * NEST_ITF_NUM);
    // mMethods's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaConstant*) * CONST_NUM);
    // end address
    mBasePtr = mBasePtr + sizeof(MetaMethod*) * MTH_NUM;

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
    if (constant->GetType()->IsStringType()) {
        mStringPool.Add(constant->GetValue()->StringValue());
    }
    else if (constant->GetType()->IsEnumerationType()) {
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
    // mParameters's address
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
    int CONST_NUM = ns->GetConstantNumber();
    int CLS_NUM = ns->GetCoclassNumber();
    int ENUMN_NUM = ns->GetEnumerationNumber();
    int ITF_NUM = ns->GetInterfaceNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // add mName to StringPool
    mStringPool.Add(ns->ToString());
    // mConstantIndexes's address
    mBasePtr = ALIGN4(mBasePtr + sizeof(MetaNamespace));
    // mCoclassIndexes's address
    mBasePtr = ALIGN4(mBasePtr + sizeof(int) * CONST_NUM);
    // mEnumerationIndexes's address
    mBasePtr = ALIGN4(mBasePtr + sizeof(int) * CLS_NUM);
    // mInterfaceIndexes's address
    mBasePtr = ALIGN4(mBasePtr + sizeof(int) * ENUMN_NUM);
    // end address
    mBasePtr = mBasePtr + sizeof(int) * ITF_NUM;
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
    // end address
    mBasePtr = ALIGN(mBasePtr + mStringPool.GetSize());
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
    int CONST_NUM = module->GetConstantNumber();
    int CLS_NUM = module->GetCoclassNumber();
    int ENUMN_NUM = module->GetEnumerationNumber();
    int ITF_NUM = module->GetInterfaceNumber();
    int NS_NUM = module->GetNamespaceNumber();
    int TP_NUM = module->GetTypeNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    MetaComponent* mc = reinterpret_cast<MetaComponent*>(mBasePtr);
    mc->mMagic = CCM_MAGIC;
    mc->mSize = mSize;
    module->GetUuid().Assign(mc->mUuid);
    mc->mNamespaceNumber = NS_NUM;
    mc->mConstantNumber = CONST_NUM;
    mc->mCoclassNumber = CLS_NUM;
    mc->mEnumerationNumber = ENUMN_NUM;
    mc->mExternalEnumerationNumber = 0;
    mc->mInterfaceNumber = ITF_NUM;
    mc->mExternalInterfaceNumber = 0;
    mc->mTypeNumber = TP_NUM;
    // mNamespaces's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaComponent));
    mc->mNamespaces = reinterpret_cast<MetaNamespace**>(mBasePtr);
    // mConstants's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaNamespace*) * NS_NUM);
    mc->mConstants = reinterpret_cast<MetaConstant**>(mBasePtr);
    // mCoclasses's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaConstant*) * CONST_NUM);
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
    // mStringPool's address
    mBasePtr = mBasePtr + sizeof(MetaType*) * TP_NUM;
    mc->mStringPool = reinterpret_cast<char*>(mBasePtr);
    // end address
    mBasePtr = mBasePtr + mStringPool.GetSize();
    memcpy(mc->mStringPool, mStringPool.GetData(), mStringPool.GetSize());

    mc->mName = WriteString(module->GetName());
    mc->mUrl = WriteString(module->GetUrl());

    for (int i = 0; i < NS_NUM; i++) {
        mc->mNamespaces[i] = WriteMetaNamespace(module->GetNamespace(i));
        mc->mExternalInterfaceNumber += module->GetNamespace(i)->GetExternalInterfaceNumber();
        mc->mExternalEnumerationNumber += module->GetNamespace(i)->GetExternalEnumerationNumber();
    }

    for (int i = 0; i < CONST_NUM; i++) {
        mc->mConstants[i] = WriteMetaConstant(module->GetConstant(i));
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

    for (int i = 0; i < TP_NUM; i++) {
        mc->mTypes[i] = WriteMetaType(module->GetType(i));
    }
}

MetaCoclass* MetaBuilder::WriteMetaCoclass(
    /* [in] */ Coclass* klass)
{
    int ITF_NUM = klass->GetInterfaceNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    MetaCoclass* mc = reinterpret_cast<MetaCoclass*>(mBasePtr);
    klass->GetUuid().Assign(mc->mUuid);
    mc->mName = WriteString(klass->GetName());
    mc->mNamespace = WriteString(klass->GetNamespace()->ToString());
    mc->mInterfaceNumber = ITF_NUM;
    // mInterfaceIndexes's address
    mBasePtr = ALIGN4(mBasePtr + sizeof(MetaCoclass));
    mc->mInterfaceIndexes = reinterpret_cast<int*>(mBasePtr);
    mc->mConstructorDefault = klass->HasDefaultConstructor();
    mc->mConstructorDeleted = klass->IsConstructorDeleted();
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
    mc->mName = WriteString(constant->GetName());
    Type* type = constant->GetType();
    mc->mTypeIndex = mModule->IndexOf(type);
    mc->mValue.mAttributes = 0;
    if (type->IsByteType() || type->IsShortType() || type->IsIntegerType()) {
        mc->mValue.mInteger = constant->GetValue()->IntegerValue();
    }
    else if (type->IsLongType()) {
        mc->mValue.mLong = constant->GetValue()->LongValue();
    }
    else if (type->IsCharType()) {
        mc->mValue.mInteger = constant->GetValue()->CharacterValue();
    }
    else if (type->IsFloatType()) {
        mc->mValue.mFloat = constant->GetValue()->FloatValue();
        if (constant->GetValue()->IsPositiveInfinity()) {
            mc->mValue.mAttributes |= POSITIVE_INFINITY_MASK;
        }
        if (constant->GetValue()->IsNegativeInfinity()) {
            mc->mValue.mAttributes |= NEGATIVE_INFINITY_MASK;
        }
        if (constant->GetValue()->IsNaN()) {
            mc->mValue.mAttributes |= NAN_MASK;
        }
        if (constant->GetValue()->IsScientificNotation()) {
            mc->mValue.mAttributes |= SCIENTIFIC_NOTATION_MASK;
        }
    }
    else if (type->IsDoubleType()) {
        mc->mValue.mDouble = constant->GetValue()->DoubleValue();
        if (constant->GetValue()->IsPositiveInfinity()) {
            mc->mValue.mAttributes |= POSITIVE_INFINITY_MASK;
        }
        if (constant->GetValue()->IsNegativeInfinity()) {
            mc->mValue.mAttributes |= NEGATIVE_INFINITY_MASK;
        }
        if (constant->GetValue()->IsNaN()) {
            mc->mValue.mAttributes |= NAN_MASK;
        }
        if (constant->GetValue()->IsScientificNotation()) {
            mc->mValue.mAttributes |= SCIENTIFIC_NOTATION_MASK;
        }
    }
    else if (type->IsBooleanType()) {
        mc->mValue.mBoolean = constant->GetValue()->BooleanValue();
    }
    else if (type->IsStringType()) {
        mc->mValue.mString = WriteString(
                constant->GetValue()->StringValue());
    }
    else if (type->IsEnumerationType()) {
        mc->mValue.mString = WriteString(
                    constant->GetValue()->EnumeratorValue());
    }
    if (type->IsIntegralType()) {
        mc->mValue.mAttributes |= (constant->GetValue()->GetRadix() & RADIX_MASK);
    }
    else mc->mValue.mAttributes |= 0;
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
    me->mName = WriteString(enumn->GetName());
    me->mNamespace = WriteString(enumn->GetNamespace()->ToString());
    me->mEnumeratorNumber = ENUMR_NUM;
    // mEnumerators's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaEnumeration));
    me->mEnumerators = reinterpret_cast<MetaEnumerator**>(mBasePtr);
    me->mExternal = enumn->IsExternal();
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
    me->mName = WriteString(enumr->mName);
    me->mValue = enumr->mValue;
    // end address
    mBasePtr = mBasePtr + sizeof(MetaEnumerator);

    return me;
}

MetaInterface* MetaBuilder::WriteMetaInterface(
    /* [in] */ Interface* itf)
{
    int NEST_ITF_NUM = itf->GetNestedInterfaceNumber();
    int CONST_NUM = itf->GetConstantNumber();
    int MTH_NUM = itf->GetMethodNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    MetaInterface* mi = reinterpret_cast<MetaInterface*>(mBasePtr);
    itf->GetUuid().Assign(mi->mUuid);
    mi->mName = WriteString(itf->GetName());
    mi->mNamespace = WriteString(itf->GetNamespace()->ToString());
    Interface* baseItf = itf->GetBaseInterface();
    mi->mBaseInterfaceIndex = baseItf != nullptr ?
            mModule->IndexOf(baseItf) : -1;
    Interface* outerItf = itf->GetOuterInterface();
    mi->mOuterInterfaceIndex = outerItf != nullptr ?
            mModule->IndexOf(outerItf) : -1;
    mi->mNestedInterfaceNumber = NEST_ITF_NUM;
    mi->mConstantNumber = CONST_NUM;
    mi->mMethodNumber = MTH_NUM;
    mi->mExternal = itf->IsExternal();
    // mNestedInterfaceIndexes's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaInterface));
    mi->mNestedInterfaceIndexes = reinterpret_cast<int*>(mBasePtr);
    // mConstants's address
    mBasePtr = ALIGN(mBasePtr + sizeof(int) * NEST_ITF_NUM);
    mi->mConstants = reinterpret_cast<MetaConstant**>(mBasePtr);
    // mMethods's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaConstant*) * CONST_NUM);
    mi->mMethods = reinterpret_cast<MetaMethod**>(mBasePtr);
    // end address
    mBasePtr = mBasePtr + sizeof(MetaMethod*) * MTH_NUM;
    for (int i = 0; i < NEST_ITF_NUM; i++) {
        mi->mNestedInterfaceIndexes[i] = mModule->IndexOf(itf->GetNestedInterface(i));
    }

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
    int PARAM_NUM = method->GetParameterNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    MetaMethod* mm = reinterpret_cast<MetaMethod*>(mBasePtr);
    mm->mName = WriteString(method->GetName());
    mm->mSignature = WriteString(method->GetSignature());
    mm->mReturnTypeIndex = mModule->IndexOf(method->GetReturnType());
    mm->mParameterNumber = PARAM_NUM;
    // mParameters's address
    mBasePtr = ALIGN(mBasePtr + sizeof(MetaMethod));
    mm->mParameters = reinterpret_cast<MetaParameter**>(mBasePtr);
    mm->mDeleted = method->IsDeleted();
    mm->mReference = method->IsReference();
    // end address
    mBasePtr = mBasePtr + sizeof(MetaParameter*) * PARAM_NUM;

    for (int i = 0; i < PARAM_NUM; i++) {
        mm->mParameters[i] = WriteMetaParameter(method->GetParameter(i));
    }

    return mm;
}

MetaNamespace* MetaBuilder::WriteMetaNamespace(
    /* [in] */ Namespace* ns)
{
    int CONST_NUM = ns->GetConstantNumber();
    int CLS_NUM = ns->GetCoclassNumber();
    int ENUMN_NUM = ns->GetEnumerationNumber();
    int ITF_NUM = ns->GetInterfaceNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    MetaNamespace* mn = reinterpret_cast<MetaNamespace*>(mBasePtr);
    mn->mName = WriteString(ns->ToString());
    mn->mInterfaceWrappedIndex = ns->IsInterfaceWrapper() ? mModule->IndexOf(ns->GetInterfaceWrapped()) : -1;
    mn->mConstantNumber = CONST_NUM;
    mn->mCoclassNumber = CLS_NUM;
    mn->mEnumerationNumber = ENUMN_NUM;
    mn->mExternalEnumerationNumber = ns->GetExternalEnumerationNumber();
    mn->mInterfaceNumber = ITF_NUM;
    mn->mExternalInterfaceNumber = ns->GetExternalInterfaceNumber();
    // mConstantIndexes's address
    mBasePtr = ALIGN4(mBasePtr + sizeof(MetaNamespace));
    mn->mConstantIndexes = reinterpret_cast<int*>(mBasePtr);
    // mCoclassIndexes's address
    mBasePtr = ALIGN4(mBasePtr + sizeof(int) * CONST_NUM);
    mn->mCoclassIndexes = reinterpret_cast<int*>(mBasePtr);
    // mEnumerationIndexes's address
    mBasePtr = ALIGN4(mBasePtr + sizeof(int) * CLS_NUM);
    mn->mEnumerationIndexes = reinterpret_cast<int*>(mBasePtr);
    // mInterfaceIndexes's address
    mBasePtr = ALIGN4(mBasePtr + sizeof(int) * ENUMN_NUM);
    mn->mInterfaceIndexes = reinterpret_cast<int*>(mBasePtr);
    // end address
    mBasePtr = mBasePtr + sizeof(int) * ITF_NUM;

    for (int i = 0; i < CONST_NUM; i++) {
        Constant* constant = ns->GetConstant(i);
        mn->mConstantIndexes[i] = mModule->IndexOf(constant);
    }

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

    return mn;
}

MetaParameter* MetaBuilder::WriteMetaParameter(
    /* [in] */ Parameter* param)
{
    // begin address
    mBasePtr = ALIGN(mBasePtr);
    MetaParameter* mp = reinterpret_cast<MetaParameter*>(mBasePtr);
    mp->mName = WriteString(param->GetName());
    mp->mAttribute = param->GetAttribute();
    Type* type = param->GetType();
    mp->mTypeIndex = mModule->IndexOf(type);
    if (param->HasDefaultValue()){
        mp->mHasDefaultValue = true;
        if (type->IsByteType() || type->IsShortType() || type->IsIntegerType()) {
            mp->mDefaultValue.mInteger = param->GetDefaultValue()->IntegerValue();
        }
        else if (type->IsLongType()) {
            mp->mDefaultValue.mLong = param->GetDefaultValue()->LongValue();
        }
        else if (type->IsCharType()) {
            mp->mDefaultValue.mInteger = param->GetDefaultValue()->CharacterValue();
        }
        else if (type->IsFloatType()) {
            mp->mDefaultValue.mFloat = param->GetDefaultValue()->FloatValue();
            if (param->GetDefaultValue()->IsPositiveInfinity()) {
                mp->mDefaultValue.mAttributes |= POSITIVE_INFINITY_MASK;
            }
            if (param->GetDefaultValue()->IsNegativeInfinity()) {
                mp->mDefaultValue.mAttributes |= NEGATIVE_INFINITY_MASK;
            }
            if (param->GetDefaultValue()->IsNaN()) {
                mp->mDefaultValue.mAttributes |= NAN_MASK;
            }
            if (param->GetDefaultValue()->IsScientificNotation()) {
                mp->mDefaultValue.mAttributes |= SCIENTIFIC_NOTATION_MASK;
            }
        }
        else if (type->IsDoubleType()) {
            mp->mDefaultValue.mDouble = param->GetDefaultValue()->DoubleValue();
            if (param->GetDefaultValue()->IsPositiveInfinity()) {
                mp->mDefaultValue.mAttributes |= POSITIVE_INFINITY_MASK;
            }
            if (param->GetDefaultValue()->IsNegativeInfinity()) {
                mp->mDefaultValue.mAttributes |= NEGATIVE_INFINITY_MASK;
            }
            if (param->GetDefaultValue()->IsNaN()) {
                mp->mDefaultValue.mAttributes |= NAN_MASK;
            }
            if (param->GetDefaultValue()->IsScientificNotation()) {
                mp->mDefaultValue.mAttributes |= SCIENTIFIC_NOTATION_MASK;
            }
        }
        else if (type->IsBooleanType()) {
            mp->mDefaultValue.mBoolean = param->GetDefaultValue()->BooleanValue();
        }
        else if (type->IsPointerType()) {
            mp->mDefaultValue.mLong = param->GetDefaultValue()->LongValue();
        }
    }
    else {
        mp->mHasDefaultValue = false;
    }
    // end address
    mBasePtr = mBasePtr + sizeof(MetaParameter);

    return mp;
}

MetaType* MetaBuilder::WriteMetaType(
    /* [in] */ Type* type)
{
    // begin address
    mBasePtr = ALIGN(mBasePtr);
    MetaType* mt = reinterpret_cast<MetaType*>(mBasePtr);
    mt->mPointerNumber = 0;
    mt->mReference = false;
    if (type->IsReferenceType()) {
        mt->mReference = true;
        type = ((ReferenceType*)type)->GetBaseType();
    }
    if (type->IsPointerType()) {
        mt->mPointerNumber = ((PointerType*)type)->GetPointerNumber();
        type = ((PointerType*)type)->GetBaseType();
    }
    mt->mKind = Type2CcdlType(type);
    if (type->IsEnumerationType()) {
        mt->mIndex = mModule->IndexOf((Enumeration*)type);
    }
    else if (type->IsInterfaceType()) {
        mt->mIndex = mModule->IndexOf((Interface*)type);
    }
    else if (type->IsCoclassType()) {
        mt->mIndex = mModule->IndexOf((Coclass*)type);
    }
    else {
        mt->mIndex = mModule->IndexOf(type);
    }
    mt->mNestedTypeIndex = -1;
    if (type->IsArrayType()) {
        mt->mNestedTypeIndex =  mModule->IndexOf(
                ((ArrayType*)type)->GetElementType());
    }
    // end address
    mBasePtr = mBasePtr + sizeof(MetaType);

    return mt;
}

char* MetaBuilder::WriteString(
    /* [in] */ const String& string)
{
    return mMetaComponet->mStringPool + mStringPool.FindOffset(string);
}

CcmTypeKind MetaBuilder::Type2CcdlType(
    /* [in] */ Type* type)
{
    if (type->IsBooleanType()) {
        return CcmTypeKind::Boolean;
    }
    else if (type->IsCharType()) {
        return CcmTypeKind::Char;
    }
    else if (type->IsByteType()) {
        return CcmTypeKind::Byte;
    }
    else if (type->IsShortType()) {
        return CcmTypeKind::Short;
    }
    else if (type->IsIntegerType()) {
        return CcmTypeKind::Integer;
    }
    else if (type->IsLongType()) {
        return CcmTypeKind::Long;
    }
    else if (type->IsFloatType()) {
        return CcmTypeKind::Float;
    }
    else if (type->IsDoubleType()) {
        return CcmTypeKind::Double;
    }
    else if (type->IsStringType()) {
        return CcmTypeKind::String;
    }
    else if (type->IsCoclassIDType()) {
        return CcmTypeKind::CoclassID;
    }
    else if (type->IsComponentIDType()){
        return CcmTypeKind::ComponentID;
    }
    else if (type->IsInterfaceIDType()) {
        return CcmTypeKind::InterfaceID;
    }
    else if (type->IsHANDLEType()) {
        return CcmTypeKind::HANDLE;
    }
    else if (type->IsECodeType()) {
        return CcmTypeKind::ECode;
    }
    else if (type->IsEnumerationType()) {
        return CcmTypeKind::Enum;
    }
    else if (type->IsArrayType()) {
        return CcmTypeKind::Array;
    }
    else if (type->IsInterfaceType()) {
        return CcmTypeKind::Interface;
    }
    else if (type->IsCoclassType()) {
        return CcmTypeKind::Coclass;
    }
    else if (type->IsTripleType()) {
        return CcmTypeKind::Triple;
    }
    return CcmTypeKind::Unknown;
}

}
}
