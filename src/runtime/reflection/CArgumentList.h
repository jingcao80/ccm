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

#ifndef __CCM_CARGUMENTLIST_H__
#define __CCM_CARGUMENTLIST_H__

#include "ccmrefbase.h"
#include "Component.h"

using ccm::metadata::MetaComponent;
using ccm::metadata::MetaMethod;

namespace ccm {

class CArgumentList
    : public LightRefBase
    , public IArgumentList
{
public:
    CArgumentList(
        /* [in] */ MetaComponent* mc,
        /* [in] */ MetaMethod* mm);

    ~CArgumentList();

    CCM_INTERFACE_DECL();

    ECode GetInputArgumentOfByte(
        /* [in] */ Integer index,
        /* [out] */ Byte* value);

    ECode SetInputArgumentOfByte(
        /* [in] */ Integer index,
        /* [in] */ Byte value);

    ECode GetInputArgumentOfShort(
        /* [in] */ Integer index,
        /* [out] */ Short* value);

    ECode SetInputArgumentOfShort(
        /* [in] */ Integer index,
        /* [in] */ Short value);

    ECode GetInputArgumentOfInteger(
        /* [in] */ Integer index,
        /* [out] */ Integer* value);

    ECode SetInputArgumentOfInteger(
        /* [in] */ Integer index,
        /* [in] */ Integer value);

    ECode GetInputArgumentOfLong(
        /* [in] */ Integer index,
        /* [out] */ Long* value);

    ECode SetInputArgumentOfLong(
        /* [in] */ Integer index,
        /* [in] */ Long value);

    ECode GetInputArgumentOfFloat(
        /* [in] */ Integer index,
        /* [out] */ Float* value);

    ECode SetInputArgumentOfFloat(
        /* [in] */ Integer index,
        /* [out] */ Float value);

    ECode GetInputArgumentOfDouble(
        /* [in] */ Integer index,
        /* [out] */ Double* value);

    ECode SetInputArgumentOfDouble(
        /* [in] */ Integer index,
        /* [in] */ Double value);

    ECode GetInputArgumentOfChar(
        /* [in] */ Integer index,
        /* [out] */ Char* value);

    ECode SetInputArgumentOfChar(
        /* [in] */ Integer index,
        /* [in] */ Char value);

    ECode GetInputArgumentOfBoolean(
        /* [in] */ Integer index,
        /* [out] */ Boolean* value);

    ECode SetInputArgumentOfBoolean(
        /* [in] */ Integer index,
        /* [in] */ Boolean value);

    ECode GetInputArgumentOfString(
        /* [in] */ Integer index,
        /* [out] */ String* value);

    ECode SetInputArgumentOfString(
        /* [in] */ Integer index,
        /* [in] */ const String& value);

    ECode GetInputArgumentOfHANDLE(
        /* [in] */ Integer index,
        /* [out] */ HANDLE* value);

    ECode SetInputArgumentOfHANDLE(
        /* [in] */ Integer index,
        /* [in] */ HANDLE value);

    ECode GetInputArgumentOfECode(
        /* [in] */ Integer index,
        /* [out] */ ECode* value);

    ECode SetInputArgumentOfECode(
        /* [in] */ Integer index,
        /* [in] */ ECode value);

    ECode GetInputArgumentOfCoclassID(
        /* [in] */ Integer index,
        /* [out] */ CoclassID* value);

    ECode SetInputArgumentOfCoclassID(
        /* [in] */ Integer index,
        /* [in] */ const CoclassID& value);

    ECode GetInputArgumentOfComponentID(
        /* [in] */ Integer index,
        /* [out] */ ComponentID* value);

    ECode SetInputArgumentOfComponentID(
        /* [in] */ Integer index,
        /* [in] */ const ComponentID& value);

    ECode GetInputArgumentOfInterfaceID(
        /* [in] */ Integer index,
        /* [out] */ InterfaceID* value);

    ECode SetInputArgumentOfInterfaceID(
        /* [in] */ Integer index,
        /* [in] */ const InterfaceID& value);

    ECode GetInputArgumentOfArray(
        /* [in] */ Integer index,
        /* [out] */ HANDLE* value);

    ECode SetInputArgumentOfArray(
        /* [in] */ Integer index,
        /* [in] */ HANDLE value);

    ECode GetInputArgumentOfEnumeration(
        /* [in] */ Integer index,
        /* [out] */ Integer* value);

    ECode SetInputArgumentOfEnumeration(
        /* [in] */ Integer index,
        /* [in] */ Integer value);

    ECode GetInputArgumentOfInterface(
        /* [in] */ Integer index,
        /* [out] */ IInterface** value);

    ECode SetInputArgumentOfInterface(
        /* [in] */ Integer index,
        /* [in] */ IInterface* value);

    ECode AssignOutputArgumentOfByte(
        /* [in] */ Integer index,
        /* [in] */ Byte value);

    ECode SetOutputArgumentOfByte(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr);

    ECode AssignOutputArgumentOfShort(
        /* [in] */ Integer index,
        /* [in] */ Short value);

    ECode SetOutputArgumentOfShort(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr);

    ECode AssignOutputArgumentOfInteger(
        /* [in] */ Integer index,
        /* [in] */ Integer value);

    ECode SetOutputArgumentOfInteger(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr);

    ECode AssignOutputArgumentOfLong(
        /* [in] */ Integer index,
        /* [in] */ Long value);

    ECode SetOutputArgumentOfLong(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr);

    ECode AssignOutputArgumentOfFloat(
        /* [in] */ Integer index,
        /* [in] */ Float value);

    ECode SetOutputArgumentOfFloat(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr);

    ECode AssignOutputArgumentOfDouble(
        /* [in] */ Integer index,
        /* [in] */ Double value);

    ECode SetOutputArgumentOfDouble(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr);

    ECode AssignOutputArgumentOfChar(
        /* [in] */ Integer index,
        /* [in] */ Char value);

    ECode SetOutputArgumentOfChar(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr);

    ECode AssignOutputArgumentOfBoolean(
        /* [in] */ Integer index,
        /* [in] */ Boolean value);

    ECode SetOutputArgumentOfBoolean(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr);

    ECode AssignOutputArgumentOfString(
        /* [in] */ Integer index,
        /* [in] */ const String& value);

    ECode SetOutputArgumentOfString(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr);

    ECode AssignOutputArgumentOfHANDLE(
        /* [in] */ Integer index,
        /* [in] */ HANDLE value);

    ECode SetOutputArgumentOfHANDLE(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr);

    ECode AssignOutputArgumentOfECode(
        /* [in] */ Integer index,
        /* [in] */ ECode value);

    ECode SetOutputArgumentOfECode(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr);

    ECode AssignOutputArgumentOfCoclassID(
        /* [in] */ Integer index,
        /* [in] */ const CoclassID& value);

    ECode SetOutputArgumentOfCoclassID(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr);

    ECode AssignOutputArgumentOfComponentID(
        /* [in] */ Integer index,
        /* [in] */ const ComponentID& value);

    ECode SetOutputArgumentOfComponentID(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr);

    ECode AssignOutputArgumentOfInterfaceID(
        /* [in] */ Integer index,
        /* [in] */ const InterfaceID& value);

    ECode SetOutputArgumentOfInterfaceID(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr);

    ECode AssignOutputArgumentOfArray(
        /* [in] */ Integer index,
        /* [in] */ HANDLE value);

    ECode SetOutputArgumentOfArray(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr);

    ECode AssignOutputArgumentOfEnumeration(
        /* [in] */ Integer index,
        /* [in] */ Integer value);

    ECode SetOutputArgumentOfEnumeration(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr);

    ECode AssignOutputArgumentOfInterface(
        /* [in] */ Integer index,
        /* [in] */ IInterface* value);

    ECode SetOutputArgumentOfInterface(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr);

    void* GetData();

private:
    Integer CalculateBufferSize();

public:
    void* mData;
};

}

#endif // __CCM_CARGUMENTLIST_H__
