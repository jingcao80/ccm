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

#include "metadata/MetadataResolver.h"
#include "ast/ArrayType.h"
#include "ast/EnumerationType.h"
#include "ast/InterfaceType.h"
#include "ast/PointerType.h"
#include "ast/PostfixExpression.h"
#include "ast/ReferenceType.h"
#include "parser/World.h"

namespace cdlc {

AutoPtr<Type> MetadataResolver::ResolveType(
    /* [in] */ const String& typeName,
    /* [in] */ Namespace* ns,
    /* [in] */ como::MetaNamespace* mn)
{
    mResolvingTypename = ns->ToString() + "::" + typeName;

    for (int i = 0; i < mn->mEnumerationNumber; i++) {
        como::MetaEnumeration* me = mComponent->mEnumerations[
                mn->mEnumerationIndexes[i]];
        if (typeName.Equals(me->mName)) {
            if (me->mProperties & TYPE_EXTERNAL) {
                char** moduleNamePP = reinterpret_cast<char**>(
                        ALIGN((uintptr_t)me + sizeof(como::MetaEnumeration)));
                AutoPtr<Module> module = World::GetInstance()->FindModule(*moduleNamePP);
                if (module != nullptr) {
                    return module->FindType(String::Format("%s::%s", me->mNamespace, me->mName));
                }
                return nullptr;
            }
            else {
                return BuildEnumeration(ns, me);
            }
        }
    }

    for (int i = 0; i < mn->mInterfaceNumber; i++) {
        como::MetaInterface* mi = mComponent->mInterfaces[
                mn->mInterfaceIndexes[i]];
        if (typeName.Equals(mi->mName)) {
            if (mi->mProperties & TYPE_EXTERNAL) {
                char** moduleNamePP = reinterpret_cast<char**>(
                        ALIGN((uintptr_t)mi + sizeof(como::MetaInterface)));
                AutoPtr<Module> module = World::GetInstance()->FindModule(*moduleNamePP);
                if (module != nullptr) {
                    return module->FindType(String::Format("%s::%s", mi->mNamespace, mi->mName));
                }
                return nullptr;
            }
            else {
                return BuildInterface(ns, mi);
            }
        }
    }

    return nullptr;
}

AutoPtr<Type> MetadataResolver::BuildEnumeration(
    /* [in] */ Namespace* ns,
    /* [in] */ como::MetaEnumeration* me)
{
    AutoPtr<EnumerationType> enumeration = new EnumerationType();
    enumeration->SetName(me->mName);
    ns->AddType(enumeration);

    for (int i = 0; i < me->mEnumeratorNumber; i++) {
        como::MetaEnumerator* enumr = me->mEnumerators[i];
        enumeration->AddEnumerator(enumr->mName, enumr->mValue);
    }

    return enumeration;
}

AutoPtr<Type> MetadataResolver::BuildInterface(
    /* [in] */ Namespace* ns,
    /* [in] */ como::MetaInterface* mi)
{
    AutoPtr<InterfaceType> interface = new InterfaceType();
    mResolvingType = interface;
    interface->SetName(mi->mName);
    ns->AddType(interface);

    if (mi->mBaseInterfaceIndex != -1) {
        como::MetaInterface* baseMi = mComponent->mInterfaces[mi->mBaseInterfaceIndex];
        String fullBaseIntfName = String::Format("%s::%s", baseMi->mNamespace, baseMi->mName);
        AutoPtr<Type> type = mModule->FindType(fullBaseIntfName);
        if (type->IsInterfaceType()) {
            interface->SetBaseInterface(InterfaceType::CastFrom(type));
        }
    }

    Attributes attrs;
    attrs.mUuid = UUID::Parse(mi->mUuid)->Dump();
    interface->SetAttributes(attrs);

    for (int i = 0; i < mi->mMethodNumber; i++) {
        AutoPtr<Method> method = BuildMethod(mi->mMethods[i]);
        interface->AddMethod(method);
    }

    return interface;
}

AutoPtr<Method> MetadataResolver::BuildMethod(
    /* [in] */ como::MetaMethod* mm)
{
    AutoPtr<Method> method = new Method();
    method->SetName(mm->mName);
    AutoPtr<Type> type = BuildType(mComponent->mTypes[mm->mReturnTypeIndex]);
    method->SetReturnType(type);

    for (int i = 0; i < mm->mParameterNumber; i++) {
        AutoPtr<Parameter> parameter = BuildParameter(mm->mParameters[i]);
        method->AddParameter(parameter);
    }

    return method;
}

AutoPtr<Parameter> MetadataResolver::BuildParameter(
    /* [in] */ como::MetaParameter* mp)
{
    AutoPtr<Parameter> parameter = new Parameter();
    parameter->SetName(mp->mName);
    AutoPtr<Type> type = BuildType(mComponent->mTypes[mp->mTypeIndex]);
    parameter->SetType(type);
    if (mp->mProperties & PARAMETER_IN) {
        parameter->SetAttributes(Parameter::IN);
    }
    if (mp->mProperties & PARAMETER_OUT) {
        parameter->SetAttributes(Parameter::OUT);
    }
    if (mp->mProperties & PARAMETER_CALLEE) {
        parameter->SetAttributes(Parameter::CALLEE);
    }
    if (mp->mProperties & PARAMETER_VALUE_DEFAULT) {
        como::MetaValue* mv = reinterpret_cast<como::MetaValue*>(
                ALIGN((uintptr_t)mp + sizeof(como::MetaParameter)));
        AutoPtr<Expression> value = BuildValue(type, mv);
        parameter->SetDefaultValue(value);
    }
    return parameter;
}

AutoPtr<Type> MetadataResolver::BuildType(
    /* [in] */ como::MetaType* mt)
{
    AutoPtr<Type> type;
    String typeStr;
    if (mt->mKind != como::TypeKind::Array) {
        switch (mt->mKind) {
            case como::TypeKind::Char:
                typeStr = "como::Char";
                break;
            case como::TypeKind::Byte:
                typeStr = "como::Byte";
                break;
            case como::TypeKind::Short:
                typeStr = "como::Short";
                break;
            case como::TypeKind::Integer:
                typeStr = "como::Integer";
                break;
            case como::TypeKind::Long:
                typeStr = "como::Long";
                break;
            case como::TypeKind::Float:
                typeStr = "como::Float";
                break;
            case como::TypeKind::Double:
                typeStr = "como::Double";
                break;
            case como::TypeKind::Boolean:
                typeStr = "como::Boolean";
                break;
            case como::TypeKind::String:
                typeStr = "como::String";
                break;
            case como::TypeKind::CoclassID:
                typeStr = "como::CoclassID";
                break;
            case como::TypeKind::ComponentID:
                typeStr = "como::ComponentID";
                break;
            case como::TypeKind::InterfaceID:
                typeStr = "como::InterfaceID";
                break;
            case como::TypeKind::HANDLE:
                typeStr = "como::HANDLE";
                break;
            case como::TypeKind::ECode:
                typeStr = "como::ECode";
                break;
            case como::TypeKind::Enum: {
                como::MetaEnumeration* me = mComponent->mEnumerations[mt->mIndex];
                typeStr = String::Format("%s::%s", me->mNamespace, me->mName);
                break;
            }
            case como::TypeKind::Interface: {
                como::MetaInterface* mi = mComponent->mInterfaces[mt->mIndex];
                typeStr = String::Format("%s::%s", mi->mNamespace, mi->mName);
                break;
            }
            case como::TypeKind::Triple:
                typeStr = "como::Triple";
                break;
            case como::TypeKind::TypeKind:
                typeStr = "como::TypeKind";
                break;
            default:
                return nullptr;
        }

        if (mt->mProperties & TYPE_EXTERNAL) {
            char** moduleNamePP = reinterpret_cast<char**>(
                    ALIGN((uintptr_t)mt + sizeof(como::MetaType)));
            AutoPtr<Module> module = World::GetInstance()->FindModule(*moduleNamePP);
            if (module != nullptr) {
                type = module->FindType(typeStr);
            }
        }
        else {
            type = typeStr.Equals(mResolvingTypename)
                ? mResolvingType : mModule->FindType(typeStr);
        }

        if (type == nullptr) {
            return nullptr;
        }
    }
    else {
        AutoPtr<Type> elementType = BuildType(mComponent->mTypes[mt->mIndex]);
        if (elementType == nullptr) {
            return nullptr;
        }
        typeStr = String::Format("Array<%s>", elementType->ToString().string());

        AutoPtr<ArrayType> array = ArrayType::CastFrom(mModule->FindType(typeStr));
        if (array == nullptr) {
            array = new ArrayType();
            array->SetElementType(elementType);
            mModule->AddTemporaryType(array);
        }
        type = array;
    }

    if ((mt->mProperties & TYPE_NUMBER_MASK) > 0) {
        int N = mt->mProperties & TYPE_NUMBER_MASK;
        while (N >= 1) {
            if ((mt->mProperties >> (N * 2)) & TYPE_POINTER) {
                int ptrNumber = 0;
                while ((N >= 1) && ((mt->mProperties >> (N * 2)) & TYPE_POINTER)) {
                    typeStr += "*";
                    ptrNumber++;
                    N--;
                }
                AutoPtr<PointerType> pointer = PointerType::CastFrom(mModule->FindType(typeStr));
                if (pointer == nullptr) {
                    pointer = new PointerType();
                    pointer->SetBaseType(type);
                    pointer->SetPointerNumber(ptrNumber);
                    mModule->AddTemporaryType(pointer);
                }
                type = pointer;
            }
            else {
                int refNumber = 0;
                while ((N >= 1) && ((mt->mProperties >> (N * 2)) & TYPE_REFERENCE)) {
                    typeStr += "&";
                    refNumber++;
                    N--;
                }
                AutoPtr<ReferenceType> reference = ReferenceType::CastFrom(mModule->FindType(typeStr));
                if (reference == nullptr) {
                    reference = new ReferenceType();
                    reference->SetBaseType(type);
                    reference->SetReferenceNumber(refNumber);
                    mModule->AddTemporaryType(reference);
                }
                type = reference;
            }
        }
    }

    return type;
}

AutoPtr<Expression> MetadataResolver::BuildValue(
    /* [in] */ Type* type,
    /* [in] */ como::MetaValue* mv)
{
    if (type->IsBooleanType()) {
        AutoPtr<PostfixExpression> expr = new PostfixExpression();
        expr->SetType(type);
        expr->SetBooleanValue(mv->mBooleanValue);
        return expr;
    }
    else if (type->IsCharType()) {
        AutoPtr<PostfixExpression> expr = new PostfixExpression();
        expr->SetType(type);
        expr->SetIntegralValue(mv->mIntegralValue);
        return expr;
    }
    else if (type->IsByteType() || type->IsShortType() ||
             type->IsIntegerType() || type->IsLongType() ||
             type->IsEnumerationType() || type->IsHANDLEType()) {
        AutoPtr<PostfixExpression> expr = new PostfixExpression();
        expr->SetType(type);
        expr->SetIntegralValue(mv->mIntegralValue);
        expr->SetRadix(mv->mProperties & VALUE_RADIX_MASK);
        return expr;
    }
    else if (type->IsFloatType() || type->IsDoubleType()) {
        AutoPtr<PostfixExpression> expr = new PostfixExpression();
        expr->SetType(type);
        expr->SetFloatingPointValue(mv->mFloatingPointValue);
        expr->SetScientificNotation(mv->mProperties & VALUE_SCIENTIFIC_NOTATION);
        return expr;
    }
    else if (type->IsStringType()) {
        AutoPtr<PostfixExpression> expr = new PostfixExpression();
        expr->SetType(type);
        expr->SetStringValue(mv->mStringValue);
        return expr;
    }

    return nullptr;
}

}
