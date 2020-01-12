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

#include "como/io/Bits.h"
#include "como/io/ByteOrder.h"

namespace como {
namespace io {

COMO_INTERFACE_IMPL_1(ByteOrder, SyncObject, IByteOrder);

AutoPtr<IByteOrder> ByteOrder::Order()
{
    return Bits::ByteOrder();
}

ECode ByteOrder::ToString(
    /* [out] */ String& desc)
{
    desc = mName;
    return NOERROR;
}

AutoPtr<IByteOrder> ByteOrder::GetBIG_ENDIAN()
{
    static AutoPtr<IByteOrder> BIG_ENDIAN_ = new ByteOrder(String("BIG_ENDIAN"));
    return BIG_ENDIAN_;
}

AutoPtr<IByteOrder> ByteOrder::GetLITTLE_ENDIAN()
{
    static AutoPtr<IByteOrder> LITTLE_ENDIAN_ = new ByteOrder(String("LITTLE_ENDIAN"));
    return LITTLE_ENDIAN_;
}

}
}
