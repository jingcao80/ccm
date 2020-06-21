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

#ifndef __COMO_TEXT_CHOICEFORMAT_H__
#define __COMO_TEXT_CHOICEFORMAT_H__

#include "como/text/NumberFormat.h"
#include "como.text.IChoiceFormat.h"
#include "como.text.IParsePosition.h"

namespace como {
namespace text {

class ChoiceFormat
    : public NumberFormat
    , public IChoiceFormat
{
public:
    COMO_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ const String& newPattern);

    ECode Constructor(
        /* [in] */ const Array<Double>& limits,
        /* [in] */ const Array<String>& formats);

    ECode ApplyPattern(
        /* [in] */ const String& newPattern) override;

    ECode ToPattern(
        /* [out] */ String& pattern) override;

    ECode SetChoices(
        /* [in] */ const Array<Double>& limits,
        /* [in] */ const Array<String>& formats) override;

    ECode GetLimits(
        /* [out, callee] */ Array<Double>* limits) override;

    ECode GetFormats(
        /* [out, callee] */ Array<String>* formats) override;

    ECode Format(
        /* [in] */ Long number,
        /* [out] */ IStringBuffer* toAppendTo,
        /* [in] */ IFieldPosition* status) override;

    ECode Format(
        /* [in] */ Double number,
        /* [out] */ IStringBuffer* toAppendTo,
        /* [in] */ IFieldPosition* status) override;

    ECode Parse(
        /* [in] */ const String& text,
        /* [in] */ IParsePosition* status,
        /* [out] */ AutoPtr<INumber>& number) override;

    static Double NextDouble(
        /* [in] */ Double d);

    static Double PreviousDouble(
        /* [in] */ Double d);

    ECode GetHashCode(
        /* [out] */ Integer& hash) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& same) override;

    static Double NextDouble(
        /* [in] */ Double d,
        /* [in] */ Boolean positive);

    static Array<Double> DoubleArraySize(
        /* [in] */ const Array<Double>& array);

    static Array<String> DoubleArraySize(
        /* [in] */ const Array<String>& array);

protected:
    ECode CloneImpl(
        /* [in] */ IChoiceFormat* newObj);

private:
    /**
     * A list of lower bounds for the choices. The formatter will return
     * choiceFormats[i] if the number being formatted is greater than or equal to
     * choiceLimits[i] and less than choiceLimits[i+1].
     */
    Array<Double> mChoiceLimits;

    /**
     * A list of choice strings.  The formatter will return
     * choiceFormats[i] if the number being formatted is greater than or equal to
     * choiceLimits[i] and less than choiceLimits[i+1].
     */
    Array<String> mChoiceFormats;

    static constexpr Long SIGN = 0x8000000000000000LL;
    static constexpr Long POSITIVEINFINITY = 0x7FF0000000000000LL;
};

}
}

#endif // __COMO_TEXT_CHOICEFORMAT_H__
