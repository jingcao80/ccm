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

#ifndef __LIBCORE_UTIL_ZONEINFODB_H__
#define __LIBCORE_UTIL_ZONEINFODB_H__

#include "ccm/core/SyncObject.h"
#include "ccm.io.IFile.h"
#include "libcore.io.IBufferIterator.h"
#include "libcore.io.IMemoryMappedFile.h"
#include "libcore.util.IBasicLruCache.h"
#include "libcore.util.IZoneInfo.h"
#include "libcore.util.IZoneInfoDB.h"
#include "libcore.util.IZoneInfoDBTzData.h"

using ccm::core::SyncObject;
using ccm::io::IFile;
using libcore::io::IBufferIterator;
using libcore::io::IMemoryMappedFile;

namespace libcore {
namespace util {

class ZoneInfoDB
    : public IZoneInfoDB
{
public:
    class TzData
        : public SyncObject
        , public IZoneInfoDBTzData
    {
    public:
        virtual ~TzData();

        CCM_INTERFACE_DECL();

        static AutoPtr<IZoneInfoDBTzData> LoadTzDataWithFallback(
            /* [in] */ const Array<String>& paths);

        static AutoPtr<IZoneInfoDBTzData> LoadTzData(
            /* [in] */ const String& path);

        ECode GetBufferIterator(
            /* [in] */ const String& id,
            /* [out] */ IBufferIterator** it) override;

        ECode Validate() override;

        ECode MakeTimeZoneUncached(
            /* [in] */ const String& id,
            /* [out] */ IZoneInfo** zoneInfo);

        ECode GetAvailableIDs(
            /* [out, callee] */ Array<String>* ids) override;

        ECode GetAvailableIDs(
            /* [in] */ Integer rawUtcOffset,
            /* [out, callee] */ Array<String>* ids) override;

        ECode GetVersion(
            /* [out] */ String* version) override;

        ECode GetZoneTab(
            /* [out] */ String* zoneTab) override;

        ECode MakeTimeZone(
            /* [in] */ const String& id,
            /* [out] */ IZoneInfo** zone) override;

        ECode HasTimeZone(
            /* [in] */ const String& id,
            /* [out] */ Boolean* result) override;

        ECode Close() override;

        static ECode GetRulesVersion(
            /* [in] */ IFile* tzDataFile,
            /* [out] */ String* version);

    private:
        TzData();

        static AutoPtr<IZoneInfoDBTzData> CreateFallback();

        void PopulateFallback();

        Boolean LoadData(
            /* [in] */ const String& path);

        ECode ReadHeader();

        static ECode ValidateOffset(
            /* [in] */ Integer offset,
            /* [in] */ Integer size);

        ECode ReadZoneTab(
            /* [in] */ IBufferIterator* it,
            /* [in] */ Integer zoneTabOffset,
            /* [in] */ Integer zoneTabSize);

        ECode ReadIndex(
            /* [in] */ IBufferIterator* it,
            /* [in] */ Integer indexOffset,
            /* [in] */ Integer dataOffset);

        Array<Integer> GetRawUtcOffsets();

        ECode CheckNotClosed();

    private:
        // The database reserves 40 bytes for each id.
        static constexpr Integer SIZEOF_TZNAME = 40;

        // The database uses 32-bit (4 byte) integers.
        static constexpr Integer SIZEOF_TZINT = 4;

        // Each index entry takes up this number of bytes.
        static constexpr Integer SIZEOF_INDEX_ENTRY = SIZEOF_TZNAME + 3 * SIZEOF_TZINT;

        Boolean mClosed = false;

        AutoPtr<IMemoryMappedFile> mMappedFile;

        String mVersion;
        String mZoneTab;

        Array<String> mIds;
        Array<Integer> mByteOffsets;
        Array<Integer> mRawUtcOffsetsCache;

        static constexpr Integer CACHE_SIZE = 1;
        AutoPtr<IBasicLruCache> mCache;
    };

public:
    static AutoPtr<IZoneInfoDBTzData> GetInstance();

private:
    ZoneInfoDB();
};

}
}

#endif // __LIBCORE_UTIL_ZONEINFODB_H__
