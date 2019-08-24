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
#include "ast/BooleanType.h"
#include "ast/ByteType.h"
#include "ast/CharType.h"
#include "ast/CoclassIDType.h"
#include "ast/ComponentIDType.h"
#include "ast/DoubleType.h"
#include "ast/ECodeType.h"
#include "ast/FloatType.h"
#include "ast/HANDLEType.h"
#include "ast/IntegerType.h"
#include "ast/InterfaceIDType.h"
#include "ast/InterfaceType.h"
#include "ast/LongType.h"
#include "ast/ShortType.h"
#include "ast/StringType.h"
#include "ast/TripleType.h"

namespace cdlc {

void BuildinTypeBuilder::Process()
{
    mModule->ParseNamespace("como");
    BuildPrimitiveType();
    BuildIInterface();
}

void BuildinTypeBuilder::BuildPrimitiveType()
{
    AutoPtr<Namespace> ns;
    AutoPtr<Type> type;

    ns = mModule->FindNamespace("como");
    // add BooleanType
    type = new BooleanType();
    type->SetNamespace(ns);
    ns->AddType(type);
    // add CharType
    type = new CharType();
    type->SetNamespace(ns);
    ns->AddType(type);
    // add ByteType
    type = new ByteType();
    type->SetNamespace(ns);
    ns->AddType(type);
    // add ShortType
    type = new ShortType();
    type->SetNamespace(ns);
    ns->AddType(type);
    // add IntegerType
    type = new IntegerType();
    type->SetNamespace(ns);
    ns->AddType(type);
    // add LongType
    type = new LongType();
    type->SetNamespace(ns);
    ns->AddType(type);
    // add FloatType
    type = new FloatType();
    type->SetNamespace(ns);
    ns->AddType(type);
    // add DoubleType
    type = new DoubleType();
    type->SetNamespace(ns);
    ns->AddType(type);
    // add StringType
    type = new StringType();
    type->SetNamespace(ns);
    ns->AddType(type);
    // add TripleType
    type = new TripleType();
    type->SetNamespace(ns);
    ns->AddType(type);
    // add HANDLEType
    type = new HANDLEType();
    type->SetNamespace(ns);
    ns->AddType(type);
    // add CoclassIDType
    type = new CoclassIDType();
    type->SetNamespace(ns);
    ns->AddType(type);
    // add ComponentIDType
    type = new ComponentIDType();
    type->SetNamespace(ns);
    ns->AddType(type);
    // add InterfaceIDType
    type = new InterfaceIDType();
    type->SetNamespace(ns);
    ns->AddType(type);
    // add ECodeType
    type = new ECodeType();
    type->SetNamespace(ns);
    ns->AddType(type);
}

void BuildinTypeBuilder::BuildIInterface()
{
    AutoPtr<InterfaceType> iinterface = new InterfaceType("IInterface");
    iinterface->SetNamespace(mModule->FindNamespace("como"));
}

}
