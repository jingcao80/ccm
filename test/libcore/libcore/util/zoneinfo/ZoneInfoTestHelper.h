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

#ifndef __ZONEINFOTESTHELPER_H__
#define __ZONEINFOTESTHELPER_H__

#include "ccm/core/CoreUtils.h"
#include "ccm.core.IInteger.h"
#include "ccm.io.IByteArrayOutputStream.h"
#include "ccm.io.IOutputStream.h"
#include "ccm.util.IList.h"
#include <ccmautoptr.h>
#include <ccmobject.h>

using namespace ccm;
using ccm::core::CoreUtils;
using ccm::core::IInteger;
using ccm::io::IByteArrayOutputStream;
using ccm::io::IOutputStream;
using ccm::util::IList;

class ZoneInfoTestHelper
{
public:
    class ZicDataBuilder
        : public Object
    {
    public:
        inline ZicDataBuilder()
        {}

        inline void SetMagic(
            /* [in] */ Integer magic)
        {
            mMagic = magic;
        }

        inline void SetTypeCountOverride(
            /* [in] */ Integer typesCountOverride)
        {
            mTypesCountOverride = std::move(CoreUtils::Box(typesCountOverride));
        }

        inline void SetTransitionCountOverride(
            /* [in] */ Integer transitionCountOverride)
        {
            mTransitionCountOverride = std::move(CoreUtils::Box(transitionCountOverride));
        }

        inline void SetTransitionsAndTypes(
            /* [in] */ const Array<Array<Integer>>& transitionPairs,
            /* [in] */ const Array<Array<Integer>>& typePairs)
        {
            SetTransitions(transitionPairs);
            SetTypes(typePairs);
        }

        void SetTransitions(
            /* [in] */ const Array<Array<Integer>>& transitionPair);

        void SetTypes(
            /* [in] */ const Array<Array<Integer>>& typePairs);

        inline void InitializeToValid()
        {
            Array<Array<Integer>> transitionPairs;
            Array<Array<Integer>> typePairs{ {3600, 0} };
            SetTransitions(transitionPairs);
            SetTypes(typePairs);
        }

        Array<Byte> Build();

    private:
        Integer mMagic = 0x545a6966;
        AutoPtr<IInteger> mTransitionCountOverride;
        Array<Integer> mTransitionTimes;
        Array<Byte> mTransitionTypes;
        AutoPtr<IInteger> mTypesCountOverride;
        Array<Integer> mIsDsts;
        Array<Integer> mOffsetsSeconds;
    };

    class TzDataBuilder
        : public Object
    {
    private:
        class ZicDatum
            : public Object
        {
        public:
            ZicDatum(
                /* [in] */ const String& id,
                /* [in] */ const Array<Byte>& data)
                : mId(id)
                , mData(data)
            {}

        public:
            String mId;
            Array<Byte> mData;
        };

    public:
        TzDataBuilder();

        inline void SetHeaderMagic(
            /* [in] */ const String& headerMagic)
        {
            mHeaderMagic = headerMagic;
        }

        inline void SetIndexOffsetOverride(
            /* [in] */ Integer indexOffset)
        {
            mIndexOffsetOverride = std::move(CoreUtils::Box(indexOffset));
        }

        inline void SetDataOffsetOverride(
            /* [in] */ Integer dataOffset)
        {
            mDataOffsetOverride = std::move(CoreUtils::Box(dataOffset));
        }

        inline void SetZoneTabOffsetOverride(
            /* [in] */ Integer zoneTabOffset)
        {
            mZoneTabOffsetOverride = std::move(CoreUtils::Box(zoneTabOffset));
        }

        inline void AddZicData(
            /* [in] */ const String& id,
            /* [in] */ const Array<Byte>& data)
        {
            mZicData->Add((IObject*)new ZicDatum(id, data));
        }

        inline void SetZoneTab(
            /* [in] */ const String& zoneTab)
        {
            mZoneTab = zoneTab;
        }

        inline void InitializeToValid()
        {
            SetHeaderMagic(String("tzdata9999a"));
            AutoPtr<ZicDataBuilder> zdBuilder = new ZicDataBuilder();
            zdBuilder->InitializeToValid();
            AddZicData(String("Europe/Elbonia"), zdBuilder->Build());
            SetZoneTab(String("ZoneTab data"));
        }

        inline void ClearZicData()
        {
            mZicData->Clear();
        }

        Array<Byte> Build();

    private:
        String mHeaderMagic;
        AutoPtr<IList> mZicData;
        String mZoneTab;
        AutoPtr<IInteger> mIndexOffsetOverride;
        AutoPtr<IInteger> mDataOffsetOverride;
        AutoPtr<IInteger> mZoneTabOffsetOverride;
    };

public:
    inline static void WriteByteArray(
        /* [in] */ IByteArrayOutputStream* baos,
        /* [in] */ const Array<Byte>& array)
    {
        IOutputStream::Probe(baos)->Write(array, 0, array.GetLength());
    }

    inline static void WriteByte(
        /* [in] */ IByteArrayOutputStream* baos,
        /* [in] */ Integer value)
    {
        IOutputStream::Probe(baos)->Write(value);
    }

    inline static void WriteIntegerArray(
        /* [in] */ IByteArrayOutputStream* baos,
        /* [in] */ const Array<Integer>& array)
    {
        for (Integer value : array) {
            WriteInteger(baos, value);
        }
    }

    static void WriteInteger(
        /* [in] */ IByteArrayOutputStream* os,
        /* [in] */ Integer value);

    static void SetInteger(
        /* [out] */ Array<Byte>& bytes,
        /* [in] */ Integer offset,
        /* [in] */ Integer value);

private:
    ZoneInfoTestHelper();
};

#endif // __ZONEINFOTESTHELPER_H__
