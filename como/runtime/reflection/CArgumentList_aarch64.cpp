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

#include "reflection/CArgumentList_aarch64.h"
#include "util/comolog.h"
#include <cstdlib>

namespace como {

COMO_INTERFACE_IMPL_LIGHT_1(CArgumentList, LightRefBase, IArgumentList)

CArgumentList::CArgumentList(
    /* [in] */ const Array<IMetaParameter*>& parameters)
    : mParameterNumber(parameters.GetLength())
{
    Init(parameters);
}

CArgumentList::~CArgumentList()
{
    if (mParameterInfos != nullptr) {
        free(mParameterInfos);
    }
    if (mParameterBuffer != nullptr) {
        free(mParameterBuffer);
    }
}

template<typename T>
static inline T Get(Byte* buffer, Integer pos)
{
    return *reinterpret_cast<T*>(buffer + pos);
}

template<typename T>
static inline void Put(Byte* buffer, Integer pos, T value)
{
    *reinterpret_cast<T*>(buffer + pos) = value;
}

ECode CArgumentList::GetInputArgumentOfByte(
    /* [in] */ Integer index,
    /* [out] */ Byte& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Byte) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    value = Get<Byte>(mParameterBuffer, mParameterInfos[index].mPos);
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfByte(
    /* [in] */ Integer index,
    /* [in] */ Byte value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Byte) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<Byte>(mParameterBuffer, mParameterInfos[index].mPos, value);
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfShort(
    /* [in] */ Integer index,
    /* [out] */ Short& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Short) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    value = Get<Short>(mParameterBuffer, mParameterInfos[index].mPos);
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfShort(
    /* [in] */ Integer index,
    /* [in] */ Short value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Short) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<Short>(mParameterBuffer, mParameterInfos[index].mPos, value);
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfInteger(
    /* [in] */ Integer index,
    /* [out] */ Integer& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Integer) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    value = Get<Integer>(mParameterBuffer, mParameterInfos[index].mPos);
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfInteger(
    /* [in] */ Integer index,
    /* [in] */ Integer value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Integer) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<Integer>(mParameterBuffer, mParameterInfos[index].mPos, value);
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfLong(
    /* [in] */ Integer index,
    /* [out] */ Long& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Long) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    value = Get<Long>(mParameterBuffer, mParameterInfos[index].mPos);
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfLong(
    /* [in] */ Integer index,
    /* [in] */ Long value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Long) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<Long>(mParameterBuffer, mParameterInfos[index].mPos, value);
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfFloat(
    /* [in] */ Integer index,
    /* [out] */ Float& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Float) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    value = Get<Float>(mParameterBuffer, mParameterInfos[index].mPos);
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfFloat(
    /* [in] */ Integer index,
    /* [out] */ Float value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Float) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<Float>(mParameterBuffer, mParameterInfos[index].mPos, value);
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfDouble(
    /* [in] */ Integer index,
    /* [out] */ Double& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Double) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    value = Get<Double>(mParameterBuffer, mParameterInfos[index].mPos);
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfDouble(
    /* [in] */ Integer index,
    /* [in] */ Double value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Double) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<Double>(mParameterBuffer, mParameterInfos[index].mPos, value);
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfChar(
    /* [in] */ Integer index,
    /* [out] */ Char& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Char) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    value = Get<Char>(mParameterBuffer, mParameterInfos[index].mPos);
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfChar(
    /* [in] */ Integer index,
    /* [in] */ Char value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Char) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<Char>(mParameterBuffer, mParameterInfos[index].mPos, value);
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfBoolean(
    /* [in] */ Integer index,
    /* [out] */ Boolean& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Boolean) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    value = Get<Boolean>(mParameterBuffer, mParameterInfos[index].mPos);
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfBoolean(
    /* [in] */ Integer index,
    /* [in] */ Boolean value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Boolean) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<Boolean>(mParameterBuffer, mParameterInfos[index].mPos, value);
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfString(
    /* [in] */ Integer index,
    /* [out] */ String& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::String) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    value = *Get<String*>(mParameterBuffer, mParameterInfos[index].mPos);
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfString(
    /* [in] */ Integer index,
    /* [in] */ const String& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::String) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<String*>(mParameterBuffer, mParameterInfos[index].mPos, const_cast<String*>(&value));
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfHANDLE(
    /* [in] */ Integer index,
    /* [out] */ HANDLE& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::HANDLE) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    value = Get<HANDLE>(mParameterBuffer, mParameterInfos[index].mPos);
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfHANDLE(
    /* [in] */ Integer index,
    /* [in] */ HANDLE value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::HANDLE) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<HANDLE>(mParameterBuffer, mParameterInfos[index].mPos, value);
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfECode(
    /* [in] */ Integer index,
    /* [out] */ ECode& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::ECode) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    value = Get<ECode>(mParameterBuffer, mParameterInfos[index].mPos);
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfECode(
    /* [in] */ Integer index,
    /* [in] */ ECode value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::ECode) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<ECode>(mParameterBuffer, mParameterInfos[index].mPos, value);
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfCoclassID(
    /* [in] */ Integer index,
    /* [out] */ CoclassID& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::CoclassID) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    value = *Get<CoclassID*>(mParameterBuffer, mParameterInfos[index].mPos);
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfCoclassID(
    /* [in] */ Integer index,
    /* [in] */ const CoclassID& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::CoclassID) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<CoclassID*>(mParameterBuffer, mParameterInfos[index].mPos, const_cast<CoclassID*>(&value));
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfComponentID(
    /* [in] */ Integer index,
    /* [out] */ ComponentID& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::ComponentID) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    value = *Get<ComponentID*>(mParameterBuffer, mParameterInfos[index].mPos);
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfComponentID(
    /* [in] */ Integer index,
    /* [in] */ const ComponentID& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::ComponentID) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<ComponentID*>(mParameterBuffer, mParameterInfos[index].mPos, const_cast<ComponentID*>(&value));
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfInterfaceID(
    /* [in] */ Integer index,
    /* [out] */ InterfaceID& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::InterfaceID) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    value = *Get<InterfaceID*>(mParameterBuffer, mParameterInfos[index].mPos);
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfInterfaceID(
    /* [in] */ Integer index,
    /* [in] */ const InterfaceID& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::InterfaceID) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<InterfaceID*>(mParameterBuffer, mParameterInfos[index].mPos, const_cast<InterfaceID*>(&value));
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfArray(
    /* [in] */ Integer index,
    /* [out] */ Triple& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Array) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    value = *Get<Triple*>(mParameterBuffer, mParameterInfos[index].mPos);
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfArray(
    /* [in] */ Integer index,
    /* [in] */ const Triple& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Array) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<Triple*>(mParameterBuffer, mParameterInfos[index].mPos, const_cast<Triple*>(&value));
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfEnumeration(
    /* [in] */ Integer index,
    /* [out] */ Integer& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Enum) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    value = Get<Integer>(mParameterBuffer, mParameterInfos[index].mPos);
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfEnumeration(
    /* [in] */ Integer index,
    /* [in] */ Integer value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Enum) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<Integer>(mParameterBuffer, mParameterInfos[index].mPos, value);
    return NOERROR;
}

