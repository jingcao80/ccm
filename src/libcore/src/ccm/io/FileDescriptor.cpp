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

#include "ccm/io/CFileDescriptor.h"
#include "ccm/io/FileDescriptor.h"
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

namespace ccm {
namespace io {

CCM_INTERFACE_IMPL_1(FileDescriptor, SyncObject, IFileDescriptor);

AutoPtr<IFileDescriptor> FileDescriptor::GetIN()
{
    static AutoPtr<IFileDescriptor> IN = DupFd(0);
    return IN;
}

AutoPtr<IFileDescriptor> FileDescriptor::GetOUT()
{
    static AutoPtr<IFileDescriptor> OUT = DupFd(1);
    return OUT;
}

AutoPtr<IFileDescriptor> FileDescriptor::GetERR()
{
    static AutoPtr<IFileDescriptor> ERR = DupFd(2);
    return ERR;
}

ECode FileDescriptor::Constructor()
{
    mDescriptor = -1;
    return NOERROR;
}

ECode FileDescriptor::Constructor(
    /* [in] */ Integer descriptor)
{
    mDescriptor = descriptor;
    return NOERROR;
}

ECode FileDescriptor::Valid(
    /* [out] */ Boolean* valid)
{
    VALIDATE_NOT_NULL(valid);

    *valid = mDescriptor != -1;
    return NOERROR;
}

ECode FileDescriptor::Sync()
{
    if (TEMP_FAILURE_RETRY(fsync(mDescriptor)) == -1) {
        return E_IO_SYNC_FAILED_EXCEPTION;
    }
    return NOERROR;
}

ECode FileDescriptor::GetInt(
    /* [out] */ Integer* fd)
{
    VALIDATE_NOT_NULL(fd);

    *fd = mDescriptor;
    return NOERROR;
}

ECode FileDescriptor::SetInt(
    /* [in] */ Integer fd)
{
    mDescriptor = fd;
    return NOERROR;
}

ECode FileDescriptor::IsSocket(
    /* [out] */ Boolean* socket)
{
    VALIDATE_NOT_NULL(socket);

    int error;
    socklen_t error_length = sizeof(error);
    *socket = TEMP_FAILURE_RETRY(getsockopt(mDescriptor, SOL_SOCKET,
            SO_ERROR, &error, &error_length)) == 0;
    return NOERROR;
}

AutoPtr<IFileDescriptor> FileDescriptor::DupFd(
    /* [in] */ Integer fd)
{
    int nfd = TEMP_FAILURE_RETRY(fcntl(fd, F_DUPFD_CLOEXEC, 0));
    CHECK(nfd != -1);
    AutoPtr<IFileDescriptor> fdObj;
    CFileDescriptor::New(nfd, IID_IFileDescriptor, (IInterface**)&fdObj);
    return fdObj;
}

}
}
