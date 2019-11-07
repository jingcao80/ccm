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

#include "phase/ParameterTypeChecker.h"
#include "ast/PointerType.h"
#include "ast/ReferenceType.h"
#include "util/Logger.h"

namespace cdlc {

bool ParameterTypeChecker::Process()
{
    bool ret = CheckInterfaces();

    ret = CheckCoclasses() & ret;

    return ret;
}

bool ParameterTypeChecker::CheckInterfaces()
{
    bool ret = true;

    int N = mModule->GetInterfaceNumber();
    for (int i = 0; i < N; i++) {
        mInterface = mModule->GetInterface(i);
        if (!mInterface->IsExternal()){
            ret = CheckInterface(mInterface) && ret;
        }
    }
    return ret;
}

bool ParameterTypeChecker::CheckCoclasses()
{
    bool ret = true;

    int N = mModule->GetCoclassNumber();
    for (int i = 0; i < N; i++) {
        mCoclass = mModule->GetCoclass(i);
        ret = CheckCoclass(mCoclass) && ret;
    }
    return ret;
}

bool ParameterTypeChecker::CheckInterface(
    /* [in] */ InterfaceType* interface)
{
    bool ret = true;

    int N = interface->GetMethodNumber();
    for (int i = 0; i < N; i++) {
        mMethod = interface->GetMethod(i);
        ret = CheckMethod(mMethod) && ret;
    }
    return ret;
}

bool ParameterTypeChecker::CheckCoclass(
    /* [in] */ CoclassType* coclass)
{
    bool ret = true;

    int N = coclass->GetConstructorNumber();
    for (int i = 0; i < N; i++) {
        mMethod = coclass->GetConstructor(i);
        ret = CheckConstructor(mMethod) && ret;
    }
    return ret;
}

bool ParameterTypeChecker::CheckMethod(
    /* [in] */ Method* method)
{
    bool ret = true;

    int N = method->GetParameterNumber();
    for (int i = 0; i < N; i++) {
        AutoPtr<Parameter> parameter = method->GetParameter(i);
        ret = CheckMethodParameter(parameter) && ret;
    }
    return ret;
}

bool ParameterTypeChecker::CheckMethodParameter(
    /* [in] */ Parameter* parameter)
{
    bool ret = IsTypeValid(parameter->GetType(), parameter->IsIn(),
            parameter->IsOut(), parameter->IsCallee());
    if (!ret) {
        Logger::E("ParameterTypeChecker", "The type \"%s\" of the parameter"
                " \"%s\" in \"%s::%s\" is illegal.",
                parameter->GetType()->ToString().string(),
                parameter->GetName().string(),
                mInterface->ToString().string(),
                mMethod->GetName().string());
    }
    return ret;
}

bool ParameterTypeChecker::CheckConstructor(
    /* [in] */ Method* method)
{
    bool ret = true;

    int N = method->GetParameterNumber();
    for (int i = 0; i < N - 2; i++) {
        AutoPtr<Parameter> parameter = method->GetParameter(i);
        ret = CheckConstructorParameter(parameter) && ret;
    }
    return ret;
}

bool ParameterTypeChecker::CheckConstructorParameter(
    /* [in] */ Parameter* parameter)
{
    bool ret = false;
    if (!parameter->IsOut()) {
        ret = IsTypeValid(parameter->GetType(), true, false, false);
    }
    if (!ret) {
        Logger::E("ParameterTypeChecker", "The type \"%s\" of the parameter"
                " \"%s\" in coclass \"%s\" constructor is illegal.",
                parameter->GetType()->ToString().string(),
                parameter->GetName().string(),
                mCoclass->ToString().string());
    }
    return ret;
}

bool ParameterTypeChecker::IsTypeValid(
    /* [in] */ AutoPtr<Type> type,
    /* [in] */ bool isIn,
    /* [in] */ bool isOut,
    /* [in] */ bool isCallee)
{
    int pointerNumber = 0;
    int referenceNumber = 0;
    while (type->IsPointerType() || type->IsReferenceType()) {
        if (type->IsPointerType()) {
            PointerType* pointer = PointerType::CastFrom(type);
            pointerNumber += pointer->GetPointerNumber();
            type = pointer->GetBaseType();
        }
        else {
            ReferenceType* reference = ReferenceType::CastFrom(type);
            referenceNumber += reference->GetReferenceNumber();
            type = reference->GetBaseType();
        }
    }

    if ((type->IsBuildinType() && !type->IsInterfaceType()) || type->IsEnumerationType()) {
        if (isCallee) {
            return false;
        }
        if (isOut && pointerNumber + referenceNumber > 1) {
            return false;
        }
        if (!isOut && pointerNumber + referenceNumber > 1) {
            return false;
        }
    }
    else if (type->IsArrayType() || type->IsTripleType()) {
        if (pointerNumber + referenceNumber > 1) {
            return false;
        }
    }
    else if (type->IsInterfaceType()) {
        if (isCallee) {
            return false;
        }
        if (isIn && pointerNumber + referenceNumber != 1) {
            return false;
        }
        if (isOut && pointerNumber + referenceNumber != 2) {
            return false;
        }
    }
    return true;
}

}
