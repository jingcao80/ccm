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

#include "CArgumentList.h"

namespace ccm {

CCM_INTERFACE_IMPL_LIGHT_1(CArgumentList, IArgumentList)

CArgumentList::CArgumentList(
    /* [in] */ MetaComponent* mc,
    /* [in] */ MetaMethod* mm)
{}

CArgumentList::~CArgumentList()
{}

ECode CArgumentList::GetInputArgumentOfByte(
    /* [in] */ Integer index,
    /* [out] */ Byte* value)
{
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfByte(
    /* [in] */ Integer index,
    /* [in] */ Byte value)
{
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfShort(
    /* [in] */ Integer index,
    /* [out] */ Short* value)
{
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfShort(
    /* [in] */ Integer index,
    /* [in] */ Short value)
{
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfInteger(
    /* [in] */ Integer index,
    /* [out] */ Integer* value)
{
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfInteger(
    /* [in] */ Integer index,
    /* [in] */ Integer value)
{
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfLong(
    /* [in] */ Integer index,
    /* [out] */ Long* value)
{
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfLong(
    /* [in] */ Integer index,
    /* [in] */ Long value)
{
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfFloat(
    /* [in] */ Integer index,
    /* [out] */ Float* value)
{
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfFloat(
    /* [in] */ Integer index,
    /* [out] */ Float value)
{
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfDouble(
    /* [in] */ Integer index,
    /* [out] */ Double* value)
{
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfDouble(
    /* [in] */ Integer index,
    /* [in] */ Double value)
{
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfChar(
    /* [in] */ Integer index,
    /* [out] */ Char* value)
{
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfChar(
    /* [in] */ Integer index,
    /* [in] */ Char value)
{
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfBoolean(
    /* [in] */ Integer index,
    /* [out] */ Boolean* value)
{
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfBoolean(
    /* [in] */ Integer index,
    /* [in] */ Boolean value)
{
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfString(
    /* [in] */ Integer index,
    /* [out] */ String* value)
{
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfString(
    /* [in] */ Integer index,
    /* [in] */ const String& value)
{
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfHANDLE(
    /* [in] */ Integer index,
    /* [out] */ HANDLE* value)
{
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfHANDLE(
    /* [in] */ Integer index,
    /* [in] */ HANDLE value)
{
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfECode(
    /* [in] */ Integer index,
    /* [out] */ ECode* value)
{
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfECode(
    /* [in] */ Integer index,
    /* [in] */ ECode value)
{
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfCoclassID(
    /* [in] */ Integer index,
    /* [out] */ CoclassID* value)
{
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfCoclassID(
    /* [in] */ Integer index,
    /* [in] */ const CoclassID& value)
{
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfComponentID(
    /* [in] */ Integer index,
    /* [out] */ ComponentID* value)
{
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfComponentID(
    /* [in] */ Integer index,
    /* [in] */ const ComponentID& value)
{
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfInterfaceID(
    /* [in] */ Integer index,
    /* [out] */ InterfaceID* value)
{
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfInterfaceID(
    /* [in] */ Integer index,
    /* [in] */ const InterfaceID& value)
{
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfArray(
    /* [in] */ Integer index,
    /* [out] */ HANDLE* value)
{
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfArray(
    /* [in] */ Integer index,
    /* [in] */ HANDLE value)
{
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfEnumeration(
    /* [in] */ Integer index,
    /* [out] */ Integer* value)
{
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfEnumeration(
    /* [in] */ Integer index,
    /* [in] */ Integer value)
{
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfInterface(
    /* [in] */ Integer index,
    /* [out] */ IInterface** value)
{
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfInterface(
    /* [in] */ Integer index,
    /* [in] */ IInterface* value)
{
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfByte(
    /* [in] */ Integer index,
    /* [in] */ Byte value)
{
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfByte(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfShort(
    /* [in] */ Integer index,
    /* [in] */ Short value)
{
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfShort(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfInteger(
    /* [in] */ Integer index,
    /* [in] */ Integer value)
{
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfInteger(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfLong(
    /* [in] */ Integer index,
    /* [in] */ Long value)
{
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfLong(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfFloat(
    /* [in] */ Integer index,
    /* [in] */ Float value)
{
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfFloat(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfDouble(
    /* [in] */ Integer index,
    /* [in] */ Double value)
{
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfDouble(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfChar(
    /* [in] */ Integer index,
    /* [in] */ Char value)
{
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfChar(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfBoolean(
    /* [in] */ Integer index,
    /* [in] */ Boolean value)
{
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfBoolean(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfString(
    /* [in] */ Integer index,
    /* [in] */ const String& value)
{
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfString(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfHANDLE(
    /* [in] */ Integer index,
    /* [in] */ HANDLE value)
{
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfHANDLE(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfECode(
    /* [in] */ Integer index,
    /* [in] */ ECode value)
{
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfECode(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfCoclassID(
    /* [in] */ Integer index,
    /* [in] */ const CoclassID& value)
{
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfCoclassID(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfComponentID(
    /* [in] */ Integer index,
    /* [in] */ const ComponentID& value)
{
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfComponentID(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfInterfaceID(
    /* [in] */ Integer index,
    /* [in] */ const InterfaceID& value)
{
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfInterfaceID(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfArray(
    /* [in] */ Integer index,
    /* [in] */ HANDLE value)
{
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfArray(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfEnumeration(
    /* [in] */ Integer index,
    /* [in] */ Integer value)
{
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfEnumeration(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfInterface(
    /* [in] */ Integer index,
    /* [in] */ IInterface* value)
{
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfInterface(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return NOERROR;
}

}
