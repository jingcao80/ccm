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

#ifndef __COMO_UTIL_REGEX_MATCHER_H__
#define __COMO_UTIL_REGEX_MATCHER_H__

#include "como.core.ICharSequence.h"
#include "como.core.IStringBuffer.h"
#include "como.util.regex.IMatcher.h"
#include "como.util.regex.IMatchResult.h"
#include "como.util.regex.IPattern.h"
#include "como/core/SyncObject.h"
#include <ccmautoptr.h>
#include <ccmrefbase.h>

using como::core::ICharSequence;
using como::core::IStringBuffer;
using como::core::SyncObject;

namespace como {
namespace util {
namespace regex {

class Matcher final
    : public SyncObject
    , public IMatcher
    , public IMatchResult
{
private:
    class OffsetBasedMatchResult
        : public LightRefBase
        , public IMatchResult
    {
    public:
        OffsetBasedMatchResult(
            /* [in] */ const String& input,
            /* [in] */ const Array<Integer>& offsets);

        COMO_INTERFACE_DECL();

        ECode Start(
            /* [out] */ Integer* index) override;

        ECode Start(
            /* [in] */ Integer group,
            /* [out] */ Integer* index) override;

        ECode End(
            /* [out] */ Integer* index) override;

        ECode End(
            /* [in] */ Integer group,
            /* [out] */ Integer* index) override;

        ECode Group(
            /* [out] */ String* subseq) override;

        ECode Group(
            /* [in] */ Integer group,
            /* [out] */ String* subseq) override;

        ECode GroupCount(
            /* [out] */ Integer* number) override;

    private:
        String mInput;
        Array<Integer> mOffsets;
    };

public:
    Matcher();

    COMO_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ IPattern* parent,
        /* [in] */ ICharSequence* text);

    ECode Pattern(
        /* [out] */ IPattern** pattern) override;

    ECode ToMatchResult(
        /* [out] */ IMatchResult** result) override;

    ECode UsePattern(
        /* [in] */ IPattern* newPattern) override;

    ECode End(
        /* [out] */ Integer* index) override;

    ECode End(
        /* [in] */ Integer group,
        /* [out] */ Integer* index) override;

    ECode End(
        /* [in] */ const String& name,
        /* [out] */ Integer* index) override;

    ECode Group(
        /* [out] */ String* subseq) override;

    ECode Group(
        /* [in] */ Integer group,
        /* [out] */ String* subseq) override;

    ECode Group(
        /* [in] */ const String& name,
        /* [out] */ String* subseq) override;

    ECode GroupCount(
        /* [out] */ Integer* number) override;

    ECode Matches(
        /* [out] */ Boolean* result) override;

    ECode Find(
        /* [out] */ Boolean* result) override;

    ECode Find(
        /* [in] */ Integer start,
        /* [out] */ Boolean* result) override;

    ECode LookingAt(
        /* [out] */ Boolean* result) override;

    static ECode QuoteReplacement(
        /* [in] */ const String& s,
        /* [out] */ String* str);

    ECode AppendReplacement(
        /* [in] */ IStringBuffer* sb,
        /* [in] */ const String& replacement) override;

    ECode AppendTail(
        /* [in] */ IStringBuffer* sb) override;

    ECode ReplaceAll(
        /* [in] */ const String& replacement,
        /* [out] */ String* str) override;

    ECode ReplaceFirst(
        /* [in] */ const String& replacement,
        /* [out] */ String* str) override;

    ECode Region(
        /* [in] */ Integer start,
        /* [in] */ Integer end) override;

    ECode RegionStart(
        /* [out] */ Integer* start) override;

    ECode RegionEnd(
        /* [out] */ Integer* end) override;

    ECode HasTransparentBounds(
        /* [out] */ Boolean* result) override;

    ECode UseTransparentBounds(
        /* [in] */ Boolean value) override;

