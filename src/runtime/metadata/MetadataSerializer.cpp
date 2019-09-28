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

#include "MetadataSerializer.h"
#include "comodef.h"

namespace como {
namespace metadata {

MetadataSerializer::MetadataSerializer(
    /* [in] */ MetaComponent* mc)
    : mComponent(mc)
    , mBasePtr(reinterpret_cast<uintptr_t>(mc))
    , mMetadataSize(mc->mSize)
{}

void MetadataSerializer::Serialize()
{
    MetaComponent* mc = mComponent;

    mc->mName = reinterpret_cast<char*>(SerializeAdjust(mc->mName));
    mc->mUri = reinterpret_cast<char*>(SerializeAdjust(mc->mUri));

    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        MetaNamespace* mn = mc->mNamespaces[i];
        SerializeMetaNamespace(mn);
        mc->mNamespaces[i] = reinterpret_cast<MetaNamespace*>(SerializeAdjust(mn));
    }
    mc->mNamespaces = reinterpret_cast<MetaNamespace**>(SerializeAdjust(mc->mNamespaces));

    for (int i = 0; i < mc->mConstantNumber; i++) {
        MetaConstant* mk = mc->mConstants[i];
        SerializeMetaConstant(mk);
        mc->mConstants[i] = reinterpret_cast<MetaConstant*>(SerializeAdjust(mk));
    }
    mc->mConstants = reinterpret_cast<MetaConstant**>(SerializeAdjust(mc->mConstants));

    for (int i = 0; i < mc->mCoclassNumber; i++) {
        MetaCoclass* mk = mc->mCoclasses[i];
        SerializeMetaCoclass(mk);
        mc->mCoclasses[i] = reinterpret_cast<MetaCoclass*>(SerializeAdjust(mk));
    }
    mc->mCoclasses = reinterpret_cast<MetaCoclass**>(SerializeAdjust(mc->mCoclasses));

    for (int i = 0; i < mc->mEnumerationNumber; i++) {
        MetaEnumeration* me = mc->mEnumerations[i];
        SerializeMetaEnumeration(me);
        mc->mEnumerations[i] = reinterpret_cast<MetaEnumeration*>(SerializeAdjust(me));
    }
    mc->mEnumerations = reinterpret_cast<MetaEnumeration**>(SerializeAdjust(mc->mEnumerations));

    for (int i = 0; i < mc->mInterfaceNumber; i++) {
        MetaInterface* mi = mc->mInterfaces[i];
        SerializeMetaInterface(mi);
        mc->mInterfaces[i] = reinterpret_cast<MetaInterface*>(SerializeAdjust(mi));
    }
    mc->mInterfaces = reinterpret_cast<MetaInterface**>(SerializeAdjust(mc->mInterfaces));

    for (int i = 0; i < mc->mTypeNumber; i++) {
        MetaType* mt = mc->mTypes[i];
        SerializeMetaType(mt);
        mc->mTypes[i] = reinterpret_cast<MetaType*>(SerializeAdjust(mt));
    }
    mc->mTypes = reinterpret_cast<MetaType**>(SerializeAdjust(mc->mTypes));

