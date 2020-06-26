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

#include "ZoneInfoTestHelper.h"
#include "como/io/ByteBufferFactory.h"
#include "como.core.CInteger.h"
#include "como.core.IArrayHolder.h"
#include "como.io.CByteArrayOutputStream.h"
#include "como.io.IBuffer.h"
#include "como.io.IByteBuffer.h"
#include "como.util.CArrayList.h"
#include "como.util.CHashMap.h"
#include "como.util.IIterator.h"
#include "como.util.IMap.h"
#include <coredef.h>

using como::core::CInteger;
using como::core::IArrayHolder;
using como::io::ByteBufferFactory;
using como::io::CByteArrayOutputStream;
using como::io::IBuffer;
using como::io::IByteBuffer;
using como::io::IID_IByteArrayOutputStream;
using como::util::CArrayList;
using como::util::CHashMap;
using como::util::IID_IList;
using como::util::IID_IMap;
using como::util::IIterator;
using como::util::IMap;

typedef unsigned int    UInteger;

void ZoneInfoTestHelper::ZicDataBuilder::SetTransitions(
    /* [in] */ const Array<Array<Integer>>& transitionPair)
{
    Array<Integer> transitions(transitionPair.GetLength());
    Array<Byte> types(transitionPair.GetLength());
    for (Integer i = 0; i < transitionPair.GetLength(); i++) {
        transitions[i] = transitionPair[i][0];
        types[i] = transitionPair[i][1];
    }
    mTransitionTimes = transitions;
    mTransitionTypes = types;
}

void ZoneInfoTestHelper::ZicDataBuilder::SetTypes(
    /* [in] */ const Array<Array<Integer>>& typePairs)
{
    Array<Integer> isDsts(typePairs.GetLength());
    Array<Integer> offsetSeconds(typePairs.GetLength());
    for (Integer i = 0; i < typePairs.GetLength(); i++) {
        offsetSeconds[i] = typePairs[i][0];
        isDsts[i] = typePairs[i][1];
    }
    mIsDsts = isDsts;
    mOffsetsSeconds = offsetSeconds;
}

Array<Byte> ZoneInfoTestHelper::ZicDataBuilder::Build()
{
    AutoPtr<IByteArrayOutputStream> baos;
    CByteArrayOutputStream::New(IID_IByteArrayOutputStream, (IInterface**)&baos);

    WriteInteger(baos, mMagic);

    for (Integer i = 0; i < 28; ++i) {
        IOutputStream::Probe(baos)->Write(i);
    }
    Integer transitionsCount = mTransitionCountOverride != nullptr ?
            CoreUtils::Unbox(mTransitionCountOverride) : mTransitionTimes.GetLength();
    WriteInteger(baos, transitionsCount);

    Integer typesCount = mTypesCountOverride != nullptr ?
            CoreUtils::Unbox(mTypesCountOverride) : mOffsetsSeconds.GetLength();
    WriteInteger(baos, typesCount);
    WriteInteger(baos, 0xdeadbeef);

    WriteIntegerArray(baos, mTransitionTimes);

    WriteByteArray(baos, mTransitionTypes);

    for (Integer i = 0; i < mOffsetsSeconds.GetLength(); i++) {
        WriteInteger(baos, mOffsetsSeconds[i]);
        WriteByte(baos, mIsDsts[i]);
        WriteByte(baos, i);
    }
    Array<Byte> array;
    baos->ToByteArray(&array);
    return array;
}

//----------------------------------------------------------------------------------

ZoneInfoTestHelper::TzDataBuilder::TzDataBuilder()
{
    CArrayList::New(IID_IList, (IInterface**)&mZicData);
}

