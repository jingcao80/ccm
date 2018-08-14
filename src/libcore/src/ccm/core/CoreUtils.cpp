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

#include "ccm/core/CoreUtils.h"
#include "ccm/core/CChar.h"
#include "ccm/core/CString.h"

namespace ccm {
namespace core {

AutoPtr<IChar> CoreUtils::Box(
    /* [in] */ Char c)
{
    AutoPtr<IChar> ch;
    CChar::New(c, IID_IChar, (IInterface**)&ch);
    return ch;
}

AutoPtr<ICharSequence> CoreUtils::Box(
    /* [in] */ const String& str)
{
    AutoPtr<ICharSequence> seq;
    CString::New(str, IID_ICharSequence, (IInterface**)&seq);
    return seq;
}

Char CoreUtils::Unbox(
    /* [in] */ IChar* ch)
{
    Char c;
    ch->GetValue(&c);
    return c;
}

String CoreUtils::Unbox(
    /* [in] */ ICharSequence* seq)
{
    String str;
    if (seq != nullptr) {
        seq->ToString(&str);
    }
    return str;
}

Array<String> CoreUtils::Unbox(
    /* [in] */ const Array<ICharSequence*>& seqArray)
{
    if (seqArray.IsEmpty()) {
        return Array<String>::Null();
    }

    Long size = seqArray.GetLength();
    Array<String> strArray(size);
    for (Long i = 0; i < size; i++) {
        strArray[i] = Unbox(seqArray[i]);
    }
    return strArray;
}

}
}
