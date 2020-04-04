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

#include "como/core/AutoLock.h"
#include "como/core/CoreUtils.h"
#include "como/core/System.h"
#include "como/io/CFileInputStream.h"
#include "como/util/Arrays.h"
#include "como/util/CArrayList.h"
#include "como.core.IAutoCloseable.h"
#include "como.core.ICharSequence.h"
#include "como.util.IList.h"
#include "como.util.ITimeZone.h"
#include "libcore/io/MemoryMappedFile.h"
#include "libcore/util/BasicLruCache.h"
#include "libcore/util/TimeZoneDataFiles.h"
#include "libcore/util/ZoneInfo.h"
#include "libcore/util/ZoneInfoDB.h"
#include <comolog.h>

using como::core::AutoLock;
using como::core::CoreUtils;
using como::core::E_ILLEGAL_STATE_EXCEPTION;
using como::core::IAutoCloseable;
using como::core::ICharSequence;
using como::core::IID_ICharSequence;
using como::core::System;
using como::io::CFileInputStream;
using como::io::E_IO_EXCEPTION;
using como::io::IInputStream;
using como::io::IID_IInputStream;
using como::util::Arrays;
using como::util::CArrayList;
using como::util::IID_IList;
using como::util::IList;
using como::util::ITimeZone;
using libcore::io::MemoryMappedFile;