    ECode HasAnchoringBounds(
        /* [out] */ Boolean* result) override;

    ECode UseAnchoringBounds(
        /* [in] */ Boolean value) override;

    ECode ToString(
        /* [in] */ String* str) override;

    ECode HitEnd(
        /* [out] */ Boolean* result) override;

    ECode RequireEnd(
        /* [out] */ Boolean* result) override;

    ECode Reset() override;

    ECode Reset(
        /* [in] */ ICharSequence* input) override;

    ECode Start(
        /* [out] */ Integer* index) override;

    ECode Start(
        /* [in] */ Integer group,
        /* [out] */ Integer* index) override;

    ECode Start(
        /* [in] */ const String& name,
        /* [out] */ Integer* index) override;

private:
    ECode AppendEvaluated(
        /* [in] */ IStringBuffer* buffer,
        /* [in] */ const String& s);

    ECode Reset(
        /* [in] */ ICharSequence* input,
        /* [in] */ Integer start,
        /* [in] */ Integer end);

    void ResetForInput();

    ECode EnsureMatch();

    static ECode GetMatchedGroupIndex(
        /* [in] */ HANDLE patternAddr,
        /* [in] */ const String& name,
        /* [out] */ Integer* group);

    static Integer GetMatchedGroupIndexImpl(
        /* [in] */ HANDLE patternAddr,
        /* [in] */ const String& name);

    static Boolean FindImpl(
        /* [in] */ HANDLE addr,
        /* [in] */ Integer startIndex,
        /* [in] */ Array<Integer>& offsets);

    static Boolean FindNextImpl(
        /* [in] */ HANDLE addr,
        /* [in] */ Array<Integer>& offsets);

    static Integer GroupCountImpl(
        /* [in] */ HANDLE addr);

    static Boolean HitEndImpl(
        /* [in] */ HANDLE addr);

    static Boolean LookingAtImpl(
        /* [in] */ HANDLE addr,
        /* [in] */ Array<Integer>& offsets);

    static Boolean MatchesImpl(
        /* [in] */ HANDLE addr,
        /* [in] */ Array<Integer>& offsets);

    static HANDLE OpenImpl(
        /* [in] */ HANDLE patternAddr);

    static Boolean RequireEndImpl(
        /* [in] */ HANDLE addr);

    static void SetInputImpl(
        /* [in] */ HANDLE addr,
        /* [in] */ const String& s,
        /* [in] */ Integer start,
        /* [in] */ Integer end);

    static void UseAnchoringBoundsImpl(
        /* [in] */ HANDLE addr,
        /* [in] */ Boolean value);

    static void UseTransparentBoundsImpl(
        /* [in] */ HANDLE addr,
        /* [in] */ Boolean value);

private:
    AutoPtr<IPattern> mPattern;

    /**
     * The address of the native peer.
     * Uses of this must be manually synchronized to avoid native crashes.
     */
    HANDLE mNative = 0;

    /**
     * Holds the input text.
     */
    String mInput;

    /**
     * Holds the start of the region, or 0 if the matching should start at the
     * beginning of the text.
     */
    Integer mRegionStart = 0;

    /**
     * Holds the end of the region, or input.length() if the matching should
     * go until the end of the input.
     */
    Integer mRegionEnd = 0;

    /**
     * Holds the position where the next append operation will take place.
     */
    Integer mAppendPos = 0;

    /**
     * Reflects whether a match has been found during the most recent find
     * operation.
     */
    Boolean mMatchFound = false;

    /**
     * Holds the offsets for the most recent match.
     */
    Array<Integer> mMatchOffsets;

    /**
     * Reflects whether the bounds of the region are anchoring.
     */
    Boolean mAnchoringBounds = true;

    /**
     * Reflects whether the bounds of the region are transparent.
     */
    Boolean mTransparentBounds = false;
};

}
}
}

#endif // __COMO_UTIL_REGEX_MATCHER_H__
