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
#include "como/core/SyncObject.h"
#include "como/io/charset/CoderResult.h"
#include "como/util/CHashMap.h"
#include <comolog.h>

using como::core::AutoLock;
using como::core::CoreUtils;
using como::util::CHashMap;
using como::util::IID_IMap;

namespace como {
namespace io {
namespace charset {

const Array<String> CoderResult::sNames {
    String("UNDERFLOW"), String("OVERFLOW"), String("MALFORMED"), String("UNMAPPABLE")
};

static AutoPtr<ICoderResult> CreateCoderResult(
    /* [in] */ Integer type,
    /* [in] */ Integer length)
{
    AutoPtr<CoderResult> cr = new CoderResult();
    cr->Constructor(type, length);
    return (ICoderResult*)cr.Get();
}

AutoPtr<ICoderResult> CoderResult::GetUNDERFLOW()
{
    static AutoPtr<ICoderResult> UNDERFLOW = CreateCoderResult(CR_UNDERFLOW, 0);
    return UNDERFLOW;
}

AutoPtr<ICoderResult> CoderResult::GetOVERFLOW()
{
    static AutoPtr<ICoderResult> OVERFLOW = CreateCoderResult(CR_OVERFLOW, 0);
    return OVERFLOW;
}

AutoPtr<CoderResult::Cache> CoderResult::GetMalformedCache()
{
    class _Cache : public Cache
    {
    public:
        AutoPtr<ICoderResult> Create(
            /* [in] */ Integer len) override
        {
            AutoPtr<CoderResult> cr = new CoderResult();
            cr->Constructor(CR_MALFORMED, len);
            return (ICoderResult*)cr.Get();
        }
    };

    static AutoPtr<Cache> sMalformedCache = new _Cache();
    return sMalformedCache;
}

AutoPtr<CoderResult::Cache> CoderResult::GetUnmappableCache()
{
    class _Cache : public Cache
    {
    public:
        AutoPtr<ICoderResult> Create(
            /* [in] */ Integer len) override
        {
            AutoPtr<CoderResult> cr = new CoderResult();
            cr->Constructor(CR_UNMAPPABLE, len);
            return (ICoderResult*)cr.Get();
        }
    };

    static AutoPtr<Cache> sUnmappableCache = new _Cache();
    return sUnmappableCache;
}

COMO_INTERFACE_IMPL_1(CoderResult, SyncObject, ICoderResult);

ECode CoderResult::Constructor(
    /* [in] */ Integer type,
    /* [in] */ Integer length)
{
    mType = type;
    mLength = length;
    return NOERROR;
}

ECode CoderResult::ToString(
    /* [out] */ String& desc)
{
    String nm = sNames[mType];
    Boolean error;
    desc = (IsError(error), error) ?
            String::Format("%s[%d]", nm.string(), mLength) : nm;
    return NOERROR;
}

ECode CoderResult::IsUnderflow(
    /* [out] */ Boolean& underflow)
{
    underflow = (mType == CR_UNDERFLOW);
    return NOERROR;
}

ECode CoderResult::IsOverflow(
    /* [out] */ Boolean& overflow)
{
    overflow = (mType == CR_OVERFLOW);
    return NOERROR;
}

ECode CoderResult::IsError(
    /* [out] */ Boolean& error)
{
    error = (mType >= CR_ERROR_MIN);
    return NOERROR;
}

ECode CoderResult::IsMalformed(
    /* [out] */ Boolean& malformed)
{
    malformed = (mType == CR_MALFORMED);
    return NOERROR;
}

ECode CoderResult::IsUnmappable(
    /* [out] */ Boolean& unmappable)
{
    unmappable = (mType == CR_UNMAPPABLE);
    return NOERROR;
}

ECode CoderResult::GetLength(
    /* [out] */ Integer& length)
{
    Boolean error;
    if (IsError(error), !error) {
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    length = mLength;
    return NOERROR;
}

AutoPtr<ICoderResult> CoderResult::MalformedForLength(
    /* [in] */ Integer length)
{
    return GetMalformedCache()->Get(length);
}

AutoPtr<ICoderResult> CoderResult::UnmappableForLength(
    /* [in] */ Integer length)
{
    return GetUnmappableCache()->Get(length);
}

ECode CoderResult::ThrowException()
{
    switch (mType) {
        case CR_UNDERFLOW: return E_BUFFER_UNDERFLOW_EXCEPTION;
        case CR_OVERFLOW: return E_BUFFER_OVERFLOW_EXCEPTION;
        case CR_MALFORMED: return E_MALFORMED_INPUT_EXCEPTION;
        case CR_UNMAPPABLE: return E_UNMAPPABLE_CHARACTER_EXCEPTION;
        default: CHECK(false); return NOERROR;
    }
}

//--------------------------------------------------------------------------

AutoPtr<ICoderResult> CoderResult::Cache::Get(
    /* [in] */ Integer len)
{
    if (len <= 0) {
        Logger::E("CoderResult", "Non-positive length");
        return nullptr;
    }
    AutoLock lock(this);
    if (mCache == nullptr) {
        CHashMap::New(IID_IMap, (IInterface**)&mCache);
    }
    AutoPtr<IInteger> k = CoreUtils::Box(len);
    AutoPtr<IWeakReference> w;
    AutoPtr<ICoderResult> e;
    mCache->Get(k, (IInterface**)&w);
    if (w != nullptr) {
        w->Resolve(IID_ICoderResult, (IInterface**)&e);
    }
    if (e == nullptr) {
        e = Create(len);
        IWeakReferenceSource::Probe(e)->GetWeakReference(w);
        mCache->Put(k, w);
    }
    return e;
}

}
}
}
