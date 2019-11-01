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

#include "ast/ReferenceType.h"
#include "ast/Module.h"
#include "ast/Namespace.h"

namespace cdlc {

bool ReferenceType::IsReferenceType()
{
    return true;
}

String ReferenceType::GetSignature()
{
    String signature = mBaseType->GetSignature();
    for (int i = 0; i < mReferenceNumber; i++) {
        signature += "&";
    }
    return signature;
}

String ReferenceType::ToString()
{
    String typeStr = mBaseType->ToString();
    for (int i = 0; i < mReferenceNumber; i++) {
        typeStr += "&";
    }
    return typeStr;
}

AutoPtr<Node> ReferenceType::Clone(
    /* [in] */ Module* module,
    /* [in] */ bool deepCopy)
{
    AutoPtr<ReferenceType> clone = new ReferenceType();
    CloneBase(clone, module);
    AutoPtr<Type> baseType = module->FindType(mBaseType->ToString());
    if (baseType == nullptr) {
        baseType = mBaseType->Clone(module, deepCopy);
    }
    clone->mBaseType = baseType;
    clone->mReferenceNumber = mReferenceNumber;
    module->AddTemporaryType(clone);
    return clone;
}

}
