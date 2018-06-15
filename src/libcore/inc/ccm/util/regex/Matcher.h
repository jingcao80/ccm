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

#ifndef __CCM_UTIL_REGEX_MATCHER_H__
#define __CCM_UTIL_REGEX_MATCHER_H__

#include "ccm.core.ICharSequence.h"
#include "ccm.core.IStringBuffer.h"
#include "ccm.util.regex.IMatcher.h"
#include "ccm.util.regex.IMatchResult.h"
#include "ccm.util.regex.IPattern.h"
#include "ccm/core/SyncObject.h"

using ccm::core::ICharSequence;
using ccm::core::IStringBuffer;
using ccm::core::SyncObject;

namespace ccm {
namespace util {
namespace regex {

class Matcher final
    : public SyncObject
    , public IMatcher
    , public IMatchResult
{
public:
    Matcher();

    CCM_INTERFACE_DECL();

    ECode constructor(
        /* [in] */ IPattern* parent,
        /* [in] */ ICharSequence* text);

    ECode AppendReplacement(
        /* [in] */ IStringBuffer* sb,
        /* [in] */ const String& replacement);

    ECode AppendTail(
        /* [in] */ IStringBuffer* sb);

    ECode End(
        /* [in] */ const String& name,
        /* [out] */ Integer* index);

    ECode Find(
        /* [out] */ Boolean* result);

    ECode Find(
        /* [in] */ Integer start,
        /* [out] */ Boolean* result);

    ECode Group(
        /* [in] */ const String& name,
        /* [out] */ String* subseq);

    ECode HasAnchoringBounds(
        /* [out] */ Boolean* result);

    ECode HasTransparentBounds(
        /* [out] */ Boolean* result);

    ECode HitEnd(
        /* [out] */ Boolean* result);

    ECode LookingAt(
        /* [out] */ Boolean* result);

    ECode Matches(
        /* [out] */ Boolean* result);

    ECode Pattern(
        /* [out] */ IPattern* pattern);

    ECode Region(
        /* [in] */ Integer start,
        /* [in] */ Integer end);

    ECode RegionStart(
        /* [out] */ Integer* start);

    ECode RegionEnd(
        /* [out] */ Integer* end);

    ECode ReplaceAll(
        /* [in] */ const String& replacement,
        /* [out] */ String* str);

    ECode ReplaceFirst(
        /* [in] */ const String& replacement,
        /* [out] */ String* str);

    ECode RequireEnd(
        /* [out] */ Boolean* result);

    ECode Reset();

    ECode Reset(
        /* [in] */ ICharSequence* input);

    ECode Start(
        /* [in] */ const String& name,
        /* [out] */ Integer* index);

    ECode ToMatchResult(
        /* [out] */ IMatchResult** result);

    ECode UseAnchoringBounds(
        /* [in] */ Boolean value);

    ECode UsePattern(
        /* [in] */ IPattern* newPattern);

    ECode UseTransparentBounds(
        /* [in] */ Boolean value);

    ECode End(
        /* [out] */ Integer* index);

    ECode End(
        /* [in] */ Integer group,
        /* [out] */ Integer* index);

    ECode Group(
        /* [out] */ String* subseq);

    ECode Group(
        /* [in] */ Integer group,
        /* [out] */ String* subseq);

    ECode GroupCount(
        /* [out] */ Integer* number);

    ECode Start(
        /* [out] */ Integer* index);

    ECode Start(
        /* [in] */ Integer group,
        /* [out] */ Integer* index);
};

}
}
}

#endif // __CCM_UTIL_REGEX_MATCHER_H__
