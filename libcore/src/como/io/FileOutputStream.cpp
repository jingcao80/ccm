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

#include "como/core/AutoLock.h"
#include "como/core/System.h"
#include "como/io/CFile.h"
#include "como/io/CFileDescriptor.h"
#include "como/io/FileOutputStream.h"
#include "como/io/ch/FileChannelImpl.h"
#include "comort/system/BlockGuard.h"
#include "comort/system/CloseGuard.h"
#include "libcore/io/IoBridge.h"
#include "como.core.ISecurityManager.h"
#include "como.io.IFile.h"
#include "comort.system.IBlockGuardPolicy.h"
#include <ccmlogger.h>
#include <fcntl.h>

using como::core::AutoLock;
using como::core::ISecurityManager;
using como::core::System;
using como::io::ch::FileChannelImpl;
using comort::system::BlockGuard;
using comort::system::CloseGuard;
using comort::system::IBlockGuardPolicy;
using libcore::io::IoBridge;

extern ECode fileOpen(const char* path, int flags, int* fd);

namespace como {
namespace io {

COMO_INTERFACE_IMPL_1(FileOutputStream, OutputStream, IFileOutputStream);

FileOutputStream::FileOutputStream()
{
    mGuard = CloseGuard::Get();
    mTracker = new IoTracker();
}

FileOutputStream::~FileOutputStream()
{
    mGuard->WarnIfOpen();

    if (mFd != nullptr) {
        if (mFd == FileDescriptor::GetOUT() || mFd == FileDescriptor::GetERR()) {
            Flush();
        }
        else {
            Close();
        }
    }
}

ECode FileOutputStream::Constructor(
    /* [in] */ const String& name)
{
    AutoPtr<IFile> f;
    if (!name.IsNull()) {
        CFile::New(name, IID_IFile, (IInterface**)&f);
    }
    return Constructor(f, false);
}

ECode FileOutputStream::Constructor(
    /* [in] */ const String& name,
    /* [in] */ Boolean append)
{
    AutoPtr<IFile> f;
    if (!name.IsNull()) {
        CFile::New(name, IID_IFile, (IInterface**)&f);
    }
    return Constructor(f, append);
}

ECode FileOutputStream::Constructor(
    /* [in] */ IFile* file)
{
    return Constructor(file, false);
}

ECode FileOutputStream::Constructor(
    /* [in] */ IFile* file,
    /* [in] */ Boolean append)
{
    String name;
    if (file == nullptr || (file->GetPath(&name), name.IsNull())) {
        return como::core::E_NULL_POINTER_EXCEPTION;
    }
    if (CFile::From(file)->IsInvalid()) {
        Logger::E("FileOutputStream", "Invalid file path");
        return E_FILE_NOT_FOUND_EXCEPTION;
    }
    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckWrite(name));
    }
    CFileDescriptor::New(IID_IFileDescriptor, (IInterface**)&mFd);
    mAppend = append;
    mPath = name;
    mIsFdOwner = true;

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    FAIL_RETURN(Open(name, append));
    mGuard->Open(String("Close"));
    return NOERROR;
}

ECode FileOutputStream::Constructor(
    /* [in] */ IFileDescriptor* fdObj)
{
    return Constructor(fdObj, false);
}

ECode FileOutputStream::Constructor(
    /* [in] */ IFileDescriptor* fdObj,
    /* [in] */ Boolean isFdOwner)
{
    if (fdObj == nullptr) {
        Logger::E("FileOutputStream", "fdObj == null");
        return como::core::E_NULL_POINTER_EXCEPTION;
    }

    mFd = fdObj;
    mPath = nullptr;
    mAppend = false;
    mIsFdOwner = isFdOwner;
    return NOERROR;
}

ECode FileOutputStream::Open(
    /* [in] */ const String& name,
    /* [in] */ Boolean append)
{
    int fd;
    FAIL_RETURN(fileOpen(mPath.string(),
            O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC), &fd));
    mFd->SetInt(fd);
    return NOERROR;
}

ECode FileOutputStream::Write(
    /* [in] */ Integer byte)
{
    Array<Byte> buf(1);
    buf[0] = (Byte)byte;
    return Write(buf, 0, 1);
}

ECode FileOutputStream::Write(
    /* [in] */ const Array<Byte>& buffer)
{
    return Write(buffer, 0, buffer.GetLength());
}

ECode FileOutputStream::Write(
    /* [in] */ const Array<Byte>& buffer,
    /* [in] */ Integer offset,
    /* [in] */ Integer size)
{
    VOLATILE_GET(Boolean closed, mClosed);
    if (closed && size > 0) {
        Logger::E("FileOutputStream", "Stream Closed");
        return E_IO_EXCEPTION;
    }
    mTracker->TrackIo(size);
    return IoBridge::Write(mFd, buffer, offset, size);
}

ECode FileOutputStream::Close()
{
    {
        AutoLock lock(mCloseLock);

        VOLATILE_GET(Boolean closed, mClosed);
        if (closed) {
            return NOERROR;
        }
        VOLATILE_SET(mClosed, true);
    }

    mGuard->Close();

    if (mChannel != nullptr) {
        /*
         * Decrement FD use count associated with the channel
         * The use count is incremented whenever a new channel
         * is obtained from this stream.
         */
        mChannel->Close();
    }

    if (mIsFdOwner) {
        return IoBridge::CloseAndSignalBlockedThreads(mFd);
    }
    return NOERROR;
}

ECode FileOutputStream::GetFD(
    /* [out] */ IFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd);

    if (mFd != nullptr) {
        *fd = mFd;
        REFCOUNT_ADD(*fd);
        return NOERROR;
    }
    return E_IO_EXCEPTION;
}

ECode FileOutputStream::GetChannel(
    /* [out] */ IFileChannel** channel)
{
    VALIDATE_NOT_NULL(channel);

    AutoLock lock(this);

    if (mChannel == nullptr) {
        mChannel = FileChannelImpl::Open(mFd, mPath, false, true, mAppend, (IFileOutputStream*)this);
    }
    *channel = mChannel;
    REFCOUNT_ADD(*channel);
    return NOERROR;
}

}
}
