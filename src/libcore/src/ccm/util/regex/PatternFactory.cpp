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

#include "ccm/util/regex/Pattern.h"
#include "ccm/util/regex/PatternFactory.h"

namespace ccm {
namespace util {
namespace regex {

ECode PatternFactory::Compile(
    /* [in] */ const String& regex,
    /* [out] */ IPattern** pattern)
{
    return Pattern::Compile(regex, pattern);
}

ECode PatternFactory::Compile(
    /* [in] */ const String& regex,
    /* [in] */ Integer flags,
    /* [out] */ IPattern** pattern)
{
    return Pattern::Compile(regex, flags, pattern);
}

ECode PatternFactory::FastSplit(
    /* [in] */ const String& re,
    /* [in] */ const String& input,
    /* [in] */ Integer limit,
    /* [out, callee] */ Array<String>* strArray)
{
    return Pattern::FastSplit(re, input, limit, strArray);
}

ECode PatternFactory::Matches(
    /* [in] */ const String& regex,
    /* [in] */ ICharSequence* input,
    /* [out] */ Boolean* matched)
{
    return Pattern::Matches(regex, input, matched);
}

ECode PatternFactory::Quote(
    /* [in] */ const String& s,
    /* [out] */ String* pattStr)
{
    return Pattern::Quote(s, pattStr);
}

}
}
}