ECode CArgumentList::GetInputArgumentOfInterface(
    /* [in] */ Integer index,
    /* [out] */ AutoPtr<IInterface>& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Interface) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    value = Get<IInterface*>(mParameterBuffer, mParameterInfos[index].mPos);
    return NOERROR;
}

ECode CArgumentList::SetInputArgumentOfInterface(
    /* [in] */ Integer index,
    /* [in] */ IInterface* value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Interface) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::IN) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<IInterface*>(mParameterBuffer, mParameterInfos[index].mPos, value);
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfByte(
    /* [in] */ Integer index,
    /* [in] */ Byte value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Byte) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Byte* addr = Get<Byte*>(mParameterBuffer, mParameterInfos[index].mPos);
    *addr = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfByte(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Byte) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<HANDLE>(mParameterBuffer, mParameterInfos[index].mPos, addr);
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfShort(
    /* [in] */ Integer index,
    /* [in] */ Short value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Short) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Short* addr = Get<Short*>(mParameterBuffer, mParameterInfos[index].mPos);
    *addr = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfShort(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Short) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<HANDLE>(mParameterBuffer, mParameterInfos[index].mPos, addr);
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfInteger(
    /* [in] */ Integer index,
    /* [in] */ Integer value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Integer) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Integer* addr = Get<Integer*>(mParameterBuffer, mParameterInfos[index].mPos);
    *addr = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfInteger(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Integer) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<HANDLE>(mParameterBuffer, mParameterInfos[index].mPos, addr);
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfLong(
    /* [in] */ Integer index,
    /* [in] */ Long value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Long) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Long* addr = Get<Long*>(mParameterBuffer, mParameterInfos[index].mPos);
    *addr = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfLong(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Long) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<HANDLE>(mParameterBuffer, mParameterInfos[index].mPos, addr);
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfFloat(
    /* [in] */ Integer index,
    /* [in] */ Float value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Float) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Float* addr = Get<Float*>(mParameterBuffer, mParameterInfos[index].mPos);
    *addr = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfFloat(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Float) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<HANDLE>(mParameterBuffer, mParameterInfos[index].mPos, addr);
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfDouble(
    /* [in] */ Integer index,
    /* [in] */ Double value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Double) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Double* addr = Get<Double*>(mParameterBuffer, mParameterInfos[index].mPos);
    *addr = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfDouble(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Double) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<HANDLE>(mParameterBuffer, mParameterInfos[index].mPos, addr);
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfChar(
    /* [in] */ Integer index,
    /* [in] */ Char value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Char) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Char* addr = Get<Char*>(mParameterBuffer, mParameterInfos[index].mPos);
    *addr = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfChar(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Char) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<HANDLE>(mParameterBuffer, mParameterInfos[index].mPos, addr);
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfBoolean(
    /* [in] */ Integer index,
    /* [in] */ Boolean value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Boolean) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Boolean* addr = Get<Boolean*>(mParameterBuffer, mParameterInfos[index].mPos);
    *addr = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfBoolean(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Boolean) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<HANDLE>(mParameterBuffer, mParameterInfos[index].mPos, addr);
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfString(
    /* [in] */ Integer index,
    /* [in] */ const String& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::String) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    String* addr = Get<String*>(mParameterBuffer, mParameterInfos[index].mPos);
    *addr = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfString(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::String) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<HANDLE>(mParameterBuffer, mParameterInfos[index].mPos, addr);
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfHANDLE(
    /* [in] */ Integer index,
    /* [in] */ HANDLE value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::HANDLE) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    HANDLE* addr = Get<HANDLE*>(mParameterBuffer, mParameterInfos[index].mPos);
    *addr = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfHANDLE(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::HANDLE) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<HANDLE>(mParameterBuffer, mParameterInfos[index].mPos, addr);
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfECode(
    /* [in] */ Integer index,
    /* [in] */ ECode value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::ECode) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    ECode* addr = Get<ECode*>(mParameterBuffer, mParameterInfos[index].mPos);
    *addr = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfECode(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::ECode) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<HANDLE>(mParameterBuffer, mParameterInfos[index].mPos, addr);
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfCoclassID(
    /* [in] */ Integer index,
    /* [in] */ const CoclassID& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::CoclassID) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    CoclassID* addr = Get<CoclassID*>(mParameterBuffer, mParameterInfos[index].mPos);
    *addr = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfCoclassID(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::CoclassID) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<HANDLE>(mParameterBuffer, mParameterInfos[index].mPos, addr);
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfComponentID(
    /* [in] */ Integer index,
    /* [in] */ const ComponentID& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::ComponentID) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    ComponentID* addr = Get<ComponentID*>(mParameterBuffer, mParameterInfos[index].mPos);
    *addr = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfComponentID(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::ComponentID) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<HANDLE>(mParameterBuffer, mParameterInfos[index].mPos, addr);
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfInterfaceID(
    /* [in] */ Integer index,
    /* [in] */ const InterfaceID& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::InterfaceID) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    InterfaceID* addr = Get<InterfaceID*>(mParameterBuffer, mParameterInfos[index].mPos);
    *addr = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfInterfaceID(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::InterfaceID) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<HANDLE>(mParameterBuffer, mParameterInfos[index].mPos, addr);
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfArray(
    /* [in] */ Integer index,
    /* [in] */ const Triple& value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Array) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Triple* addr = Get<Triple*>(mParameterBuffer, mParameterInfos[index].mPos);
    *addr = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfArray(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Array) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<HANDLE>(mParameterBuffer, mParameterInfos[index].mPos, addr);
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfEnumeration(
    /* [in] */ Integer index,
    /* [in] */ Integer value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Enum) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Integer* addr = Get<Integer*>(mParameterBuffer, mParameterInfos[index].mPos);
    *addr = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfEnumeration(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Enum) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<HANDLE>(mParameterBuffer, mParameterInfos[index].mPos, addr);
    return NOERROR;
}

