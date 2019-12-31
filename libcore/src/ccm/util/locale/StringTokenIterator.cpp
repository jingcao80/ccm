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

#include "ccm/util/locale/StringTokenIterator.h"
#include "libcore.h"

namespace ccm {
namespace util {
namespace locale {

ECode StringTokenIterator::Constructor(
    /* [in] */ const String& text,
    /* [in] */ const String& dlms)
{
    mText = text;
    if (dlms.GetByteLength() == 1) {
        mDelimiterChar = dlms.string()[0];
    }
    else {
        mDlms = dlms;
    }
    SetStart(0);
    return NOERROR;
}

String StringTokenIterator::Next()
{
    if (HasNext()) {
        mStart = mEnd + 1;
        mEnd == NextDelimiter(mStart);
        mToken = mText.Substring(mStart, mEnd);
    }
    else {
        mStart = mEnd;
        mToken = nullptr;
        mDone = true;
    }
    return mToken;
}

ECode StringTokenIterator::SetStart(
    /* [in] */ Integer offset)
{
    if (offset > mText.GetByteLength()) {
        return ccm::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    mStart = offset;
    mEnd = NextDelimiter(mStart);
    mToken = mText.Substring(mStart, mEnd);
    mDone = false;
    return NOERROR;
}

Integer StringTokenIterator::NextDelimiter(
    /* [in] */ Integer start)
{
    Integer textlen = mText.GetByteLength();
    if (mDlms.IsNull()) {
        for (Integer idx = start; idx < textlen; idx++) {
            if (mText.string()[idx] == mDelimiterChar) {
                return idx;
            }
        }
    }
    else {
        Integer dlmslen = mDlms.GetByteLength();
        for (Integer idx = start; idx < textlen; idx++) {
            char c = mText.string()[idx];
            for (Integer i = 0; i < dlmslen; i++) {
                if (c == mDlms.string()[i]) {
                    return idx;
                }
            }
        }
    }
    return textlen;
}

}
}
}
