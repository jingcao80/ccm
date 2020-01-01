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

#ifndef __COMO_UTIL_STRINGTOKENIZER_H__
#define __COMO_UTIL_STRINGTOKENIZER_H__

#include "como/core/SyncObject.h"
#include "como.util.IEnumeration.h"
#include "como.util.IStringTokenizer.h"

using como::core::SyncObject;

namespace como {
namespace util {

class StringTokenizer
    : public SyncObject
    , public IStringTokenizer
    , public IEnumeration
{
public:
    COMO_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ const String& str,
        /* [in] */ const String& delim,
        /* [in] */ Boolean returnDelims);

    ECode Constructor(
        /* [in] */ const String& str,
        /* [in] */ const String& delim);

    ECode Constructor(
        /* [in] */ const String& str);

    ECode HasMoreTokens(
        /* [out] */ Boolean* hasMore) override;

    ECode NextToken(
        /* [out] */ String* token = nullptr) override;

    ECode NextToken(
        /* [in] */ const String& delim,
        /* [out] */ String* token = nullptr) override;

    ECode HasMoreElements(
        /* [out] */ Boolean* hasMore) override;

    ECode NextElement(
        /* [out] */ IInterface** element = nullptr) override;

    ECode CountTokens(
        /* [out] */ Integer* number) override;

private:
    /**
     * Set maxDelimCodePoint to the highest char in the delimiter set.
     */
    void SetMaxDelimCodePoint();

    Integer SkipDelimiters(
        /* [in] */ Integer startPos);

    Integer ScanToken(
        /* [in] */ Integer startPos);

private:
    Integer mCurrentPosition;
    Integer mNewPosition;
    Integer mMaxPosition;
    String mStr;
    String mDelimiters;
    Boolean mRetDelims;
    Boolean mDelimsChanged;

    /**
     * maxDelimCodePoint stores the value of the delimiter character with the
     * highest value. It is used to optimize the detection of delimiter
     * characters.
     */
    Integer mMaxDelimCodePoint;

//    /**
//     * If delimiters include any surrogates (including surrogate
//     * pairs), hasSurrogates is true and the tokenizer uses the
//     * different code path.
//     */
//    private boolean hasSurrogates = false;
//
//    /**
//     * When hasSurrogates is true, delimiters are converted to code
//     * points and isDelimiter(int) is used to determine if the given
//     * codepoint is a delimiter.
//     */
//    private int[] delimiterCodePoints;
};

}
}

#endif // __COMO_UTIL_STRINGTOKENIZER_H__
