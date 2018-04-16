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

#include "ByteType.h"

namespace ccdl {
namespace ast {

ByteType::ByteType()
{
    SetName(String("Byte"));
}

bool ByteType::IsPrimitiveType()
{
    return true;
}

bool ByteType::IsNumericType()
{
    return true;
}

bool ByteType::IsIntegralType()
{
    return true;
}

bool ByteType::IsByteType()
{
    return true;
}

String ByteType::Signature()
{
    return String("B");
}

}
}