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

#ifndef __CCM_IO_FILEOUTPUTSTREAM_H__
#define __CCM_IO_FILEOUTPUTSTREAM_H__

#include "ccm/io/OutputStream.h"
#include "ccm.io.IFile.h"
#include "ccm.io.IFileDescriptor.h"
#include "ccm.io.IFileOutputStream.h"
#include <ccmautoptr.h>

namespace ccm {
namespace io {

class FileOutputStream
    : public OutputStream
    , public IFileOutputStream
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ const String& name);

    ECode Constructor(
        /* [in] */ const String& name,
        /* [in] */ Boolean append);

    ECode Constructor(
        /* [in] */ IFile* file);

    ECode Constructor(
        /* [in] */ IFile* file,
        /* [in] */ Boolean append);

    ECode Constructor(
        /* [in] */ IFileDescriptor* fdObj);

    ECode Constructor(
        /* [in] */ IFileDescriptor* fdObj,
        /* [in] */ Boolean isFdOwner);

    ECode Close() override;

    ECode Flush() override;

    ECode Write(
        /* [in] */ Integer byte) override;

    ECode Write(
        /* [in] */ const Array<Byte>& buffer) override;

    ECode Write(
        /* [in] */ const Array<Byte>& buffer,
        /* [in] */ Integer offset,
        /* [in] */ Integer size) override;
};

}
}

#endif // __CCM_IO_FILEOUTPUTSTREAM_H__
