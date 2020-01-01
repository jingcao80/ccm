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

#ifndef __COMO_MISC_FORMATTEDFLOATINGDECIMAL_H__
#define __COMO_MISC_FORMATTEDFLOATINGDECIMAL_H__

#include "como/core/SyncObject.h"
#include "como.core.IThreadLocal.h"
#include "como.misc.IFormattedFloatingDecimal.h"
#include "como.misc.IFloatingDecimalBinaryToASCIIConverter.h"

using como::core::IThreadLocal;
using como::core::SyncObject;

namespace como {
namespace misc {

class FormattedFloatingDecimal
    : public SyncObject
    , public IFormattedFloatingDecimal
{
public:
    COMO_INTERFACE_DECL();

    static ECode ValueOf(
        /* [in] */ Double d,
        /* [in] */ Integer precision,
        /* [in] */ FormattedFloatingDecimalForm form,
        /* [out] */ IFormattedFloatingDecimal** fd);

    ECode GetExponentRounded(
        /* [out] */ Integer* exponent) override;

    ECode GetMantissa(
        /* [out, callee] */ Array<Char>* mantissa) override;

    ECode GetExponent(
        /* [out, callee] */ Array<Char>* exponent) override;

private:
    ECode Constructor(
        /* [in] */ Integer precision,
        /* [in] */ FormattedFloatingDecimalForm form,
        /* [in] */ IFloatingDecimalBinaryToASCIIConverter* fdConverter);

    static Array<Char>& GetBuffer();

    static Integer ApplyPrecision(
        /* [in] */ Integer decExp,
        /* [out] */ Array<Char>& digits,
        /* [in] */ Integer nDigits,
        /* [in] */ Integer prec);

    void FillCompatible(
        /* [in] */ Integer precision,
        /* [out] */ Array<Char>& digits,
        /* [in] */ Integer nDigits,
        /* [in] */ Integer exp,
        /* [in] */ Boolean isNegative);

    static Array<Char> Create(
        /* [in] */ Boolean isNegative,
        /* [in] */ Integer size);

    void FillDecimal(
        /* [in] */ Integer precision,
        /* [out] */ Array<Char>& digits,
        /* [in] */ Integer nDigits,
        /* [in] */ Integer exp,
        /* [in] */ Boolean isNegative);

    void FillScientific(
        /* [in] */ Integer precision,
        /* [out] */ Array<Char>& digits,
        /* [in] */ Integer nDigits,
        /* [in] */ Integer exp,
        /* [in] */ Boolean isNegative);

    static AutoPtr<IThreadLocal> GetThreadLocalCharBuffer();

private:
    Integer mDecExponentRounded;
    Array<Char> mMantissa;
    Array<Char> mExponent;
};

}
}

#endif // __COMO_MISC_FORMATTEDFLOATINGDECIMAL_H__
