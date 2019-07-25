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

#include "ZoneInfoTestHelper.h"
#include "ccm/io/ByteBufferFactory.h"
#include "ccm.core.CInteger.h"
#include "ccm.core.IArrayHolder.h"
#include "ccm.io.CByteArrayOutputStream.h"
#include "ccm.io.IBuffer.h"
#include "ccm.io.IByteBuffer.h"
#include "ccm.util.CArrayList.h"
#include "ccm.util.CHashMap.h"
#include "ccm.util.IIterator.h"
#include "ccm.util.IMap.h"
#include <coredef.h>

using ccm::core::CInteger;
using ccm::core::IArrayHolder;
using ccm::io::ByteBufferFactory;
using ccm::io::CByteArrayOutputStream;
using ccm::io::IBuffer;
using ccm::io::IByteBuffer;
using ccm::io::IID_IByteArrayOutputStream;
using ccm::util::CArrayList;
using ccm::util::CHashMap;
using ccm::util::IID_IList;
using ccm::util::IID_IMap;
using ccm::util::IIterator;
using ccm::util::IMap;

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
    headerMagicBytes.Copy(mHeaderMagic.string(), mHeaderMagic.GetByteLength());
    IOutputStream::Probe(baos)->Write(headerMagicBytes, 0, headerMagicBytes.GetLength());
    IOutputStream::Probe(baos)->Write(0);

    Integer indexOffsetOffset;
    baos->GetSize(&indexOffsetOffset);
    WriteInteger(baos, 0);
    Integer dataOffsetOffset;
    baos->GetSize(&dataOffsetOffset);
    WriteInteger(baos, 0);
    Integer zoneTabOffsetOffset;
    baos->GetSize(&zoneTabOffsetOffset);
    WriteInteger(baos, 0);

    AutoPtr<IByteArrayOutputStream> dataBytes;
    CByteArrayOutputStream::New(IID_IByteArrayOutputStream, (IInterface**)&dataBytes);
    AutoPtr<IMap> offsets;
    CHashMap::New(IID_IMap, (IInterface**)&offsets);
    FOR_EACH(ZicDatum*, datum, IObject::Probe, mZicData) {
        Integer offset;
        dataBytes->GetSize(&offset);
        offsets->Put(CoreUtils::Box(datum->mId), CoreUtils::Box(offset));
        WriteByteArray(dataBytes, datum->mData);
    } END_FOR_EACH();

    Integer indexOffset;
    baos->GetSize(&indexOffset);

    FOR_EACH(ZicDatum*, zicDatum, IObject::Probe, mZicData) {
        String id = zicDatum->mId;
        Array<Byte> idBytes(id.GetByteLength());
        idBytes.Copy(id.string(), id.GetByteLength());
        Array<Byte> paddedIdBytes(40);
        paddedIdBytes.Copy(0, idBytes, 0, idBytes.GetLength());
        WriteByteArray(baos, paddedIdBytes);
        AutoPtr<IInteger> value;
        offsets->Get(CoreUtils::Box(id), (IInterface**)&value);
        WriteInteger(baos, CoreUtils::Unbox(value));
        WriteInteger(baos, zicDatum->mData.GetLength());
        WriteInteger(baos, 0);
    } END_FOR_EACH();

    Integer dataOffset;
    baos->GetSize(&dataOffset);
    Array<Byte> bytes;
    dataBytes->ToByteArray(&bytes);
    WriteByteArray(baos, bytes);

    Integer zoneTabOffset;
    baos->GetSize(&zoneTabOffset);
    Array<Byte> zontTabBytes(mZoneTab.GetByteLength());
    zontTabBytes.Copy(mZoneTab.string(), mZoneTab.GetByteLength());
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
    IBuffer::Probe(bb)->GetArray((IInterface**)&holder);
    Array<Byte> bytes;
    holder->GetArray(&bytes);
    WriteByteArray(os, bytes);
}

void ZoneInfoTestHelper::SetInteger(
    /* [out] */ Array<Byte>& bytes,
    /* [in] */ Integer offset,
    /* [in] */ Integer value)
{
    bytes[offset] = ((unsigned Integer)value) >> 24;
    bytes[offset + 1] = ((unsigned Integer)value) >> 16;
    bytes[offset + 2] = ((unsigned Integer)value) >> 8;
    bytes[offset + 3] = value;
}
