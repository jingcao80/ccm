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

/*
 * Copyright (C) 2005 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "CParcel.h"

namespace ccm {

const CoclassID CID_CParcel =
        {{0x28208469,0x8814,0x49af,0x80f7,{0x8,0xb,0x1,0x4,0x7,0x3,0x9,0x1,0xf,0xb,0x9,0x4}}, &CID_CCMRuntime};

CCM_INTERFACE_IMPL_1(CParcel, Object, IParcel);

CCM_OBJECT_IMPL(CParcel);

ECode CParcel::ReadChar(
    /* [out] */ Char* value)
{
    return NOERROR;
}

ECode CParcel::WriteChar(
    /* [in] */ Char value)
{
    return NOERROR;
}

ECode CParcel::ReadByte(
    /* [out] */ Byte* value)
{
    return NOERROR;
}

ECode CParcel::WriteByte(
    /* [in] */ Byte value)
{
    return NOERROR;
}

ECode CParcel::ReadShort(
    /* [out] */ Short* value)
{
    return NOERROR;
}

ECode CParcel::WriteShort(
    /* [in] */ Short value)
{
    return NOERROR;
}

ECode CParcel::ReadInteger(
    /* [out] */ Integer* value)
{
    return NOERROR;
}

ECode CParcel::WriteInteger(
    /* [in] */ Integer value)
{
    return NOERROR;
}

ECode CParcel::ReadLong(
    /* [out] */ Long* value)
{
    return NOERROR;
}

ECode CParcel::WriteLong(
    /* [in] */ Long value)
{
    return NOERROR;
}

ECode CParcel::ReadFloat(
    /* [out] */ Float* value)
{
    return NOERROR;
}

ECode CParcel::WriteFloat(
    /* [in] */ Float value)
{
    return NOERROR;
}

ECode CParcel::ReadDouble(
    /* [out] */ Double* value)
{
    return NOERROR;
}

ECode CParcel::WriteDouble(
    /* [in] */ Double value)
{
    return NOERROR;
}

ECode CParcel::ReadBoolean(
    /* [out] */ Boolean* value)
{
    return NOERROR;
}

ECode CParcel::WriteBoolean(
    /* [in] */ Boolean value)
{
    return NOERROR;
}

ECode CParcel::ReadString(
    /* [out] */ String* value)
{
    return NOERROR;
}

ECode CParcel::WriteString(
    /* [in] */ String value)
{
    return NOERROR;
}

ECode CParcel::ReadCoclassID(
    /* [out] */ CoclassID* value)
{
    return NOERROR;
}

ECode CParcel::WriteCoclassID(
    /* [in] */ CoclassID value)
{
    return NOERROR;
}

ECode CParcel::ReadComponentID(
    /* [out] */ ComponentID* value)
{
    return NOERROR;
}

ECode CParcel::WriteComponentID(
    /* [in] */ ComponentID value)
{
    return NOERROR;
}

ECode CParcel::ReadInterfaceID(
    /* [out] */ InterfaceID* value)
{
    return NOERROR;
}

ECode CParcel::WriteInterfaceID(
    /* [in] */ InterfaceID value)
{
    return NOERROR;
}

ECode CParcel::ReadHANDLE(
    /* [out] */ HANDLE* value)
{
    return NOERROR;
}

ECode CParcel::WriteHANDLE(
    /* [in] */ HANDLE value)
{
    return NOERROR;
}

ECode CParcel::ReadECode(
    /* [out] */ ECode* value)
{
    return NOERROR;
}

ECode CParcel::WriteECode(
    /* [in] */ ECode value)
{
    return NOERROR;
}

ECode CParcel::ReadEnumeration(
    /* [out] */ Integer* value)
{
    return NOERROR;
}

ECode CParcel::WriteEnumeration(
    /* [in] */ Integer value)
{
    return NOERROR;
}

ECode CParcel::ReadArray(
    /* [out] */ HANDLE* array)
{
    return NOERROR;
}

ECode CParcel::WriteArray(
    /* [in] */ HANDLE array)
{
    return NOERROR;
}

ECode CParcel::ReadInterface(
    /* [out] */ IInterface** value)
{
    return NOERROR;
}

ECode CParcel::WriteInterface(
    /* [in] */ IInterface* value)
{
    return NOERROR;
}

}
