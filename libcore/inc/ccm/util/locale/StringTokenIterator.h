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

#ifndef __CCM_UTIL_LOCALE_STRINGTOKENITERATOR_H__
#define __CCM_UTIL_LOCALE_STRINGTOKENITERATOR_H__

#include <ccmrefbase.h>
#include <ccmtypes.h>

namespace ccm {
namespace util {
namespace locale {

class StringTokenIterator
    : public LightRefBase
{
public:
    ECode Constructor(
        /* [in] */ const String& text,
        /* [in] */ const String& dlms);

    String First();

    String Current();

    Integer CurrentStart();

    Integer CurrentEnd();

    Boolean IsDone();

    String Next();

    Boolean HasNext();

    ECode SetStart(
        /* [in] */ Integer offset);

    void SetText(
        /* [in] */ const String& text);

private:
    Integer NextDelimiter(
        /* [in] */ Integer start);

private:
    String mText;
    String mDlms;
    Char mDelimiterChar = 0;
    String mToken;
    Integer mStart = 0;
    Integer mEnd = 0;
    Boolean mDone = false;
};

inline String StringTokenIterator::First()
{
    SetStart(0);
    return mToken;
}

inline String StringTokenIterator::Current()
{
    return mToken;
}

inline Integer StringTokenIterator::CurrentStart()
{
    return mStart;
}

inline Integer StringTokenIterator::CurrentEnd()
{
    return mEnd;
}

inline Boolean StringTokenIterator::IsDone()
{
    return mDone;
}

inline Boolean StringTokenIterator::HasNext()
{
    return mEnd < mText.GetByteLength();
}

inline void StringTokenIterator::SetText(
    /* [in] */ const String& text)
{
    mText = text;
    SetStart(0);
}

}
}
}

#endif // __CCM_UTIL_LOCALE_STRINGTOKENITERATOR_H__
