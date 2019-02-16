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

#ifndef __CCM_MATH_MATHCONTEXT_H__
#define __CCM_MATH_MATHCONTEXT_H__

#include "ccm/core/SyncObject.h"
#include "ccm.io.ISerializable.h"
#include "ccm.math.IMathContext.h"

using ccm::core::SyncObject;
using ccm::io::ISerializable;

namespace ccm {
namespace math {

class MathContext
    : public SyncObject
    , public IMathContext
    , public ISerializable
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Integer precision);

    ECode Constructor(
        /* [in] */ Integer precision,
        /* [in] */ RoundingMode roundingMode);

    ECode Constructor(
        /* [in] */ const String& s);

    ECode GetPrecision(
        /* [out] */ Integer* precision) override;

    ECode GetRoundingMode(
        /* [out] */ RoundingMode* roundingMode) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* same) override;

    ECode GetHashCode(
        /* [out] */ Integer* hash) override;

    ECode ToString(
        /* [out] */ String* desc) override;

    static const AutoPtr<IMathContext> GetDECIMAL128();

    static const AutoPtr<IMathContext> GetDECIMAL32();

    static const AutoPtr<IMathContext> GetDECIMAL64();

    static const AutoPtr<IMathContext> GetUNLIMITED();

private:
    ECode CheckValid();

    static RoundingMode ParseRoundingMode(
        /* [in] */ const String& mode);

private:
    /**
     * The number of digits to be used for an operation; results are rounded to
     * this precision.
     */
    Integer mPrecision = 0;

    /**
     * A {@code RoundingMode} object which specifies the algorithm to be used
     * for rounding.
     */
    RoundingMode mRoundingMode;
};

}
}

#endif // __CCM_MATH_MATHCONTEXT_H__
