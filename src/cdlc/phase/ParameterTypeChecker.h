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

#ifndef __CDLC_PARAMETERTYPECHECKER_H__
#define __CDLC_PARAMETERTYPECHECKER_H__

#include "ast/InterfaceType.h"
#include "ast/Method.h"
#include "ast/Module.h"
#include "ast/Parameter.h"
#include "parser/Phase.h"

namespace cdlc {

class ParameterTypeChecker
    : public Phase
{
public:
    inline ParameterTypeChecker();

    bool Process() override;

private:
    bool CheckInterfaces();

    bool CheckCoclasses();

    bool CheckInterface(
        /* [in] */ InterfaceType* interface);

    bool CheckCoclass(
        /* [in] */ CoclassType* coclass);

    bool CheckMethod(
        /* [in] */ Method* method);

    bool CheckMethodParameter(
        /* [in] */ Parameter* parameter);

    bool CheckConstructor(
        /* [in] */ Method* method);

    bool CheckConstructorParameter(
        /* [in] */ Parameter* parameter);

    bool IsTypeValid(
        /* [in] */ AutoPtr<Type> type,
        /* [in] */ bool isIn,
        /* [in] */ bool isOut,
        /* [in] */ bool isCallee);

private:
    Module* mModule = nullptr;
    AutoPtr<InterfaceType> mInterface;
    AutoPtr<CoclassType> mCoclass;
    AutoPtr<Method> mMethod;
};

ParameterTypeChecker::ParameterTypeChecker()
{
    mModule = mWorld->GetWorkingModule();
}

}

#endif // __CDLC_PARAMETERTYPECHECKER_H__