ECode CArgumentList::AssignOutputArgumentOfInterface(
    /* [in] */ Integer index,
    /* [in] */ IInterface* value)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Interface) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    IInterface** addr = Get<IInterface**>(mParameterBuffer, mParameterInfos[index].mPos);
    *addr = value;
    return NOERROR;
}

ECode CArgumentList::SetOutputArgumentOfInterface(
    /* [in] */ Integer index,
    /* [in] */ HANDLE addr)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mParameterInfos[index].mKind != TypeKind::Interface) {
        return E_TYPE_MISMATCH_EXCEPTION;
    }

    if (mParameterInfos[index].mIOAttr != IOAttribute::OUT &&
            mParameterInfos[index].mIOAttr != IOAttribute::IN_OUT) {
        return E_IOATTRIBUTE_MISMATCH_EXCEPTION;
    }

    Put<HANDLE>(mParameterBuffer, mParameterInfos[index].mPos, addr);
    return NOERROR;
}

ECode CArgumentList::GetArgumentAddress(
    /* [in] */ Integer index,
    /* [out] */ HANDLE& addr)
{
    if (index < 0 || index >= mParameterNumber) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    addr = reinterpret_cast<HANDLE>(mParameterBuffer + mParameterInfos[index].mPos);
    return NOERROR;
}

