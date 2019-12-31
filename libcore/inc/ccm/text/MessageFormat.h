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

#ifndef __CCM_TEXT_MESSAGEFORMAT_H__
#define __CCM_TEXT_MESSAGEFORMAT_H__

#include "ccm/text/Format.h"
#include "ccm.core.IStringBuffer.h"
#include "ccm.core.IStringBuilder.h"
#include "ccm.text.IAttributedCharacterIterator.h"
#include "ccm.text.ICharacterIterator.h"
#include "ccm.text.IFieldPosition.h"
#include "ccm.text.IFormat.h"
#include "ccm.text.IMessageFormat.h"
#include "ccm.text.IMessageFormatField.h"
#include "ccm.util.IList.h"
#include "ccm.util.ILocale.h"

using ccm::core::IStringBuffer;
using ccm::core::IStringBuilder;
using ccm::util::IList;
using ccm::util::ILocale;

namespace ccm {
namespace text {

static AutoPtr<IMessageFormatField> CreateARGUMENT();

class MessageFormat
    : public BaseFormat
    , public IMessageFormat
{
public:
    class Field
        : public BaseFormat::Field
        , public IMessageFormatField
    {
    public:
        CCM_INTERFACE_DECL();

        static AutoPtr<IMessageFormatField> GetARGUMENT();

    protected:
        inline ECode Constructor(
            /* [in] */ const String& name)
        {
            return BaseFormat::Field::Constructor(name);
        }

        friend AutoPtr<IMessageFormatField> CreateARGUMENT();
    };

public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ const String& pattern);

    ECode Constructor(
        /* [in] */ const String& pattern,
        /* [in] */ ILocale* locale);

    ECode SetLocale(
        /* [in] */ ILocale* locale) override;

    ECode GetLocale(
        /* [out] */ ILocale** locale) override;

    ECode ApplyPattern(
        /* [in] */ const String& pattern) override;

    ECode ToPattern(
        /* [out] */ String* pattern) override;

    ECode SetFormatsByArgumentIndex(
        /* [in] */ const Array<IFormat*>& newFormats) override;

    ECode SetFormats(
        /* [in] */ const Array<IFormat*>& newFormats) override;

    ECode SetFormatByArgumentIndex(
        /* [in] */ Integer argumentIndex,
        /* [in] */ IFormat* newFormat) override;

    ECode SetFormat(
        /* [in] */ Integer formatElementIndex,
        /* [in] */ IFormat* newFormat) override;

    ECode GetFormatsByArgumentIndex(
        /* [out, callee] */ Array<IFormat*>* formats) override;

    ECode GetFormats(
        /* [out, callee] */ Array<IFormat*>* formats) override;

    ECode Format(
        /* [in] */ const Array<IInterface*>& arguments,
        /* [out] */ IStringBuffer* result,
        /* [in] */ IFieldPosition* pos) override;

    static ECode Format(
        /* [in] */ const String& pattern,
        /* [in] */ Array<IInterface*>* arguments,
        /* [out] */ String* message);

    ECode Format(
        /* [in] */ IInterface* arguments,
        /* [in] */ IStringBuffer* result,
        /* [in] */ IFieldPosition* pos) override final;

    ECode FormatToCharacterIterator(
        /* [in] */ IInterface* arguments,
        /* [out] */ IAttributedCharacterIterator** cit) override;

    ECode Parse(
        /* [in] */ const String& source,
        /* [in] */ IParsePosition* pos,
        /* [out, callee] */ Array<IInterface*>* result) override;

    ECode Parse(
        /* [in] */ const String& source,
        /* [out, callee] */ Array<IInterface*>* result) override;

    ECode ParseObject(
        /* [in] */ const String& source,
        /* [in] */ IParsePosition* pos,
        /* [out] */ IInterface** result) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* same) override;

    ECode GetHashCode(
        /* [out] */ Integer* hash) override;

protected:
    ECode CloneImpl(
        /* [in] */ IMessageFormat* newObj);

private:
    ECode Subformat(
        /* [in] */ const Array<IInterface*>& arguments,
        /* [out] */ IStringBuffer* result,
        /* [in] */ IFieldPosition* fp,
        /* [in] */ IList* characterIterators);

    void Append(
        /* [out] */ IStringBuffer* result,
        /* [in] */ ICharacterIterator* iterator);

    ECode MakeFormat(
        /* [in] */ Integer position,
        /* [in] */ Integer offsetNumber,
        /* [in] */ const Array<IStringBuilder*>& textSegments);

    static Integer FindKeyword(
        /* [in] */ const String& s,
        /* [in] */ const Array<String>& list);

    static void CopyAndFixQuotes(
        /* [in] */ const String& source,
        /* [in] */ Integer start,
        /* [in] */ Integer end,
        /* [out] */ IStringBuilder* target);

private:
    /**
     * The locale to use for formatting numbers and dates.
     */
    AutoPtr<ILocale> mLocale;

    /**
     * The string that the formatted values are to be plugged into.  In other words, this
     * is the pattern supplied on construction with all of the {} expressions taken out.
     */
    String mPattern = String("");

    /** The initially expected number of subformats in the format */
    static constexpr Integer INITIAL_FORMATS = 10;

    /**
     * An array of formatters, which are used to format the arguments.
     */
    Array<IFormat*> mFormats = Array<IFormat*>(INITIAL_FORMATS);

    /**
     * The positions where the results of formatting each argument are to be inserted
     * into the pattern.
     */
    Array<Integer> mOffsets = Array<Integer>(INITIAL_FORMATS);

    /**
     * The argument numbers corresponding to each formatter.  (The formatters are stored
     * in the order they occur in the pattern, not in the order in which the arguments
     * are specified.)
     */
    Array<Integer> mArgumentNumbers = Array<Integer>(INITIAL_FORMATS);

    Integer mMaxOffset = -1;

    // Indices for segments
    static constexpr Integer SEG_RAW = 0;
    static constexpr Integer SEG_INDEX = 1;
    static constexpr Integer SEG_TYPE = 2;
    static constexpr Integer SEG_MODIFIER = 3;

    // Indices for type keywords
    static constexpr Integer TYPE_NULL    = 0;
    static constexpr Integer TYPE_NUMBER  = 1;
    static constexpr Integer TYPE_DATE    = 2;
    static constexpr Integer TYPE_TIME    = 3;
    static constexpr Integer TYPE_CHOICE  = 4;

    static Array<String> TYPE_KEYWORDS;

    // Indices for number modifiers
    static constexpr Integer MODIFIER_DEFAULT = 0;
    static constexpr Integer MODIFIER_CURRENCY = 1;
    static constexpr Integer MODIFIER_PERCENT  = 2;
    static constexpr Integer MODIFIER_INTEGER  = 3;

    static Array<String> NUMBER_MODIFIER_KEYWORDS;

    static Array<String> DATE_TIME_MODIFIER_KEYWORDS;

    static Array<Integer> DATE_TIME_MODIFIERS;
};

}
}

#endif // __CCM_TEXT_MESSAGEFORMAT_H__