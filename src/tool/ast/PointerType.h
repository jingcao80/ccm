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

#ifndef __CCDL_AST_POINTERTYPE_H__
#define __CCDL_AST_POINTERTYPE_H__

#include "Type.h"

namespace ccdl {
namespace ast {

class PointerType : public Type
{
public:
    PointerType()
        : mBaseType(nullptr)
        , mPointerNumber(0)
    {}

    bool IsPointerType() override
    { return true; }

    inline Type* GetBaseType()
    { return mBaseType; }

    inline void SetBaseType(
        /* [in] */ Type* baseType)
    { mBaseType = baseType; }

    inline int GetPointerNumber()
    { return mPointerNumber; }

    inline void SetPointerNumber(
        /* [in] */ int ptrNumber)
    { mPointerNumber = ptrNumber; }

    String Signature() override;

    String ToString() override;

private:
    Type* mBaseType;
    int mPointerNumber;
};

}
}

#endif // __CCDL_AST_POINTERTYPE_H__
