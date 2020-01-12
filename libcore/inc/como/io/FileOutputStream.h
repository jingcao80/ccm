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

#ifndef __COMO_IO_FILEOUTPUTSTREAM_H__
#define __COMO_IO_FILEOUTPUTSTREAM_H__

#include "como/core/volatile.h"
#include "como/core/SyncObject.h"
#include "como/io/OutputStream.h"
#include "libcore/io/IoTracker.h"
#include "como.io.IFile.h"
#include "como.io.IFileDescriptor.h"
#include "como.io.IFileOutputStream.h"
#include "como.io.channels.IFileChannel.h"
#include "comort.system.ICloseGuard.h"
#include <comosp.h>

using como::core::SyncObject;
using como::io::channels::IFileChannel;
using comort::system::ICloseGuard;
using libcore::io::IoTracker;

namespace como {
namespace io {

class FileOutputStream
    : public OutputStream
    , public IFileOutputStream
{
public:
    FileOutputStream();

    ~FileOutputStream();

    COMO_INTERFACE_DECL();

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

    ECode Write(
        /* [in] */ Integer byte) override;

    ECode Write(
        /* [in] */ const Array<Byte>& buffer) override;

    ECode Write(
        /* [in] */ const Array<Byte>& buffer,
        /* [in] */ Integer offset,
        /* [in] */ Integer size) override;

    ECode Close() override;

    ECode GetFD(
        /* [out] */ IFileDescriptor** fd) override final;

    ECode GetChannel(
        /* [out] */ IFileChannel** channel) override;

private:
    ECode Open(
        /* [in] */ const String& name,
        /* [in] */ Boolean append);

private:
    AutoPtr<IFileDescriptor> mFd;

    Boolean mAppend;

    AutoPtr<IFileChannel> mChannel;

    SyncObject mCloseLock;
    VOLATILE Boolean mClosed = false;

    String mPath;

    AutoPtr<ICloseGuard> mGuard;
    Boolean mIsFdOwner;
    AutoPtr<IoTracker> mTracker;
};

}
}

#endif // __COMO_IO_FILEOUTPUTSTREAM_H__
