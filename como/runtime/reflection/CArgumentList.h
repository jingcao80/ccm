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

#ifndef __COMO_CARGUMENTLIST_H__
#define __COMO_CARGUMENTLIST_H__

#include "metadata/Component.h"
#include "reflection/reflection.h"
#include "util/comoref.h"

namespace como {

class CArgumentList
    : public LightRefBase
    , public IArgumentList
{
    friend class CMetaConstructor;
    friend class CMetaMethod;

public:
    CArgumentList(
        /* [in] */ const Array<IMetaParameter*>& parameters);

    ~CArgumentList();

    COMO_INTERFACE_DECL();

    ECode GetInputArgumentOfByte(
        /* [in] */ Integer index,
        /* [out] */ Byte& value) override;

    ECode SetInputArgumentOfByte(
        /* [in] */ Integer index,
        /* [in] */ Byte value) override;

    ECode GetInputArgumentOfShort(
        /* [in] */ Integer index,
        /* [out] */ Short& value) override;

    ECode SetInputArgumentOfShort(
        /* [in] */ Integer index,
        /* [in] */ Short value) override;

    ECode GetInputArgumentOfInteger(
        /* [in] */ Integer index,
        /* [out] */ Integer& value) override;

    ECode SetInputArgumentOfInteger(
        /* [in] */ Integer index,
        /* [in] */ Integer value) override;

    ECode GetInputArgumentOfLong(
        /* [in] */ Integer index,
        /* [out] */ Long& value) override;

    ECode SetInputArgumentOfLong(
        /* [in] */ Integer index,
        /* [in] */ Long value) override;

    ECode GetInputArgumentOfFloat(
        /* [in] */ Integer index,
        /* [out] */ Float& value) override;

    ECode SetInputArgumentOfFloat(
        /* [in] */ Integer index,
        /* [out] */ Float value) override;

    ECode GetInputArgumentOfDouble(
        /* [in] */ Integer index,
        /* [out] */ Double& value) override;

    ECode SetInputArgumentOfDouble(
        /* [in] */ Integer index,
        /* [in] */ Double value) override;

    ECode GetInputArgumentOfChar(
        /* [in] */ Integer index,
        /* [out] */ Char& value) override;

    ECode SetInputArgumentOfChar(
        /* [in] */ Integer index,
        /* [in] */ Char value) override;

    ECode GetInputArgumentOfBoolean(
        /* [in] */ Integer index,
        /* [out] */ Boolean& value) override;

    ECode SetInputArgumentOfBoolean(
        /* [in] */ Integer index,
        /* [in] */ Boolean value) override;

    ECode GetInputArgumentOfString(
        /* [in] */ Integer index,
        /* [out] */ String& value) override;

    ECode SetInputArgumentOfString(
        /* [in] */ Integer index,
        /* [in] */ const String& value) override;

    ECode GetInputArgumentOfHANDLE(
        /* [in] */ Integer index,
        /* [out] */ HANDLE& value) override;

    ECode SetInputArgumentOfHANDLE(
        /* [in] */ Integer index,
        /* [in] */ HANDLE value) override;

    ECode GetInputArgumentOfECode(
        /* [in] */ Integer index,
        /* [out] */ ECode& value) override;

    ECode SetInputArgumentOfECode(
        /* [in] */ Integer index,
        /* [in] */ ECode value) override;

    ECode GetInputArgumentOfCoclassID(
        /* [in] */ Integer index,
        /* [out] */ CoclassID& value) override;

    ECode SetInputArgumentOfCoclassID(
        /* [in] */ Integer index,
        /* [in] */ const CoclassID& value) override;

    ECode GetInputArgumentOfComponentID(
        /* [in] */ Integer index,
        /* [out] */ ComponentID& value) override;

    ECode SetInputArgumentOfComponentID(
        /* [in] */ Integer index,
        /* [in] */ const ComponentID& value) override;

    ECode GetInputArgumentOfInterfaceID(
        /* [in] */ Integer index,
        /* [out] */ InterfaceID& value) override;

    ECode SetInputArgumentOfInterfaceID(
        /* [in] */ Integer index,
        /* [in] */ const InterfaceID& value) override;

    ECode GetInputArgumentOfArray(
        /* [in] */ Integer index,
        /* [out] */ Triple& value) override;

    ECode SetInputArgumentOfArray(
        /* [in] */ Integer index,
        /* [in] */ const Triple& value) override;

    ECode GetInputArgumentOfEnumeration(
        /* [in] */ Integer index,
        /* [out] */ Integer& value) override;

    ECode SetInputArgumentOfEnumeration(
        /* [in] */ Integer index,
        /* [in] */ Integer value) override;

    ECode GetInputArgumentOfInterface(
        /* [in] */ Integer index,
        /* [out] */ AutoPtr<IInterface>& value) override;

    ECode SetInputArgumentOfInterface(
        /* [in] */ Integer index,
        /* [in] */ IInterface* value) override;

    ECode AssignOutputArgumentOfByte(
        /* [in] */ Integer index,
        /* [in] */ Byte value) override;

    ECode SetOutputArgumentOfByte(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr) override;

    ECode AssignOutputArgumentOfShort(
        /* [in] */ Integer index,
        /* [in] */ Short value) override;

    ECode SetOutputArgumentOfShort(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr) override;

    ECode AssignOutputArgumentOfInteger(
        /* [in] */ Integer index,
        /* [in] */ Integer value) override;

    ECode SetOutputArgumentOfInteger(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr) override;

    ECode AssignOutputArgumentOfLong(
        /* [in] */ Integer index,
        /* [in] */ Long value) override;

    ECode SetOutputArgumentOfLong(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr) override;

    ECode AssignOutputArgumentOfFloat(
        /* [in] */ Integer index,
        /* [in] */ Float value) override;

    ECode SetOutputArgumentOfFloat(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr) override;

    ECode AssignOutputArgumentOfDouble(
        /* [in] */ Integer index,
        /* [in] */ Double value) override;

    ECode SetOutputArgumentOfDouble(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr) override;

    ECode AssignOutputArgumentOfChar(
        /* [in] */ Integer index,
        /* [in] */ Char value) override;

    ECode SetOutputArgumentOfChar(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr) override;

    ECode AssignOutputArgumentOfBoolean(
        /* [in] */ Integer index,
        /* [in] */ Boolean value) override;

    ECode SetOutputArgumentOfBoolean(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr) override;

    ECode AssignOutputArgumentOfString(
        /* [in] */ Integer index,
        /* [in] */ const String& value) override;

    ECode SetOutputArgumentOfString(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr) override;

    ECode AssignOutputArgumentOfHANDLE(
        /* [in] */ Integer index,
        /* [in] */ HANDLE value) override;

    ECode SetOutputArgumentOfHANDLE(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr) override;

    ECode AssignOutputArgumentOfECode(
        /* [in] */ Integer index,
        /* [in] */ ECode value) override;

    ECode SetOutputArgumentOfECode(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr) override;

    ECode AssignOutputArgumentOfCoclassID(
        /* [in] */ Integer index,
        /* [in] */ const CoclassID& value) override;

    ECode SetOutputArgumentOfCoclassID(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr) override;

    ECode AssignOutputArgumentOfComponentID(
        /* [in] */ Integer index,
        /* [in] */ const ComponentID& value) override;

    ECode SetOutputArgumentOfComponentID(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr) override;

    ECode AssignOutputArgumentOfInterfaceID(
        /* [in] */ Integer index,
        /* [in] */ const InterfaceID& value) override;

    ECode SetOutputArgumentOfInterfaceID(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr) override;

    ECode AssignOutputArgumentOfArray(
        /* [in] */ Integer index,
        /* [in] */ const Triple& value) override;

    ECode SetOutputArgumentOfArray(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr) override;

    ECode AssignOutputArgumentOfEnumeration(
        /* [in] */ Integer index,
        /* [in] */ Integer value) override;

    ECode SetOutputArgumentOfEnumeration(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr) override;

    ECode AssignOutputArgumentOfInterface(
        /* [in] */ Integer index,
        /* [in] */ IInterface* value) override;

    ECode SetOutputArgumentOfInterface(
        /* [in] */ Integer index,
        /* [in] */ HANDLE addr) override;

    ECode GetArgumentAddress(
        /* [in] */ Integer index,
        /* [out] */ HANDLE& addr) override;

private:
    void Init(
        /* [in] */ const Array<IMetaParameter*>& parameters);

    void InitParameterInfo(
        /* [in, out] */ Integer& bufferPos,
        /* [in] */ IMetaParameter* parameter,
        /* [out] */ ParameterInfo& paramInfo);

    inline Integer GetParameterNumber();

    inline ParameterInfo* GetParameterInfos();

    inline Byte* GetParameterBuffer();

private:
    Integer mParameterNumber;
    ParameterInfo* mParameterInfos = nullptr;
    Byte* mParameterBuffer = nullptr;
};

Integer CArgumentList::GetParameterNumber()
{
    return mParameterNumber;
}

ParameterInfo* CArgumentList::GetParameterInfos()
{
    return mParameterInfos;
}

Byte* CArgumentList::GetParameterBuffer()
{
    return mParameterBuffer;
}

} // namespace como

#endif // __COMO_CARGUMENTLIST_H__
