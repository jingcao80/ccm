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

#include "como/core/CDouble.h"
#include "como/core/CStringBuffer.h"
#include "como/core/Math.h"
#include "como/core/StringUtils.h"
#include "como/text/ChoiceFormat.h"
#include "como/util/Arrays.h"
#include "como.core.IDouble.h"
#include "como.core.IStringBuffer.h"
#include <ccmlogger.h>

using como::core::CDouble;
using como::core::CStringBuffer;
using como::core::IDouble;
using como::core::IID_INumber;
using como::core::IID_IStringBuffer;
using como::core::IStringBuffer;
using como::core::Math;
using como::core::StringUtils;
using como::util::Arrays;

namespace como {
namespace text {

COMO_INTERFACE_IMPL_1(ChoiceFormat, NumberFormat, IChoiceFormat);

ECode ChoiceFormat::ApplyPattern(
    /* [in] */ const String& newPattern)
{
    Array<IStringBuffer*> segments(2);
    for (Integer i = 0; i < segments.GetLength(); ++i) {
        AutoPtr<IStringBuffer> sb;
        CStringBuffer::New(IID_IStringBuffer, (IInterface**)&sb);
        segments.Set(i, sb);
    }
    Array<Double> newChoiceLimits(30);
    Array<String> newChoiceFormats(30);
    Integer count = 0;
    Integer part = 0;
    Double startValue = 0;
    Double oldStartValue = IDouble::NaN;
    Boolean inQuote = false;
    for (Integer i = 0; i < newPattern.GetLength(); ++i) {
        Char ch = newPattern.GetChar(i);
        if (ch == U'\'') {
            // Check for "''" indicating a literal quote
            if ((i + 1) < newPattern.GetLength() && newPattern.GetChar(i + 1) == ch) {
                segments[part]->Append(ch);
                ++i;
            }
            else {
                inQuote = !inQuote;
            }
        }
        else if (inQuote) {
            segments[part]->Append(ch);
        }
        else if (ch == U'<' || ch == U'#' || ch == 0x2264) {
            Integer size;
            if (segments[0]->GetLength(&size), size == 0) {
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            String tempBuffer;
            segments[0]->ToString(&tempBuffer);
            if (tempBuffer.Equals(String::ValueOf(0x221E))) {
                startValue = IDouble::POSITIVE_INFINITY;
            }
            else if (tempBuffer.Equals(String::ValueOf(-0x221E))) {
                startValue = IDouble::NEGATIVE_INFINITY;
            }
            else {
                ECode ec = StringUtils::ParseDouble(tempBuffer, &startValue);
                if (FAILED(ec)) {
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }
            }
            if (ch == U'<' && startValue != IDouble::POSITIVE_INFINITY &&
                    startValue != IDouble::NEGATIVE_INFINITY) {
                startValue = NextDouble(startValue);
            }
            if (startValue <= oldStartValue) {
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            segments[0]->SetLength(0);
            part = 1;
        }
        else if (ch == U'|') {
            if (count == newChoiceLimits.GetLength()) {
                newChoiceLimits = DoubleArraySize(newChoiceLimits);
                newChoiceFormats = DoubleArraySize(newChoiceFormats);
            }
            String tempBuffer;
            segments[1]->ToString(&tempBuffer);
            newChoiceLimits[count] = startValue;
            newChoiceFormats[count] = tempBuffer;
            ++count;
            oldStartValue = startValue;
            segments[1]->SetLength(0);
            part = 0;
        }
        else {
            segments[part]->Append(ch);
        }
    }
    // clean up last one
    if (part == 1) {
        if (count == newChoiceLimits.GetLength()) {
            newChoiceLimits = DoubleArraySize(newChoiceLimits);
            newChoiceFormats = DoubleArraySize(newChoiceFormats);
        }
        String tempBuffer;
        segments[1]->ToString(&tempBuffer);
        newChoiceLimits[count] = startValue;
        newChoiceFormats[count] = tempBuffer;
        ++count;
    }
    mChoiceLimits = Array<Double>(count);
    mChoiceLimits.Copy(0, newChoiceLimits, 0, count);
    mChoiceFormats = Array<String>(count);
    mChoiceFormats.Copy(0, newChoiceFormats, 0, count);
    return NOERROR;
}

ECode ChoiceFormat::ToPattern(
    /* [out] */ String* pattern)
{
    VALIDATE_NOT_NULL(pattern);

    AutoPtr<IStringBuffer> result;
    CStringBuffer::New(IID_IStringBuffer, (IInterface**)&result);
    for (Integer i = 0; i < mChoiceLimits.GetLength(); ++i) {
        if (i != 0) {
            result->Append(U'|');
        }
        // choose based upon which has less precision
        // approximate that by choosing the closest one to an integer.
        // could do better, but it's not worth it.
        Double less = PreviousDouble(mChoiceLimits[i]);
        Double tryLessOrEqual = Math::Abs(Math::IEEEremainder(mChoiceLimits[i], 1.0));
        Double tryLess = Math::Abs(Math::IEEEremainder(less, 1.0));

        if (tryLessOrEqual < tryLess) {
            result->Append(String("") + mChoiceLimits[i]);
            result->Append(U'#');
        }
        else {
            if (mChoiceLimits[i] == IDouble::POSITIVE_INFINITY) {
                result->Append(String::ValueOf(0x221E));
            }
            else if (mChoiceLimits[i] == IDouble::NEGATIVE_INFINITY) {
                result->Append(String::ValueOf(-0x221E));
            }
            else {
                result->Append(less);
            }
            result->Append(U'<');
        }
        // Append choiceFormats[i], using quotes if there are special characters.
        // Single quotes themselves must be escaped in either case.
        String text = mChoiceFormats[i];
        Boolean needQuote = text.IndexOf(U'<') >= 0 ||
                text.IndexOf(U'#') >= 0 ||
                text.IndexOf((Char)0x2264) >= 0 ||
                text.IndexOf(U'|') >= 0;
        if (needQuote) {
            result->Append(U'\'');
        }
        if (text.IndexOf(U'\'') < 0) {
            result->Append(text);
        }
        else {
            for (Integer j = 0; j < text.GetLength(); ++j) {
                Char c = text.GetChar(j);
                result->Append(c);
                if (c == U'\'') {
                    result->Append(c);
                }
            }
        }
        if (needQuote) {
            result->Append(U'\'');
        }
    }
    return result->ToString(pattern);
}

ECode ChoiceFormat::Constructor(
    /* [in] */ const String& newPattern)
{
    return ApplyPattern(newPattern);
}

ECode ChoiceFormat::Constructor(
    /* [in] */ const Array<Double>& limits,
    /* [in] */ const Array<String>& formats)
{
    return SetChoices(limits, formats);
}

ECode ChoiceFormat::SetChoices(
    /* [in] */ const Array<Double>& limits,
    /* [in] */ const Array<String>& formats)
{
    if (limits.GetLength() != formats.GetLength()) {
        Logger::E("ChoiceFormat", "Array and limit arrays must be of the same length.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Arrays::CopyOf(limits, limits.GetLength(), &mChoiceLimits);
    Arrays::CopyOf(formats, formats.GetLength(), &mChoiceFormats);
    return NOERROR;
}

ECode ChoiceFormat::GetLimits(
    /* [out, callee] */ Array<Double>* limits)
{
    VALIDATE_NOT_NULL(limits);

    return Arrays::CopyOf(mChoiceLimits, mChoiceLimits.GetLength(), limits);
}

ECode ChoiceFormat::GetFormats(
    /* [out, callee] */ Array<String>* formats)
{
    VALIDATE_NOT_NULL(formats);

    return Arrays::CopyOf(mChoiceFormats, mChoiceFormats.GetLength(), formats);
}

ECode ChoiceFormat::Format(
    /* [in] */ Long number,
    /* [out] */ IStringBuffer* toAppendTo,
    /* [in] */ IFieldPosition* status)
{
    return Format((Double)number, toAppendTo, status);
}

ECode ChoiceFormat::Format(
    /* [in] */ Double number,
    /* [out] */ IStringBuffer* toAppendTo,
    /* [in] */ IFieldPosition* status)
{
    Integer i;
    for (i = 0; i < mChoiceLimits.GetLength(); ++i) {
        if (number < mChoiceLimits[i]) {
            break;
        }
    }
    --i;
    if (i < 0) {
        i = 0;
    }
    return toAppendTo->Append(mChoiceFormats[i]);
}

ECode ChoiceFormat::Parse(
    /* [in] */ const String& text,
    /* [in] */ IParsePosition* status,
    /* [out] */ INumber** number)
{
    VALIDATE_NOT_NULL(number);

    // find the best number (defined as the one with the longest parse)
    Integer start;
    status->GetIndex(&start);
    Integer furthest = start;
    Double bestNumber = IDouble::NaN;
    Double tempNumber = 0;
    for (Integer i = 0; i < mChoiceFormats.GetLength(); ++i) {
        String tempString = mChoiceFormats[i];
        if (text.RegionMatches(start, tempString, 0, tempString.GetLength())) {
            status->SetIndex(start + tempString.GetLength());
            tempNumber = mChoiceLimits[i];
            Integer index;
            if (status->GetIndex(&index), index > furthest) {
                furthest = index;
                bestNumber = tempNumber;
                if (furthest == text.GetLength()) {
                    break;
                }
            }
        }
    }
    status->SetIndex(furthest);
    if (furthest == start) {
        status->SetErrorIndex(furthest);
    }
    return CDouble::New(bestNumber, IID_INumber, (IInterface**)number);
}

Double ChoiceFormat::NextDouble(
    /* [in] */ Double d)
{
    return NextDouble(d, true);
}

Double ChoiceFormat::PreviousDouble(
    /* [in] */ Double d)
{
    return NextDouble(d, false);
}

ECode ChoiceFormat::CloneImpl(
    /* [in] */ IChoiceFormat* newObj)
{
    NumberFormat::CloneImpl(INumberFormat::Probe(newObj));

    ChoiceFormat* other = (ChoiceFormat*)newObj;
    other->mChoiceLimits = mChoiceLimits.Clone();
    other->mChoiceFormats = mChoiceFormats.Clone();
    return NOERROR;
}

ECode ChoiceFormat::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    Integer result = mChoiceLimits.GetLength();
    if (mChoiceFormats.GetLength() > 0) {
        result ^= mChoiceFormats[mChoiceFormats.GetLength() - 1].GetHashCode();
    }
    *hash = result;
    return NOERROR;
}

ECode ChoiceFormat::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    ChoiceFormat* other = (ChoiceFormat*)IChoiceFormat::Probe(obj);
    if (other == nullptr) {
        *same = false;
        return NOERROR;
    }
    if (this == other) {
        *same = true;
        return NOERROR;
    }
    *same = Arrays::Equals(mChoiceLimits, other->mChoiceLimits) &&
            Arrays::Equals(mChoiceFormats, other->mChoiceFormats);
    return NOERROR;
}

Double ChoiceFormat::NextDouble(
    /* [in] */ Double d,
    /* [in] */ Boolean positive)
{
    if (Math::IsNaN(d)) {
        return d;
    }

    /* zero's are also a special case */
    if (d = 0.0) {
        Double smallestPositiveDouble = Math::LongBitsToDouble(1LL);
        if (positive) {
            return smallestPositiveDouble;
        }
        else {
            return -smallestPositiveDouble;
        }
    }

    /* if entering here, d is a nonzero value */

    /* hold all bits in a long for later use */
    Long bits = Math::DoubleToLongBits(d);

    /* strip off the sign bit */
    Long magnitude = bits & ~SIGN;

    /* if next double away from zero, increase magnitude */
    if ((bits > 0) == positive) {
        if (magnitude != POSITIVEINFINITY) {
            magnitude += 1;
        }
    }
    else {
        /* else decrease magnitude */
        magnitude -= 1;
    }

    /* restore sign bit and return */
    Long signbit = bits & SIGN;
    return Math::LongBitsToDouble(magnitude | signbit);
}

Array<Double> ChoiceFormat::DoubleArraySize(
    /* [in] */ const Array<Double>& array)
{
    Integer oldSize = array.GetLength();
    Array<Double> newArray(oldSize * 2);
    newArray.Copy(0, array, 0, oldSize);
    return newArray;
}

Array<String> ChoiceFormat::DoubleArraySize(
    /* [in] */ const Array<String>& array)
{
    Integer oldSize = array.GetLength();
    Array<String> newArray(oldSize * 2);
    newArray.Copy(0, array, 0, oldSize);
    return newArray;
}

}
}
