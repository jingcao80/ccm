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

#ifndef __COMO_IO_CHARSET_STREAMENCODER_H__
#define __COMO_IO_CHARSET_STREAMENCODER_H__

#include "como/core/SyncObject.h"
#include "como.core.ISynchronize.h"
#include "como.io.charset.IStreamEncoder.h"

using como::core::ISynchronize;

namespace como {
namespace io {
namespace charset {

class StreamEncoder
{
public:
    static ECode ForOutputStreamWriter(
        /* [in] */ IOutputStream* out,
        /* [in] */ ISynchronize* lock,
        /* [in] */ const String& charsetName,
        /* [out] */ IStreamEncoder** encoder);

    static ECode ForOutputStreamWriter(
        /* [in] */ IOutputStream* out,
        /* [in] */ ISynchronize* lock,
        /* [in] */ ICharset* cs,
        /* [out] */ IStreamEncoder** encoder);

    static ECode ForOutputStreamWriter(
        /* [in] */ IOutputStream* out,
        /* [in] */ ISynchronize* lock,
        /* [in] */ ICharsetEncoder* enc,
        /* [out] */ IStreamEncoder** encoder);
};

}
}
}

#endif // __COMO_IO_CHARSET_STREAMENCODER_H__
