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

#include "MetaSerializer.h"

namespace ccm {
namespace metadata {

MetaSerializer::MetaSerializer()
    : mMetaComponent(nullptr)
    , mBasePtr(0)
    , mDataSize(0)
{}

MetaSerializer::MetaSerializer(
    /* [in] */ MetaComponent* mc)
    : mMetaComponent(mc)
    , mBasePtr(reinterpret_cast<uintptr_t>(mc))
    , mDataSize(mc->mSize)
{}

void MetaSerializer::Serialize()
{
    MetaComponent* mc = mMetaComponent;

    mc->mName = (char*)SerializeAdjust(mc->mName);
    mc->mUrl = (char*)SerializeAdjust(mc->mUrl);

    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        MetaNamespace* mn = mc->mNamespaces[i];
        SerializeMetaNamespace(mn);
        mc->mNamespaces[i] = (MetaNamespace*)SerializeAdjust(mn);
    }
    mc->mNamespaces = (MetaNamespace**)SerializeAdjust(mc->mNamespaces);

    for (int i = 0; i < mc->mConstantNumber; i++) {
        MetaConstant* mk = mc->mConstants[i];
        SerializeMetaConstant(mk);
        mc->mConstants[i] = (MetaConstant*)SerializeAdjust(mk);
    }
    mc->mConstants = (MetaConstant**)SerializeAdjust(mc->mConstants);

    for (int i = 0; i < mc->mCoclassNumber; i++) {
        MetaCoclass* mk = mc->mCoclasses[i];
        SerializeMetaCoclass(mk);
        mc->mCoclasses[i] = (MetaCoclass*)SerializeAdjust(mk);
    }
    mc->mCoclasses = (MetaCoclass**)SerializeAdjust(mc->mCoclasses);

    for (int i = 0; i < mc->mEnumerationNumber; i++) {
        MetaEnumeration* me = mc->mEnumerations[i];
        SerializeMetaEnumeration(me);
        mc->mEnumerations[i] = (MetaEnumeration*)SerializeAdjust(me);
    }
    mc->mEnumerations = (MetaEnumeration**)SerializeAdjust(mc->mEnumerations);

    for (int i = 0; i < mc->mInterfaceNumber; i++) {
        MetaInterface* mi = mc->mInterfaces[i];
        SerializeMetaInterface(mi);
        mc->mInterfaces[i] = (MetaInterface*)SerializeAdjust(mi);
    }
    mc->mInterfaces = (MetaInterface**)SerializeAdjust(mc->mInterfaces);

    for (int i = 0; i < mc->mTypeNumber; i++) {
        MetaType* mt = mc->mTypes[i];
        SerializeMetaType(mt);
        mc->mTypes[i] = (MetaType*)SerializeAdjust(mt);
    }
    mc->mTypes = (MetaType**)SerializeAdjust(mc->mTypes);

