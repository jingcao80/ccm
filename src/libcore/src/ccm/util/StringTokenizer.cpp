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

#include "ccm/core/Character.h"
#include "ccm/core/CoreUtils.h"
#include "ccm/util/StringTokenizer.h"
#include "ccm.core.ICharSequence.h"

using ccm::core::Character;
using ccm::core::CoreUtils;
using ccm::core::ICharSequence;

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_2(StringTokenizer, SyncObject, IStringTokenizer, IEnumeration);

ECode StringTokenizer::Constructor(
    /* [in] */ const String& str,
    /* [in] */ const String& delim,
    /* [in] */ Boolean returnDelims)
{
    if (str.IsNull() || delim.IsNull()) {
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    mCurrentPosition = 0;
    mNewPosition = -1;
    mDelimsChanged = false;
    mStr = str;
    mMaxPosition = str.GetLength();
    mDelimiters = delim;
    mRetDelims = returnDelims;
    SetMaxDelimCodePoint();
    return NOERROR;
}

ECode StringTokenizer::Constructor(
    /* [in] */ const String& str,
    /* [in] */ const String& delim)
{
    return Constructor(str, delim, false);
}

ECode StringTokenizer::Constructor(
    /* [in] */ const String& str)
{
    return Constructor(str, String(" \t\n\r\f"), false);
}

void StringTokenizer::SetMaxDelimCodePoint()
{
    Integer m = 0;
    Integer c;
    Integer count = 0;
    for (Integer i = 0; i < mDelimiters.GetLength(); i++) {
        c = mDelimiters.GetChar(i);
        if (m < c) {
            m = c;
        }
    }
    mMaxDelimCodePoint = m;
}

Integer StringTokenizer::SkipDelimiters(
    /* [in] */ Integer startPos)
{
    Integer position = startPos;
    while (!mRetDelims && position < mMaxPosition) {
        Char c = mStr.GetChar(position);
        if (c > mMaxDelimCodePoint || mDelimiters.IndexOf(c) < 0) {
            break;
        }
        position++;
    }
    return position;
}

Integer StringTokenizer::ScanToken(
    /* [in] */ Integer startPos)
{
    Integer position = startPos;
    while (position < mMaxPosition) {
        Char c = mStr.GetChar(position);
        if (c <= mMaxDelimCodePoint && mDelimiters.IndexOf(c) >= 0) {
            break;
        }
        position++;
    }
    if (mRetDelims && (startPos == position)) {
        Char c = mStr.GetChar(position);
        if (c <= mMaxDelimCodePoint && mDelimiters.IndexOf(c) >= 0) {
            position++;
        }
    }
    return position;
}

ECode StringTokenizer::HasMoreTokens(
    /* [out] */ Boolean* hasMore)
{
    VALIDATE_NOT_NULL(hasMore);

    mNewPosition = SkipDelimiters(mCurrentPosition);
    *hasMore = mNewPosition < mMaxPosition;
    return NOERROR;
}

ECode StringTokenizer::NextToken(
    /* [out] */ String* token)
{
    mCurrentPosition = (mNewPosition >= 0 && !mDelimsChanged) ?
            mNewPosition : SkipDelimiters(mCurrentPosition);

    mDelimsChanged = false;
    mNewPosition = -1;

    if (mCurrentPosition >= mMaxPosition) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    Integer start = mCurrentPosition;
    mCurrentPosition = ScanToken(mCurrentPosition);
    if (token != nullptr) {
        *token = mStr.Substring(start, mCurrentPosition);
    }
    return NOERROR;
}

ECode StringTokenizer::NextToken(
    /* [in] */ const String& delim,
    /* [out] */ String* token)
{
    mDelimiters = delim;
    mDelimsChanged = true;
    SetMaxDelimCodePoint();
    return NextToken(token);
}

ECode StringTokenizer::HasMoreElements(
    /* [out] */ Boolean* hasMore)
{
    return HasMoreTokens(hasMore);
}

ECode StringTokenizer::NextElement(
    /* [out] */ IInterface** element)
{
    String token;
    FAIL_RETURN(NextToken(&token));
    if (element != nullptr) {
        CoreUtils::Box(token).MoveTo((ICharSequence**)element);
    }
    return NOERROR;
}

ECode StringTokenizer::CountTokens(
    /* [out] */ Integer* number)
{
    VALIDATE_NOT_NULL(number);

    Integer count = 0;
    Integer currpos = mCurrentPosition;
    while (currpos < mMaxPosition) {
        currpos = SkipDelimiters(currpos);
        if (currpos >= mMaxPosition) {
            break;
        }
        currpos = ScanToken(currpos);
        count++;
    }
    *number = count;
    return NOERROR;
}

}
}
