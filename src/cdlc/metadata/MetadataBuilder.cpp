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

#include "metadata/MetadataBuilder.h"
#include "ast/ArrayType.h"
#include "ast/PointerType.h"
#include "ast/ReferenceType.h"
#include "util/Logger.h"

namespace cdlc {

const char* MetadataBuilder::TAG = "MetadataBuilder";

std::shared_ptr<como::MetaComponent> MetadataBuilder::Build()
{
    mSize = CalculateMetadata();
    void* metadata = calloc(mSize, 1);
    if (metadata == nullptr) {
        Logger::E(TAG, "Out of memory.");
        return nullptr;
    }

    mMetaComponent.reset(
            new(metadata) como::MetaComponent,
            [](como::MetaComponent* p){ free(p); });

    WriteMetadata(reinterpret_cast<uintptr_t>(metadata));

    return mMetaComponent;
}

size_t MetadataBuilder::CalculateMetadata()
{
    mBasePtr = 0;
    CalculateMetaComponent(mModule);
    return mBasePtr;
}

void MetadataBuilder::CalculateMetaComponent(
    /* [in] */ Module* module)
{
    int CN = module->GetConstantNumber();
    int EN = module->GetEnumerationNumber();
    int IN = module->GetInterfaceNumber();
    int KN = module->GetCoclassNumber();
    int TN = module->GetTypeNumber();
    int NN = module->GetNamespaceNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // add mName to StringPool
    mPool.Add(module->GetName());
    // add mUri to StringPool
    mPool.Add(module->GetUri());
    // mNamespaces address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaComponent));
    // mConstants address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaNamespace*) * NN);
    // mCoclasses address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaConstant*) * CN);
    // mEnumerations address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaCoclass*) * KN);
    // mInterfaces address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaEnumeration*) * EN);
    // mTypes address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaInterface*) * IN);
    // mStringPool address
    mBasePtr = mBasePtr + sizeof(como::MetaType*) * TN;

    for (int i = 0; i < NN; i++) {
        CalculateMetaNamespace(module->GetNamespace(i));
    }

    for (int i = 0; i < CN; i++) {
        CalculateMetaConstant(module->GetConstant(i));
    }

    for (int i = 0; i < KN; i++) {
        CalculateMetaCoclass(module->GetCoclass(i));
    }

    for (int i = 0; i < EN; i++) {
        CalculateMetaEnumeration(module->GetEnumeration(i));
    }

    for (int i = 0; i < IN; i++) {
        CalculateMetaInterface(module->GetInterface(i));
    }

    auto& types = module->GetTypes();
    for (auto it = types.begin(); it != types.end(); it++) {
        CalculateMetaType(it->second);
    }

    // end address
    CalculateStringPool();
}

void MetadataBuilder::CalculateMetaNamespace(
    /* [in] */ Namespace* ns)
{
    int CN = ns->GetConstantNumber();
    int EN = ns->GetEnumerationNumber();
    int IN = ns->GetInterfaceNumber();
    int KN = ns->GetCoclassNumber();
    int NN = ns->GetNamespaceNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // add mName to StringPool
    mPool.Add(ns->ToString());
    // mNamespaces address
    mBasePtr =  ALIGN(mBasePtr + sizeof(como::MetaNamespace));
    // mConstantIndexes address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaNamespace*) * NN);
    // mCoclassIndexes address
    mBasePtr = ALIGN(mBasePtr + sizeof(int) * CN);
    // mEnumerationIndexes address
    mBasePtr = ALIGN(mBasePtr + sizeof(int) * KN);
    // mInterfaceIndexes address
    mBasePtr = ALIGN(mBasePtr + sizeof(int) * EN);
    // end address
    mBasePtr = mBasePtr + sizeof(int) * IN;

    for (int i = 0; i < NN; i++) {
        CalculateMetaNamespace(ns->GetNamespace(i));
    }
}

void MetadataBuilder::CalculateMetaConstant(
    /* [in] */ Constant* constant)
{
    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // add mName to StringPool
    mPool.Add(constant->GetName());
    // add value to StringPool when constant is String or Enumerator
    if (constant->GetType()->IsStringType()) {
        mPool.Add(constant->GetValue()->StringValue());
    }
    else if (constant->GetType()->IsEnumerationType()) {
        mPool.Add(constant->GetValue()->EnumeratorValue());
    }
    // end address
    mBasePtr = mBasePtr + sizeof(como::MetaConstant);
}

void MetadataBuilder::CalculateMetaCoclass(
    /* [in] */ CoclassType* klass)
{
    int IN = klass->GetInterfaceNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // add mName to StringPool
    mPool.Add(klass->GetName());
    // add mNamespace to StringPool
    mPool.Add(klass->GetNamespace()->ToString());
    // mInterfaceIndexes address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaCoclass));
    // end address
    mBasePtr = mBasePtr + sizeof(int) * IN;
}

void MetadataBuilder::CalculateMetaEnumeration(
    /* [in] */ EnumerationType* enumeration)
{
    int EN = enumeration->GetEnumeratorNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // add mName to StringPool
    mPool.Add(enumeration->GetName());
    // add mNamespace to StringPool
    mPool.Add(enumeration->GetNamespace()->ToString());
    // mEnumerators address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaEnumeration));
    // end address
    mBasePtr = mBasePtr + sizeof(como::MetaEnumerator*) * EN;

    for (int i = 0; i < EN; i++) {
        CalculateMetaEnumerator(enumeration->GetEnumerator(i));
    }
}

void MetadataBuilder::CalculateMetaEnumerator(
    /* [in] */ EnumerationType::Enumerator* enumerator)
{
    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // add mName to StringPool
    mPool.Add(enumerator->mName);
    // end address
    mBasePtr = mBasePtr + sizeof(como::MetaEnumerator);
}

void MetadataBuilder::CalculateMetaInterface(
    /* [in] */ InterfaceType* interface)
{
    int IN = interface->GetNestedInterfaceNumber();
    int CN = interface->GetConstantNumber();
    int MN = interface->GetMethodNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // add mName to StringPool
    mPool.Add(interface->GetName());
    // add mNamespace to StringPool
    mPool.Add(interface->GetNamespace()->ToString());
    // mNestedInterfaceIndexes address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaInterface));
    // mConstants address
    mBasePtr = ALIGN(mBasePtr + sizeof(int) * IN);
    // mMethods address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaConstant*) * CN);
    // end address
    mBasePtr = mBasePtr + sizeof(como::MetaMethod*) * MN;

    for (int i = 0; i < CN; i++) {
        CalculateMetaConstant(interface->GetConstant(i));
    }

    for (int i = 0;  i < MN; i++) {
        CalculateMetaMethod(interface->GetMethod(i));
    }
}

void MetadataBuilder::CalculateMetaMethod(
    /* [in] */ Method* method)
{
    int PN = method->GetParameterNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // add mName to StringPool
    mPool.Add(method->GetName());
    // add mSignature to StringPool
    mPool.Add(method->GetSignature());
    // mParameters address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaMethod));
    // end address
    mBasePtr = mBasePtr + sizeof(como::MetaParameter*) * PN;

    for (int i = 0; i < PN; i++) {
        CalculateMetaParameter(method->GetParameter(i));
    }
}

void MetadataBuilder::CalculateMetaParameter(
    /* [in] */ Parameter* parameter)
{
    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // add mName to StringPool
    mPool.Add(parameter->GetName());
    // end address
    mBasePtr = mBasePtr + sizeof(como::MetaParameter);

    if (parameter->HasDefaultValue()) {
        CalculateMetaValue(parameter);
    }
}

void MetadataBuilder::CalculateMetaValue(
    /* [in] */ Parameter* parameter)
{
    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // add value to StringPool when expression is String or Enumerator
    if (parameter->GetType()->IsStringType()) {
        mPool.Add(parameter->GetDefaultValue()->StringValue());
    }
    else if (parameter->GetType()->IsEnumerationType()) {
        mPool.Add(parameter->GetDefaultValue()->EnumeratorValue());
    }
    else if (parameter->GetType()->IsPointerType()) {
        mPool.Add(parameter->GetDefaultValue()->StringValue());
    }
    // end address
    mBasePtr = mBasePtr + sizeof(como::MetaValue);
}

void MetadataBuilder::CalculateMetaType(
    /* [in] */ Type* type)
{
    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // end address
    mBasePtr = mBasePtr + sizeof(como::MetaType);

    if (type->GetExternalModule() != nullptr) {
        mBasePtr = ALIGN(mBasePtr);
        // add the name of external module to StringPool
        mPool.Add(type->GetExternalModule()->GetName());
        mBasePtr = mBasePtr + sizeof(char**);
    }
}

void MetadataBuilder::CalculateStringPool()
{
    // begin address
    mBasePtr = ALIGN(mBasePtr);
    // end address
    mBasePtr = mBasePtr + mPool.GetSize();
}

void MetadataBuilder::WriteMetadata(
    /* [in] */ uintptr_t base)
{
    mBasePtr = base;
    WriteMetaComponent(mModule);
}

void MetadataBuilder::WriteMetaComponent(
    /* [in] */ Module* module)
{
    int CN = module->GetConstantNumber();
    int EN = module->GetEnumerationNumber();
    int IN = module->GetInterfaceNumber();
    int KN = module->GetCoclassNumber();
    int TN = module->GetTypeNumber();
    int NN = module->GetNamespaceNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    como::MetaComponent* mc = reinterpret_cast<como::MetaComponent*>(mBasePtr);
    mc->mMagic = COMO_MAGIC;
    mc->mSize = mSize;
    AssignFrom(mc->mUuid, module->GetUUID());
    mc->mNamespaceNumber = NN;
    mc->mConstantNumber = CN;
    mc->mCoclassNumber = KN;
    mc->mEnumerationNumber = EN;
    mc->mInterfaceNumber = IN;
    mc->mTypeNumber = TN;
    // mNamespaces address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaComponent));
    mc->mNamespaces = NN > 0 ? reinterpret_cast<como::MetaNamespace**>(mBasePtr) : nullptr;
    // mConstants address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaNamespace*) * NN);
    mc->mConstants = CN > 0 ? reinterpret_cast<como::MetaConstant**>(mBasePtr) : nullptr;
    // mCoclasses address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaConstant*) * CN);
    mc->mCoclasses = KN > 0 ? reinterpret_cast<como::MetaCoclass**>(mBasePtr) : nullptr;
    // mEnumerations address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaCoclass*) * KN);
    mc->mEnumerations = EN > 0 ? reinterpret_cast<como::MetaEnumeration**>(mBasePtr) : nullptr;
    // mInterfaces address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaEnumeration*) * EN);
    mc->mInterfaces = IN > 0 ? reinterpret_cast<como::MetaInterface**>(mBasePtr) : nullptr;
    // mTypes address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaInterface*) * IN);
    mc->mTypes = TN > 0 ? reinterpret_cast<como::MetaType**>(mBasePtr) : nullptr;
    // mStringPool address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaType*) * TN);
    mc->mStringPool = mPool.GetSize() > 0 ? reinterpret_cast<char*>(mBasePtr) : nullptr;
    // end address
    mBasePtr = mBasePtr + mPool.GetSize();

    memcpy(mc->mStringPool, mPool.GetData(), mPool.GetSize());

    mc->mName = WriteString(module->GetName());
    mc->mUri = WriteString(module->GetUri());

    for (int i = 0; i < NN; i++) {
        mc->mNamespaces[i] = WriteMetaNamespace(module->GetNamespace(i));
    }

    for (int i = 0; i < CN; i++) {
        mc->mConstants[i] = WriteMetaConstant(module->GetConstant(i));
    }

    for (int i = 0; i < KN; i++) {
        mc->mCoclasses[i] = WriteMetaCoclass(module->GetCoclass(i));
    }

    for (int i = 0; i < EN; i++) {
        mc->mEnumerations[i] = WriteMetaEnumeration(module->GetEnumeration(i));
    }

    for (int i = 0; i < IN; i++) {
        mc->mInterfaces[i] = WriteMetaInterface(module->GetInterface(i));
    }

    auto& types = module->GetTypes();
    auto it = types.begin();
    for (int i = 0; i < TN && it != types.end(); i++, it++) {
        mc->mTypes[i] = WriteMetaType(it->second);
    }
}

como::MetaNamespace* MetadataBuilder::WriteMetaNamespace(
    /* [in] */ Namespace* ns)
{
    int CN = ns->GetConstantNumber();
    int EN = ns->GetEnumerationNumber();
    int IN = ns->GetInterfaceNumber();
    int KN = ns->GetCoclassNumber();
    int NN = ns->GetNamespaceNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    como::MetaNamespace* mn = reinterpret_cast<como::MetaNamespace*>(mBasePtr);
    mn->mName = WriteString(ns->ToString());
    mn->mInterfaceWrappedIndex = ns->IsInterfaceWrapper() ? mModule->IndexOf(ns->GetInterfaceWrapped()) : -1;
    mn->mNamespaceNumber = NN;
    mn->mConstantNumber = CN;
    mn->mCoclassNumber = KN;
    mn->mEnumerationNumber = EN;
    mn->mInterfaceNumber = IN;
    // mNamespaces address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaNamespace));
    mn->mNamespaces = NN > 0 ? reinterpret_cast<como::MetaNamespace**>(mBasePtr) : nullptr;
    // mConstantIndexes address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaNamespace*) * NN);
    mn->mConstantIndexes = CN > 0 ? reinterpret_cast<int*>(mBasePtr) : nullptr;
    // mCoclassIndexes address
    mBasePtr = ALIGN(mBasePtr + sizeof(int) * CN);
    mn->mCoclassIndexes = KN > 0 ? reinterpret_cast<int*>(mBasePtr) : nullptr;
    // mEnumerationIndexes address
    mBasePtr = ALIGN(mBasePtr + sizeof(int) * KN);
    mn->mEnumerationIndexes = EN > 0 ? reinterpret_cast<int*>(mBasePtr) : nullptr;
    // mInterfaceIndexes address
    mBasePtr = ALIGN(mBasePtr + sizeof(int) * EN);
    mn->mInterfaceIndexes = IN > 0 ? reinterpret_cast<int*>(mBasePtr) : nullptr;
    // end address
    mBasePtr = mBasePtr + sizeof(int) * IN;

    for (int i = 0; i < NN; i++) {
        mn->mNamespaces[i] = WriteMetaNamespace(ns->GetNamespace(i));
    }

    for (int i = 0; i < CN; i++) {
        mn->mConstantIndexes[i] = mModule->IndexOf(ns->GetConstant(i));
    }

    for (int i = 0; i < KN; i++) {
        mn->mCoclassIndexes[i] = mModule->IndexOf(ns->GetCoclass(i));
    }

    for (int i = 0; i < EN; i++) {
        mn->mEnumerationIndexes[i] = mModule->IndexOf(ns->GetEnumeration(i));
    }

    for (int i = 0; i < IN; i++) {
        mn->mInterfaceIndexes[i] = mModule->IndexOf(ns->GetInterface(i));
    }

    return mn;
}

como::MetaConstant* MetadataBuilder::WriteMetaConstant(
    /* [in] */ Constant* constant)
{
    // begin address
    mBasePtr = ALIGN(mBasePtr);
    como::MetaConstant* mc = reinterpret_cast<como::MetaConstant*>(mBasePtr);
    mc->mName = WriteString(constant->GetName());
    AutoPtr<Type> type = constant->GetType();
    mc->mTypeIndex = mModule->IndexOf(type);
    mc->mValue.mProperties = 0;
    if (type->IsBooleanType()) {
        mc->mValue.mBooleanValue = constant->GetValue()->BooleanValue();
    }
    else if (type->IsIntegralType()) {
        mc->mValue.mIntegralValue = constant->GetValue()->LongValue();
        mc->mValue.mProperties |= (constant->GetValue()->GetRadix() & VALUE_RADIX_MASK);
    }
    else if (type->IsFloatingPointType()) {
        mc->mValue.mFloatingPointValue = constant->GetValue()->DoubleValue();
        if (constant->GetValue()->IsPositiveInfinity()) {
            mc->mValue.mProperties |= VALUE_POSITIVE_INFINITY;
        }
        if (constant->GetValue()->IsNegativeInfinity()) {
            mc->mValue.mProperties |= VALUE_NEGATIVE_INFINITY;
        }
        if (constant->GetValue()->IsNaN()) {
            mc->mValue.mProperties |= VALUE_NAN;
        }
        if (constant->GetValue()->IsScientificNotation()) {
            mc->mValue.mProperties |= VALUE_SCIENTIFIC_NOTATION;
        }
    }
    else if (type->IsStringType()) {
        mc->mValue.mStringValue = WriteString(constant->GetValue()->StringValue());
    }
    else if (type->IsEnumerationType()) {
        mc->mValue.mStringValue = WriteString(constant->GetValue()->EnumeratorValue());
    }
    // end address
    mBasePtr = mBasePtr + sizeof(como::MetaConstant);

    return mc;
}

como::MetaCoclass* MetadataBuilder::WriteMetaCoclass(
    /* [in] */ CoclassType* klass)
{
    int IN = klass->GetInterfaceNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    como::MetaCoclass* mc = reinterpret_cast<como::MetaCoclass*>(mBasePtr);
    AssignFrom(mc->mUuid, klass->GetUUID());
    mc->mName = WriteString(klass->GetName());
    mc->mNamespace = WriteString(klass->GetNamespace()->ToString());
    mc->mInterfaceNumber = IN;
    // mInterfaceIndexes address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaCoclass));
    mc->mInterfaceIndexes = IN > 0 ? reinterpret_cast<int*>(mBasePtr) : nullptr;
    mc->mProperties = 0;
    if (klass->HasDefaultConstructor()) {
        mc->mProperties |= COCLASS_CONSTRUCTOR_DEFAULT;
    }
    if (klass->IsConstructorDeleted()) {
        mc->mProperties |= COCLASS_CONSTRUCTOR_DELETED;
    }
    // end address
    mBasePtr = mBasePtr + sizeof(int) * IN;

    for (int i = 0; i < IN; i++) {
        mc->mInterfaceIndexes[i] = mModule->IndexOf(klass->GetInterface(i));
    }

    return mc;
}

como::MetaEnumeration* MetadataBuilder::WriteMetaEnumeration(
    /* [in] */ EnumerationType* enumeration)
{
    int EN = enumeration->GetEnumeratorNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    como::MetaEnumeration* me = reinterpret_cast<como::MetaEnumeration*>(mBasePtr);
    me->mName = WriteString(enumeration->GetName());
    me->mNamespace = WriteString(enumeration->GetNamespace()->ToString());
    me->mEnumeratorNumber = EN;
    // mEnumerators address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaEnumeration));
    me->mEnumerators = EN > 0 ? reinterpret_cast<como::MetaEnumerator**>(mBasePtr) : nullptr;
    // end address
    mBasePtr = mBasePtr + sizeof(como::MetaEnumerator*) * EN;

    for (int i = 0; i < EN; i++) {
        me->mEnumerators[i] = WriteMetaEnumerator(enumeration->GetEnumerator(i));
    }

    return me;
}

como::MetaEnumerator* MetadataBuilder::WriteMetaEnumerator(
    /* [in] */ EnumerationType::Enumerator* enumerator)
{
    // begin address
    mBasePtr = ALIGN(mBasePtr);
    como::MetaEnumerator* me = reinterpret_cast<como::MetaEnumerator*>(mBasePtr);
    me->mName = WriteString(enumerator->mName);
    me->mValue = enumerator->mValue;
    // end address
    mBasePtr = mBasePtr + sizeof(como::MetaEnumerator);

    return me;
}

como::MetaInterface* MetadataBuilder::WriteMetaInterface(
    /* [in] */ InterfaceType* interface)
{
    int IN = interface->GetNestedInterfaceNumber();
    int CN = interface->GetConstantNumber();
    int MN = interface->GetMethodNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    como::MetaInterface* mi = reinterpret_cast<como::MetaInterface*>(mBasePtr);
    AssignFrom(mi->mUuid, interface->GetUUID());
    mi->mName = WriteString(interface->GetName());
    mi->mNamespace = WriteString(interface->GetNamespace()->ToString());
    mi->mBaseInterfaceIndex = mModule->IndexOf(interface->GetBaseInterface());
    mi->mOuterInterfaceIndex = mModule->IndexOf(interface->GetOuterInterface());
    mi->mNestedInterfaceNumber = IN;
    mi->mConstantNumber = CN;
    mi->mMethodNumber = MN;
    // mNestedInterfaceIndexes address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaInterface));
    mi->mNestedInterfaceIndexes = IN > 0 ? reinterpret_cast<int*>(mBasePtr) : nullptr;
    // mConstants address
    mBasePtr = ALIGN(mBasePtr + sizeof(int) * IN);
    mi->mConstants = CN > 0 ? reinterpret_cast<como::MetaConstant**>(mBasePtr) : nullptr;
    // mMethods address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaConstant*) * CN);
    mi->mMethods = MN > 0 ? reinterpret_cast<como::MetaMethod**>(mBasePtr) : nullptr;
    // end address
    mBasePtr = mBasePtr + sizeof(como::MetaMethod*) * MN;

    for (int i = 0; i < IN; i++) {
        mi->mNestedInterfaceIndexes[i] = mModule->IndexOf(
                interface->GetNestedInterface(i));
    }

    for (int i = 0; i < CN; i++) {
        mi->mConstants[i] = WriteMetaConstant(interface->GetConstant(i));
    }

    for (int i = 0; i < MN; i++) {
        mi->mMethods[i] = WriteMetaMethod(interface->GetMethod(i));
    }

    return mi;
}

como::MetaMethod* MetadataBuilder::WriteMetaMethod(
    /* [in] */ Method* method)
{
    int PN = method->GetParameterNumber();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    como::MetaMethod* mm = reinterpret_cast<como::MetaMethod*>(mBasePtr);
    mm->mName = WriteString(method->GetName());
    mm->mSignature = WriteString(method->GetSignature());
    mm->mReturnTypeIndex = mModule->IndexOf(method->GetReturnType());
    mm->mParameterNumber = PN;
    // mParameters address
    mBasePtr = ALIGN(mBasePtr + sizeof(como::MetaMethod));
    mm->mParameters = PN > 0 ? reinterpret_cast<como::MetaParameter**>(mBasePtr) : nullptr;
    mm->mProperties = 0;
    if (method->IsDeleted()) {
        mm->mProperties |= METHOD_DELETED;
    }
    if (method->IsReference()) {
        mm->mProperties |= METHOD_RETURN_REFERENCE;
    }
    // end address
    mBasePtr = mBasePtr + sizeof(como::MetaParameter*) * PN;

    for (int i = 0; i < PN; i++) {
        mm->mParameters[i] = WriteMetaParameter(method->GetParameter(i));
    }

    return mm;
}

como::MetaParameter* MetadataBuilder::WriteMetaParameter(
    /* [in] */ Parameter* parameter)
{
    // begin address
    mBasePtr = ALIGN(mBasePtr);
    como::MetaParameter* mp = reinterpret_cast<como::MetaParameter*>(mBasePtr);
    mp->mName = WriteString(parameter->GetName());
    mp->mProperties = 0;
    if (parameter->IsIn()) {
        mp->mProperties |= PARAMETER_IN;
    }
    if (parameter->IsOut()) {
        mp->mProperties |= PARAMETER_OUT;
    }
    if (parameter->IsCallee()) {
        mp->mProperties |= PARAMETER_CALLEE;
    }
    AutoPtr<Type> type = parameter->GetType();
    mp->mTypeIndex = mModule->IndexOf(type);
    // end address
    mBasePtr = mBasePtr + sizeof(como::MetaParameter);

    if (parameter->HasDefaultValue()) {
        mp->mProperties |= PARAMETER_VALUE_DEFAULT;
        WriteMetaValue(parameter);
    }

    return mp;
}

como::MetaValue* MetadataBuilder::WriteMetaValue(
    /* [in] */ Parameter* parameter)
{
    AutoPtr<Type> type = parameter->GetType();
    AutoPtr<Expression> value = parameter->GetDefaultValue();

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    como::MetaValue* mv = reinterpret_cast<como::MetaValue*>(mBasePtr);
    if (type->IsBooleanType()) {
        mv->mBooleanValue = value->BooleanValue();
    }
    else if (type->IsIntegralType()) {
        mv->mIntegralValue = value->LongValue();
        mv->mProperties |= (value->GetRadix() & VALUE_RADIX_MASK);
    }
    else if (type->IsFloatingPointType()) {
        mv->mFloatingPointValue = value->DoubleValue();
        if (value->IsPositiveInfinity()) {
            mv->mProperties |= VALUE_POSITIVE_INFINITY;
        }
        if (value->IsNegativeInfinity()) {
            mv->mProperties |= VALUE_NEGATIVE_INFINITY;
        }
        if (value->IsNaN()) {
            mv->mProperties |= VALUE_NAN;
        }
        if (value->IsScientificNotation()) {
            mv->mProperties |= VALUE_SCIENTIFIC_NOTATION;
        }
    }
    else if (type->IsStringType()) {
        mv->mStringValue = WriteString(value->StringValue());
    }
    else if (type->IsEnumerationType()) {
        mv->mStringValue = WriteString(value->EnumeratorValue());
    }
    else if (type->IsPointerType()) {
        mv->mIntegralValue = value->LongValue();
    }

    // end address
    mBasePtr = mBasePtr + sizeof(como::MetaValue);

    return mv;
}

como::MetaType* MetadataBuilder::WriteMetaType(
    /* [in] */ Type* type)
{
    Type* self = type;

    // begin address
    mBasePtr = ALIGN(mBasePtr);
    como::MetaType* mt = reinterpret_cast<como::MetaType*>(mBasePtr);
    mt->mProperties = 0;
    if (type->IsReferenceType()) {
        mt->mProperties |= TYPE_REFERENCE;
        type = ReferenceType::CastFrom(type)->GetBaseType();
    }
    if (type->IsPointerType()) {
        mt->mProperties |= TYPE_POINTER;
        mt->mProperties |= ((PointerType::CastFrom(type)->GetPointerNumber() << 2)
                & TYPE_POINTER_NUMBER_MASK);
        type = PointerType::CastFrom(type)->GetBaseType();
    }
    mt->mKind = ToTypeKind(type);
    mt->mIndex = 0;
    if (type->IsEnumerationType()) {
        mt->mIndex = mModule->IndexOf(EnumerationType::CastFrom(type));
    }
    else if (type->IsInterfaceType()) {
        mt->mIndex = mModule->IndexOf(InterfaceType::CastFrom(type));
    }
    else if (type->IsCoclassType()) {
        mt->mIndex = mModule->IndexOf(CoclassType::CastFrom(type));
    }
    else if (type->IsArrayType()) {
        mt->mIndex = mModule->IndexOf(ArrayType::CastFrom(type)->GetElementType());
    }
    // end address
    mBasePtr = mBasePtr + sizeof(como::MetaType);

    if (self->GetExternalModule() != nullptr) {
        mt->mProperties |= TYPE_EXTERNAL;
        mBasePtr = ALIGN(mBasePtr);
        *(char**)mBasePtr = WriteString(self->GetExternalModule()->GetName());
        mBasePtr = mBasePtr + sizeof(char**);
    }

    return mt;
}

char* MetadataBuilder::WriteString(
    /* [in] */ const String& string)
{
    return mMetaComponent->mStringPool + mPool.FindOffset(string);
}

void MetadataBuilder::AssignFrom(
    /* [out]*/ como::UUID& target,
    /* [in] */ cdlc::UUID* src)
{
    target.mData1 = src->mData1;
    target.mData2 = src->mData2;
    target.mData3 = src->mData3;
    target.mData4 = src->mData4;
    memcpy(target.mData5, src->mData5, 12);
}

como::TypeKind MetadataBuilder::ToTypeKind(
    /* [in] */ Type* type)
{
    if (type->IsBooleanType()) {
        return como::TypeKind::Boolean;
    }
    else if (type->IsCharType()) {
        return como::TypeKind::Char;
    }
    else if (type->IsByteType()) {
        return como::TypeKind::Byte;
    }
    else if (type->IsShortType()) {
        return como::TypeKind::Short;
    }
    else if (type->IsIntegerType()) {
        return como::TypeKind::Integer;
    }
    else if (type->IsLongType()) {
        return como::TypeKind::Long;
    }
    else if (type->IsFloatType()) {
        return como::TypeKind::Float;
    }
    else if (type->IsDoubleType()) {
        return como::TypeKind::Double;
    }
    else if (type->IsStringType()) {
        return como::TypeKind::String;
    }
    else if (type->IsCoclassIDType()) {
        return como::TypeKind::CoclassID;
    }
    else if (type->IsComponentIDType()){
        return como::TypeKind::ComponentID;
    }
    else if (type->IsInterfaceIDType()) {
        return como::TypeKind::InterfaceID;
    }
    else if (type->IsHANDLEType()) {
        return como::TypeKind::HANDLE;
    }
    else if (type->IsECodeType()) {
        return como::TypeKind::ECode;
    }
    else if (type->IsEnumerationType()) {
        return como::TypeKind::Enum;
    }
    else if (type->IsInterfaceType()) {
        return como::TypeKind::Interface;
    }
    else if (type->IsCoclassType()) {
        return como::TypeKind::Coclass;
    }
    else if (type->IsArrayType()) {
        return como::TypeKind::Array;
    }
    else if (type->IsTripleType()) {
        return como::TypeKind::Triple;
    }
    return como::TypeKind::Unknown;
}

}
