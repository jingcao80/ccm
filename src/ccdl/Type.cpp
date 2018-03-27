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

#include "Namespace.h"
#include "Type.h"

namespace ccm {
namespace ccdl {

Type::~Type()
{
    mNamespace = nullptr;
}

void Type::SetNamespace(
    /* [in] */ Namespace* ns)
{
    mNamespace = ns;
}

bool Type::IsPrimitiveType()
{
    return false;
}

bool Type::IsNumericType()
{
    return false;
}

bool Type::IsIntegralType()
{
    return false;
}

bool Type::IsFloatingPointType()
{
    return false;
}

bool Type::IsEnumeration()
{
    return false;
}

bool Type::IsInterface()
{
    return false;
}

bool Type::IsCoclass()
{
    return false;
}

String Type::ToString()
{
    return mNamespace == nullptr ? mName : mNamespace->ToString() + mName;
}

String Type::ToShortString()
{
    return mName;
}

String Type::Dump(
    /* [in] */ const String& prefix)
{
    return ToString();
}

}
}
