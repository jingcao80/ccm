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

#include "libcore/util/CharsetUtils.h"

namespace libcore {
namespace util {

ECode CharsetUtils::AsciiBytesToChars(
    /* [in] */ const Array<Byte>& bytes,
    /* [in] */ Integer offset,
    /* [in] */ Integer length,
    /* [out] */ Array<Char>& chars)
{
    if (bytes.IsNull() || chars.IsNull()) {
        return NOERROR;
    }

    static constexpr Char REPLACEMENT_CHAR = 0xfffd;
    for (Integer i = 0; i < length; i++) {
        Byte b = bytes[i + offset];
        chars[i] = b <= 0x7f ? b : REPLACEMENT_CHAR;
    }
    return NOERROR;
}

ECode CharsetUtils::IsoLatin1BytesToChars(
    /* [in] */ const Array<Byte>& bytes,
    /* [in] */ Integer offset,
    /* [in] */ Integer length,
    /* [out] */ Array<Char>& chars)
{
    if (bytes.IsNull() || chars.IsNull()) {
        return NOERROR;
    }

    for (Integer i = 0; i < length; i++) {
        chars[i] = bytes[i + offset];
    }
    return NOERROR;
}

}
}
