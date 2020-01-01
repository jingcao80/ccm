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

#ifndef __COMO_UTIL_REGEX_PATTERNFACTORY_H__
#define __COMO_UTIL_REGEX_PATTERNFACTORY_H__

#include "como.core.ICharSequence.h"

using como::core::ICharSequence;

namespace como {
namespace util {
namespace regex {

class COM_PUBLIC PatternFactory
{
public:
    static ECode Compile(
        /* [in] */ const String& regex,
        /* [out] */ IPattern** pattern);

    static ECode Compile(
        /* [in] */ const String& regex,
        /* [in] */ Integer flags,
        /* [out] */ IPattern** pattern);

    static ECode FastSplit(
        /* [in] */ const String& re,
        /* [in] */ const String& input,
        /* [in] */ Integer limit,
        /* [out, callee] */ Array<String>* strArray);

    static ECode Matches(
        /* [in] */ const String& regex,
        /* [in] */ ICharSequence* input,
        /* [out] */ Boolean* matched);

    static ECode Quote(
        /* [in] */ const String& s,
        /* [out] */ String* pattStr);

private:
    PatternFactory();
};

}
}
}

#endif // __COMO_UTIL_REGEX_PATTERNFACTORY_H__