namespace libcore {
namespace util {

COMO_INTERFACE_IMPL_1(ZoneInfoDB::TzData, SyncObject, IZoneInfoDBTzData);

ZoneInfoDB::TzData::TzData()
{
    class _BasicLruCache
        : public BasicLruCache
    {
    public:
        ECode Constructor(
            /* [in] */ Integer maxSize,
            /* [in] */ TzData* owner)
        {
            FAIL_RETURN(BasicLruCache::Constructor(maxSize));
            mOwner = owner;
            return NOERROR;
        }

    protected:
        ECode Create(
            /* [in] */ IInterface* id,
            /* [out] */ IInterface** value) override
        {
            return mOwner->MakeTimeZoneUncached(
                    CoreUtils::Unbox(ICharSequence::Probe(id)), (IZoneInfo**)value);
        }

    private:
        TzData* mOwner;
    };

    AutoPtr<_BasicLruCache> cache = new _BasicLruCache();
    cache->Constructor(CACHE_SIZE, this);
    mCache = (IBasicLruCache*)cache;
}

ZoneInfoDB::TzData::~TzData()
{
    Close();
}

AutoPtr<IZoneInfoDBTzData> ZoneInfoDB::TzData::LoadTzDataWithFallback(
    /* [in] */ const Array<String>& paths)
{
    for (String path : paths) {
        AutoPtr<TzData> tzData = new TzData();
        if (tzData->LoadData(path)) {
            return (IZoneInfoDBTzData*)tzData.Get();
        }
    }

    Logger::E("ZoneInfoDB", "Couldn't find any %s file!", TZDATA_FILE.string());
    return TzData::CreateFallback();
}

AutoPtr<IZoneInfoDBTzData> ZoneInfoDB::TzData::LoadTzData(
    /* [in] */ const String& path)
{
    AutoPtr<TzData> tzData = new TzData();
    if (tzData->LoadData(path)) {
        return (IZoneInfoDBTzData*)tzData.Get();
    }
    return nullptr;
}

AutoPtr<IZoneInfoDBTzData> ZoneInfoDB::TzData::CreateFallback()
{
    AutoPtr<TzData> tzData = new TzData();
    tzData->PopulateFallback();
    return (IZoneInfoDBTzData*)tzData.Get();
}

ECode ZoneInfoDB::TzData::GetBufferIterator(
    /* [in] */ const String& id,
    /* [out] */ IBufferIterator** it)
{
    VALIDATE_NOT_NULL(it);

    FAIL_RETURN(CheckNotClosed());

    Integer index = Arrays::BinarySearch(mIds, id);
    if (index < 0) {
        *it = nullptr;
        return NOERROR;
    }

    Integer byteOffset = mByteOffsets[index];
    mMappedFile->BigEndianIterator(it);
    (*it)->Skip(byteOffset);
    return NOERROR;
}

void ZoneInfoDB::TzData::PopulateFallback()
{
    mVersion = "missing";
    mZoneTab = "# Emergency fallback data.\n";
    mIds = { String("GMT") };
    mByteOffsets = mRawUtcOffsetsCache = Array<Integer>(1);
}


Boolean ZoneInfoDB::TzData::LoadData(
    /* [in] */ const String& path)
{
    mMappedFile = nullptr;
    ECode ec = MemoryMappedFile::MmapRO(path, &mMappedFile);
    if (FAILED(ec)) {
        return false;
    }
    ec = ReadHeader();
    if (FAILED(ec)) {
        Close();

        Logger::E("ZoneInfoDB", "%s file \"%s\" was present but invalid!",
                TZDATA_FILE.string(), path.string());
        return false;
    }
    return true;
}

ECode ZoneInfoDB::TzData::ReadHeader()
{
    AutoPtr<IBufferIterator> it;
    mMappedFile->BigEndianIterator(&it);

    Array<Byte> tzdataVersion(12);
    FAIL_RETURN(it->ReadByteArray(tzdataVersion, 0, tzdataVersion.GetLength()));
    String magic(tzdataVersion, 0, 6);
    if (!magic.Equals("tzdata") || tzdataVersion[11] != 0) {
        Logger::E("ZoneInfoDB", "bad tzdata magic: %s",
                String(tzdataVersion, 0, tzdataVersion.GetLength()).string());
        return E_IO_EXCEPTION;
    }
    mVersion = String(tzdataVersion, 6, 5);

    Integer fileSize;
    mMappedFile->GetSize(&fileSize);
    Integer indexOffset;
    FAIL_RETURN(it->ReadInteger(&indexOffset));
    FAIL_RETURN(ValidateOffset(indexOffset, fileSize));
    Integer dataOffset;
    FAIL_RETURN(it->ReadInteger(&dataOffset));
    FAIL_RETURN(ValidateOffset(dataOffset, fileSize));
    Integer zonetabOffset;
    FAIL_RETURN(it->ReadInteger(&zonetabOffset));
    FAIL_RETURN(ValidateOffset(zonetabOffset, fileSize));

    if (indexOffset >= dataOffset || dataOffset >= zonetabOffset) {
        Logger::E("ZoneInfoDB", "Invalid offset: index_offset=%d, data_offset=%d"
                ", zonetab_offset=%d, fileSize=%d", indexOffset, dataOffset,
                zonetabOffset, fileSize);
        return E_IO_EXCEPTION;
    }

    FAIL_RETURN(ReadIndex(it, indexOffset, dataOffset));
    return ReadZoneTab(it, zonetabOffset, fileSize - zonetabOffset);
}

ECode ZoneInfoDB::TzData::ReadZoneTab(
    /* [in] */ IBufferIterator* it,
    /* [in] */ Integer zoneTabOffset,
    /* [in] */ Integer zoneTabSize)
{
    Array<Byte> bytes(zoneTabSize);
    it->Seek(zoneTabOffset);
    FAIL_RETURN(it->ReadByteArray(bytes, 0, bytes.GetLength()));
    mZoneTab = String(bytes, 0, bytes.GetLength());
    return NOERROR;
}

ECode ZoneInfoDB::TzData::ReadIndex(
    /* [in] */ IBufferIterator* it,
    /* [in] */ Integer indexOffset,
    /* [in] */ Integer dataOffset)
{
    it->Seek(indexOffset);

    Array<Byte> idBytes(SIZEOF_TZNAME);
    Integer indexSize = dataOffset - indexOffset;
    if (indexSize % SIZEOF_INDEX_ENTRY != 0) {
        Logger::E("ZoneInfoDB", "Index size is not divisible by %d, indexSize=%d",
                SIZEOF_INDEX_ENTRY, indexSize);
        return E_IO_EXCEPTION;
    }
    Integer entryCount = indexSize / SIZEOF_INDEX_ENTRY;

    mByteOffsets = Array<Integer>(entryCount);
    mIds = Array<String>(entryCount);

    for (Integer i = 0; i < entryCount; i++) {
        // Read the fixed length timezone ID.
        FAIL_RETURN(it->ReadByteArray(idBytes, 0, idBytes.GetLength()));

        // Read the offset into the file where the data for ID can be found.
        Integer value;
        FAIL_RETURN(it->ReadInteger(&value));
        mByteOffsets[i] = value + dataOffset;

        Integer length;
        FAIL_RETURN(it->ReadInteger(&length));
        if (length < 44) {
            Logger::E("ZoneInfoDB", "length in index file < sizeof(tzhead)");
            return E_IO_EXCEPTION;
        }
        it->Skip(4); // Skip the unused 4 bytes that used to be the raw offset.

        // Calculate the true length of the ID.
        Integer len = 0;
        while (idBytes[len] != 0 && len < idBytes.GetLength()) {
            len++;
        }
        if (len == 0) {
            Logger::E("ZoneInfoDB", "Invalid ID at index=%d", i);
            return E_IO_EXCEPTION;
        }
        mIds[i] = String(idBytes, 0, len);
        if (i > 0) {
            if (mIds[i].Compare(mIds[i - 1]) <= 0) {
                Logger::E("ZoneInfoDB", "Index not sorted or contains multiple entries with the same ID"
                        ", index=%d, ids[i]=%s, ids[i - 1]=%s", i, mIds[i].string(), mIds[i - 1].string());
                return E_IO_EXCEPTION;
            }
        }
    }
    return NOERROR;
}

ECode ZoneInfoDB::TzData::ValidateOffset(
    /* [in] */ Integer offset,
    /* [in] */ Integer size)
{
    if (offset < 0 || offset >= size) {
        Logger::E("ZoneInfoDB", "Invalid offset=%d, size=%d", offset, size);
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode ZoneInfoDB::TzData::Validate()
{
    FAIL_RETURN(CheckNotClosed());
    // Validate the data in the tzdata file by loading each and every zone.
    Array<String> ids;
    GetAvailableIDs(&ids);
    for (String id : ids) {
        AutoPtr<IZoneInfo> zoneInfo;
        FAIL_RETURN(MakeTimeZoneUncached(id, &zoneInfo));
        if (zoneInfo == nullptr) {
            Logger::E("ZoneInfoDB", "Unable to find data for ID=%s", id.string());
            return E_IO_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode ZoneInfoDB::TzData::MakeTimeZoneUncached(
    /* [in] */ const String& id,
    /* [out] */ IZoneInfo** zoneInfo)
{
    AutoPtr<IBufferIterator> it;
    GetBufferIterator(id, &it);
    if (it == nullptr) {
        *zoneInfo = nullptr;
        return NOERROR;
    }

    return ZoneInfo::ReadTimeZone(id, it, System::GetCurrentTimeMillis(), zoneInfo);
}

ECode ZoneInfoDB::TzData::GetAvailableIDs(
    /* [out, callee] */ Array<String>* ids)
{
    VALIDATE_NOT_NULL(ids);

    FAIL_RETURN(CheckNotClosed());
    *ids = mIds.Clone();
    return NOERROR;
}

ECode ZoneInfoDB::TzData::GetAvailableIDs(
    /* [in] */ Integer rawUtcOffset,
    /* [out, callee] */ Array<String>* ids)
{
    VALIDATE_NOT_NULL(ids);

    FAIL_RETURN(CheckNotClosed());
    AutoPtr<IList> matches;
    CArrayList::New(IID_IList, (IInterface**)&matches);
    Array<Integer> rawUtcOffsets = GetRawUtcOffsets();
    for (Integer i = 0; i < rawUtcOffsets.GetLength(); ++i) {
        if (rawUtcOffsets[i] == rawUtcOffset) {
            matches->Add(CoreUtils::Box(mIds[i]));
        }
    }
    Array<ICharSequence*> idObjs;
    matches->ToArray(IID_ICharSequence, (Array<IInterface*>*)&idObjs);
    *ids = CoreUtils::Unbox(idObjs);
    return NOERROR;
}

Array<Integer> ZoneInfoDB::TzData::GetRawUtcOffsets()
{
    AutoLock lock(this);

    if (!mRawUtcOffsetsCache.IsNull()) {
        return mRawUtcOffsetsCache;
    }
    mRawUtcOffsetsCache = Array<Integer>(mIds.GetLength());
    for (Integer i = 0; i < mIds.GetLength(); ++i) {
        // This creates a TimeZone, which is quite expensive. Hence the cache.
        // Note that icu4c does the same (without the cache), so if you're
        // switching this code over to icu4j you should check its performance.
        // Telephony shouldn't care, but someone converting a bunch of calendar
        // events might.
        AutoPtr<IInterface> value;
        mCache->Get(CoreUtils::Box(mIds[i]), &value);
        Integer offset;
        ITimeZone::Probe(value)->GetRawOffset(&offset);
        mRawUtcOffsetsCache[i] = offset;
    }
    return mRawUtcOffsetsCache;
}

ECode ZoneInfoDB::TzData::GetVersion(
    /* [out] */ String* version)
{
    VALIDATE_NOT_NULL(version);

    FAIL_RETURN(CheckNotClosed());
    *version = mVersion;
    return NOERROR;
}

ECode ZoneInfoDB::TzData::GetZoneTab(
    /* [out] */ String* zoneTab)
{
    VALIDATE_NOT_NULL(zoneTab);

    FAIL_RETURN(CheckNotClosed());
    *zoneTab = mZoneTab;
    return NOERROR;
}

ECode ZoneInfoDB::TzData::MakeTimeZone(
    /* [in] */ const String& id,
    /* [out] */ IZoneInfo** zone)
{
    VALIDATE_NOT_NULL(zone);

    FAIL_RETURN(CheckNotClosed());
    AutoPtr<IZoneInfo> zoneInfo;
    mCache->Get(CoreUtils::Box(id), (IInterface**)&zoneInfo);
    if (zoneInfo == nullptr) {
        *zone = nullptr;
        return NOERROR;
    }
    else {
        AutoPtr<IZoneInfo> z = (IZoneInfo*)CoreUtils::Clone(zoneInfo, IID_IZoneInfo).Get();
        z.MoveTo(zone);
        return NOERROR;
    }
}

ECode ZoneInfoDB::TzData::HasTimeZone(
    /* [in] */ const String& id,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(CheckNotClosed());
    AutoPtr<IInterface> zoneInfo;
    mCache->Get(CoreUtils::Box(id), &zoneInfo);
    *result = zoneInfo != nullptr;
    return NOERROR;
}

ECode ZoneInfoDB::TzData::Close()
{
    if (!mClosed) {
        mClosed = true;

        mIds.Clear();
        mByteOffsets.Clear();
        mRawUtcOffsetsCache.Clear();
        if (mMappedFile != nullptr) {
            IAutoCloseable::Probe(mMappedFile)->Close();
        }
        mMappedFile = nullptr;
        mCache->EvictAll();
    }
    return NOERROR;
}

ECode ZoneInfoDB::TzData::CheckNotClosed()
{
    if (mClosed) {
        Logger::E("ZoneInfoDB", "TzData is closed");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode ZoneInfoDB::TzData::GetRulesVersion(
    /* [in] */ IFile* tzDataFile,
    /* [out] */ String* version)
{
    VALIDATE_NOT_NULL(version);

    AutoPtr<IInputStream> is;
    FAIL_RETURN(CFileInputStream::New(tzDataFile, IID_IInputStream, (IInterface**)&is));

    const Integer bytesToRead = 12;
    Array<Byte> tzdataVersion(bytesToRead);
    Integer bytesRead;
    FAIL_RETURN(is->Read(tzdataVersion, 0, bytesToRead, bytesRead));
    if (bytesRead != bytesToRead) {
        Logger::E("ZoneInfoDB",  "File too short: only able to read %s bytes.", bytesRead);
        IAutoCloseable::Probe(is)->Close();
        return E_IO_EXCEPTION;
    }

    String magic(tzdataVersion, 0, 6);
    if (!magic.Equals("tzdata") || tzdataVersion[11] != 0) {
        Logger::E("ZoneInfoDB", "bad tzdata magic: %s",
                String(tzdataVersion, 0, tzdataVersion.GetLength()).string());
        IAutoCloseable::Probe(is)->Close();
        return E_IO_EXCEPTION;
    }

    *version = String(tzdataVersion, 6, 5);
    IAutoCloseable::Probe(is)->Close();
    return NOERROR;
}

//------------------------------------------------------------------------

AutoPtr<IZoneInfoDBTzData> ZoneInfoDB::GetInstance()
{
    static const AutoPtr<IZoneInfoDBTzData> DATA =
            TzData::LoadTzDataWithFallback(TimeZoneDataFiles::GetTimeZoneFilePaths(TZDATA_FILE));
    return DATA;
}

}
}
