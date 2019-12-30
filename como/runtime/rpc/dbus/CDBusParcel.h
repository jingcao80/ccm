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

#ifndef __COMO_CDBUSPARCEL_H__
#define __COMO_CDBUSPARCEL_H__

#include "util/comoobj.h"

namespace como {

extern const CoclassID CID_CDbusParcel;

COCLASS_ID(28208469-8814-49af-80f7-8b147391fb94)
class CDBusParcel
    : public Object
    , public IParcel
{
public:
    CDBusParcel();

    ~CDBusParcel();

    COMO_INTERFACE_DECL();

    COMO_OBJECT_DECL();

    ECode ReadChar(
        /* [out] */ Char& value) override;

    ECode WriteChar(
        /* [in] */ Char value) override;

    ECode ReadByte(
        /* [out] */ Byte& value) override;

    ECode WriteByte(
        /* [in] */ Byte value) override;

    ECode ReadShort(
        /* [out] */ Short& value) override;

    ECode WriteShort(
        /* [in] */ Short value) override;

    ECode ReadInteger(
        /* [out] */ Integer& value) override;

    ECode WriteInteger(
        /* [in] */ Integer value) override;

    ECode ReadLong(
        /* [out] */ Long& value) override;

    ECode WriteLong(
        /* [in] */ Long value) override;

    ECode ReadFloat(
        /* [out] */ Float& value) override;

    ECode WriteFloat(
        /* [in] */ Float value) override;

    ECode ReadDouble(
        /* [out] */ Double& value) override;

    ECode WriteDouble(
        /* [in] */ Double value) override;

    ECode ReadBoolean(
        /* [out] */ Boolean& value) override;

    ECode WriteBoolean(
        /* [in] */ Boolean value) override;

    ECode ReadString(
        /* [out] */ String& value) override;

    ECode WriteString(
        /* [in] */ const String& value) override;

    ECode ReadCoclassID(
        /* [out] */ CoclassID& value) override;

    ECode WriteCoclassID(
        /* [in] */ const CoclassID& value) override;

    ECode ReadComponentID(
        /* [out] */ ComponentID& value) override;

    ECode WriteComponentID(
        /* [in] */ const ComponentID& value) override;

    ECode ReadInterfaceID(
        /* [out] */ InterfaceID& value) override;

    ECode WriteInterfaceID(
        /* [in] */ const InterfaceID& value) override;

    ECode ReadECode(
        /* [out] */ ECode& value) override;

    ECode WriteECode(
        /* [in] */ ECode value) override;

    ECode ReadEnumeration(
        /* [out] */ Integer& value) override;

    ECode WriteEnumeration(
        /* [in] */ Integer value) override;

    ECode ReadArray(
        /* [out] */ Triple* array) override;

    ECode WriteArray(
        /* [in] */ const Triple& array) override;

    ECode ReadInterface(
        /* [out] */ AutoPtr<IInterface>& value) override;

    ECode WriteInterface(
        /* [in] */ IInterface* value) override;

    ECode GetData(
        /* [out] */ HANDLE& data) override;

    ECode GetDataSize(
        /* [out] */ Long& size) override;

    ECode SetData(
        /* [in] */ HANDLE data,
        /* [in] */ Long size) override;

    ECode GetDataPosition(
        /* [out] */ Long& pos) override;

    ECode SetDataPosition(
        /* [in] */ Long pos) override;

    ECode GetPayload(
        /* [out] */ HANDLE& payload) override;

    ECode SetPayload(
        /* [in] */ HANDLE payload,
        /* [in] */ Boolean release) override;

    static ECode CreateObject(
        /* [out] */ AutoPtr<IParcel>& parcel);

private:
    ECode Read(
        /* [in] */ void* outData,
        /* [in] */ Long len) const;

    const void* ReadInplace(
        /* [in] */ Long len) const;

    ECode Write(
        /* [in] */ const void* data,
        /* [in] */ Long len);

    void* WriteInplace(
        /* [in] */ Long len);

    ECode FinishWrite(
        /* [in] */ Long len);

    ECode GrowData(
        /* [in] */ Long len);

    ECode RestartWrite(
        /* [in] */ Long desired);

    ECode ContinueWrite(
        /* [in] */ Long desired);

    template<class T>
    ECode ReadAligned(
        /* [out] */ T* value) const;

    template<class T>
    ECode WriteAligned(
        /* [in] */ T value);

private:
    static constexpr Integer TAG_NULL = 0;
    static constexpr Integer TAG_NOT_NULL = 1;

    ECode mError;
    Byte* mData;
    Long mDataSize;
    Long mDataCapacity;
    mutable Long mDataPos;
};

} // namespace como

#endif //__COMO_CDBUSPARCEL_H__