    mc->mStringPool = (char*)SerializeAdjust(mc->mStringPool);
}

void MetaSerializer::SerializeMetaCoclass(
    /* [in] */ MetaCoclass* mc)
{
    mc->mName = (char*)SerializeAdjust(mc->mName);
    mc->mNamespace = (char*)SerializeAdjust(mc->mNamespace);
    mc->mInterfaceIndexes = (int*)SerializeAdjust(mc->mInterfaceIndexes);
}

void MetaSerializer::SerializeMetaConstant(
    /* [in] */ MetaConstant* mc)
{
    mc->mName = (char*)SerializeAdjust(mc->mName);
    if (mMetaComponent->mTypes[mc->mTypeIndex]->mKind == CcmTypeKind::String) {
        mc->mValue.mString = (char*)SerializeAdjust(mc->mValue.mString);
    }
}

void MetaSerializer::SerializeMetaEnumeration(
    /* [in] */ MetaEnumeration* me)
{
    me->mName = (char*)SerializeAdjust(me->mName);
    me->mNamespace = (char*)SerializeAdjust(me->mNamespace);
    for (int i = 0; i < me->mEnumeratorNumber; i++) {
        MetaEnumerator* mr = me->mEnumerators[i];
        SerializeMetaEnumerator(mr);
        me->mEnumerators[i] = (MetaEnumerator*)SerializeAdjust(mr);
    }
    me->mEnumerators = (MetaEnumerator**)SerializeAdjust(me->mEnumerators);
}

void MetaSerializer::SerializeMetaEnumerator(
    /* [in] */ MetaEnumerator* me)
{
    me->mName = (char*)SerializeAdjust(me->mName);
}

void MetaSerializer::SerializeMetaInterface(
    /* [in] */ MetaInterface* mi)
{
    mi->mName = (char*)SerializeAdjust(mi->mName);
    mi->mNamespace = (char*)SerializeAdjust(mi->mNamespace);
    mi->mNestedInterfaceIndexes = (int*)SerializeAdjust(mi->mNestedInterfaceIndexes);

    for (int i = 0; i < mi->mConstantNumber; i++) {
        MetaConstant* mc = mi->mConstants[i];
        SerializeMetaConstant(mc);
        mi->mConstants[i] = (MetaConstant*)SerializeAdjust(mc);
    }
    mi->mConstants = (MetaConstant**)SerializeAdjust(mi->mConstants);

    for (int i = 0; i < mi->mMethodNumber; i++) {
        MetaMethod* mm = mi->mMethods[i];
        SerializeMetaMethod(mm);
        mi->mMethods[i] = (MetaMethod*)SerializeAdjust(mm);
    }
    mi->mMethods = (MetaMethod**)SerializeAdjust(mi->mMethods);
}

void MetaSerializer::SerializeMetaMethod(
    /* [in] */ MetaMethod* mm)
{
    mm->mName = (char*)SerializeAdjust(mm->mName);
    mm->mSignature = (char*)SerializeAdjust(mm->mSignature);

    for (int i = 0; i < mm->mParameterNumber; i++) {
        MetaParameter* mp = mm->mParameters[i];
        SerializeMetaParameter(mp);
        mm->mParameters[i] = (MetaParameter*)SerializeAdjust(mp);
    }
    mm->mParameters = (MetaParameter**)SerializeAdjust(mm->mParameters);
}

void MetaSerializer::SerializeMetaNamespace(
    /* [in] */ MetaNamespace* mn)
{
    mn->mName = (char*)SerializeAdjust(mn->mName);
    mn->mConstantIndexes = (int*)SerializeAdjust(mn->mConstantIndexes);
    mn->mCoclassIndexes = (int*)SerializeAdjust(mn->mCoclassIndexes);
    mn->mEnumerationIndexes = (int*)SerializeAdjust(mn->mEnumerationIndexes);
    mn->mInterfaceIndexes = (int*)SerializeAdjust(mn->mInterfaceIndexes);
}

void MetaSerializer::SerializeMetaParameter(
    /* [in] */ MetaParameter* mp)
{
    mp->mName = (char*)SerializeAdjust(mp->mName);
}

void MetaSerializer::SerializeMetaType(
    /* [in] */ MetaType* mt)
{}

ptrdiff_t MetaSerializer::SerializeAdjust(
    /* [in] */ void* addr)
{
    return reinterpret_cast<uintptr_t>(addr) - mBasePtr;
}

void MetaSerializer::Deserialize(
    /* [in] */ uintptr_t basePtr)
{
    mBasePtr = basePtr;

    MetaComponent* mc = reinterpret_cast<MetaComponent*>(mBasePtr);
    mMetaComponent = mc;
    mc->mName = (char*)DeserializeAdjust(mc->mName);
    mc->mUrl = (char*)DeserializeAdjust(mc->mUrl);

    mc->mTypes = (MetaType**)DeserializeAdjust(mc->mTypes);
    for (int i = 0; i < mc->mTypeNumber; i++) {
        mc->mTypes[i] = (MetaType*)DeserializeAdjust(mc->mTypes[i]);
        MetaType* mt = mc->mTypes[i];
        DeserializeMetaType(mt);
    }

    mc->mNamespaces = (MetaNamespace**)DeserializeAdjust(mc->mNamespaces);
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        mc->mNamespaces[i] = (MetaNamespace*)DeserializeAdjust(mc->mNamespaces[i]);
        MetaNamespace* mn = mc->mNamespaces[i];
        DeserializeMetaNamespace(mn);
    }

    mc->mConstants = (MetaConstant**)DeserializeAdjust(mc->mConstants);
    for (int i = 0; i < mc->mConstantNumber; i++) {
        mc->mConstants[i] = (MetaConstant*)DeserializeAdjust(mc->mConstants[i]);
        MetaConstant* mk = mc->mConstants[i];
        DeserializeMetaConstant(mk);
    }

    mc->mCoclasses = (MetaCoclass**)DeserializeAdjust(mc->mCoclasses);
    for (int i = 0; i < mc->mCoclassNumber; i++) {
        mc->mCoclasses[i] = (MetaCoclass*)DeserializeAdjust(mc->mCoclasses[i]);
        MetaCoclass* mk = mc->mCoclasses[i];
        DeserializeMetaCoclass(mk);
    }

    mc->mEnumerations = (MetaEnumeration**)DeserializeAdjust(mc->mEnumerations);
    for (int i = 0; i < mc->mEnumerationNumber; i++) {
        mc->mEnumerations[i] = (MetaEnumeration*)DeserializeAdjust(mc->mEnumerations[i]);
        MetaEnumeration* me = mc->mEnumerations[i];
        DeserializeMetaEnumeration(me);
    }

