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

#ifndef __LIBCORE_ICU_STRINGBYTESINK_H__
#define __LIBCORE_ICU_STRINGBYTESINK_H__

#include "ccm/core/CStringBuilder.h"
#include <ccmautoptr.h>
#include <unicode/bytestream.h>

using ccm::core::CStringBuilder;
using ccm::core::IID_IStringBuilder;
using ccm::core::IStringBuilder;

namespace libcore {
namespace icu {

class StringByteSink
    : public U_ICU_NAMESPACE::ByteSink
{
public:
    StringByteSink();

    /**
     * Append "bytes[0,n-1]" to this.
     * @param data the pointer to the bytes
     * @param n the number of bytes; must be non-negative
     */
    void Append(
        /* [in] */ const char* data,
        /* [in] */ int32_t n) override;

    void Clear();

    String ToString();

private:
    AutoPtr<IStringBuilder> mBuilder;
};

inline StringByteSink::StringByteSink()
{
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&mBuilder);
}

inline void StringByteSink::Append(
    /* [in] */ const char* data,
    /* [in] */ int32_t n)
{
    mBuilder->Append(String(data));
}

inline void StringByteSink::Clear()
{
    Integer size;
    mBuilder->GetLength(&size);
    mBuilder->Delete(0, size);
}

inline String StringByteSink::ToString()
{
    String str;
    mBuilder->ToString(&str);
    return str;
}

}
}

#endif // __LIBCORE_ICU_STRINGBYTESINK_H__
