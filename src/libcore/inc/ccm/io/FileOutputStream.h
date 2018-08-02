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

#include "ccm/core/SyncObject.h"
#include "ccm/io/OutputStream.h"
#include "libcore/io/IoTracker.h"
#include "ccm.io.IFile.h"
#include "ccm.io.IFileDescriptor.h"
#include "ccm.io.IFileOutputStream.h"
#include "ccm.io.channels.IFileChannel.h"
#include "ccmrt.system.ICloseGuard.h"
#include <ccmautoptr.h>

using ccm::core::SyncObject;
using ccm::io::channels::IFileChannel;
using ccmrt::system::ICloseGuard;
using libcore::io::IoTracker;

namespace ccm {
namespace io {

class FileOutputStream
    : public OutputStream
    , public IFileOutputStream
{
public:
    FileOutputStream();

    ~FileOutputStream();

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
    Boolean mClosed = false;

    String mPath;

    AutoPtr<ICloseGuard> mGuard;
    Boolean mIsFdOwner;
    AutoPtr<IoTracker> mTracker;
};

}
}

#endif // __CCM_IO_FILEOUTPUTSTREAM_H__
