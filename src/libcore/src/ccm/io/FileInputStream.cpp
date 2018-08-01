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

#include "ccm/core/AutoLock.h"
#include "ccm/core/System.h"
#include "ccm/io/CFile.h"
#include "ccm/io/CFileDescriptor.h"
#include "ccm/io/FileDescriptor.h"
#include "ccm/io/FileInputStream.h"
#include "ccm/io/ch/FileChannelImpl.h"
#include "ccmrt/system/BlockGuard.h"
#include "ccmrt/system/CloseGuard.h"
#include "libcore/io/IoBridge.h"
#include "ccm.core.IInteger.h"
#include "ccm.core.ISecurityManager.h"
#include "ccmrt.system.IBlockGuardPolicy.h"
#include <ccmlogger.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

using ccm::core::AutoLock;
using ccm::core::IInteger;
using ccm::core::ISecurityManager;
using ccm::core::System;
using ccm::io::ch::FileChannelImpl;
using ccmrt::system::BlockGuard;
using ccmrt::system::CloseGuard;
using ccmrt::system::IBlockGuardPolicy;
using libcore::io::IoBridge;

extern ECode fileOpen(const char* path, int flags, int* fd);

namespace ccm {
namespace io {

CCM_INTERFACE_IMPL_1(FileInputStream, InputStream, IFileInputStream);

FileInputStream::FileInputStream()
{
    mGuard = CloseGuard::Get();
    mTracker = new IoTracker();
}

FileInputStream::~FileInputStream()
{
    mGuard->WarnIfOpen();

    if (mFd != nullptr && mFd != FileDescriptor::GetIN()) {
        Close();
    }
}

ECode FileInputStream::Constructor(
    /* [in] */ const String& name)
{
    AutoPtr<IFile> f;
    if (!name.IsNull()) {
        CFile::New(name, IID_IFile, (IInterface**)&f);
    }
    return Constructor(f);
}

ECode FileInputStream::Constructor(
    /* [in] */ IFile* file)
{
    String name;
    if (file != nullptr) {
        file->GetPath(&name);
    }
    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckRead(name));
    }
    if (name.IsNull()) {
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    if (CFile::From(file)->IsInvalid()) {
        Logger::E("FileInputStream", "Invalid file path");
        return E_FILE_NOT_FOUND_EXCEPTION;
    }
    CFileDescriptor::New(IID_IFileDescriptor, (IInterface**)&mFd);
    mIsFdOwner = true;
    mPath = name;

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    FAIL_RETURN(Open(name));
    mGuard->Open(String("close"));
    return NOERROR;
}

ECode FileInputStream::Constructor(
    /* [in] */ IFileDescriptor* fdObj)
{
    return Constructor(fdObj, false);
}

ECode FileInputStream::Constructor(
    /* [in] */ IFileDescriptor* fdObj,
    /* [in] */ Boolean isFdOwner)
{
    if (fdObj == nullptr) {
        Logger::E("FileInputStream", "fdObj == nullptr");
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    mFd = fdObj;
    mIsFdOwner = isFdOwner;
    mPath = nullptr;
    return NOERROR;
}

ECode FileInputStream::Open(
    /* [in] */ const String& name)
{
    int fd;
    FAIL_RETURN(fileOpen(name.string(), O_RDONLY, &fd));
    mFd->SetInt(fd);
    return NOERROR;
}

ECode FileInputStream::Read(
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    Array<Byte> b(1);
    Integer n;
    FAIL_RETURN(Read(b, 0, 1, &n));
    *value = n != -1 ? b[0] & 0xff : -1;
    return NOERROR;
}

ECode FileInputStream::Read(
    /* [out] */ Array<Byte>& buffer,
    /* [in] */ Integer offset,
    /* [in] */ Integer size,
    /* [out] */ Integer* number)
{
    VALIDATE_NOT_NULL(number);

    if (mClosed && size > 0) {
        Logger::E("FileInputStream", "Stream Closed");
        return E_IO_EXCEPTION;
    }
    FAIL_RETURN(mTracker->TrackIo(size));
    return IoBridge::Read(mFd, buffer, offset, size, number);
}

ECode FileInputStream::Skip(
    /* [in] */ Long byteCount,
    /* [out] */ Long* number)
{
    VALIDATE_NOT_NULL(number);

    if (mClosed) {
        Logger::E("FileInputStream", "Stream Closed");
        return E_IO_EXCEPTION;
    }

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    ECode ec = Skip0(byteCount, number);
    if (ec == E_USE_MANUAL_SKIP_EXCEPTION) {
        return InputStream::Skip(byteCount, number);
    }
    return NOERROR;
}

ECode FileInputStream::Skip0(
    /* [in] */ Long byteCount,
    /* [out] */ Long* number)
{
    Integer fd;
    mFd->GetInt(&fd);
    Long cur = 0, end = 0;
    if ((cur = lseek64(fd, 0, SEEK_CUR)) == -1) {
        if (errno == ESPIPE) {
            return E_USE_MANUAL_SKIP_EXCEPTION;
        }
        else {
            Logger::E("FileInputStream", "Seek error");
            return E_IO_EXCEPTION;
        }
    }
    else if ((end = lseek64(fd, byteCount, SEEK_CUR)) == -1) {
        Logger::E("FileInputStream", "Seek error");
        return E_IO_EXCEPTION;
    }
    *number = (end - cur);
    return NOERROR;
}

static int available(int fd, Long* bytes)
{
    int n;
    // Unlike the original OpenJdk implementation, we use FIONREAD for all file
    // types. For regular files, this is specified to return the difference
    // between the current position and the file size. Note that this can be
    // negative if we're positioned past the end of the file. We must return 0
    // in that case.
    if (ioctl(fd, FIONREAD, &n) != -1) {
        if (n < 0) {
            n = 0;
        }
        *bytes = n;
        return 1;
    }

    // FIONREAD is specified to return ENOTTY when fd refers to a file
    // type for which this ioctl isn't implemented.
    if (errno == ENOTTY) {
        *bytes = 0;
        return 1;
    }

    // Raise an exception for all other error types.
    return 0;
}

ECode FileInputStream::Available(
    /* [out] */ Integer* number)
{
    VALIDATE_NOT_NULL(number);

    if (mClosed) {
        Logger::E("FileInputStream", "Stream Closed");
        return E_IO_EXCEPTION;
    }

    Integer fd;
    mFd->GetInt(&fd);
    Long ret;
    if (available(fd, &ret)) {
        if (ret > IInteger::MAX_VALUE) {
            ret = (Integer)IInteger::MAX_VALUE;
        }
        *number = ret;
        return NOERROR;
    }
    return E_IO_EXCEPTION;
}

ECode FileInputStream::Close()
{
    {
        AutoLock lock(mCloseLock);

        if (mClosed) {
            return NOERROR;
        }
        mClosed = true;
    }

    mGuard->Close();

    if (mChannel != nullptr) {
        /*
         * Decrement the FD use count associated with the channel
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

ECode FileInputStream::GetFD(
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

ECode FileInputStream::GetChannel(
    /* [out] */ IFileChannel** channel)
{
    VALIDATE_NOT_NULL(channel);

    AutoLock lock(this);

    if (mChannel == nullptr) {
        mChannel = FileChannelImpl::Open(mFd, mPath, true, false, (IFileInputStream*)this);
    }
    *channel = mChannel;
    REFCOUNT_ADD(*channel);
    return NOERROR;
}

}
}