    mc->mStringPool = reinterpret_cast<char*>(SerializeAdjust(mc->mStringPool));
}

void MetadataSerializer::SerializeMetaNamespace(
    /* [in] */ MetaNamespace* mn)
{
    mn->mName = reinterpret_cast<char*>(SerializeAdjust(mn->mName));

    for (int i = 0; i < mn->mNamespaceNumber; i++) {
        MetaNamespace* innerMn = mn->mNamespaces[i];
        SerializeMetaNamespace(innerMn);
        mn->mNamespaces[i] = reinterpret_cast<MetaNamespace*>(SerializeAdjust(innerMn));
    }
    mn->mNamespaces = reinterpret_cast<MetaNamespace**>(SerializeAdjust(mn->mNamespaces));

    mn->mConstantIndexes = reinterpret_cast<int*>(SerializeAdjust(mn->mConstantIndexes));
    mn->mCoclassIndexes = reinterpret_cast<int*>(SerializeAdjust(mn->mCoclassIndexes));
    mn->mEnumerationIndexes = reinterpret_cast<int*>(SerializeAdjust(mn->mEnumerationIndexes));
    mn->mInterfaceIndexes = reinterpret_cast<int*>(SerializeAdjust(mn->mInterfaceIndexes));
}

void MetadataSerializer::SerializeMetaConstant(
    /* [in] */ MetaConstant* mc)
{
    mc->mName = reinterpret_cast<char*>(SerializeAdjust(mc->mName));
    TypeKind kind = mComponent->mTypes[mc->mTypeIndex]->mKind;
    if (kind == TypeKind::String || kind == TypeKind::Enum) {
        mc->mValue.mStringValue = reinterpret_cast<char*>(SerializeAdjust(mc->mValue.mStringValue));
    }
}

void MetadataSerializer::SerializeMetaCoclass(
    /* [in] */ MetaCoclass* mc)
{
    mc->mName = reinterpret_cast<char*>(SerializeAdjust(mc->mName));
    mc->mNamespace = reinterpret_cast<char*>(SerializeAdjust(mc->mNamespace));
    mc->mInterfaceIndexes = reinterpret_cast<int*>(SerializeAdjust(mc->mInterfaceIndexes));
}

void MetadataSerializer::SerializeMetaEnumeration(
    /* [in] */ MetaEnumeration* me)
{
    me->mName = reinterpret_cast<char*>(SerializeAdjust(me->mName));
    me->mNamespace = reinterpret_cast<char*>(SerializeAdjust(me->mNamespace));
    for (int i = 0; i < me->mEnumeratorNumber; i++) {
        MetaEnumerator* mr = me->mEnumerators[i];
        SerializeMetaEnumerator(mr);
        me->mEnumerators[i] = reinterpret_cast<MetaEnumerator*>(SerializeAdjust(mr));
    }
    me->mEnumerators = reinterpret_cast<MetaEnumerator**>(SerializeAdjust(me->mEnumerators));
}

void MetadataSerializer::SerializeMetaEnumerator(
    /* [in] */ MetaEnumerator* me)
{
    me->mName = reinterpret_cast<char*>(SerializeAdjust(me->mName));
}

void MetadataSerializer::SerializeMetaInterface(
    /* [in] */ MetaInterface* mi)
{
    mi->mName = reinterpret_cast<char*>(SerializeAdjust(mi->mName));
    mi->mNamespace = reinterpret_cast<char*>(SerializeAdjust(mi->mNamespace));
    mi->mNestedInterfaceIndexes = reinterpret_cast<int*>(SerializeAdjust(mi->mNestedInterfaceIndexes));

    for (int i = 0; i < mi->mConstantNumber; i++) {
        MetaConstant* mc = mi->mConstants[i];
        SerializeMetaConstant(mc);
        mi->mConstants[i] = reinterpret_cast<MetaConstant*>(SerializeAdjust(mc));
    }
    mi->mConstants = reinterpret_cast<MetaConstant**>(SerializeAdjust(mi->mConstants));

    for (int i = 0; i < mi->mMethodNumber; i++) {
        MetaMethod* mm = mi->mMethods[i];
        SerializeMetaMethod(mm);
        mi->mMethods[i] = reinterpret_cast<MetaMethod*>(SerializeAdjust(mm));
    }
    mi->mMethods = reinterpret_cast<MetaMethod**>(SerializeAdjust(mi->mMethods));
}

void MetadataSerializer::SerializeMetaMethod(
    /* [in] */ MetaMethod* mm)
{
    mm->mName = reinterpret_cast<char*>(SerializeAdjust(mm->mName));
    mm->mSignature = reinterpret_cast<char*>(SerializeAdjust(mm->mSignature));

    for (int i = 0; i < mm->mParameterNumber; i++) {
        MetaParameter* mp = mm->mParameters[i];
        SerializeMetaParameter(mp);
        mm->mParameters[i] = reinterpret_cast<MetaParameter*>(SerializeAdjust(mp));
    }
    mm->mParameters = reinterpret_cast<MetaParameter**>(SerializeAdjust(mm->mParameters));
}

void MetadataSerializer::SerializeMetaParameter(
    /* [in] */ MetaParameter* mp)
{
    mp->mName = reinterpret_cast<char*>(SerializeAdjust(mp->mName));
    if (mp->mProperties & PARAMETER_VALUE_DEFAULT) {
        MetaType* mt = mComponent->mTypes[mp->mTypeIndex];
        MetaValue* mv = reinterpret_cast<MetaValue*>(ALIGN((uintptr_t)mp + sizeof(MetaParameter)));
        SerializeMetaValue(mt, mv);
    }
}

void MetadataSerializer::SerializeMetaValue(
    /* [in] */ MetaType* mt,
    /* [in] */ MetaValue* mv)
{
    if (mt->mKind == TypeKind::String || mt->mKind == TypeKind::Enum) {
        mv->mStringValue = reinterpret_cast<char*>(SerializeAdjust(mv->mStringValue));
    }
}

void MetadataSerializer::SerializeMetaType(
    /* [in] */ MetaType* mt)
{
    if (mt->mProperties & TYPE_EXTERNAL) {
        char** externalPtr = reinterpret_cast<char**>(ALIGN((uintptr_t)mt + sizeof(como::MetaType)));
        *externalPtr = reinterpret_cast<char*>(SerializeAdjust(*externalPtr));
    }
}

ptrdiff_t MetadataSerializer::SerializeAdjust(
    /* [in] */ void* addr)
{
    return reinterpret_cast<uintptr_t>(addr) - mBasePtr;
}

void MetadataSerializer::Deserialize(
    /* [in] */ uintptr_t basePtr)
{
    mBasePtr = basePtr;

    MetaComponent* mc = reinterpret_cast<MetaComponent*>(mBasePtr);
    mComponent = mc;
    mc->mName = reinterpret_cast<char*>(DeserializeAdjust(mc->mName));
    mc->mUri = reinterpret_cast<char*>(DeserializeAdjust(mc->mUri));

    mc->mTypes = reinterpret_cast<MetaType**>(DeserializeAdjust(mc->mTypes));
    for (int i = 0; i < mc->mTypeNumber; i++) {
        mc->mTypes[i] = reinterpret_cast<MetaType*>(DeserializeAdjust(mc->mTypes[i]));
        DeserializeMetaType(mc->mTypes[i]);
    }

    mc->mNamespaces = reinterpret_cast<MetaNamespace**>(DeserializeAdjust(mc->mNamespaces));
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        mc->mNamespaces[i] = reinterpret_cast<MetaNamespace*>(DeserializeAdjust(mc->mNamespaces[i]));
        DeserializeMetaNamespace(mc->mNamespaces[i]);
    }

