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

#include "ccm/util/regex/Matcher.h"

namespace ccm {
namespace util {
namespace regex {

CCM_INTERFACE_IMPL_2(Matcher, SyncObject, IMatcher, IMatchResult);

Matcher::Matcher()
{}

ECode Matcher::constructor(
    /* [in] */ IPattern* parent,
    /* [in] */ ICharSequence* text)
{
    return NOERROR;
}

ECode Matcher::AppendReplacement(
    /* [in] */ IStringBuffer* sb,
    /* [in] */ const String& replacement)
{
    return NOERROR;
}

ECode Matcher::AppendTail(
    /* [in] */ IStringBuffer* sb)
{
    return NOERROR;
}

ECode Matcher::End(
    /* [in] */ const String& name,
    /* [out] */ Integer* index)
{
    return NOERROR;
}

ECode Matcher::Find(
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode Matcher::Find(
    /* [in] */ Integer start,
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode Matcher::Group(
    /* [in] */ const String& name,
    /* [out] */ String* subseq)
{
    return NOERROR;
}

ECode Matcher::HasAnchoringBounds(
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode Matcher::HasTransparentBounds(
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode Matcher::HitEnd(
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode Matcher::LookingAt(
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode Matcher::Matches(
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode Matcher::Pattern(
    /* [out] */ IPattern* pattern)
{
    return NOERROR;
}

ECode Matcher::Region(
    /* [in] */ Integer start,
    /* [in] */ Integer end)
{
    return NOERROR;
}

ECode Matcher::RegionStart(
    /* [out] */ Integer* start)
{
    return NOERROR;
}

ECode Matcher::RegionEnd(
    /* [out] */ Integer* end)
{
    return NOERROR;
}

ECode Matcher::ReplaceAll(
    /* [in] */ const String& replacement,
    /* [out] */ String* str)
{
    return NOERROR;
}

ECode Matcher::ReplaceFirst(
    /* [in] */ const String& replacement,
    /* [out] */ String* str)
{
    return NOERROR;
}

ECode Matcher::RequireEnd(
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode Matcher::Reset()
{
    return NOERROR;
}

ECode Matcher::Reset(
    /* [in] */ ICharSequence* input)
{
    return NOERROR;
}

ECode Matcher::Start(
    /* [in] */ const String& name,
    /* [out] */ Integer* index)
{
    return NOERROR;
}

ECode Matcher::ToMatchResult(
    /* [out] */ IMatchResult** result)
{
    return NOERROR;
}

ECode Matcher::UseAnchoringBounds(
    /* [in] */ Boolean value)
{
    return NOERROR;
}

ECode Matcher::UsePattern(
    /* [in] */ IPattern* newPattern)
{
    return NOERROR;
}

ECode Matcher::UseTransparentBounds(
    /* [in] */ Boolean value)
{
    return NOERROR;
}

ECode Matcher::End(
    /* [out] */ Integer* index)
{
    return NOERROR;
}

ECode Matcher::End(
    /* [in] */ Integer group,
    /* [out] */ Integer* index)
{
    return NOERROR;
}

ECode Matcher::Group(
    /* [out] */ String* subseq)
{
    return NOERROR;
}

ECode Matcher::Group(
    /* [in] */ Integer group,
    /* [out] */ String* subseq)
{
    return NOERROR;
}

ECode Matcher::GroupCount(
    /* [out] */ Integer* number)
{
    return NOERROR;
}

ECode Matcher::Start(
    /* [out] */ Integer* index)
{
    return NOERROR;
}

ECode Matcher::Start(
    /* [in] */ Integer group,
    /* [out] */ Integer* index)
{
    return NOERROR;
}

}
}
}
