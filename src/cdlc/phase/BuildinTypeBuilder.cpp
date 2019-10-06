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

#include "phase/BuildinTypeBuilder.h"
#include "ast/Attributes.h"
#include "ast/BooleanType.h"
#include "ast/ByteType.h"
#include "ast/CharType.h"
#include "ast/CoclassIDType.h"
#include "ast/ComponentIDType.h"
#include "ast/DoubleType.h"
#include "ast/ECodeType.h"
#include "ast/EnumerationType.h"
#include "ast/FloatType.h"
#include "ast/HANDLEType.h"
#include "ast/IntegerType.h"
#include "ast/InterfaceIDType.h"
#include "ast/InterfaceType.h"
#include "ast/LongType.h"
#include "ast/Method.h"
#include "ast/Parameter.h"
#include "ast/PointerType.h"
#include "ast/PostfixExpression.h"
#include "ast/ReferenceType.h"
#include "ast/ShortType.h"
#include "ast/StringType.h"
#include "ast/TripleType.h"

namespace cdlc {

bool BuildinTypeBuilder::Process()
{
    mModule->ParseNamespace("como");
    BuildPrimitiveType();
    BuildIInterface();
    return true;
}

void BuildinTypeBuilder::BuildPrimitiveType()
{
    AutoPtr<Namespace> ns;
    AutoPtr<Type> type;

    ns = mModule->FindNamespace("como");

    // add BooleanType
    type = new BooleanType();
    ns->AddType(type);

    // add CharType
    type = new CharType();
    ns->AddType(type);

    // add ByteType
    type = new ByteType();
    ns->AddType(type);

    // add ShortType
    type = new ShortType();
    ns->AddType(type);

    // add IntegerType
    type = new IntegerType();
    ns->AddType(type);

    // add LongType
    type = new LongType();
    ns->AddType(type);

    // add FloatType
    type = new FloatType();
    ns->AddType(type);

    // add DoubleType
    type = new DoubleType();
    ns->AddType(type);

    // add StringType
    type = new StringType();
    ns->AddType(type);

    // add TripleType
    type = new TripleType();
    ns->AddType(type);

    // add HANDLEType
    type = new HANDLEType();
    ns->AddType(type);

    // add CoclassIDType
    type = new CoclassIDType();
    ns->AddType(type);

    // add ComponentIDType
    type = new ComponentIDType();
    ns->AddType(type);

    // add InterfaceIDType
    type = new InterfaceIDType();
    ns->AddType(type);

    // add ECodeType
    type = new ECodeType();
    ns->AddType(type);

    // add TypeKind
    AutoPtr<EnumerationType> typeKindType = new EnumerationType();
    typeKindType->SetName("TypeKind");
    typeKindType->AddEnumerator("Unknown", 0);
    typeKindType->AddEnumerator("Char", 1);
    typeKindType->AddEnumerator("Byte", 2);
    typeKindType->AddEnumerator("Short", 3);
    typeKindType->AddEnumerator("Integer", 4);
    typeKindType->AddEnumerator("Long", 5);
    typeKindType->AddEnumerator("Float", 6);
    typeKindType->AddEnumerator("Double", 7);
    typeKindType->AddEnumerator("Boolean", 8);
    typeKindType->AddEnumerator("String", 9);
    typeKindType->AddEnumerator("CoclassID", 10);
    typeKindType->AddEnumerator("ComponentID", 11);
    typeKindType->AddEnumerator("InterfaceID", 12);
    typeKindType->AddEnumerator("HANDLE", 13);
    typeKindType->AddEnumerator("ECode", 14);
    typeKindType->AddEnumerator("Enum", 15);
    typeKindType->AddEnumerator("Array", 16);
    typeKindType->AddEnumerator("Interface", 17);
    typeKindType->AddEnumerator("Coclass", 18);
    typeKindType->AddEnumerator("Triple", 19);
    typeKindType->AddEnumerator("TypeKind", 20);
    ns->AddType(typeKindType);
}

void BuildinTypeBuilder::BuildIInterface()
{
    AutoPtr<Namespace> ns = mModule->FindNamespace("como");

    // add IInterface
    AutoPtr<InterfaceType> iinterface = new InterfaceType();
    iinterface->SetName("IInterface");
    iinterface->SetNamespace(ns);
    Attributes attrs;
    attrs.mUuid = "00000000-0000-0000-0000-000000000000";
    attrs.mVersion = "0.1.0";
    iinterface->SetAttributes(attrs);
    ns->AddInterfaceType(iinterface);

    // add Probe method
    AutoPtr<Method> method = new Method();
    method->SetName("Probe");
    AutoPtr<PointerType> retType = new PointerType();
    retType->SetBaseType(mModule->FindType("como::IInterface"));
    retType->SetPointerNumber(1);
    mModule->AddTemporaryType(retType);
    method->SetReturnType(retType);
    AutoPtr<Parameter> param = new Parameter();
    param->SetName("iid");
    param->SetType(mModule->FindType("como::InterfaceID"));
    param->SetAttributes(Parameter::IN);
    method->AddParameter(param);
    iinterface->AddMethod(method);

    // add AddRef method
    method = new Method();
    method->SetName("AddRef");
    method->SetReturnType(mModule->FindType("como::Integer"));
    param = new Parameter();
    param->SetName("id");
    param->SetType(mModule->FindType("como::HANDLE"));
    AutoPtr<PostfixExpression> expr = new PostfixExpression();
    expr->SetIntegralValue(0);
    param->SetDefaultValue(expr);
    param->SetAttributes(Parameter::IN);
    method->AddParameter(param);
    iinterface->AddMethod(method);

    // add Release method
    method = new Method();
    method->SetName("Release");
    method->SetReturnType(mModule->FindType("como::Integer"));
    method->AddParameter(param);
    iinterface->AddMethod(method);

    // add GetInterfaceID method
    method = new Method();
    method->SetName("GetInterfaceID");
    method->SetReturnType(mModule->FindType("como::ECode"));
    param = new Parameter();
    param->SetName("object");
    param->SetType(mModule->FindType("como::IInterface*"));
    param->SetAttributes(Parameter::IN);
    method->AddParameter(param);
    param = new Parameter();
    param->SetName("iid");
    AutoPtr<ReferenceType> refType = new ReferenceType();
    refType->SetBaseType(mModule->FindType("como::InterfaceID"));
    mModule->AddTemporaryType(refType);
    param->SetType(refType);
    param->SetAttributes(Parameter::OUT);
    method->AddParameter(param);
    iinterface->AddMethod(method);
}

}
