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

#ifndef __LIBCORE_IO_IOUTILS_H__
#define __LIBCORE_IO_IOUTILS_H__

#include "ccm.io.IFileDescriptor.h"
#include "ccm.io.charset.ICharset.h"
#include <ccmobject.h>
#include <ccmtypes.h>

using namespace ccm;
using ccm::io::IFileDescriptor;
using ccm::io::charset::ICharset;

namespace libcore {
namespace io {

class IoUtils final
{
private:
    class FileReader
        : public Object
    {
    public:
        ECode Constructor(
            /* [in] */ const String& absolutePath);

        ECode ReadFully();

        Array<Byte> ToByteArray();

        String ToString(
            /* [in] */ ICharset* cs);

    private:
        AutoPtr<IFileDescriptor> mFd;
        Boolean mUnknownLength = false;

        Array<Byte> mBytes;
        Integer mCount = 0;
    };

public:
    static ECode Close(
        /* [in] */ IFileDescriptor* fd);

    static void CloseQuietly(
        /* [in] */ IFileDescriptor* fd);

    static ECode ReadFileAsString(
        /* [in] */ const String& absolutePath,
        /* [out] */ String* content);

private:
    IoUtils();
};

}
}

#endif // __LIBCORE_IO_IOUTILS_H__
