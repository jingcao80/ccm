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

#include "ccm/core/CoreUtils.h"
#include "ccm/core/CStringBuilder.h"
#include "ccm/util/CArrayList.h"
#include "ccm/util/EmptyArray.h"
#include "ccm/util/regex/Matcher.h"
#include "ccm/util/regex/Pattern.h"
#include "ccm.util.IArrayList.h"
#include <ccmautoptr.h>
#include <ccmlogger.h>
#include <unicode/regex.h>
#include <unicode/unistr.h>

using ccm::core::CoreUtils;
using ccm::core::CStringBuilder;
using ccm::core::ICharSequence;
using ccm::core::IStringBuilder;
using ccm::core::IID_IStringBuilder;
using ccm::io::IID_ISerializable;

namespace ccm {
namespace util {
namespace regex {

CCM_INTERFACE_IMPL_2(Pattern, SyncObject, IPattern, ISerializable);

Pattern::Pattern()
{}

Pattern::~Pattern()
{
    if (mNative != 0) {
        delete reinterpret_cast<icu::RegexPattern*>(mNative);
    }
}

ECode Pattern::Constructor(
    /* [in] */ const String& p,
    /* [in] */ Integer f)
{
    if ((f & CANON_EQ) != 0) {
        Logger::E("Pattern", "CANON_EQ flag not supported");
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    Integer supportedFlags = CASE_INSENSITIVE | COMMENTS | DOTALL | LITERAL | MULTILINE | UNICODE_CASE | UNIX_LINES;
    if ((f & ~supportedFlags) != 0) {
        Logger::E("Pattern", "Unsupported flags: 0x%08x", (f & ~supportedFlags));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mPattern = p;
    mFlags = f;
    return Compile();
}

ECode Pattern::Compile(
    /* [in] */ const String& regex,
    /* [out] */ IPattern** pattern)
{
    VALIDATE_NOT_NULL(pattern);

    AutoPtr<Pattern> pObj = new Pattern();
    FAIL_RETURN(pObj->Constructor(regex, 0));
    *pattern = (IPattern*)pObj;
    REFCOUNT_ADD(*pattern);
    return NOERROR;
}

ECode Pattern::Compile(
    /* [in] */ const String& regex,
    /* [in] */ Integer flags,
    /* [out] */ IPattern** pattern)
{
    VALIDATE_NOT_NULL(pattern);

    AutoPtr<Pattern> pObj = new Pattern();
    FAIL_RETURN(pObj->Constructor(regex, flags));
    *pattern = (IPattern*)pObj;
    REFCOUNT_ADD(*pattern);
    return NOERROR;
}

ECode Pattern::GetPattern(
    /* [out] */ String* pattStr)
{
    VALIDATE_NOT_NULL(pattStr);

    *pattStr = mPattern;
    return NOERROR;
}

ECode Pattern::ToString(
    /* [out] */ String* pattStr)
{
    VALIDATE_NOT_NULL(pattStr);

    *pattStr = mPattern;
    return NOERROR;
}

ECode Pattern::Matcher(
    /* [in] */ ICharSequence* input,
    /* [out] */ IMatcher** matcher)
{
    VALIDATE_NOT_NULL(matcher);

    AutoPtr<ccm::util::regex::Matcher> mObj =
            new ccm::util::regex::Matcher();
    FAIL_RETURN(mObj->Constructor(this, input));
    *matcher = (IMatcher*)mObj;
    REFCOUNT_ADD(*matcher);
    return NOERROR;
}

ECode Pattern::Flags(
    /* [out] */ Integer* flags)
{
    VALIDATE_NOT_NULL(flags);

    *flags = mFlags;
    return NOERROR;
}

ECode Pattern::Matches(
    /* [in] */ const String& regex,
    /* [in] */ ICharSequence* input,
    /* [out] */ Boolean* matched)
{
    VALIDATE_NOT_NULL(matched);

    AutoPtr<IPattern> p;
    Compile(regex, (IPattern**)&p);
    AutoPtr<IMatcher> m;
    p->Matcher(input, (IMatcher**)&m);
    return m->Matches(matched);
}

ECode Pattern::Split(
    /* [in] */ ICharSequence* input,
    /* [in] */ Integer limit,
    /* [out, callee] */ Array<String>* strArray)
{
    VALIDATE_NOT_NULL(strArray);

    String inputStr;
    input->ToString(&inputStr);
    Array<String> fast;
    FastSplit(mPattern, inputStr, limit, &fast);
    if (!fast.IsEmpty()) {
        *strArray = fast;
        return NOERROR;
    }

    Integer index = 0;
    Boolean matchLimited = limit > 0;
    AutoPtr<IArrayList> matchList;
    CArrayList::New(IID_IArrayList, (IInterface**)&matchList);
    AutoPtr<IMatcher> m;
    Matcher(input, (IMatcher**)&m);

    Integer size;
    Boolean found;
    while(m->Find(&found), found) {
        if (!matchLimited || (matchList->GetSize(&size), size < limit - 1)) {
            Integer startIndex;
            m->Start(&startIndex);
            AutoPtr<ICharSequence> match;
            input->SubSequence(index, startIndex, (ICharSequence**)&match);
            matchList->Add(match);
            m->End(&index);
        }
        else if (matchList->GetSize(&size), size == limit - 1) { // last one
            Integer len;
            input->GetLength(&len);
            AutoPtr<ICharSequence> match;
            input->SubSequence(index, len, (ICharSequence**)&match);
            matchList->Add(match);
            m->End(&index);
        }
    }

    // If no match was found, return this
    if (index == 0) {
        String str;
        input->ToString(&str);
        *strArray = Array<String>(1);
        strArray->Set(0, str);
        return NOERROR;
    }

    // Add remaining segment
    if (!matchLimited || (matchList->GetSize(&size), size < limit)) {
        Integer len;
        input->GetLength(&len);
        AutoPtr<ICharSequence> match;
        input->SubSequence(index, len, (ICharSequence**)&match);
        matchList->Add(match);
    }

    // Construct result
    matchList->GetSize(&size);
    if (limit == 0) {
        while (size > 0) {
            AutoPtr<IInterface> obj;
            matchList->Get(size - 1, (IInterface**)&obj);
            if (CoreUtils::Unbox(ICharSequence::Probe(obj)).Equals("")) {
                size--;
            }
            else break;
        }
    }
    AutoPtr<IList> subList;
    matchList->SubList(0, size, (IList**)&subList);
    Array<IInterface*> seqArray;
    subList->ToArray(&seqArray);
    *strArray = CoreUtils::Unbox(seqArray);
    return NOERROR;
}

ECode Pattern::FastSplit(
    /* [in] */ const String& re,
    /* [in] */ const String& input,
    /* [in] */ Integer limit,
    /* [out, callee] */ Array<String>* strArray)
{
    VALIDATE_NOT_NULL(strArray);

    static const String FASTSPLIT_METACHARACTERS("\\?*+[](){}^$.|");

    // Can we do it cheaply?
    Integer len = re.GetLength();
    if (len == 0) {
        *strArray = Array<String>::Null();
        return NOERROR;
    }
    Char ch = re.GetChar(0);
    if (len == 1 && FASTSPLIT_METACHARACTERS.IndexOf(ch) == -1) {
        // We're looking for a single non-metacharacter. Easy.
    }
    else if (len == 2 && ch == '\\') {
        // We're looking for a quoted character.
        // Quoted metacharacters are effectively single non-metacharacters.
        ch = re.GetChar(1);
        if (FASTSPLIT_METACHARACTERS.IndexOf(ch) == -1) {
            *strArray = Array<String>::Null();
            return NOERROR;
        }
    }
    else {
        *strArray = Array<String>::Null();
        return NOERROR;
    }

    // We can do this cheaply...

    // Unlike Perl, which considers the result of splitting the empty string to be the empty
    // array, We returns an array containing the empty string.
    if (input.IsEmpty()) {
        *strArray = Array<String>(1);
        strArray->Set(0, String(""));
        return NOERROR;
    }

    // Count separators
    Integer separatorCount = 0;
    Integer begin = 0;
    Integer end;
    while (separatorCount + 1 != limit && (end = input.IndexOf(ch, begin)) != -1) {
        ++separatorCount;
        begin = end + 1;
    }
    Integer lastPartEnd = input.GetLength();
    if (limit == 0 && begin == lastPartEnd) {
        // Last part is empty for limit == 0, remove all trailing empty matches.
        if (separatorCount == lastPartEnd) {
            // Input contains only separators.
            *strArray = EmptyArray::STRING;
            return NOERROR;
        }
        // Find the beginning of trailing separators.
        do {
            --begin;
        } while (input.GetChar(begin - 1) == ch);
        // Reduce separatorCount and fix lastPartEnd.
        separatorCount -= input.GetLength() - begin;
        lastPartEnd = begin;
    }

    // Collect the result parts.
    Array<String> result(separatorCount + 1);
    begin = 0;
    for (Integer i = 0; i != separatorCount; ++i) {
        end = input.IndexOf(ch, begin);
        result[i] = input.Substring(begin, end);
        begin = end + 1;
    }
    // Add last part.
    result[separatorCount] = input.Substring(begin, lastPartEnd);
    *strArray = result;
    return NOERROR;
}

ECode Pattern::Split(
    /* [in] */ ICharSequence* input,
    /* [out, callee] */ Array<String>* strArray)
{
    return Split(input, 0, strArray);
}

ECode Pattern::Quote(
    /* [in] */ const String& s,
    /* [out] */ String* pattStr)
{
    VALIDATE_NOT_NULL(pattStr);

    Integer slashEIndex = s.IndexOf("\\E");
    if (slashEIndex == -1) {
        *pattStr = String("\\Q") + s + "\\E";
        return NOERROR;
    }

    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(s.GetLength() * 2, IID_IStringBuilder, (IInterface**)&sb);
    sb->Append(String("\\Q"));
    slashEIndex = 0;
    Integer current = 0;
    while ((slashEIndex = s.IndexOf("\\E", current)) != -1) {
        sb->Append(s.Substring(current, slashEIndex));
        current = slashEIndex + 2;
        sb->Append(String("\\E\\\\E\\Q"));
    }
    sb->Append(s.Substring(current, s.GetLength()));
    sb->Append(String("\\E"));
    return sb->ToString(pattStr);
}

ECode Pattern::Compile()
{
    if (mPattern.IsNull()) {
        Logger::E("Pattern", "pattern == null");
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }

    String icuPattern = mPattern;
    if ((mFlags & LITERAL) != 0) {
        Quote(mPattern, &icuPattern);
    }

    // These are the flags natively supported by ICU.
    // They even have the same value in native code.
    Integer icuFlags = mFlags & (CASE_INSENSITIVE | COMMENTS | MULTILINE | DOTALL | UNIX_LINES);
    return CompileImpl(icuPattern, icuFlags, &mNative);
}

// ICU documentation: http://icu-project.org/apiref/icu4c/classRegexPattern.html

static const char* RegexDetailMessage(UErrorCode status)
{
    // These human-readable error messages were culled from "utypes.h", and then slightly tuned
    // to make more sense in context.
    // If we don't have a special-case, we'll just return the textual name of
    // the enum value (such as U_REGEX_RULE_SYNTAX), which is better than nothing.
    switch (status) {
    case U_REGEX_INTERNAL_ERROR: return "An internal error was detected";
    case U_REGEX_RULE_SYNTAX: return "Syntax error in regexp pattern";
    case U_REGEX_INVALID_STATE: return "Matcher in invalid state for requested operation";
    case U_REGEX_BAD_ESCAPE_SEQUENCE: return "Unrecognized backslash escape sequence in pattern";
    case U_REGEX_PROPERTY_SYNTAX: return "Incorrect Unicode property";
    case U_REGEX_UNIMPLEMENTED: return "Use of unimplemented feature";
    case U_REGEX_MISMATCHED_PAREN: return "Incorrectly nested parentheses in regexp pattern";
    case U_REGEX_NUMBER_TOO_BIG: return "Decimal number is too large";
    case U_REGEX_BAD_INTERVAL: return "Error in {min,max} interval";
    case U_REGEX_MAX_LT_MIN: return "In {min,max}, max is less than min";
    case U_REGEX_INVALID_BACK_REF: return "Back-reference to a non-existent capture group";
    case U_REGEX_INVALID_FLAG: return "Invalid value for match mode flags";
    case U_REGEX_LOOK_BEHIND_LIMIT: return "Look-behind pattern matches must have a bounded maximum length";
    case U_REGEX_SET_CONTAINS_STRING: return "Regular expressions cannot have UnicodeSets containing strings";
    case U_REGEX_OCTAL_TOO_BIG: return "Octal character constants must be <= 0377.";
    case U_REGEX_MISSING_CLOSE_BRACKET: return "Missing closing bracket in character class";
    case U_REGEX_INVALID_RANGE: return "In a character range [x-y], x is greater than y";
    case U_REGEX_STACK_OVERFLOW: return "Regular expression backtrack stack overflow";
    case U_REGEX_TIME_OUT: return "Maximum allowed match time exceeded";
    case U_REGEX_STOPPED_BY_CALLER: return "Matching operation aborted by user callback function";
    default:
        return u_errorName(status);
    }
}

ECode Pattern::CompileImpl(
    /* [in] */ const String& regex,
    /* [in] */ Integer flags,
    /* [out] */ HANDLE* handle)
{
    flags |= UREGEX_ERROR_ON_UNKNOWN_ESCAPES;

    UErrorCode status = U_ZERO_ERROR;
    UParseError error;
    error.offset = -1;

    Array<Short> regexUTF16 = regex.GetUTF16Chars();
    icu::UnicodeString regexString;
    regexString.setTo(false, (const UChar*)regexUTF16.GetPayload(), regexUTF16.GetLength());
    icu::RegexPattern* result = icu::RegexPattern::compile(regexString, flags, error, status);
    if (!U_SUCCESS(status)) {
        Logger::E("Pattern", "%s: %s", regex.string(), RegexDetailMessage(status));
        return E_PATTERN_SYNTAX_EXCEPTION;
    }
    *handle = reinterpret_cast<HANDLE>(result);
    return NOERROR;
}

}
}
}