void CArgumentList::Init(
    /* [in] */ const Array<IMetaParameter*>& parameters)
{
    Integer bufferPos = 8; // for this pointer

    if (mParameterNumber > 0) {
        mParameterInfos = reinterpret_cast<ParameterInfo*>(malloc(sizeof(ParameterInfo) * mParameterNumber));
        if (mParameterInfos == nullptr) {
            Logger::E("CArgumentList", "Out of memory.");
            return;
        }

        for (Integer i = 0; i < mParameterNumber; i++) {
            InitParameterInfo(bufferPos, parameters[i], mParameterInfos[i]);
        }
    }

    mParameterBuffer = reinterpret_cast<Byte*>(malloc(sizeof(Byte) * bufferPos));
}

void CArgumentList::InitParameterInfo(
    /* [in, out] */ Integer& bufferPos,
    /* [in] */ IMetaParameter* parameter,
    /* [out] */ ParameterInfo& paramInfo)
{
    AutoPtr<IMetaType> type;
    parameter->GetType(type);
    parameter->GetIOAttribute(paramInfo.mIOAttr);
    type->GetTypeKind(paramInfo.mKind);
    type->GetTypeModification(paramInfo.mMode);
    if (paramInfo.mIOAttr == IOAttribute::IN) {
        switch(paramInfo.mKind) {
            case TypeKind::Char:
            case TypeKind::Byte:
            case TypeKind::Short:
            case TypeKind::Integer:
            case TypeKind::Float:
            case TypeKind::Boolean:
            case TypeKind::ECode:
            case TypeKind::Enum:
            case TypeKind::TypeKind:
                paramInfo.mPos = bufferPos;
                paramInfo.mSize = 4;
                break;
            case TypeKind::Long:
            case TypeKind::Double:
            case TypeKind::String:
            case TypeKind::CoclassID:
            case TypeKind::ComponentID:
            case TypeKind::InterfaceID:
            case TypeKind::HANDLE:
            case TypeKind::Array:
            case TypeKind::Interface:
            case TypeKind::Triple:
                paramInfo.mPos = ALIGN8(bufferPos);
                paramInfo.mSize = 8;
                break;
            default:
                CHECK(0);
                break;
        }
    }
    else {
        switch(paramInfo.mKind) {
            case TypeKind::Char:
            case TypeKind::Byte:
            case TypeKind::Short:
            case TypeKind::Integer:
            case TypeKind::Long:
            case TypeKind::Float:
            case TypeKind::Double:
            case TypeKind::Boolean:
            case TypeKind::String:
            case TypeKind::CoclassID:
            case TypeKind::ComponentID:
            case TypeKind::InterfaceID:
            case TypeKind::HANDLE:
            case TypeKind::ECode:
            case TypeKind::Enum:
            case TypeKind::Array:
            case TypeKind::Interface:
            case TypeKind::Triple:
            case TypeKind::TypeKind:
                paramInfo.mPos = ALIGN8(bufferPos);
                paramInfo.mSize = 8;
                break;
            default:
                CHECK(0);
                break;
        }
    }
    bufferPos += paramInfo.mSize;
}

} // namespace como
