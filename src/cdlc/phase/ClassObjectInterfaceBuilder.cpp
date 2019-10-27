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

#include "phase/ClassObjectInterfaceBuilder.h"
#include "ast/InterfaceType.h"
#include "ast/Method.h"
#include "ast/Parameter.h"
#include "ast/PointerType.h"
#include "ast/ReferenceType.h"
#include "util/Logger.h"

namespace cdlc {

const char* ClassObjectInterfaceBuilder::TAG = "ClassObjectInterfaceBuilder";

bool ClassObjectInterfaceBuilder::Process()
{
    mIInterfaceType = mModule->FindType("como::IInterface");
    if (mIInterfaceType == nullptr) {
        Logger::E(TAG, "Type \"como::IInterface\" not found.");
        return false;
    }
    mInterfaceIDType = mModule->FindType("como::InterfaceID");
    if (mInterfaceIDType == nullptr) {
        Logger::E(TAG, "Type \"como::InterfaceID\" not found.");
        return false;
    }
    mIClassObjectType = mModule->FindType("como::IClassObject");
    if (mIClassObjectType == nullptr) {
        mIClassObjectType = mWorld->FindType("como::IClassObject");
        if (mIClassObjectType == nullptr) {
            Logger::E(TAG, "Type \"como::IClassObject\" not found.");
            return false;
        }
        mIClassObjectType = mIClassObjectType->Clone(mModule, true);
    }

    for (int i = 0; i < mModule->GetCoclassNumber(); i++) {
        BuildCoclassObjectInterface(mModule->GetCoclass(i));
    }

    return true;
}

void ClassObjectInterfaceBuilder::BuildCoclassObjectInterface(
    /* [in] */ CoclassType* klass)
{
    bool hasConstructorWithArguments = false;
    for (int i = 0; i < klass->GetConstructorNumber(); i++) {
        AutoPtr<Method> constructor = klass->GetConstructor(i);
        if (constructor->GetParameterNumber() > 0) {
            hasConstructorWithArguments = true;
            break;
        }
    }

    if (hasConstructorWithArguments) {
        AutoPtr<InterfaceType> clsObjIntf = new InterfaceType();
        clsObjIntf->SetName(String::Format("I%sClassObject", klass->GetName().string()));
        clsObjIntf->SetBaseInterface(InterfaceType::CastFrom(mIInterfaceType));
        klass->GetNamespace()->AddInterfaceType(clsObjIntf);
        klass->AddInterface(clsObjIntf);
        for (int i = 0; i < klass->GetConstructorNumber(); i++) {
            AutoPtr<Method> m = klass->GetConstructor(i);
            m->SetName("CreateObject");
            AutoPtr<Parameter> p = new Parameter();
            p->SetName("iid");
            p->SetType(mInterfaceIDType);
            p->SetAttributes(Parameter::IN);
            m->AddParameter(p);
            p = new Parameter();
            p->SetName("object");
            AutoPtr<Type> t;
            if (m->IsReference()) {
                t = mModule->FindType("como::IInterface&&");
                if (t == nullptr) {
                    AutoPtr<ReferenceType> reference = new ReferenceType();
                    reference->SetBaseType(mIInterfaceType);
                    reference->SetReferenceNumber(2);
                    mModule->AddTemporaryType(reference);
                    t = reference;
                }
            }
            else {
                t = mModule->FindType("como::IInterface**");
                if (t == nullptr) {
                    AutoPtr<PointerType> pointer = new PointerType();
                    pointer->SetBaseType(mIInterfaceType);
                    pointer->SetPointerNumber(2);
                    mModule->AddTemporaryType(pointer);
                    t = pointer;
                }
            }
            p->SetType(t);
            p->SetAttributes(Parameter::OUT);
            m->AddParameter(p);
            clsObjIntf->AddMethod(m);
        }
    }
    else {
        klass->AddInterface(InterfaceType::CastFrom(mIClassObjectType));
    }
}

}
