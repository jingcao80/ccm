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

#ifndef __CCM_IO_CHARSET_CODERRESULT_H__
#define __CCM_IO_CHARSET_CODERRESULT_H__

#include "ccm/core/SyncObject.h"
#include "ccm.io.charset.ICoderResult.h"
#include "ccm.util.IMap.h"

using ccm::core::SyncObject;
using ccm::util::IMap;

namespace ccm {
namespace io {
namespace charset {

static AutoPtr<ICoderResult> CreateCoderResult(Integer, Integer);

class CoderResult
    : public SyncObject
    , public ICoderResult
{
private:
    class Cache
        : public SyncObject
    {
    public:
        AutoPtr<ICoderResult> Get(
            /* [in] */ Integer len);

    protected:
        virtual AutoPtr<ICoderResult> Create(
            /* [in] */ Integer len) = 0;

    private:
        AutoPtr<IMap> mCache;
    };

public:
    CCM_INTERFACE_DECL();

    ECode ToString(
        /* [out] */ String* desc) override;

    ECode IsUnderflow(
        /* [out] */ Boolean* underflow) override;

    ECode IsOverflow(
        /* [out] */ Boolean* overflow) override;

    ECode IsError(
        /* [out] */ Boolean* error) override;

    ECode IsMalformed(
        /* [out] */ Boolean* malformed) override;

    ECode IsUnmappable(
        /* [out] */ Boolean* unmappable) override;

    ECode GetLength(
        /* [out] */ Integer* length) override;

    static AutoPtr<ICoderResult> MalformedForLength(
        /* [in] */ Integer length);

    static AutoPtr<ICoderResult> UnmappableForLength(
        /* [in] */ Integer length);

    ECode ThrowException() override;

    static AutoPtr<ICoderResult> GetUNDERFLOW();

    static AutoPtr<ICoderResult> GetOVERFLOW();

    static AutoPtr<Cache> GetMalformedCache();

    static AutoPtr<Cache> GetUnmappableCache();

private:
    friend AutoPtr<ICoderResult> CreateCoderResult(Integer, Integer);

    ECode Constructor(
        /* [in] */ Integer type,
        /* [in] */ Integer length);

private:
    static constexpr Integer CR_UNDERFLOW = 0;
    static constexpr Integer CR_OVERFLOW = 1;
    static constexpr Integer CR_ERROR_MIN = 2;
    static constexpr Integer CR_MALFORMED = 2;
    static constexpr Integer CR_UNMAPPABLE = 3;

    static const Array<String> sNames;

    Integer mType = 0;
    Integer mLength = 0;
};

}
}
}

#endif // __CCM_IO_CHARSET_CODERRESULT_H__
