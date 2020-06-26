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
#include "como/core/CStringBuffer.h"
#include "como/core/CStringBuilder.h"
#include "como/core/nativeapi.h"
#include "como/util/regex/Matcher.h"
#include "como/util/regex/Pattern.h"
#include "como.core.IStringBuilder.h"
#include <comolog.h>
#include <unicode/regex.h>
#include <memory>

using como::core::AutoLock;
using como::core::CoreUtils;
using como::core::CStringBuffer;
using como::core::CStringBuilder;
using como::core::IStringBuffer;
using como::core::IStringBuilder;
using como::core::IID_IStringBuffer;
using como::core::IID_IStringBuilder;

namespace como {
namespace util {
namespace regex {

class MatcherState
{
public:
    MatcherState(icu::RegexMatcher* matcher)
        : mMatcher(matcher)
        , mUChars(nullptr)
        , mUText(nullptr)
        , mStatus(U_ZERO_ERROR)
    {}

    Boolean updateInput(const String& input)
    {
        // First, close the UText struct, since we're about to allocate a new one.
        if (mUText != nullptr) {
            utext_close(mUText);
            mUText = nullptr;
        }

        // Then delete the UChar* associated with the UText struct..
        mUChars.reset(nullptr);

        // TODO: We should investigate whether we can avoid an additional copy
        // in the native heap when is_copy == JNI_TRUE. The problem with doing
        // that is that we might call ReleaseStringChars with a different
        // JNIEnv* on a different downcall. This is currently safe as
        // implemented in ART, but is unlikely to be portable and the spec stays
        // silent on the matter.
        Array<Short> inputChars = input.GetUTF16Chars();
        if (inputChars.IsNull()) {
            // There will be an exception pending if we get here.
            return false;
        }

        // Make a copy of |input| on the native heap. This copy will be live
        // until the next call to updateInput or close.
        mUChars.reset(new UChar[inputChars.GetLength()]);
        if (mUChars.get() == nullptr) {
            Logger::E("Matcher", "Out of memory");
            return false;
        }

        memcpy(mUChars.get(), inputChars.GetPayload(), inputChars.GetLength() * sizeof(Short));

        // Reset any errors that might have occurred on previous patches.
        mStatus = U_ZERO_ERROR;
        mUText = utext_openUChars(nullptr, mUChars.get(), inputChars.GetLength(), &mStatus);
        if (mUText == nullptr) {
            Logger::E("Matcher", "utext_openUChars failed");
            return false;
        }

        // It is an error for ICU to have returned a non-null mUText but to
        // still have indicated an error.
        CHECK(U_SUCCESS(mStatus));

        mMatcher->reset(mUText);
        return true;
    }

    ~MatcherState()
    {
        if (mUText != nullptr) {
            utext_close(mUText);
        }
    }

    icu::RegexMatcher* matcher()
    {
        return mMatcher.get();
    }

    UErrorCode& status()
    {
        return mStatus;
    }

    void updateOffsets(Array<Integer>& offsets)
    {
        if (offsets.IsNull()) {
            return;
        }

        for (size_t i = 0, groupCount = mMatcher->groupCount(); i <= groupCount; ++i) {
            offsets[2 * i + 0] = mMatcher->start(i, mStatus);
            offsets[2 * i + 1] = mMatcher->end(i, mStatus);
        }
    }

private:
    std::unique_ptr<icu::RegexMatcher> mMatcher;
    std::unique_ptr<UChar[]> mUChars;
    UText* mUText;
    UErrorCode mStatus;

