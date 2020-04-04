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

#ifndef __COMO_IO_FILEINPUTSTREAM_H__
#define __COMO_IO_FILEINPUTSTREAM_H__

#include "como/core/volatile.h"
#include "como/io/InputStream.h"
#include "libcore/io/IoTracker.h"
#include "como.io.IFile.h"
#include "como.io.IFileDescriptor.h"
#include "como.io.IFileInputStream.h"
#include "como.io.channels.IFileChannel.h"
#include "comort.system.ICloseGuard.h"
#include <comosp.h>

using como::io::channels::IFileChannel;
using comort::system::ICloseGuard;
using libcore::io::IoTracker;

namespace como {
namespace io {

class FileInputStream
    : public InputStream
    , public IFileInputStream
{
public:
    FileInputStream();

    ~FileInputStream();

    COMO_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ const String& name);

    ECode Constructor(
        /* [in] */ IFile* file);

    ECode Constructor(
        /* [in] */ IFileDescriptor* fdObj);

    ECode Constructor(
        /* [in] */ IFileDescriptor* fdObj,
        /* [in] */ Boolean isFdOwner);

    ECode Read(
        /* [out] */ Integer& value) override;

    ECode Read(
        /* [out] */ Array<Byte>& buffer,
        /* [in] */ Integer offset,
        /* [in] */ Integer size,
        /* [out] */ Integer& number) override;

    ECode Skip(
        /* [in] */ Long byteCount,
        /* [out] */ Long& number) override;

    ECode Available(
        /* [out] */ Integer& number) override;

    ECode Close() override;

    ECode GetFD(
        /* [out] */ AutoPtr<IFileDescriptor>& fd) override final;

    ECode GetChannel(
        /* [out] */ AutoPtr<IFileChannel>& channel) override;

private:
    ECode Open(
        /* [in] */ const String& name);

    ECode Skip0(
        /* [in] */ Long byteCount,
        /* [out] */ Long& number);

private:
    AutoPtr<IFileDescriptor> mFd;

    String mPath;

    AutoPtr<IFileChannel> mChannel;

    SyncObject mCloseLock;
    VOLATILE Boolean mClosed = false;
    Boolean mIsFdOwner;

    AutoPtr<ICloseGuard> mGuard;
    AutoPtr<IoTracker> mTracker;
};

}
}

#endif // __COMO_IO_FILEINPUTSTREAM_H__
