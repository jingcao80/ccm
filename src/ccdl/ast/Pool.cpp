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

#include "ArrayType.h"
#include "PointerType.h"
#include "Pool.h"
#include "../util/StringBuilder.h"

namespace ccdl {
namespace ast {

Pool::Pool()
    : mCoclasses(20)
    , mConstants(10)
    , mEnumerations(10)
    , mInterfacePredecls(20, false)
    , mInterfaces(20)
    , mNamespaces(5)
    , mTempTypes(20)
    , mTypes(6000)
{}

Pool::~Pool()
{
    for (int i = 0; i < mInterfacePredecls.GetSize(); i++) {
        Interface* intf = mInterfacePredecls.Get(i);
        if (intf->IsPredecl()) {
            delete intf;
        }
    }
}

bool Pool::AddEnumeration(
    /* [in] */ Enumeration* enumeration)
{
    if (enumeration == nullptr) return true;

    if (!mEnumerations.Add(enumeration)) return false;
    mTypes.Put(enumeration->ToString(), enumeration);
    enumeration->SetPool(this);
    return true;
}

Enumeration* Pool::FindEnumeration(
    /* [in] */ const String& enumFullName)
{
    Type* type = FindType(enumFullName);
    if (type == nullptr || !type->IsEnumerationType()) {
        return nullptr;
    }
    return (Enumeration*)type;
}

bool Pool::AddInterfacePredeclaration(
    /* [in] */ Interface* interface)
{
    if (interface == nullptr) return true;

    if (!mInterfacePredecls.Add(interface)) return false;
    mTypes.Put(interface->ToString(), interface);
    return true;
}

bool Pool::AddInterface(
    /* [in] */ Interface* interface)
{
    if (interface == nullptr) return true;

    if (!mInterfaces.Add(interface)) return false;
    mTypes.Put(interface->ToString(), interface);
    interface->SetPool(this);
    return true;
}

Interface* Pool::FindInterface(
    /* [in] */ const String& itfFullName)
{
    Type* type = FindType(itfFullName);
    if (type == nullptr || !type->IsInterfaceType()) {
        return nullptr;
    }
    return (Interface*)type;
}

bool Pool::AddCoclass(
    /* [in] */ Coclass* klass)
{
    if (klass == nullptr) return true;

    if (!mCoclasses.Add(klass)) return false;
    mTypes.Put(klass->ToString(), klass);
    klass->SetPool(this);
    return true;
}

Coclass* Pool::FindClass(
    /* [in] */ const String& klassName)
{
    Type* type = FindType(klassName);
    if (type == nullptr || !type->IsCoclassType()) {
        return nullptr;
    }
    return (Coclass*)type;
}

bool Pool::AddConstant(
    /* [in] */ Constant* constant)
{
    if (constant == nullptr) return true;

    return mConstants.Add(constant);
}

Constant* Pool::FindConstant(
    /* [in] */ const String& constantName)
{
    for (int i = 0; i < mConstants.GetSize(); i++) {
        Constant* constant = mConstants.Get(i);
        if (constant->GetName().Equals(constantName)) {
            return constant;
        }
    }
    return nullptr;
}

bool Pool::AddNamespace(
    /* [in] */ Namespace* ns)
{
    if (ns == nullptr) return true;

    return mNamespaces.Add(ns);
}

Namespace* Pool::FindNamespace(
    /* [in] */ const String& nsString)
{
    for (int i = 0; i < mNamespaces.GetSize(); i++) {
        Namespace* ns = mNamespaces.Get(i);
        if (ns->ToShortString().Equals(nsString)) {
            return ns;
        }
    }
    return nullptr;
}

Namespace* Pool::ParseNamespace(
    /* [in] */ const String& nsString)
{
    String nss = nsString;
    Namespace* nsp = nullptr, *headNsp = nullptr, *currNsp = nullptr;
    int cIndex;
    while ((cIndex = nss.IndexOf("::")) != -1) {
        String ns = nss.Substring(0, cIndex - 1);
        if (currNsp == nullptr) {
            currNsp = FindNamespace(String("__global__"));
            nsp = currNsp->FindNamespace(ns);
            if (nsp == nullptr) {
                nsp = new Namespace(ns);
                AddNamespace(nsp);
                currNsp->AddNamespace(nsp);
            }
            currNsp = headNsp = nsp;
        }
        else {
            nsp = currNsp->FindNamespace(ns);
            if (nsp == nullptr) {
                nsp = new Namespace(ns);
                AddNamespace(nsp);
                currNsp->AddNamespace(nsp);
            }
            currNsp = nsp;
        }
        if (cIndex + 2 >= nss.GetLength()) {
            return currNsp;
        }
        nss = nss.Substring(cIndex + 2);
    }
    if (currNsp == nullptr) {
        currNsp = FindNamespace(String("__global__"));
        nsp = currNsp->FindNamespace(nss);
        if (nsp == nullptr) {
            nsp = new Namespace(nss);
            AddNamespace(nsp);
            currNsp->AddNamespace(nsp);
        }
        headNsp = nsp;
    }
    else {
        nsp = currNsp->FindNamespace(nss);
        if (nsp == nullptr) {
            nsp = new Namespace(nss);
            AddNamespace(nsp);
            currNsp->AddNamespace(nsp);
        }
        headNsp = nsp;
    }
    return headNsp;
}

bool Pool::AddTemporaryType(
    /* [in] */ Type* type)
{
    if (type == nullptr) return true;

    if (!mTempTypes.Add(type)) return false;
    mTypes.Put(type->ToString(), type);
    type->SetPool(this);
    return true;
}

Type* Pool::GetType(
    /* [in] */ int index)
{
    std::shared_ptr< ArrayList<StringMap<Type*>::Pair*> > types =
            GetTypes();
    return types->Get(index)->mValue;
}

Type* Pool::FindType(
    /* [in] */ const String& typeName)
{
    if (typeName.IsNullOrEmpty()) return nullptr;
    Type* type = mTypes.Get(typeName);
    if (type != nullptr) return type;

    int index = typeName.LastIndexOf("::");
    String nsStr = index == -1 ? String("__global__") : typeName.Substring(0, index - 1);
    Namespace* ns = FindNamespace(nsStr);
    if (ns != nullptr && !ns->IsResolved()) {
        type = ResolveType(typeName);
        if (type != nullptr) type->SetNamespace(ns);
    }
    return type;
}

int Pool::IndexOf(
    /* [in] */ Type* type)
{
    std::shared_ptr< ArrayList<StringMap<Type*>::Pair*> > types =
            GetTypes();
    for (int i = 0; i < types->GetSize(); i++) {
        StringMap<Type*>::Pair* p = types->Get(i);
        if (p->mValue == type) return i;
    }
    return -1;
}

Type* Pool::DeepCopyType(
    /* [in] */ Type* type)
{
    if (type->IsEnumerationType()) {
        Enumeration* desEnumn = new Enumeration();
        desEnumn->SetExternal(true);
        desEnumn->SetSpecialized(true);
        desEnumn->DeepCopy((Enumeration*)type, this);
        return (Type*)desEnumn;
    }
    else if (type->IsInterfaceType()) {
        Interface* desIntf = new Interface();
        desIntf->SetExternal(true);
        desIntf->SetSpecialized(true);
        desIntf->DeepCopy((Interface*)type, this);
        return (Type*)desIntf;
    }
    else if (type->IsPrimitiveType()) {
        String typeStr = type->ToString();
        return FindType(typeStr);
    }
    else if (type->IsArrayType()) {
        ArrayType* sourceArrType = (ArrayType*)type;
        Type* sourceElemType = sourceArrType->GetElementType();
        Type* newElemType = FindType(sourceElemType->ToString());
        if (newElemType == nullptr) {
            newElemType = DeepCopyType(sourceElemType);
        }
        ArrayType* arrType = new ArrayType();
        arrType->SetElementType(newElemType);
        AddTemporaryType(arrType);
        return arrType;
    }
    else if (type->IsPointerType()) {
        PointerType* sourcePtrType = (PointerType*)type;
        Type* sourceBaseType = sourcePtrType->GetBaseType();
        Type* newBaseType = FindType(sourceBaseType->ToString());
        if (newBaseType == nullptr) {
            newBaseType = DeepCopyType(sourceBaseType);
        }
        PointerType* ptrType = new PointerType();
        ptrType->SetBaseType(newBaseType);
        ptrType->SetPointerNumber(sourcePtrType->GetPointerNumber());
        AddTemporaryType(ptrType);
        return ptrType;
    }

    return nullptr;
}

Type* Pool::ShallowCopyType(
    /* [in] */ Type* type)
{
    if (type->IsEnumerationType()) {
        Enumeration* desEnumn = new Enumeration();
        desEnumn->ShallowCopy((Enumeration*)type, this);
        desEnumn->SetExternal(true);
        return desEnumn;
    }
    else if (type->IsInterfaceType()) {
        Interface* desIntf = new Interface();
        desIntf->ShallowCopy((Interface*)type, this);
        desIntf->SetExternal(true);
        return desIntf;
    }
    else if (type->IsPrimitiveType()) {
        String typeStr = type->ToString();
        return FindType(typeStr);
    }
    else if (type->IsArrayType()) {
        ArrayType* sourceArrType = (ArrayType*)type;
        Type* sourceElemType = sourceArrType->GetElementType();
        Type* newElemType = FindType(sourceElemType->ToString());
        if (newElemType == nullptr) {
            newElemType = ShallowCopyType(sourceElemType);
        }
        ArrayType* arrType = new ArrayType();
        arrType->SetElementType(newElemType);
        AddTemporaryType(arrType);
        return arrType;
    }
    else if (type->IsPointerType()) {
        PointerType* sourcePtrType = (PointerType*)type;
        Type* sourceBaseType = sourcePtrType->GetBaseType();
        Type* newBaseType = FindType(sourceBaseType->ToString());
        if (newBaseType == nullptr) {
            newBaseType = ShallowCopyType(sourceBaseType);
        }
        PointerType* ptrType = new PointerType();
        ptrType->SetBaseType(newBaseType);
        ptrType->SetPointerNumber(sourcePtrType->GetPointerNumber());
        AddTemporaryType(ptrType);
        return ptrType;
    }

    return nullptr;
}

String Pool::Dump(
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    for (int i = 0; i < mEnumerations.GetSize(); i++) {
        String enumStr = mEnumerations.Get(i)->Dump(String("  "));
        builder.Append(enumStr).Append("\n");
    }
    for (int i = 0; i < mInterfaces.GetSize(); i++) {
        if (mInterfaces.Get(i)->IsPredecl()) continue;
        String itfStr = mInterfaces.Get(i)->Dump(String("  "));
        builder.Append(itfStr).Append("\n");
    }
    for (int i = 0; i < mCoclasses.GetSize(); i++) {
        String coclassStr = mCoclasses.Get(i)->Dump(String("  "));
        builder.Append(coclassStr).Append("\n");
    }

    return builder.ToString();
}

}
}