    // Disallow copy and assignment.
    MatcherState(const MatcherState&);
    void operator=(const MatcherState&);
};

COMO_INTERFACE_IMPL_2(Matcher, SyncObject, IMatcher, IMatchResult);

Matcher::Matcher()
{}

ECode Matcher::Constructor(
    /* [in] */ IPattern* parent,
    /* [in] */ ICharSequence* text)
{
    FAIL_RETURN(UsePattern(parent));
    return Reset(text);
}

ECode Matcher::Pattern(
    /* [out] */ AutoPtr<IPattern>& pattern)
{
    pattern = mPattern;
    return NOERROR;
}

ECode Matcher::ToMatchResult(
    /* [out] */ AutoPtr<IMatchResult>& result)
{
    FAIL_RETURN(EnsureMatch());
    result = new OffsetBasedMatchResult(mInput, mMatchOffsets);
    return NOERROR;
}

ECode Matcher::UsePattern(
    /* [in] */ IPattern* newPattern)
{
    if (newPattern == nullptr) {
        Logger::E("Matcher", "newPattern == null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mPattern = newPattern;

    {
        AutoLock lock(this);
        if (mNative != 0) {
            MatcherState* state = reinterpret_cast<MatcherState*>(mNative);
            delete state;
        }
        mNative = OpenImpl(Pattern::From(mPattern)->mNative);
        if (mNative == 0) {
            Logger::E("Matcher", "newPattern is invalid");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    if (!mInput.IsNull()) {
        ResetForInput();
    }

    Integer gc;
    GroupCount(gc);
    mMatchOffsets = Array<Integer>((gc + 1) * 2);
    mMatchFound = false;
    return NOERROR;
}

ECode Matcher::End(
    /* [out] */ Integer& index)
{
    return End(0, index);
}

ECode Matcher::End(
    /* [in] */ Integer group,
    /* [out] */ Integer& index)
{
    FAIL_RETURN(EnsureMatch());
    index = mMatchOffsets[(group * 2) + 1];
    return NOERROR;
}

ECode Matcher::End(
    /* [in] */ const String& name,
    /* [out] */ Integer& index)
{
    FAIL_RETURN(EnsureMatch());
    Integer group;
    FAIL_RETURN(GetMatchedGroupIndex(
            Pattern::From(mPattern)->mNative, name, group));
    index = mMatchOffsets[group * 2 + 1];
    return NOERROR;
}

ECode Matcher::Group(
    /* [out] */ String& subseq)
{
    return Group(0, subseq);
}

ECode Matcher::Group(
    /* [in] */ Integer group,
    /* [out] */ String& subseq)
{
    FAIL_RETURN(EnsureMatch());
    Integer from = mMatchOffsets[group * 2];
    Integer to = mMatchOffsets[(group * 2) + 1];
    if (from == -1 || to == -1) {
        subseq = nullptr;
        return NOERROR;
    }
    else {
        subseq = mInput.Substring(from, to);
        return NOERROR;
    }
}

ECode Matcher::Group(
    /* [in] */ const String& name,
    /* [out] */ String& subseq)
{
    FAIL_RETURN(EnsureMatch());
    Integer group;
    FAIL_RETURN(GetMatchedGroupIndex(
            Pattern::From(mPattern)->mNative, name, group));
    Integer from = mMatchOffsets[group * 2];
    Integer to = mMatchOffsets[(group * 2) + 1];
    if (from == -1 || to == -1) {
        subseq = nullptr;
        return NOERROR;
    }
    else {
        subseq = mInput.Substring(from, to);
        return NOERROR;
    }
}

ECode Matcher::GroupCount(
    /* [out] */ Integer& number)
{
    AutoLock lock(this);
    number = GroupCountImpl(mNative);
    return NOERROR;
}

ECode Matcher::Matches(
    /* [out] */ Boolean& result)
{
    {
        AutoLock lock(this);
        mMatchFound = MatchesImpl(mNative, mMatchOffsets);
    }
    result = mMatchFound;
    return NOERROR;
}

ECode Matcher::Find(
    /* [out] */ Boolean& result)
{
    {
        AutoLock lock(this);
        mMatchFound = FindNextImpl(mNative, mMatchOffsets);
    }
    result = mMatchFound;
    return NOERROR;
}

ECode Matcher::Find(
    /* [in] */ Integer start,
    /* [out] */ Boolean& result)
{
    if (start < 0 || start > mInput.GetLength()) {
        Logger::E("Matcher", "start=%d; length=%d", start, mInput.GetLength());
        return como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    {
        AutoLock lock(this);
        mMatchFound = FindImpl(mNative, start, mMatchOffsets);
    }
    result = mMatchFound;
    return NOERROR;
}

ECode Matcher::LookingAt(
    /* [out] */ Boolean& result)
{
    {
        AutoLock lock(this);
        mMatchFound = LookingAtImpl(mNative, mMatchOffsets);
    }
    result = mMatchFound;
    return NOERROR;
}

ECode Matcher::QuoteReplacement(
    /* [in] */ const String& s,
    /* [out] */ String& str)
{
    if ((s.IndexOf(U'\\') == -1) && (s.IndexOf(U'$') == -1)) {
        str = s;
        return NOERROR;
    }
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    for (Integer i = 0; i < s.GetLength(); i++) {
        Char c = s.GetChar(i);
        if (c == U'\\' || c == U'$') {
            sb->Append(U'\\');
        }
        sb->Append(c);
    }
    return sb->ToString(str);
}

ECode Matcher::AppendReplacement(
    /* [in] */ IStringBuffer* sb,
    /* [in] */ const String& replacement)
{
    Integer start;
    FAIL_RETURN(Start(start));
    sb->Append(mInput.Substring(mAppendPos, start));
    FAIL_RETURN(AppendEvaluated(sb, replacement));
    return End(mAppendPos);
}

ECode Matcher::AppendEvaluated(
    /* [in] */ IStringBuffer* buffer,
    /* [in] */ const String& s)
{
    Boolean escape = false;
    Boolean dollar = false;
    Boolean escapeNamedGroup = false;
    Integer escapeNamedGroupStart = -1;

    for (Integer i = 0; i < s.GetLength(); i++) {
        Char c = s.GetChar(i);
        if (c == U'\\' && !escape) {
            escape = true;
        }
        else if (c == U'$' && !escape) {
            dollar = true;
        }
        else if (c >= U'0' && c <= U'9' && dollar) {
            String subseq;
            FAIL_RETURN(Group(c - U'0', subseq));
            buffer->Append(subseq);
            dollar = false;
        }
        else if (c == U'{' && dollar) {
            escapeNamedGroup = true;
            escapeNamedGroupStart = i;
        }
        else if (c == U'}' && dollar && escapeNamedGroup) {
            String namedGroupName =
                    s.Substring(escapeNamedGroupStart + 1, i);
            String subseq;
            FAIL_RETURN(Group(namedGroupName, subseq));
            buffer->Append(subseq);
            dollar = false;
            escapeNamedGroup = false;
        }
        else if (c != U'}' && dollar && escapeNamedGroup) {
            continue;
        }
        else {
            buffer->Append(c);
            dollar = false;
            escape = false;
            escapeNamedGroup = false;
        }
    }

    if (escapeNamedGroup) {
        Logger::E("Matcher", "Missing ending brace '}' from replacement string");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (escape) {
        Logger::E("Matcher", "s.length == %d", s.GetLength());
        return como::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    return NOERROR;
}

ECode Matcher::AppendTail(
    /* [in] */ IStringBuffer* sb)
{
    if (mAppendPos < mRegionEnd) {
        sb->Append(mInput.Substring(mAppendPos, mRegionEnd));
    }
    return NOERROR;
}

ECode Matcher::ReplaceAll(
    /* [in] */ const String& replacement,
    /* [out] */ String& str)
{
    FAIL_RETURN(Reset());
    AutoPtr<IStringBuffer> buffer;
    CStringBuffer::New(mInput.GetLength(),
            IID_IStringBuffer, (IInterface**)&buffer);
    Boolean found;
    while(Find(found), found) {
        FAIL_RETURN(AppendReplacement(buffer, replacement));
    }
    AppendTail(buffer);
    return buffer->ToString(str);
}

ECode Matcher::ReplaceFirst(
    /* [in] */ const String& replacement,
    /* [out] */ String& str)
{
    FAIL_RETURN(Reset());
    AutoPtr<IStringBuffer> buffer;
    CStringBuffer::New(mInput.GetLength(),
            IID_IStringBuffer, (IInterface**)&buffer);
    Boolean found;
    if (Find(found), found) {
        FAIL_RETURN(AppendReplacement(buffer, replacement));
    }
    AppendTail(buffer);
    return buffer->ToString(str);
}

ECode Matcher::Region(
    /* [in] */ Integer start,
    /* [in] */ Integer end)
{
    return Reset(CoreUtils::Box(mInput), start, end);
}

ECode Matcher::RegionStart(
    /* [out] */ Integer& start)
{
    start = mRegionStart;
    return NOERROR;
}

ECode Matcher::RegionEnd(
    /* [out] */ Integer& end)
{
    end = mRegionEnd;
    return NOERROR;
}

ECode Matcher::HasTransparentBounds(
    /* [out] */ Boolean& result)
{
    result = mTransparentBounds;
    return NOERROR;
}

ECode Matcher::UseTransparentBounds(
    /* [in] */ Boolean value)
{
    AutoLock lock(this);
    mTransparentBounds = value;
    UseTransparentBoundsImpl(mNative, value);
    return NOERROR;
}

ECode Matcher::HasAnchoringBounds(
    /* [out] */ Boolean& result)
{
    result = mAnchoringBounds;
    return NOERROR;
}

ECode Matcher::UseAnchoringBounds(
    /* [in] */ Boolean value)
{
    AutoLock lock(this);
    mAnchoringBounds = value;
    UseAnchoringBoundsImpl(mNative, value);
    return NOERROR;
}

ECode Matcher::ToString(
    /* [in] */ String& str)
{
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    sb->Append(String("como::util::regex::Matcher"));
    sb->Append("[pattern=");
    AutoPtr<IPattern> p;
    Pattern(p);
    sb->Append(Object::ToString(p));
    sb->Append(String(" region="));
    Integer start, end;
    RegionStart(start);
    RegionEnd(end);
    sb->Append(start);
    sb->Append(U',');
    sb->Append(end);
    sb->Append(String(" lastmatch="));
    String subseq;
    if (mMatchFound && (Group(subseq), !subseq.IsNull())) {
        sb->Append(subseq);
    }
    sb->Append(U']');
    return sb->ToString(str);
}

ECode Matcher::HitEnd(
    /* [out] */ Boolean& result)
{
    AutoLock lock(this);
    result = HitEndImpl(mNative);
    return NOERROR;
}

ECode Matcher::RequireEnd(
    /* [out] */ Boolean& result)
{
    AutoLock lock(this);
    result = RequireEndImpl(mNative);
    return NOERROR;
}

ECode Matcher::Reset()
{
    return Reset(CoreUtils::Box(mInput), 0, mInput.GetLength());
}

ECode Matcher::Reset(
    /* [in] */ ICharSequence* input)
{
    if (input == nullptr) {
        Logger::E("Matcher", "input == null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Integer len;
    input->GetLength(len);
    return Reset(input, 0, len);
}

ECode Matcher::Reset(
    /* [in] */ ICharSequence* input,
    /* [in] */ Integer start,
    /* [in] */ Integer end)
{
    if (input == nullptr) {
        Logger::E("Matcher", "input == null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer len;
    input->GetLength(len);
    if (start < 0 || end < 0 || start > len || end > len || start > end) {
        return como::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    input->ToString(mInput);
    mRegionStart = start;
    mRegionEnd = end;
    ResetForInput();

    mMatchFound = false;
    mAppendPos = 0;

    return NOERROR;
}

void Matcher::ResetForInput()
{
    AutoLock lock(this);
    SetInputImpl(mNative, mInput, mRegionStart, mRegionEnd);
    UseAnchoringBoundsImpl(mNative, mAnchoringBounds);
    UseTransparentBoundsImpl(mNative, mTransparentBounds);
}

ECode Matcher::EnsureMatch()
{
    if (!mMatchFound) {
        Logger::E("Matcher", "No successful match so far");
        return como::core::E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode Matcher::Start(
    /* [out] */ Integer& index)
{
    return Start(0, index);
}

ECode Matcher::Start(
    /* [in] */ Integer group,
    /* [out] */ Integer& index)
{
    FAIL_RETURN(EnsureMatch());
    index = mMatchOffsets[group * 2];
    return NOERROR;
}

ECode Matcher::Start(
    /* [in] */ const String& name,
    /* [out] */ Integer& index)
{
    FAIL_RETURN(EnsureMatch());
    Integer group;
    FAIL_RETURN(GetMatchedGroupIndex(
            Pattern::From(mPattern)->mNative, name, group));
    index = mMatchOffsets[group * 2];
    return NOERROR;
}

ECode Matcher::GetMatchedGroupIndex(
    /* [in] */ HANDLE patternAddr,
    /* [in] */ const String& name,
    /* [out] */ Integer& group)
{
    Integer result = GetMatchedGroupIndexImpl(patternAddr, name);
    if (result < 0) {
        Logger::E("Matcher", "No capturing group in the pattern with the name %s",
                name.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    group = result;
    return NOERROR;
}

Integer Matcher::GetMatchedGroupIndexImpl(
    /* [in] */ HANDLE patternAddr,
    /* [in] */ const String& name)
{
#if defined(__GLIBC__)
    return -1;
#else
    icu::RegexPattern* pattern = reinterpret_cast<icu::RegexPattern*>(patternAddr);
    Array<Short> groupName = name.GetUTF16Chars();
    icu::UnicodeString groupNameUnicodeStr;
    groupNameUnicodeStr.setTo(false, (const UChar*)groupName.GetPayload(), groupName.GetLength());
    UErrorCode status = U_ZERO_ERROR;

    // need icu 55
    Integer result = pattern->groupNumberFromName(groupNameUnicodeStr, status);
    if (U_SUCCESS(status)) {
        return result;
    }
    if (status == U_REGEX_INVALID_CAPTURE_GROUP_NAME) {
        return -1;
    }
    como::core::MaybeCauseIcuException("RegexPattern::groupNumberFromName", status);
    return -1;
#endif
}

Boolean Matcher::FindImpl(
    /* [in] */ HANDLE addr,
    /* [in] */ Integer startIndex,
    /* [in] */ Array<Integer>& offsets)
{
    MatcherState* state = reinterpret_cast<MatcherState*>(addr);
    UBool result = state->matcher()->find(startIndex, state->status());
    if (result) {
        state->updateOffsets(offsets);
    }
    return result;
}

Boolean Matcher::FindNextImpl(
    /* [in] */ HANDLE addr,
    /* [in] */ Array<Integer>& offsets)
{
    MatcherState* state = reinterpret_cast<MatcherState*>(addr);
    UBool result = state->matcher()->find();
    if (result) {
        state->updateOffsets(offsets);
    }
    return result;
}

Integer Matcher::GroupCountImpl(
    /* [in] */ HANDLE addr)
{
    MatcherState* state = reinterpret_cast<MatcherState*>(addr);
    return state->matcher()->groupCount();
}

Boolean Matcher::HitEndImpl(
    /* [in] */ HANDLE addr)
{
    MatcherState* state = reinterpret_cast<MatcherState*>(addr);
    return state->matcher()->hitEnd();
}

Boolean Matcher::LookingAtImpl(
    /* [in] */ HANDLE addr,
    /* [in] */ Array<Integer>& offsets)
{
    MatcherState* state = reinterpret_cast<MatcherState*>(addr);
    UBool result = state->matcher()->lookingAt(state->status());
    if (result) {
        state->updateOffsets(offsets);
    }
    return result;
}

Boolean Matcher::MatchesImpl(
    /* [in] */ HANDLE addr,
    /* [in] */ Array<Integer>& offsets)
{
    MatcherState* state = reinterpret_cast<MatcherState*>(addr);
    UBool result = state->matcher()->matches(state->status());
    if (result) {
        state->updateOffsets(offsets);
    }
    return result;
}

HANDLE Matcher::OpenImpl(
    /* [in] */ HANDLE patternAddr)
{
    icu::RegexPattern* pattern = reinterpret_cast<icu::RegexPattern*>(patternAddr);
    UErrorCode status = U_ZERO_ERROR;
    icu::RegexMatcher* result = pattern->matcher(status);
    if (como::core::MaybeCauseIcuException("RegexPattern::matcher", status)) {
        return 0;
    }

    return reinterpret_cast<HANDLE>(new MatcherState(result));
}

Boolean Matcher::RequireEndImpl(
    /* [in] */ HANDLE addr)
{
    MatcherState* state = reinterpret_cast<MatcherState*>(addr);
    return state->matcher()->requireEnd();
}

void Matcher::SetInputImpl(
    /* [in] */ HANDLE addr,
    /* [in] */ const String& s,
    /* [in] */ Integer start,
    /* [in] */ Integer end)
{
    MatcherState* state = reinterpret_cast<MatcherState*>(addr);
    if (state->updateInput(s)) {
        state->matcher()->region(start, end, state->status());
    }
}

void Matcher::UseAnchoringBoundsImpl(
    /* [in] */ HANDLE addr,
    /* [in] */ Boolean value)
{
    MatcherState* state = reinterpret_cast<MatcherState*>(addr);
    state->matcher()->useAnchoringBounds(value);
}

void Matcher::UseTransparentBoundsImpl(
    /* [in] */ HANDLE addr,
    /* [in] */ Boolean value)
{
    MatcherState* state = reinterpret_cast<MatcherState*>(addr);
    state->matcher()->useTransparentBounds(value);
}

//-------------------------------------------------------------------
COMO_INTERFACE_IMPL_LIGHT_1(Matcher::OffsetBasedMatchResult, LightRefBase, IMatchResult);

Matcher::OffsetBasedMatchResult::OffsetBasedMatchResult(
    /* [in] */ const String& input,
    /* [in] */ const Array<Integer>& offsets)
{
    mInput = input;
    mOffsets = offsets.Clone();
}

ECode Matcher::OffsetBasedMatchResult::Start(
    /* [out] */ Integer& index)
{
    return Start(0, index);
}

ECode Matcher::OffsetBasedMatchResult::Start(
    /* [in] */ Integer group,
    /* [out] */ Integer& index)
{
    index = mOffsets[2 * group];
    return NOERROR;
}

ECode Matcher::OffsetBasedMatchResult::End(
    /* [out] */ Integer& index)
{
    return End(0, index);
}

ECode Matcher::OffsetBasedMatchResult::End(
    /* [in] */ Integer group,
    /* [out] */ Integer& index)
{
    index = mOffsets[2 * group + 1];
    return NOERROR;
}

ECode Matcher::OffsetBasedMatchResult::Group(
    /* [out] */ String& subseq)
{
    return Group(0, subseq);
}

ECode Matcher::OffsetBasedMatchResult::Group(
    /* [in] */ Integer group,
    /* [out] */ String& subseq)
{
    Integer start, end;
    Start(group, start);
    End(group, end);
    if (start == -1 || end == -1) {
        subseq = nullptr;
        return NOERROR;
    }

    subseq = mInput.Substring(start, end);
    return NOERROR;
}

ECode Matcher::OffsetBasedMatchResult::GroupCount(
    /* [out] */ Integer& number)
{
    number = (mOffsets.GetLength() / 2) - 1;
    return NOERROR;
}

}
}
}