    mc->mConstants = reinterpret_cast<MetaConstant**>(DeserializeAdjust(mc->mConstants));
    for (int i = 0; i < mc->mConstantNumber; i++) {
        mc->mConstants[i] = reinterpret_cast<MetaConstant*>(DeserializeAdjust(mc->mConstants[i]));
        DeserializeMetaConstant(mc->mConstants[i]);
    }

    mc->mCoclasses = reinterpret_cast<MetaCoclass**>(DeserializeAdjust(mc->mCoclasses));
    for (int i = 0; i < mc->mCoclassNumber; i++) {
        mc->mCoclasses[i] = reinterpret_cast<MetaCoclass*>(DeserializeAdjust(mc->mCoclasses[i]));
        DeserializeMetaCoclass(mc->mCoclasses[i]);
    }

    mc->mEnumerations = reinterpret_cast<MetaEnumeration**>(DeserializeAdjust(mc->mEnumerations));
    for (int i = 0; i < mc->mEnumerationNumber; i++) {
        mc->mEnumerations[i] = reinterpret_cast<MetaEnumeration*>(DeserializeAdjust(mc->mEnumerations[i]));
        DeserializeMetaEnumeration(mc->mEnumerations[i]);
    }

    mc->mInterfaces = reinterpret_cast<MetaInterface**>(DeserializeAdjust(mc->mInterfaces));
    for (int i = 0; i < mc->mInterfaceNumber; i++) {
        mc->mInterfaces[i] = reinterpret_cast<MetaInterface*>(DeserializeAdjust(mc->mInterfaces[i]));
        DeserializeMetaInterface(mc->mInterfaces[i]);
    }

    mc->mStringPool = reinterpret_cast<char*>(DeserializeAdjust(mc->mStringPool));
}

void MetadataSerializer::DeserializeMetaNamespace(
    /* [in] */ MetaNamespace* mn)
{
    mn->mName = reinterpret_cast<char*>(DeserializeAdjust(mn->mName));

    mn->mNamespaces = reinterpret_cast<MetaNamespace**>(DeserializeAdjust(mn->mNamespaces));
    for (int i = 0; i < mn->mNamespaceNumber; i++) {
        mn->mNamespaces[i] = reinterpret_cast<MetaNamespace*>(DeserializeAdjust(mn->mNamespaces[i]));
        DeserializeMetaNamespace(mn->mNamespaces[i]);
    }

    mn->mConstantIndexes = reinterpret_cast<int*>(DeserializeAdjust(mn->mConstantIndexes));
    mn->mCoclassIndexes = reinterpret_cast<int*>(DeserializeAdjust(mn->mCoclassIndexes));
    mn->mEnumerationIndexes = reinterpret_cast<int*>(DeserializeAdjust(mn->mEnumerationIndexes));
    mn->mInterfaceIndexes = reinterpret_cast<int*>(DeserializeAdjust(mn->mInterfaceIndexes));
}

void MetadataSerializer::DeserializeMetaConstant(
    /* [in] */ MetaConstant* mc)
{
    mc->mName = reinterpret_cast<char*>(DeserializeAdjust(mc->mName));
    TypeKind kind = mComponent->mTypes[mc->mTypeIndex]->mKind;
    if (kind == TypeKind::String || kind == TypeKind::Enum) {
        mc->mValue.mStringValue = reinterpret_cast<char*>(DeserializeAdjust(mc->mValue.mStringValue));
    }
}

