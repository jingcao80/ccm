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

#ifndef __CCM_UTIL_REGEX_PATTERN_H__
#define __CCM_UTIL_REGEX_PATTERN_H__

#include "ccm.core.ICharSequence.h"
#include "ccm.io.ISerializable.h"
#include "ccm.util.regex.IMatcher.h"
#include "ccm.util.regex.IPattern.h"
#include "ccm/core/SyncObject.h"

using ccm::core::ICharSequence;
using ccm::core::SyncObject;
using ccm::io::ISerializable;

namespace ccm {
namespace util {
namespace regex {

class Pattern final
    : public SyncObject
    , public IPattern
    , public ISerializable
{
public:
    CCM_INTERFACE_DECL();

    ~Pattern();

    static ECode Compile(
        /* [in] */ const String& regex,
        /* [out] */ IPattern** pattern);

    static ECode Compile(
        /* [in] */ const String& regex,
        /* [in] */ Integer flags,
        /* [out] */ IPattern** pattern);

    ECode GetPattern(
        /* [out] */ String* pattStr);

    ECode ToString(
        /* [out] */ String* pattStr);

    ECode Matcher(
        /* [in] */ ICharSequence* input,
        /* [out] */ IMatcher** matcher);

    ECode Flags(
        /* [out] */ Integer* flags);

    static ECode Matches(
        /* [in] */ const String& regex,
        /* [in] */ ICharSequence* input,
        /* [out] */ Boolean* matched);

    ECode Split(
        /* [in] */ ICharSequence* input,
        /* [in] */ Integer limit,
        /* [out, callee] */ Array<String>* strArray);

    static ECode FastSplit(
        /* [in] */ const String& re,
        /* [in] */ const String& input,
        /* [in] */ Integer limit,
        /* [out, callee] */ Array<String>* strArray);

    ECode Split(
        /* [in] */ ICharSequence* input,
        /* [out, callee] */ Array<String>* strArray);

    static ECode Quote(
        /* [in] */ const String& s,
        /* [out] */ String* pattStr);

private:
    Pattern();

    ECode constructor(
        /* [in] */ const String& p,
        /* [in] */ Integer f);

    ECode Compile();

    static ECode CompileImpl(
        /* [in] */ const String& regex,
        /* [in] */ Integer flags,
        /* [out] */ HANDLE* handle);

private:
    String mPattern;

    Integer mFlags = 0;

    HANDLE mNative = 0;
};

}
}
}

#endif // __CCM_UTIL_REGEX_PATTERN_H__
