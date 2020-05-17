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

#include "como/core/StringUtils.h"
#include "como/math/CMathContext.h"
#include "como/math/MathContext.h"
#include <comolog.h>

using como::core::StringUtils;
using como::io::IID_ISerializable;

namespace como {
namespace math {

static AutoPtr<IMathContext> CreateMathContext(
    /* [in] */ Integer precision,
    /* [in] */ RoundingMode roundingMode)
{
    AutoPtr<IMathContext> mc;
    CMathContext::New(precision, roundingMode, IID_IMathContext, (IInterface**)&mc);
    return mc;
}

const AutoPtr<IMathContext> MathContext::GetDECIMAL128()
{
    static const AutoPtr<IMathContext> DECIMAL128 =
            CreateMathContext(34, RoundingMode::HALF_EVEN);
    return DECIMAL128;
}

const AutoPtr<IMathContext> MathContext::GetDECIMAL32()
{
    static const AutoPtr<IMathContext> DECIMAL32 =
            CreateMathContext(7, RoundingMode::HALF_EVEN);
    return DECIMAL32;
}

const AutoPtr<IMathContext> MathContext::GetDECIMAL64()
{
    static const AutoPtr<IMathContext> DECIMAL64 =
            CreateMathContext(16, RoundingMode::HALF_EVEN);
    return DECIMAL64;
}

const AutoPtr<IMathContext> MathContext::GetUNLIMITED()
{
    static const AutoPtr<IMathContext> UNLIMITED =
            CreateMathContext(0, RoundingMode::HALF_UP);
    return UNLIMITED;
}

COMO_INTERFACE_IMPL_2(MathContext, SyncObject, IMathContext, ISerializable);

ECode MathContext::Constructor(
    /* [in] */ Integer precision)
{
    return Constructor(precision, RoundingMode::HALF_UP);
}

ECode MathContext::Constructor(
    /* [in] */ Integer precision,
    /* [in] */ RoundingMode roundingMode)
{
    mPrecision = precision;
    mRoundingMode = roundingMode;
    return CheckValid();
}

ECode MathContext::Constructor(
    /* [in] */ const String& s)
{
    Integer precisionLength = String("precision=").GetLength();
    Integer roundingModeLength = String("roundingMode=").GetLength();

    Integer spaceIndex;
    if (!s.StartsWith("precision=") || (spaceIndex = s.IndexOf(U' ', precisionLength)) == -1) {
        Logger::E("MathContext", "Missing precision: %s", s.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String precisionString = s.Substring(precisionLength, spaceIndex);
    ECode ec = StringUtils::ParseInteger(precisionString, &mPrecision);
    if (FAILED(ec)) {
        Logger::E("MathContext", "Bad precision: %s", s.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer roundingModeStart = spaceIndex + 1;
    if (!s.RegionMatches(roundingModeStart, "roundingMode=", 0, roundingModeLength)) {
        Logger::E("MathContext", "Missing rounding mode: %s", s.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    roundingModeStart += roundingModeLength;
    mRoundingMode = ParseRoundingMode(s.Substring(roundingModeStart));

    return CheckValid();
}

ECode MathContext::CheckValid()
{
    if (mPrecision < 0) {
        Logger::E("MathContext", "Negative precision: %d", mPrecision);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (static_cast<Integer>(mRoundingMode) == -1) {
        Logger::E("MathContext", "mRoundingMode == null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode MathContext::GetPrecision(
    /* [out] */ Integer& precision)
{
    precision = mPrecision;
    return NOERROR;
}

ECode MathContext::GetRoundingMode(
    /* [out] */ RoundingMode& roundingMode)
{
    roundingMode = mRoundingMode;
    return NOERROR;
}

ECode MathContext::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& same)
{
    MathContext* other = (MathContext*)IMathContext::Probe(obj);
    if (other == nullptr) {
        same = false;
        return NOERROR;
    }
    same = (other->mPrecision == mPrecision) && (other->mRoundingMode == mRoundingMode);
    return NOERROR;
}

ECode MathContext::GetHashCode(
    /* [out] */ Integer& hash)
{
    hash = ((mPrecision << 3) | static_cast<Integer>(mRoundingMode));
    return NOERROR;
}

ECode MathContext::ToString(
    /* [out] */ String& desc)
{
    desc = String::Format("precision=%d roundingMode=%d",
            mPrecision, mRoundingMode);
    return NOERROR;
}

RoundingMode MathContext::ParseRoundingMode(
    /* [in] */ const String& mode)
{
    if (mode.Equals("UP")) {
        return RoundingMode::UP;
    }
    else if (mode.Equals("DOWN")) {
        return RoundingMode::DOWN;
    }
    else if (mode.Equals("CEILING")) {
        return RoundingMode::CEILING;
    }
    else if (mode.Equals("FLOOR")) {
        return RoundingMode::FLOOR;
    }
    else if (mode.Equals("HALF_UP")) {
        return RoundingMode::HALF_UP;
    }
    else if (mode.Equals("HALF_DOWN")) {
        return RoundingMode::HALF_DOWN;
    }
    else if (mode.Equals("HALF_EVEN")) {
        return RoundingMode::HALF_EVEN;
    }
    else if (mode.Equals("UNNECESSARY")) {
        return RoundingMode::UNNECESSARY;
    }

    return static_cast<RoundingMode>(-1);
}

}
}