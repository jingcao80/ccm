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

#ifndef __COMO_IO_CHARSET_THREADLOCALCODERS_H__
#define __COMO_IO_CHARSET_THREADLOCALCODERS_H__

#include "como.io.charset.ICharsetDecoder.h"
#include "como.io.charset.ICharsetEncoder.h"

namespace como {
namespace io {
namespace charset {

class ThreadLocalCoders
{
public:
    static AutoPtr<ICharsetDecoder> DecoderFor(
        /* [in] */ IInterface* name)
    {
        return nullptr;
    }

    static AutoPtr<ICharsetEncoder> EncoderFor(
        /* [in] */ IInterface* name)
    {
        return nullptr;
    }
};

}
}
}

#endif // __COMO_IO_CHARSET_THREADLOCALCODERS_H__
