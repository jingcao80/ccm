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

#ifndef __CCDL_AST_ARRAYTYPE_H__
#define __CCDL_AST_ARRAYTYPE_H__

#include "Type.h"

namespace ccdl {
namespace ast {

class ArrayType : public Type
{
public:
    ArrayType();

    inline bool IsArrayType() override
    { return true; }

    inline Type* GetElementType()
    { return mElementType; }

    inline void SetElementType(
        /* [in] */ Type* elemType)
    { mElementType = elemType; }

    String Signature() override;

    String ToString() override;

private:
    Type* mElementType;
};

}
}

#endif // __CCDL_AST_ARRAYTYPE_H__
