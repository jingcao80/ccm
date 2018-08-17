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

#include "ccm/util/concurrent/Helpers.h"

namespace ccm {
namespace util {
namespace concurrent {

String Helpers::ToString(
    /* [in] */ const Array<String>& a,
    /* [in] */ Integer size,
    /* [in] */ Integer charLength)
{
    Array<Char> chars(charLength + 2 * size);
    chars[0] = '[';
    Integer j = 1;
    for (Integer i = 0; i < size; i++) {
        if (i > 0) {
            chars[j++] = ',';
            chars[j++] = ' ';
        }
        const String& s = a[i];
        Integer len = s.GetLength();
        s.GetChars(0, len, chars, j);
        j += len;
    }
    chars[j] = ']';
    return String(chars);
}

}
}
}