Array<Byte> ZoneInfoTestHelper::TzDataBuilder::Build()
{
    AutoPtr<IByteArrayOutputStream> baos;
    CByteArrayOutputStream::New(IID_IByteArrayOutputStream, (IInterface**)&baos);

    Array<Byte> headerMagicBytes(mHeaderMagic.GetByteLength());
    headerMagicBytes.Copy(reinterpret_cast<const Byte*>(mHeaderMagic.string()), mHeaderMagic.GetByteLength());
    IOutputStream::Probe(baos)->Write(headerMagicBytes, 0, headerMagicBytes.GetLength());
    IOutputStream::Probe(baos)->Write(0);

    Integer indexOffsetOffset;
    baos->GetSize(indexOffsetOffset);
    WriteInteger(baos, 0);
    Integer dataOffsetOffset;
    baos->GetSize(dataOffsetOffset);
    WriteInteger(baos, 0);
    Integer zoneTabOffsetOffset;
    baos->GetSize(zoneTabOffsetOffset);
    WriteInteger(baos, 0);

    AutoPtr<IByteArrayOutputStream> dataBytes;
    CByteArrayOutputStream::New(IID_IByteArrayOutputStream, (IInterface**)&dataBytes);
    AutoPtr<IMap> offsets;
    CHashMap::New(IID_IMap, (IInterface**)&offsets);
    FOR_EACH(ZicDatum*, datum, IObject::Probe, mZicData) {
        Integer offset;
        dataBytes->GetSize(offset);
        offsets->Put(CoreUtils::Box(datum->mId), CoreUtils::Box(offset));
        WriteByteArray(dataBytes, datum->mData);
    } END_FOR_EACH();

    Integer indexOffset;
    baos->GetSize(indexOffset);

    FOR_EACH(ZicDatum*, zicDatum, IObject::Probe, mZicData) {
        String id = zicDatum->mId;
        Array<Byte> idBytes(id.GetByteLength());
        idBytes.Copy(reinterpret_cast<const Byte*>(id.string()), id.GetByteLength());
        Array<Byte> paddedIdBytes(40);
        paddedIdBytes.Copy(0, idBytes, 0, idBytes.GetLength());
        WriteByteArray(baos, paddedIdBytes);
        AutoPtr<IInterface> value;
        offsets->Get(CoreUtils::Box(id), value);
        WriteInteger(baos, CoreUtils::Unbox(IInteger::Probe(value)));
        WriteInteger(baos, zicDatum->mData.GetLength());
        WriteInteger(baos, 0);
    } END_FOR_EACH();

    Integer dataOffset;
    baos->GetSize(dataOffset);
    Array<Byte> bytes;
    dataBytes->ToByteArray(&bytes);
    WriteByteArray(baos, bytes);

    Integer zoneTabOffset;
    baos->GetSize(zoneTabOffset);
    Array<Byte> zontTabBytes(mZoneTab.GetByteLength());
    zontTabBytes.Copy(reinterpret_cast<const Byte*>(mZoneTab.string()), mZoneTab.GetByteLength());
    WriteByteArray(baos, zontTabBytes);

    baos->ToByteArray(&bytes);
    SetInteger(bytes, indexOffsetOffset, mIndexOffsetOverride != nullptr ?
            CoreUtils::Unbox(mIndexOffsetOverride) : indexOffset);
    SetInteger(bytes, dataOffsetOffset, mDataOffsetOverride != nullptr ?
            CoreUtils::Unbox(mDataOffsetOverride) : dataOffset);
    SetInteger(bytes, zoneTabOffsetOffset, mZoneTabOffsetOverride != nullptr ?
            CoreUtils::Unbox(mZoneTabOffsetOverride) : zoneTabOffset);
    return bytes;
}

//----------------------------------------------------------------------------------

void ZoneInfoTestHelper::WriteInteger(
    /* [in] */ IByteArrayOutputStream* os,
    /* [in] */ Integer value)
{
    AutoPtr<IByteBuffer> bb;
    ByteBufferFactory::Allocate(4, &bb);
    bb->PutInteger(value);
    AutoPtr<IArrayHolder> holder;
    IBuffer::Probe(bb)->GetArray(holder);
    Array<Byte> bytes;
    holder->GetArray(&bytes);
    WriteByteArray(os, bytes);
}

void ZoneInfoTestHelper::SetInteger(
    /* [out] */ Array<Byte>& bytes,
    /* [in] */ Integer offset,
    /* [in] */ Integer value)
{
    bytes[offset] = ((UInteger)value) >> 24;
    bytes[offset + 1] = ((UInteger)value) >> 16;
    bytes[offset + 2] = ((UInteger)value) >> 8;
    bytes[offset + 3] = value;
}
