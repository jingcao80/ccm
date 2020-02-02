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

#include "como/core/CArrayHolder.h"
#include "como/core/CoreUtils.h"
#include "como/core/CStringBuffer.h"
#include "como/core/CStringBuilder.h"
#include "como/core/StringUtils.h"
#include "como/text/CChoiceFormat.h"
#include "como/text/CDecimalFormat.h"
#include "como/text/CMessageFormat.h"
#include "como/text/CParsePosition.h"
#include "como/text/CSimpleDateFormat.h"
#include "como/text/DateFormat.h"
#include "como/text/DecimalFormatSymbols.h"
#include "como/text/MessageFormat.h"
#include "como/text/NumberFormat.h"
#include "como/util/Arrays.h"
#include "como/util/CArrayList.h"
#include "como/util/CLocale.h"
#include "como.core.IString.h"
#include "como.text.IChoiceFormat.h"
#include "como.text.IDateFormat.h"
#include "como.text.IDecimalFormat.h"
#include "como.text.ISimpleDateFormat.h"
#include <comolog.h>

using como::core::CArrayHolder;
using como::core::CoreUtils;
using como::core::CStringBuffer;
using como::core::CStringBuilder;
using como::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
using como::core::E_NULL_POINTER_EXCEPTION;
using como::core::IArrayHolder;
using como::core::IID_IArrayHolder;
using como::core::IID_IStringBuffer;
using como::core::IID_IStringBuilder;
using como::core::IString;
using como::core::StringUtils;
using como::util::Arrays;
using como::util::CArrayList;
using como::util::CLocale;
using como::util::IID_IList;

namespace como {
namespace text {

Array<String> MessageFormat::TYPE_KEYWORDS = {
    String(""),
    String("number"),
    String("date"),
    String("time"),
    String("choice")
};

Array<String> MessageFormat::NUMBER_MODIFIER_KEYWORDS = {
    String(""),
    String("currency"),
    String("percent"),
    String("integer")
};

Array<String> MessageFormat::DATE_TIME_MODIFIER_KEYWORDS = {
    String(""),
    String("short"),
    String("medium"),
    String("long"),
    String("full")
};

Array<Integer> MessageFormat::DATE_TIME_MODIFIERS = {
    IDateFormat::DEFAULT,
    IDateFormat::SHORT,
    IDateFormat::MEDIUM,
    IDateFormat::LONG,
    IDateFormat::FULL
};

COMO_INTERFACE_IMPL_1(MessageFormat, BaseFormat, IMessageFormat);

ECode MessageFormat::Constructor(
    /* [in] */ const String& pattern)
{
    mLocale = CLocale::GetDefault(CLocale::Category::GetFORMAT());
    return ApplyPattern(pattern);
}

ECode MessageFormat::Constructor(
    /* [in] */ const String& pattern,
    /* [in] */ ILocale* locale)
{
    mLocale = locale;
    return ApplyPattern(pattern);
}

ECode MessageFormat::SetLocale(
    /* [in] */ ILocale* locale)
{
    mLocale = locale;
    return NOERROR;
}

ECode MessageFormat::GetLocale(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale);

