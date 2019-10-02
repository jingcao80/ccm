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

#include "phase/InterfaceIntegrityChecker.h"
#include "ast/ArrayType.h"
#include "ast/PointerType.h"
#include "ast/ReferenceType.h"
#include "util/Logger.h"

namespace cdlc {

bool InterfaceIntegrityChecker::Process()
{
    bool ret = true;
    for (int i = 0; i < mModule->GetInterfaceNumber(); i++) {
        ret = CheckInterfaceIntegrity(mModule->GetInterface(i)) && ret;
    }
    return ret;
}

bool InterfaceIntegrityChecker::CheckInterfaceIntegrity(
    /* [in] */ InterfaceType* interface)
{
    bool ret = true;
    for (int i = 0; i < interface->GetMethodNumber(); i++) {
        AutoPtr<Method> method = interface->GetMethod(i);
        for (int j = 0; j < method->GetParameterNumber(); j++) {
            AutoPtr<Parameter> param = method->GetParameter(j);
            ret = CheckTypeIntegrity(param->GetType()) && ret;
        }
        ret = CheckTypeIntegrity(method->GetReturnType()) && ret;
    }
    return ret;
}

bool InterfaceIntegrityChecker::CheckTypeIntegrity(
    /* [in] */ Type* type)
{
    while (type->IsPointerType() || type->IsReferenceType() || type->IsArrayType()) {
        if (type->IsPointerType()) {
            type = PointerType::CastFrom(type)->GetBaseType();
        }
        else if (type->IsReferenceType()) {
            type = ReferenceType::CastFrom(type)->GetBaseType();
        }
        else {
            type = ArrayType::CastFrom(type)->GetElementType();
        }
    }
    if (type->IsForwardDeclared()) {
        Logger::E("Parser", "Type[%p] \"%s\" is not declared",
                type, type->ToString().string());
        return false;
    }
    return true;
}

}
