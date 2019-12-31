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

#ifndef __LIBCORE_UTIL_CHARSETUTILS_H__
#define __LIBCORE_UTIL_CHARSETUTILS_H__

#include <ccmtypes.h>

using namespace ccm;

namespace libcore {
namespace util {

class CharsetUtils
{
public:
    static ECode AsciiBytesToChars(
        /* [in] */ const Array<Byte>& bytes,
        /* [in] */ Integer offset,
        /* [in] */ Integer length,
        /* [out] */ Array<Char>& chars);

    static ECode IsoLatin1BytesToChars(
        /* [in] */ const Array<Byte>& bytes,
        /* [in] */ Integer offset,
        /* [in] */ Integer length,
        /* [out] */ Array<Char>& chars);

private:
    CharsetUtils();
};

}
}

#endif // __LIBCORE_UTIL_CHARSETUTILS_H__
