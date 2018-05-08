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
#include <stdlib.h>

using ccm::metadata::MetaType;

namespace ccm {

CCM_INTERFACE_IMPL_LIGHT_1(CArgumentList, IArgumentList)

CArgumentList::CArgumentList(
    /* [in] */ MetaComponent* mc,
    /* [in] */ MetaMethod* mm)
    : mArgumentNumber(mm->mParameterNumber)
    , mArgumentIndicators(nullptr)
    , mIntegerData(nullptr)
    , mFPData(nullptr)
    , mStackData(nullptr)
{
    CalculateDataSize(mc, mm,
            &mIntegerDataNumber, &mFPDataNumber, &mStackDataNumber);
    if (mIntegerDataNumber > 0) {
        mIntegerData = (Long*)malloc(sizeof(Long) * mIntegerDataNumber);
    }
    if (mFPDataNumber > 0) {
        mFPData = (Double*)malloc(sizeof(Double) * mFPDataNumber);
    }
    if (mStackDataNumber > 0) {
        mStackData = (Long*)malloc(sizeof(Long) * mStackDataNumber);
    }
}

CArgumentList::~CArgumentList()
{
    if (mArgumentIndicators != nullptr) {
        free(mArgumentIndicators);
        mArgumentIndicators = nullptr;
    }
    if (mIntegerData != nullptr) {
        free(mIntegerData);
        mIntegerData = nullptr;
    }
    if (mFPData != nullptr) {
        free(mFPData);
        mFPData = nullptr;
    }
    if (mStackData != nullptr) {
        free(mStackData);
        mStackData = nullptr;
    }
}

ECode CArgumentList::GetInputArgumentOfByte(
    /* [in] */ Integer index,
    /* [out] */ Byte* value)
{
    VALIDATE_NOT_NULL(value);

    if (index < 0 || index >= mArgumentNumber) {
        *value = 0;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *value = (Byte)data[pos];
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfByte(
    /* [in] */ Integer index,
    /* [in] */ Byte value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    data[pos] = value;
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfShort(
    /* [in] */ Integer index,
    /* [out] */ Short* value)
{
    VALIDATE_NOT_NULL(value);

    if (index < 0 || index >= mArgumentNumber) {
        *value = 0;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *value = (Short)data[pos];
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfShort(
    /* [in] */ Integer index,
    /* [in] */ Short value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    data[pos] = value;
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfInteger(
    /* [in] */ Integer index,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *value = (Integer)data[pos];
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfInteger(
    /* [in] */ Integer index,
    /* [in] */ Integer value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    data[pos] = value;
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfLong(
    /* [in] */ Integer index,
    /* [out] */ Long* value)
{
    VALIDATE_NOT_NULL(value);

    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *value = data[pos];
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfLong(
    /* [in] */ Integer index,
    /* [in] */ Long value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    data[pos] = value;
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfFloat(
    /* [in] */ Integer index,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);

    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Double* data = (Double*)GetDataBuffer(index);
    *value = (Float)data[pos];
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfFloat(
    /* [in] */ Integer index,
    /* [out] */ Float value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Double* data = (Double*)GetDataBuffer(index);
    data[pos] = value;
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfDouble(
    /* [in] */ Integer index,
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);

    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Double* data = (Double*)GetDataBuffer(index);
    *value = data[pos];
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfDouble(
    /* [in] */ Integer index,
    /* [in] */ Double value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Double* data = (Double*)GetDataBuffer(index);
    data[pos] = value;
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfChar(
    /* [in] */ Integer index,
    /* [out] */ Char* value)
{
    VALIDATE_NOT_NULL(value);

    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *value = (Char)data[pos];
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfChar(
    /* [in] */ Integer index,
    /* [in] */ Char value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    data[pos] = value;
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfBoolean(
    /* [in] */ Integer index,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *value = (Boolean)data[pos];
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfBoolean(
    /* [in] */ Integer index,
    /* [in] */ Boolean value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    data[pos] = value;
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfString(
    /* [in] */ Integer index,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *value = *reinterpret_cast<String*>(data[pos]);
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfString(
    /* [in] */ Integer index,
    /* [in] */ const String& value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    data[pos] = reinterpret_cast<Long>(&value);
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfHANDLE(
    /* [in] */ Integer index,
    /* [out] */ HANDLE* value)
{
    VALIDATE_NOT_NULL(value);

    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *value = (HANDLE)data[pos];
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfHANDLE(
    /* [in] */ Integer index,
    /* [in] */ HANDLE value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    data[pos] = value;
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfECode(
    /* [in] */ Integer index,
    /* [out] */ ECode* value)
{
    VALIDATE_NOT_NULL(value);

    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *value = (ECode)data[pos];
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfECode(
    /* [in] */ Integer index,
    /* [in] */ ECode value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    data[pos] = value;
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfCoclassID(
    /* [in] */ Integer index,
    /* [out] */ CoclassID* value)
{
    VALIDATE_NOT_NULL(value);

    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *value = *reinterpret_cast<CoclassID*>(data[pos]);
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfCoclassID(
    /* [in] */ Integer index,
    /* [in] */ const CoclassID& value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    data[pos] = reinterpret_cast<Long>(&value);
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfComponentID(
    /* [in] */ Integer index,
    /* [out] */ ComponentID* value)
{
    VALIDATE_NOT_NULL(value);

    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *value = *reinterpret_cast<ComponentID*>(data[pos]);
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfComponentID(
    /* [in] */ Integer index,
    /* [in] */ const ComponentID& value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    data[pos] = reinterpret_cast<Long>(&value);
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfInterfaceID(
    /* [in] */ Integer index,
    /* [out] */ InterfaceID* value)
{
    VALIDATE_NOT_NULL(value);

    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *value = *reinterpret_cast<InterfaceID*>(data[pos]);
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfInterfaceID(
    /* [in] */ Integer index,
    /* [in] */ const InterfaceID& value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    data[pos] = reinterpret_cast<Long>(&value);
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfArray(
    /* [in] */ Integer index,
    /* [out] */ HANDLE* value)
{
    VALIDATE_NOT_NULL(value);

    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *value = (HANDLE)data[pos];
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfArray(
    /* [in] */ Integer index,
    /* [in] */ HANDLE value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    data[pos] = value;
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfEnumeration(
    /* [in] */ Integer index,
    /* [out] */ Integer* value)
{
    return GetInputArgumentOfInteger(index, value);
}

ECode CArgumentList::SetInputArgumentOfEnumeration(
    /* [in] */ Integer index,
    /* [in] */ Integer value)
{
    return SetInputArgumentOfInteger(index, value);
}

ECode CArgumentList::GetInputArgumentOfInterface(
    /* [in] */ Integer index,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);

    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *value = reinterpret_cast<IInterface*>(data[pos]);
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfInterface(
    /* [in] */ Integer index,
    /* [in] */ IInterface* value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    data[pos] = reinterpret_cast<Long>(value);
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfByte(
    /* [in] */ Integer index,
    /* [in] */ Byte value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *reinterpret_cast<Byte*>(data[pos]) = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfByte(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return SetOutputArgumentAddr(index, addr);
}

ECode CArgumentList::AssignOutputArgumentOfShort(
    /* [in] */ Integer index,
    /* [in] */ Short value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *reinterpret_cast<Short*>(data[pos]) = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfShort(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return SetOutputArgumentAddr(index, addr);
}

ECode CArgumentList::AssignOutputArgumentOfInteger(
    /* [in] */ Integer index,
    /* [in] */ Integer value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *reinterpret_cast<Integer*>(data[pos]) = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfInteger(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return SetOutputArgumentAddr(index, addr);
}

ECode CArgumentList::AssignOutputArgumentOfLong(
    /* [in] */ Integer index,
    /* [in] */ Long value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *reinterpret_cast<Long*>(data[pos]) = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfLong(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return SetOutputArgumentAddr(index, addr);
}

ECode CArgumentList::AssignOutputArgumentOfFloat(
    /* [in] */ Integer index,
    /* [in] */ Float value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *reinterpret_cast<Float*>(data[pos]) = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfFloat(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return SetOutputArgumentAddr(index, addr);
}

ECode CArgumentList::AssignOutputArgumentOfDouble(
    /* [in] */ Integer index,
    /* [in] */ Double value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *reinterpret_cast<Double*>(data[pos]) = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfDouble(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return SetOutputArgumentAddr(index, addr);
}

ECode CArgumentList::AssignOutputArgumentOfChar(
    /* [in] */ Integer index,
    /* [in] */ Char value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *reinterpret_cast<Char*>(data[pos]) = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfChar(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return SetOutputArgumentAddr(index, addr);
}

ECode CArgumentList::AssignOutputArgumentOfBoolean(
    /* [in] */ Integer index,
    /* [in] */ Boolean value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *reinterpret_cast<Boolean*>(data[pos]) = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfBoolean(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return SetOutputArgumentAddr(index, addr);
}

ECode CArgumentList::AssignOutputArgumentOfString(
    /* [in] */ Integer index,
    /* [in] */ const String& value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *reinterpret_cast<String*>(data[pos]) = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfString(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return SetOutputArgumentAddr(index, addr);
}

ECode CArgumentList::AssignOutputArgumentOfHANDLE(
    /* [in] */ Integer index,
    /* [in] */ HANDLE value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *reinterpret_cast<HANDLE*>(data[pos]) = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfHANDLE(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return SetOutputArgumentAddr(index, addr);
}

ECode CArgumentList::AssignOutputArgumentOfECode(
    /* [in] */ Integer index,
    /* [in] */ ECode value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *reinterpret_cast<ECode*>(data[pos]) = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfECode(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return SetOutputArgumentAddr(index, addr);
}

ECode CArgumentList::AssignOutputArgumentOfCoclassID(
    /* [in] */ Integer index,
    /* [in] */ const CoclassID& value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *reinterpret_cast<CoclassID*>(data[pos]) = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfCoclassID(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return SetOutputArgumentAddr(index, addr);
}

ECode CArgumentList::AssignOutputArgumentOfComponentID(
    /* [in] */ Integer index,
    /* [in] */ const ComponentID& value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *reinterpret_cast<ComponentID*>(data[pos]) = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfComponentID(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return SetOutputArgumentAddr(index, addr);
}

ECode CArgumentList::AssignOutputArgumentOfInterfaceID(
    /* [in] */ Integer index,
    /* [in] */ const InterfaceID& value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *reinterpret_cast<InterfaceID*>(data[pos]) = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfInterfaceID(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return SetOutputArgumentAddr(index, addr);
}

ECode CArgumentList::AssignOutputArgumentOfArray(
    /* [in] */ Integer index,
    /* [in] */ HANDLE value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *reinterpret_cast<Triple*>(data[pos]) = *reinterpret_cast<Triple*>(value);
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfArray(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return SetOutputArgumentAddr(index, addr);
}

ECode CArgumentList::AssignOutputArgumentOfEnumeration(
    /* [in] */ Integer index,
    /* [in] */ Integer value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    *reinterpret_cast<Integer*>(data[pos]) = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfEnumeration(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return SetOutputArgumentAddr(index, addr);
}

ECode CArgumentList::AssignOutputArgumentOfInterface(
    /* [in] */ Integer index,
    /* [in] */ IInterface* value)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    REFCOUNT_ADD(value);
    REFCOUNT_RELEASE(*reinterpret_cast<IInterface**>(data[pos]));
    *reinterpret_cast<IInterface**>(data[pos]) = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfInterface(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    return SetOutputArgumentAddr(index, addr);
}

ECode CArgumentList::GetArgumentAddress(
    /* [in] */ Integer index,
    /* [out] */ HANDLE* addr)
{
    return NOERROR;
}

void CArgumentList::CalculateDataSize(
    /* [in] */ MetaComponent* mc,
    /* [in] */ MetaMethod* mm,
    /* [out] */ Integer* intDataNum,
    /* [out] */ Integer* fpDataNum,
    /* [out] */ Integer* stDataNum)
{
    if (mArgumentNumber > 0) {
        mArgumentIndicators =
                (Short*)malloc(sizeof(Short) * mArgumentNumber);
    }
    *intDataNum = 1;
    *fpDataNum = 0;
    *stDataNum = 0;
    for (Integer i = 0; i < mArgumentNumber; i++) {
        MetaType* mt = mc->mTypes[mm->mParameters[i]->mTypeIndex];
        switch (mt->mKind) {
            case CcmTypeKind::Char:
            case CcmTypeKind::Byte:
            case CcmTypeKind::Short:
            case CcmTypeKind::Integer:
            case CcmTypeKind::Long:
            case CcmTypeKind::Boolean:
            case CcmTypeKind::String:
            case CcmTypeKind::CoclassID:
            case CcmTypeKind::ComponentID:
            case CcmTypeKind::InterfaceID:
            case CcmTypeKind::HANDLE:
            case CcmTypeKind::ECode:
            case CcmTypeKind::Enum:
            case CcmTypeKind::Array:
            case CcmTypeKind::Interface: {
                if (*intDataNum < 6) {
                    mArgumentIndicators[i] = (INT_DATA << DATA_SHIFT) | *intDataNum;
                    (*intDataNum)++;
                }
                else {
                    mArgumentIndicators[i] = (STK_DATA << DATA_SHIFT) | *stDataNum;
                    (*stDataNum)++;
                }
                break;
            }
            case CcmTypeKind::Float:
            case CcmTypeKind::Double: {
                if (*fpDataNum < 8) {
                    mArgumentIndicators[i] = (FP_DATA << DATA_SHIFT) | *fpDataNum;
                    (*fpDataNum)++;
                }
                else {
                    mArgumentIndicators[i] = (STK_DATA << DATA_SHIFT) | *stDataNum;
                    (*stDataNum)++;
                }
                break;
            }
        }
    }
}

void* CArgumentList::GetDataBuffer(
    /* [in] */ Integer index)
{
    Byte tag = ((unsigned short)(mArgumentIndicators[index] & DATA_MASK)) >> DATA_SHIFT;
    if (tag == INT_DATA) return mIntegerData;
    else if (tag == FP_DATA) return mFPData;
    else return mStackData;
}

Integer CArgumentList::GetPos(
    /* [in] */ Integer index)
{
    return mArgumentIndicators[index] & ~DATA_MASK;
}

ECode CArgumentList::SetOutputArgumentAddr(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    if (index < 0 || index >= mArgumentNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer pos = GetPos(index);
    Long* data = (Long*)GetDataBuffer(index);
    data[pos] = addr;
    return NOERROR;
}

Long* CArgumentList::GetIntegerData(
    /* [out] */ Integer* number)
{
    *number = mIntegerDataNumber;
    return mIntegerData;
}

Double* CArgumentList::GetFPData(
    /* [out] */ Integer* number)
{
    *number = mFPDataNumber;
    return mFPData;
}

Long* CArgumentList::GetStackData(
    /* [out] */ Integer* number)
{
    *number = mStackDataNumber;
    return mStackData;
}

}