    *locale = mLocale;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}

ECode MessageFormat::ApplyPattern(
    /* [in] */ const String& pattern)
{
    Array<IStringBuilder*> segments(4);

    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    segments.Set(SEG_RAW, sb);

    Integer part = SEG_RAW;
    Integer formatNumber = 0;
    Boolean inQuote = false;
    Integer braceStack = 0;
    mMaxOffset = -1;
    for (Integer i = 0; i < pattern.GetLength(); ++i) {
        Char ch = pattern.GetChar(i);
        if (part == SEG_RAW) {
            if (ch == '\'') {
                if (i + 1 < pattern.GetLength() &&
                        pattern.GetChar(i + 1) == '\'') {
                    segments[part]->Append(ch);
                    ++i;
                }
                else {
                    inQuote = !inQuote;
                }
            }
            else if (ch == '{' && !inQuote) {
                part = SEG_INDEX;
                if (segments[SEG_INDEX] == nullptr) {
                    AutoPtr<IStringBuilder> sb;
                    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
                    segments.Set(SEG_INDEX, sb);
                }
            }
            else {
                segments[part]->Append(ch);
            }
        }
        else {
            if (inQuote) {
                segments[part]->Append(ch);
                if (ch == '\'') {
                    inQuote = false;
                }
            }
            else {
                switch (ch) {
                    case ',':
                        if (part < SEG_MODIFIER) {
                            if (segments[++part] == nullptr) {
                                AutoPtr<IStringBuilder> sb;
                                CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
                                segments.Set(part, sb);
                            }
                        }
                        else {
                            segments[part]->Append(ch);
                        }
                        break;
                    case '{':
                        ++braceStack;
                        segments[part]->Append(ch);
                        break;
                    case '}':
                        if (braceStack == 0) {
                            part = SEG_RAW;
                            FAIL_RETURN(MakeFormat(i, formatNumber, segments));
                            formatNumber++;
                            segments.Set(SEG_INDEX, nullptr);
                            segments.Set(SEG_TYPE, nullptr);
                            segments.Set(SEG_MODIFIER, nullptr);
                        }
                        else {
                            --braceStack;
                            segments[part]->Append(ch);
                        }
                        break;
                    case ' ': {
                        // Skip any leading space chars for SEG_TYPE.
                        Integer size;
                        if (part != SEG_TYPE || (segments[SEG_TYPE]->GetLength(size), size > 0)) {
                            segments[part]->Append(ch);
                        }
                        break;
                    }
                    case '\'':
                        inQuote = true;
                        // fall through, so we keep quotes in other parts
                    default:
                        segments[part]->Append(ch);
                        break;
                }
            }
        }
    }
    if (braceStack == 0 && part != 0) {
        mMaxOffset = -1;
        Logger::E("MessageFormat", "Unmatched braces in the pattern.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    segments[0]->ToString(mPattern);
    return NOERROR;
}

ECode MessageFormat::ToPattern(
    /* [out] */ String* pattern)
{
    VALIDATE_NOT_NULL(pattern);

    Integer lastOffset = 0;
    AutoPtr<IStringBuilder> result;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&result);
    for (Integer i = 0; i <= mMaxOffset; ++i) {
        CopyAndFixQuotes(mPattern, lastOffset, mOffsets[i], result);
        lastOffset = mOffsets[i];
        result->Append(U'{');
        result->Append(mArgumentNumbers[i]);
        AutoPtr<IFormat> fmt = mFormats[i];
        if (fmt == nullptr) {
            // do nothing, string format
        }
        else if (INumberFormat::Probe(fmt) != nullptr) {
            AutoPtr<INumberFormat> format;
            FAIL_RETURN(NumberFormat::GetInstance(mLocale, &format));
            if (Object::Equals(fmt, format)) {
                result->Append(String(",number"));
            }
            else {
                format = nullptr;
                FAIL_RETURN(NumberFormat::GetCurrencyInstance(mLocale, &format));
                if (Object::Equals(fmt, format)) {
                    result->Append(String(",number,currency"));
                }
                else {
                    format = nullptr;
                    FAIL_RETURN(NumberFormat::GetPercentInstance(mLocale, &format));
                    if (Object::Equals(fmt, format)) {
                        result->Append(String(",number,percent"));
                    }
                    else {
                        format = nullptr;
                        FAIL_RETURN(NumberFormat::GetIntegerInstance(mLocale, &format));
                        if (Object::Equals(fmt, format)) {
                            result->Append(String(",number,integer"));
                        }
                        else {
                            if (IDecimalFormat::Probe(fmt) != nullptr) {
                                IDecimalFormat::Probe(fmt)->ToPattern(pattern);
                                result->Append(String(",number,"));
                                result->Append(*pattern);
                            }
                            else if (IChoiceFormat::Probe(fmt) != nullptr) {
                                IChoiceFormat::Probe(fmt)->ToPattern(pattern);
                                result->Append(String(",choice,"));
                                result->Append(*pattern);
                            }
                            else {
                                // UNKNOWN
                            }
                        }
                    }
                }
            }
        }
        else if (IDateFormat::Probe(fmt) != nullptr) {
            Integer index;
            for (index = MODIFIER_DEFAULT; index < DATE_TIME_MODIFIERS.GetLength(); index++) {
                AutoPtr<IDateFormat> df;
                FAIL_RETURN(DateFormat::GetDateInstance(DATE_TIME_MODIFIERS[index], mLocale, &df));
                if (Object::Equals(fmt, df)) {
                    result->Append(String(",date"));
                    break;
                }
                df = nullptr;
                FAIL_RETURN(DateFormat::GetTimeInstance(DATE_TIME_MODIFIERS[index], mLocale, &df));
                if (Object::Equals(fmt, df)) {
                    result->Append(String(",time"));
                    break;
                }
            }
            if (index >= DATE_TIME_MODIFIERS.GetLength()) {
                if (ISimpleDateFormat::Probe(fmt) != nullptr) {
                    ISimpleDateFormat::Probe(fmt)->ToPattern(pattern);
                    result->Append(String(",date,"));
                    result->Append(pattern);
                }
                else {
                    // UNKNOWN
                }
            }
            else if (index != MODIFIER_DEFAULT) {
                result->Append(U',');
                result->Append(DATE_TIME_MODIFIER_KEYWORDS[index]);
            }
        }
        else {
            //result.append(", unknown");
        }
        result->Append(U'}');
    }
    CopyAndFixQuotes(mPattern, lastOffset, mPattern.GetLength(), result);
    return result->ToString(*pattern);
}

ECode MessageFormat::SetFormatsByArgumentIndex(
    /* [in] */ const Array<IFormat*>& newFormats)
{
    for (Integer i = 0; i < mMaxOffset; i++) {
        Integer j = mArgumentNumbers[i];
        if (j < newFormats.GetLength()) {
            mFormats.Set(i, newFormats[j]);
        }
    }
    return NOERROR;
}

ECode MessageFormat::SetFormats(
    /* [in] */ const Array<IFormat*>& newFormats)
{
    Integer runsToCopy = newFormats.GetLength();
    if (runsToCopy > mMaxOffset + 1) {
        runsToCopy = mMaxOffset + 1;
    }
    for (Integer i = 0; i < runsToCopy; i++) {
        mFormats.Set(i, newFormats[i]);
    }
    return NOERROR;
}

ECode MessageFormat::SetFormatByArgumentIndex(
    /* [in] */ Integer argumentIndex,
    /* [in] */ IFormat* newFormat)
{
    for (Integer j = 0; j <= mMaxOffset; j++) {
        if (mArgumentNumbers[j] == argumentIndex) {
            mFormats.Set(j, newFormat);
        }
    }
    return NOERROR;
}

ECode MessageFormat::SetFormat(
    /* [in] */ Integer formatElementIndex,
    /* [in] */ IFormat* newFormat)
{
    if (formatElementIndex > mMaxOffset) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    mFormats.Set(formatElementIndex, newFormat);
    return NOERROR;
}

ECode MessageFormat::GetFormatsByArgumentIndex(
    /* [out, callee] */ Array<IFormat*>* formats)
{
    VALIDATE_NOT_NULL(formats);

    Integer maximumArgumentNumber = -1;
    for (Integer i = 0; i <= mMaxOffset; i++) {
        if (mArgumentNumbers[i] > maximumArgumentNumber) {
            maximumArgumentNumber = mArgumentNumbers[i];
        }
    }
    Array<IFormat*> resultArray(maximumArgumentNumber + 1);
    for (Integer i = 0; i <= mMaxOffset; i++) {
        resultArray.Set(mArgumentNumbers[i], mFormats[i]);
    }
    *formats = resultArray;
    return NOERROR;
}

ECode MessageFormat::GetFormats(
    /* [out, callee] */ Array<IFormat*>* formats)
{
    VALIDATE_NOT_NULL(formats);

    Array<IFormat*> resultArray(mMaxOffset + 1);
    resultArray.Copy(0, mFormats, 0, mMaxOffset + 1);
    *formats = resultArray;
    return NOERROR;
}

ECode MessageFormat::Format(
    /* [in] */ const Array<IInterface*>& arguments,
    /* [out] */ IStringBuffer* result,
    /* [in] */ IFieldPosition* pos)
{
    return Subformat(arguments, result, pos, nullptr);
}

ECode MessageFormat::Format(
    /* [in] */ const String& pattern,
    /* [in] */ Array<IInterface*>* arguments,
    /* [out] */ String* message)
{
    VALIDATE_NOT_NULL(message);

    AutoPtr<IArrayHolder> argumentsholder;
    if (arguments != nullptr){
        CArrayHolder::New(*arguments, IID_IArrayHolder, (IInterface**)&argumentsholder);
    }
    AutoPtr<IFormat> temp;
    CMessageFormat::New(pattern, IID_IFormat, (IInterface**)&temp);
    return temp->Format(argumentsholder, message);
}

ECode MessageFormat::Format(
    /* [in] */ IInterface* arguments,
    /* [in] */ IStringBuffer* result,
    /* [in] */ IFieldPosition* pos)
{
    Array<IInterface*> argumentsArray;
    if (IArrayHolder::Probe(arguments) != nullptr) {
        IArrayHolder::Probe(arguments)->GetArray(&argumentsArray);
    }
    return Subformat(argumentsArray, result, pos, nullptr);
}

ECode MessageFormat::FormatToCharacterIterator(
    /* [in] */ IInterface* arguments,
    /* [out] */ IAttributedCharacterIterator** cit)
{
    VALIDATE_NOT_NULL(cit);

    if (IArrayHolder::Probe(arguments) == nullptr) {
        Logger::E("MessageFormat", "formatToCharacterIterator must be passed non-null object");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IStringBuffer> result;
    CStringBuffer::New(IID_IStringBuffer, (IInterface**)&result);
    AutoPtr<IList> iterators;
    CArrayList::New(IID_IList, (IInterface**)&iterators);
    Array<IInterface*> argumentsArray;
    IArrayHolder::Probe(arguments)->GetArray(&argumentsArray);
    FAIL_RETURN(Subformat(argumentsArray, result, nullptr, iterators));
    Integer size;
    if (iterators->GetSize(&size), size == 0) {
        return CreateAttributedCharacterIterator(String(""), cit);
    }
    Array<IAttributedCharacterIterator*> its;
    iterators->ToArray(IID_IAttributedCharacterIterator, (Array<IInterface*>*)&its);
    return CreateAttributedCharacterIterator(its, cit);
}

ECode MessageFormat::Parse(
    /* [in] */ const String& source,
    /* [in] */ IParsePosition* pos,
    /* [out, callee] */ Array<IInterface*>* result)
{
    VALIDATE_NOT_NULL(result);

    if (source.IsNull()) {
        *result = {};
        return NOERROR;
    }

    Integer maximumArgumentNumber = -1;
    for (Integer i = 0; i <= mMaxOffset; i++) {
        if (mArgumentNumbers[i] > maximumArgumentNumber) {
            maximumArgumentNumber = mArgumentNumbers[i];
        }
    }
    Array<IInterface*> resultArray(maximumArgumentNumber + 1);

    Integer patternOffset = 0;
    Integer sourceOffset;
    pos->GetIndex(&sourceOffset);
    AutoPtr<IParsePosition> tempStatus;
    CParsePosition::New(0, IID_IParsePosition, (IInterface**)&tempStatus);
    for (Integer i = 0; i <= mMaxOffset; ++i) {
        // match up to format
        Integer len = mOffsets[i] - patternOffset;
        if (len == 0 || mPattern.RegionMatches(patternOffset,
                source, sourceOffset, len)) {
            sourceOffset += len;
            patternOffset += len;
        }
        else {
            pos->SetErrorIndex(sourceOffset);
            *result = Array<IInterface*>::Null();
            return NOERROR; // leave index as is to signal error
        }

        // now use format
        if (mFormats[i] == nullptr) {
            // string format
            // if at end, use longest possible match
            // otherwise uses first match to intervening string
            // does NOT recursively try all possibilities
            Integer tempLength = (i != mMaxOffset) ? mOffsets[i + 1] : mPattern.GetLength();

            Integer next;
            if (patternOffset >= tempLength) {
                next = source.GetLength();
            }
            else {
                next = source.IndexOf(mPattern.Substring(patternOffset, tempLength),
                        sourceOffset);
            }

            if (next < 0) {
                pos->SetErrorIndex(sourceOffset);
                *result = Array<IInterface*>::Null();
                return NOERROR; // leave index as is to signal error
            }
            else {
                String strValue = source.Substring(sourceOffset, next);
                if (!strValue.Equals(String("{") + mArgumentNumbers[i] + "}")) {
                    resultArray.Set(mArgumentNumbers[i], CoreUtils::Box(source.Substring(sourceOffset, next)));
                }
                sourceOffset = next;
            }
        }
        else {
            tempStatus->SetIndex(sourceOffset);
            AutoPtr<IInterface> object;
            mFormats[i]->ParseObject(source, tempStatus, &object);
            resultArray.Set(mArgumentNumbers[i], object);
            Integer index;
            tempStatus->GetIndex(&index);
            if (index == sourceOffset) {
                pos->SetErrorIndex(sourceOffset);
                *result = Array<IInterface*>::Null();
                return NOERROR; // leave index as is to signal error
            }
            sourceOffset = index; // update
        }
    }
    Integer len = mPattern.GetLength() - patternOffset;
    if (len == 0 || mPattern.RegionMatches(patternOffset, source,
            sourceOffset, len)) {
        pos->SetIndex(sourceOffset + len);
    }
    else {
        pos->SetErrorIndex(sourceOffset);
        *result = Array<IInterface*>::Null();
        return NOERROR; // leave index as is to signal error
    }
    *result = resultArray;
    return NOERROR;
}

ECode MessageFormat::Parse(
    /* [in] */ const String& source,
    /* [out, callee] */ Array<IInterface*>* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IParsePosition> pos;
    CParsePosition::New(0, IID_IParsePosition, (IInterface**)&pos);
    Parse(source, pos, result);
    Integer index;
    pos->GetIndex(&index);
    if (index == 0) {
        Logger::E("MessageFormat", "MessageFormat parse error!");
        return E_PARSE_EXCEPTION;
    }
    return NOERROR;
}

ECode MessageFormat::ParseObject(
    /* [in] */ const String& source,
    /* [in] */ IParsePosition* pos,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    Array<IInterface*> resultArray;
    Parse(source, pos, &resultArray);
    CArrayHolder::New(resultArray, IID_IArrayHolder, result);
    return NOERROR;
}

ECode MessageFormat::CloneImpl(
    /* [in] */ IMessageFormat* newObj)
{
    MessageFormat* other = (MessageFormat*)newObj;

    other->mFormats = mFormats.Clone();
    for (Integer i = 0; i < mFormats.GetLength(); ++i) {
        if (mFormats[i] != nullptr) {
            other->mFormats.Set(i, (IFormat*)CoreUtils::Clone(mFormats[i], IID_IFormat).Get());
        }
    }
    other->mOffsets = mOffsets.Clone();
    other->mArgumentNumbers = mArgumentNumbers.Clone();

    return NOERROR;
}

ECode MessageFormat::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& same)
{
    MessageFormat* other = (MessageFormat*)IMessageFormat::Probe(obj);
    if (other == this) {
        same = true;
        return NOERROR;
    }
    if (other == nullptr) {
        same = false;
        return NOERROR;
    }
    same = (mMaxOffset == other->mMaxOffset &&
            mPattern.Equals(other->mPattern) &&
            Object::Equals(mLocale, other->mLocale) &&
            Arrays::Equals(mOffsets, other->mOffsets) &&
            Arrays::Equals(mArgumentNumbers, other->mArgumentNumbers) &&
            Arrays::Equals(mFormats, other->mFormats));
    return NOERROR;
}

ECode MessageFormat::GetHashCode(
    /* [out] */ Integer& hash)
{
    hash = mPattern.GetHashCode();
    return NOERROR;
}

ECode MessageFormat::Subformat(
    /* [in] */ const Array<IInterface*>& arguments,
    /* [in] */ IStringBuffer* result,
    /* [out] */ IFieldPosition* fp,
    /* [in] */ IList* characterIterators)
{
    Integer lastOffset = 0;
    Integer last;
    result->GetLength(last);
    for (Integer i = 0; i <= mMaxOffset; ++i) {
        result->Append(mPattern.Substring(lastOffset, mOffsets[i]));
        lastOffset = mOffsets[i];
        Integer argumentNumber = mArgumentNumbers[i];
        if (arguments.IsNull() || argumentNumber >= arguments.GetLength()) {
            result->Append(U'{');
            result->Append(argumentNumber);
            result->Append(U'}');
            continue;
        }
        AutoPtr<IInterface> obj = arguments[argumentNumber];
        String arg;
        AutoPtr<IFormat> subFormatter;
        if (obj == nullptr) {
            arg = "null";
        }
        else if (mFormats[i] != nullptr) {
            subFormatter = mFormats[i];
            if (IChoiceFormat::Probe(subFormatter) != nullptr) {
                mFormats[i]->Format(obj, &arg);
                if (arg.IndexOf(U'{') >= 0) {
                    subFormatter = nullptr;
                    CMessageFormat::New(arg, mLocale, IID_IFormat, (IInterface**)&subFormatter);
                    obj = nullptr;
                    CArrayHolder::New(arguments, IID_IArrayHolder, (IInterface**)&obj);
                    arg = nullptr;
                }
            }
        }
        else if (INumber::Probe(obj) != nullptr) {
            AutoPtr<INumberFormat> nf;
            NumberFormat::GetInstance(mLocale, &nf);
            subFormatter = std::move(nf);
        }
        else if (IDate::Probe(obj) != nullptr) {
            AutoPtr<IDateFormat> df;
            DateFormat::GetDateTimeInstance(
                    IDateFormat::SHORT, IDateFormat::SHORT, mLocale, &df);
            subFormatter = std::move(df);
        }
        else if (IString::Probe(obj) != nullptr) {
            IString::Probe(obj)->ToString(arg);
        }
        else {
            arg = Object::ToString(obj);
            if (arg.IsNull()) {
                arg = "null";
            }
        }

        // At this point we are in two states, either subFormatter
        // is non-null indicating we should format obj using it,
        // or arg is non-null and we should use it as the value.

        if (characterIterators != nullptr) {
            // If characterIterators is non-null, it indicates we need
            // to get the CharacterIterator from the child formatter.
            Integer length;
            result->GetLength(length);
            if (last != length) {
                String substr;
                result->Substring(last, substr);
                AutoPtr<IAttributedCharacterIterator> acit;
                CreateAttributedCharacterIterator(substr, &acit);
                characterIterators->Add(acit);
                result->GetLength(last);
            }
            if (subFormatter != nullptr) {
                AutoPtr<IAttributedCharacterIterator> subIterator;
                subFormatter->FormatToCharacterIterator(obj, &subIterator);

                Append(result, ICharacterIterator::Probe(subIterator));
                result->GetLength(length);
                if (last != length) {
                    AutoPtr<IAttributedCharacterIterator> acit;
                    CreateAttributedCharacterIterator(subIterator,
                            IAttributedCharacterIterator::IAttribute::Probe(Field::GetARGUMENT()),
                            CoreUtils::Box(argumentNumber), &acit);
                    characterIterators->Add(acit);
                    result->GetLength(last);
                }
                arg = nullptr;
            }
            if (!arg.IsNull() && arg.GetLength() > 0) {
                result->Append(arg);
                AutoPtr<IAttributedCharacterIterator> acit;
                CreateAttributedCharacterIterator(arg,
                        IAttributedCharacterIterator::IAttribute::Probe(Field::GetARGUMENT()),
                        CoreUtils::Box(argumentNumber), &acit);
                characterIterators->Add(acit);
                result->GetLength(last);
            }
        }
        else {
            if (subFormatter != nullptr) {
                subFormatter->Format(obj, &arg);
            }
            result->GetLength(last);
            result->Append(arg);
            if (i == 0 && fp != nullptr) {
                AutoPtr<IFormatField> ff;
                fp->GetFieldAttribute(&ff);
                if (Object::Equals(Field::GetARGUMENT(), ff)) {
                    Integer length;
                    result->GetLength(length);
                    fp->SetBeginIndex(last);
                    fp->SetEndIndex(length);
                }
            }
            result->GetLength(last);
        }
    }
    result->Append(mPattern.Substring(lastOffset, mPattern.GetLength()));
    Integer length;
    if (characterIterators != nullptr && (result->GetLength(length), last != length)) {
        String substr;
        result->Substring(last, substr);
        AutoPtr<IAttributedCharacterIterator> acit;
        CreateAttributedCharacterIterator(substr, &acit);
        characterIterators->Add(acit);
    }
    return NOERROR;
}

void MessageFormat::Append(
    /* [out] */ IStringBuffer* result,
    /* [in] */ ICharacterIterator* iterator)
{
    Char firstChar;
    iterator->First(&firstChar);
    if (firstChar != ICharacterIterator::DONE) {
        result->Append(firstChar);

        Char aChar;
        while (iterator->Next(&aChar), aChar != ICharacterIterator::DONE) {
            result->Append(aChar);
        }
    }
}

ECode MessageFormat::MakeFormat(
    /* [in] */ Integer position,
    /* [in] */ Integer offsetNumber,
    /* [in] */ const Array<IStringBuilder*>& textSegments)
{
    Array<String> segments(textSegments.GetLength());
    for (Integer i = 0; i < textSegments.GetLength(); i++) {
        IStringBuilder* oneseg = textSegments[i];
        if (oneseg != nullptr) {
            oneseg->ToString(segments[i]);
        }
        else {
            segments[i] = "";
        }
    }

    // get the argument number
    Integer argumentNumber;
    ECode ec = StringUtils::ParseInteger(segments[SEG_INDEX], &argumentNumber);
    if (FAILED(ec)) {
        Logger::E("MessageFormat", "can't parse argument number: %s", segments[SEG_INDEX].string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (argumentNumber < 0) {
        Logger::E("MessageFormat", "negative argument number: %d", argumentNumber);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // resize format information arrays if necessary
    if (offsetNumber >= mFormats.GetLength()) {
        Integer newLength = mFormats.GetLength() * 2;
        Array<IFormat*> newFormats(newLength);
        Array<Integer> newOffsets(newLength);
        Array<Integer> newArgumentNumbers(newLength);
        newFormats.Copy(0, mFormats, 0, mMaxOffset + 1);
        newOffsets.Copy(0, mOffsets, 0, mMaxOffset + 1);
        newArgumentNumbers.Copy(0, mArgumentNumbers, 0, mMaxOffset + 1);
        mFormats = newFormats;
        mOffsets = newOffsets;
        mArgumentNumbers = newArgumentNumbers;
    }
    Integer oldMaxOffset = mMaxOffset;
    mMaxOffset = offsetNumber;
    mOffsets[offsetNumber] = segments[SEG_RAW].GetLength();
    mArgumentNumbers[offsetNumber] = argumentNumber;

    // now get the format
    AutoPtr<IFormat> newFormat;
    if (segments[SEG_TYPE].GetLength() != 0) {
        Integer type = FindKeyword(segments[SEG_TYPE], TYPE_KEYWORDS);
        switch (type) {
            case TYPE_NULL:
                // Type "" is allowed. e.g., "{0,}", "{0,,}", and "{0,,#}"
                // are treated as "{0}".
                break;

            case TYPE_NUMBER:
                switch (FindKeyword(segments[SEG_MODIFIER], NUMBER_MODIFIER_KEYWORDS)) {
                    case MODIFIER_DEFAULT: {
                        AutoPtr<INumberFormat> nf;
                        FAIL_RETURN(NumberFormat::GetInstance(mLocale, &nf));
                        newFormat = std::move(nf);
                        break;
                    }
                    case MODIFIER_CURRENCY: {
                        AutoPtr<INumberFormat> nf;
                        FAIL_RETURN(NumberFormat::GetCurrencyInstance(mLocale, &nf));
                        newFormat = std::move(nf);
                        break;
                    }
                    case MODIFIER_PERCENT: {
                        AutoPtr<INumberFormat> nf;
                        FAIL_RETURN(NumberFormat::GetPercentInstance(mLocale, &nf));
                        newFormat = std::move(nf);
                        break;
                    }
                    case MODIFIER_INTEGER: {
                        AutoPtr<INumberFormat> nf;
                        FAIL_RETURN(NumberFormat::GetIntegerInstance(mLocale, &nf));
                        newFormat = std::move(nf);
                        break;
                    }
                    default: {
                        // DecimalFormat pattern
                        ECode ec = CDecimalFormat::New(segments[SEG_MODIFIER], DecimalFormatSymbols::GetInstance(mLocale),
                                IID_IFormat, (IInterface**)&newFormat);
                        if (FAILED(ec)) {
                            mMaxOffset = oldMaxOffset;
                            return ec;
                        }
                        break;
                    }
                }
                break;

            case TYPE_DATE:
            case TYPE_TIME: {
                Integer mod = FindKeyword(segments[SEG_MODIFIER], DATE_TIME_MODIFIER_KEYWORDS);
                if (mod >= 0 && mod < DATE_TIME_MODIFIER_KEYWORDS.GetLength()) {
                    if (type == TYPE_DATE) {
                        AutoPtr<IDateFormat> df;
                        DateFormat::GetDateInstance(DATE_TIME_MODIFIERS[mod], mLocale, &df);
                        newFormat = std::move(df);
                    }
                    else {
                        AutoPtr<IDateFormat> df;
                        DateFormat::GetDateInstance(DATE_TIME_MODIFIERS[mod], mLocale, &df);
                        newFormat = std::move(df);
                    }
                }
                else {
                    // SimpleDateFormat pattern
                    ECode ec = CSimpleDateFormat::New(segments[SEG_MODIFIER], mLocale,
                            IID_IFormat, (IInterface**)&newFormat);
                    if (FAILED(ec)) {
                        mMaxOffset = oldMaxOffset;
                        return ec;
                    }
                }
                break;
            }

            case TYPE_CHOICE: {
                ECode ec = CChoiceFormat::New(segments[SEG_MODIFIER],
                        IID_IFormat, (IInterface**)&newFormat);
                if (FAILED(ec)) {
                    mMaxOffset = oldMaxOffset;
                    Logger::E("MessageFormat", "Choice Pattern incorrect: %s", segments[SEG_MODIFIER].string());
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }
                break;
            }

            default:
                mMaxOffset = oldMaxOffset;
                Logger::E("MessageFormat", "unknown format type: %s", segments[SEG_TYPE].string());
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    mFormats.Set(offsetNumber, newFormat);
    return NOERROR;
}

Integer MessageFormat::FindKeyword(
    /* [in] */ const String& s,
    /* [in] */ const Array<String>& list)
{
    for (Integer i = 0; i < list.GetLength(); ++i) {
        if (s.Equals(list[i])) {
            return i;
        }
    }

    // Try trimmed lowercase.
    String ls = s.Trim().ToLowerCase();
    if (!ls.Equals(s)) {
        for (Integer i = 0; i < list.GetLength(); ++i) {
            if (ls.Equals(list[i])) {
                return i;
            }
        }
    }
    return -1;
}

void MessageFormat::CopyAndFixQuotes(
    /* [in] */ const String& source,
    /* [in] */ Integer start,
    /* [in] */ Integer end,
    /* [out] */ IStringBuilder* target)
{
    Boolean quoted = false;

    for (Integer i = start; i < end; ++i) {
        Char ch = source.GetChar(i);
        if (ch == U'{') {
            if (!quoted) {
                target->Append(U'\'');
                quoted = true;
            }
            target->Append(ch);
        }
        else if (ch == U'\'') {
            target->Append(String("''"));
        }
        else {
            if (quoted) {
                target->Append(U'\'');
                quoted = false;
            }
            target->Append(ch);
        }
    }
    if (quoted) {
        target->Append(U'\'');
    }
}

//---------------------------------------------------------------------------------

static AutoPtr<IMessageFormatField> CreateARGUMENT()
{
    AutoPtr<MessageFormat::Field> field = new MessageFormat::Field();
    field->Constructor(String("message argument field"));
    return (IMessageFormatField*)field.Get();
}

AutoPtr<IMessageFormatField> MessageFormat::Field::GetARGUMENT()
{
    static AutoPtr<IMessageFormatField> ARGUMENT = CreateARGUMENT();
    return ARGUMENT;
}

COMO_INTERFACE_IMPL_1(MessageFormat::Field, BaseFormat::Field, IMessageFormatField);


}
}