void MetadataSerializer::DeserializeMetaCoclass(
    /* [in] */ MetaCoclass* mc)
{
    mc->mName = reinterpret_cast<char*>(DeserializeAdjust(mc->mName));
    mc->mNamespace = reinterpret_cast<char*>(DeserializeAdjust(mc->mNamespace));
    mc->mInterfaceIndexes = reinterpret_cast<int*>(DeserializeAdjust(mc->mInterfaceIndexes));
}

void MetadataSerializer::DeserializeMetaEnumeration(
    /* [in] */ MetaEnumeration* me)
{
    me->mName = reinterpret_cast<char*>(DeserializeAdjust(me->mName));
    me->mNamespace = reinterpret_cast<char*>(DeserializeAdjust(me->mNamespace));

    me->mEnumerators = reinterpret_cast<MetaEnumerator**>(DeserializeAdjust(me->mEnumerators));
    for (int i = 0; i < me->mEnumeratorNumber; i++) {
        me->mEnumerators[i] = reinterpret_cast<MetaEnumerator*>(DeserializeAdjust(me->mEnumerators[i]));
        DeserializeMetaEnumerator(me->mEnumerators[i]);
    }
}

void MetadataSerializer::DeserializeMetaEnumerator(
    /* [in] */ MetaEnumerator* mr)
{
    mr->mName = reinterpret_cast<char*>(DeserializeAdjust(mr->mName));
}

void MetadataSerializer::DeserializeMetaInterface(
    /* [in] */ MetaInterface* mi)
{
    mi->mName = reinterpret_cast<char*>(DeserializeAdjust(mi->mName));
    mi->mNamespace = reinterpret_cast<char*>(DeserializeAdjust(mi->mNamespace));
    mi->mNestedInterfaceIndexes = reinterpret_cast<int*>(DeserializeAdjust(mi->mNestedInterfaceIndexes));

    mi->mConstants = reinterpret_cast<MetaConstant**>(DeserializeAdjust(mi->mConstants));
    for (int i = 0; i < mi->mConstantNumber; i++) {
        mi->mConstants[i] = reinterpret_cast<MetaConstant*>(DeserializeAdjust(mi->mConstants[i]));
        DeserializeMetaConstant(mi->mConstants[i]);
    }

    mi->mMethods = reinterpret_cast<MetaMethod**>(DeserializeAdjust(mi->mMethods));
    for (int i = 0; i < mi->mMethodNumber; i++) {
        mi->mMethods[i] = reinterpret_cast<MetaMethod*>(DeserializeAdjust(mi->mMethods[i]));
        DeserializeMetaMethod(mi->mMethods[i]);
    }
}

void MetadataSerializer::DeserializeMetaMethod(
    /* [in] */ MetaMethod* mm)
{
    mm->mName = reinterpret_cast<char*>(DeserializeAdjust(mm->mName));
    mm->mSignature = reinterpret_cast<char*>(DeserializeAdjust(mm->mSignature));

    mm->mParameters = reinterpret_cast<MetaParameter**>(DeserializeAdjust(mm->mParameters));
    for (int i = 0; i < mm->mParameterNumber; i++) {
        mm->mParameters[i] = reinterpret_cast<MetaParameter*>(DeserializeAdjust(mm->mParameters[i]));
        DeserializeMetaParameter(mm->mParameters[i]);
    }
}

void MetadataSerializer::DeserializeMetaParameter(
    /* [in] */ MetaParameter* mp)
{
    mp->mName = reinterpret_cast<char*>(DeserializeAdjust(mp->mName));
    if (mp->mProperties & PARAMETER_VALUE_DEFAULT) {
        MetaType* mt = mComponent->mTypes[mp->mTypeIndex];
        MetaValue* mv = reinterpret_cast<MetaValue*>(ALIGN((uintptr_t)mp + sizeof(MetaParameter)));
        DeserializeMetaValue(mt, mv);
    }
}

void MetadataSerializer::DeserializeMetaValue(
    /* [in] */ MetaType* mt,
    /* [in] */ MetaValue* mv)
{
    if (mt->mKind == TypeKind::String || mt->mKind == TypeKind::Enum) {
        mv->mStringValue = reinterpret_cast<char*>(DeserializeAdjust(mv->mStringValue));
    }
}

void MetadataSerializer::DeserializeMetaType(
    /* [in] */ MetaType* mt)
{
    if (mt->mProperties & TYPE_EXTERNAL) {
        char** externalPtr = reinterpret_cast<char**>(ALIGN((uintptr_t)mt + sizeof(como::MetaType)));
        *externalPtr = reinterpret_cast<char*>(DeserializeAdjust(*externalPtr));
    }
}

uintptr_t MetadataSerializer::DeserializeAdjust(
    /* [in] */ void* addr)
{
    return reinterpret_cast<ptrdiff_t>(addr) + mBasePtr;
}

}
}
