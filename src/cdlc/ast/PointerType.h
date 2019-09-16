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

#ifndef __CDLC_POINTERTYPE_H__
#define __CDLC_POINTERTYPE_H__

#include "ast/Type.h"
#include "util/AutoPtr.h"
#include "util/String.h"

namespace cdlc {

class PointerType
    : public Type
{
public:
    inline AutoPtr<Type> GetBaseType();

    inline void SetBaseType(
        /* [in] */ Type* type);

    inline int GetPointerNumber();

    inline void SetPointerNumber(
        /* [in] */ int number);

    bool IsPointerType() override;

    String GetSignature() override;

    String ToString() override;

    inline static AutoPtr<PointerType> CastFrom(
        /* [in] */ Type* type);

private:
    AutoPtr<Type> mBaseType;
    int mPointerNumber = 0;
};

AutoPtr<Type> PointerType::GetBaseType()
{
    return mBaseType;
}

void PointerType::SetBaseType(
    /* [in] */ Type* type)
{
    mBaseType = type;
}

int PointerType::GetPointerNumber()
{
    return mPointerNumber;
}

void PointerType::SetPointerNumber(
    /* [in] */ int number)
{
    mPointerNumber = number;
}

AutoPtr<PointerType> PointerType::CastFrom(
    /* [in] */ Type* type)
{
    return static_cast<PointerType*>(type);
}

}

#endif // __CDLC_POINTERTYPE_H__