    mc->mInterfaces = (MetaInterface**)DeserializeAdjust(mc->mInterfaces);
    for (int i = 0; i < mc->mInterfaceNumber; i++) {
        mc->mInterfaces[i] = (MetaInterface*)DeserializeAdjust(mc->mInterfaces[i]);
        MetaInterface* mi = mc->mInterfaces[i];
        DeserializeMetaInterface(mi);
    }
}

void MetaSerializer::DeserializeMetaCoclass(
    /* [in] */ MetaCoclass* mc)
{
    mc->mName = (char*)DeserializeAdjust(mc->mName);
    mc->mNamespace = (char*)DeserializeAdjust(mc->mNamespace);
    mc->mInterfaceIndexes = (int*)DeserializeAdjust(mc->mInterfaceIndexes);
}

void MetaSerializer::DeserializeMetaConstant(
    /* [in] */ MetaConstant* mc)
{
    mc->mName = (char*)DeserializeAdjust(mc->mName);
    if (mMetaComponent->mTypes[mc->mTypeIndex]->mKind == CcmTypeKind::String) {
        mc->mValue.mString = (char*)DeserializeAdjust(mc->mValue.mString);
    }
}

void MetaSerializer::DeserializeMetaEnumeration(
    /* [in] */ MetaEnumeration* me)
{
    me->mName = (char*)DeserializeAdjust(me->mName);
    me->mNamespace = (char*)DeserializeAdjust(me->mNamespace);

    me->mEnumerators = (MetaEnumerator**)DeserializeAdjust(me->mEnumerators);
    for (int i = 0; i < me->mEnumeratorNumber; i++) {
        me->mEnumerators[i] = (MetaEnumerator*)DeserializeAdjust(me->mEnumerators[i]);
        MetaEnumerator* mr = me->mEnumerators[i];
        DeserializeMetaEnumerator(mr);
    }
}

void MetaSerializer::DeserializeMetaEnumerator(
    /* [in] */ MetaEnumerator* mr)
{
    mr->mName = (char*)DeserializeAdjust(mr->mName);
}

void MetaSerializer::DeserializeMetaInterface(
    /* [in] */ MetaInterface* mi)
{
    mi->mName = (char*)DeserializeAdjust(mi->mName);
    mi->mNamespace = (char*)DeserializeAdjust(mi->mNamespace);
    mi->mNestedInterfaceIndexes =(int*)DeserializeAdjust(mi->mNestedInterfaceIndexes);

    mi->mConstants = (MetaConstant**)DeserializeAdjust(mi->mConstants);
    for (int i = 0; i < mi->mConstantNumber; i++) {
        mi->mConstants[i] = (MetaConstant*)DeserializeAdjust(mi->mConstants[i]);
        MetaConstant* mc = mi->mConstants[i];
        DeserializeMetaConstant(mc);
    }

    mi->mMethods = (MetaMethod**)DeserializeAdjust(mi->mMethods);
    for (int i = 0; i < mi->mMethodNumber; i++) {
        mi->mMethods[i] = (MetaMethod*)DeserializeAdjust(mi->mMethods[i]);
        MetaMethod* mm = mi->mMethods[i];
        DeserializeMetaMethod(mm);
    }
}

void MetaSerializer::DeserializeMetaMethod(
    /* [in] */ MetaMethod* mm)
{
    mm->mName = (char*)DeserializeAdjust(mm->mName);
    mm->mSignature = (char*)DeserializeAdjust(mm->mSignature);

    mm->mParameters = (MetaParameter**)DeserializeAdjust(mm->mParameters);
    for (int i = 0; i < mm->mParameterNumber; i++) {
        mm->mParameters[i] = (MetaParameter*)DeserializeAdjust(mm->mParameters[i]);
        MetaParameter* mp = mm->mParameters[i];
        DeserializeMetaParameter(mp);
    }
}

void MetaSerializer::DeserializeMetaNamespace(
    /* [in] */ MetaNamespace* mn)
{
    mn->mName = (char*)DeserializeAdjust(mn->mName);
    mn->mConstantIndexes = (int*)DeserializeAdjust(mn->mConstantIndexes);
    mn->mCoclassIndexes = (int*)DeserializeAdjust(mn->mCoclassIndexes);
    mn->mEnumerationIndexes = (int*)DeserializeAdjust(mn->mEnumerationIndexes);
    mn->mInterfaceIndexes = (int*)DeserializeAdjust(mn->mInterfaceIndexes);
}

void MetaSerializer::DeserializeMetaParameter(
    /* [in] */ MetaParameter* mp)
{
    mp->mName = (char*)DeserializeAdjust(mp->mName);
}

void MetaSerializer::DeserializeMetaType(
    /* [in] */ MetaType* mt)
{}

uintptr_t MetaSerializer::DeserializeAdjust(
    /* [in] */ void* addr)
{
    return reinterpret_cast<ptrdiff_t>(addr) + mBasePtr;
}

}
}
