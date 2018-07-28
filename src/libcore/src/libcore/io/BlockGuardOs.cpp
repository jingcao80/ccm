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

#include "ccmrt/system/BlockGuard.h"
#include "libcore/io/BlockGuardOs.h"
#include "libcore/io/Libcore.h"
#include "pisces/system/OsConstants.h"
#include "ccmrt.system.IBlockGuardPolicy.h"

using ccmrt::system::BlockGuard;
using ccmrt::system::IBlockGuardPolicy;
using pisces::system::OsConstants;

namespace libcore {
namespace io {

ECode BlockGuardOs::TagSocket(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ IFileDescriptor** taggedFd)
{
    return NOERROR;
}

ECode BlockGuardOs::UntagSocket(
    /* [in] */ IFileDescriptor* fd)
{
    return NOERROR;
}

ECode BlockGuardOs::Accept(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ISocketAddress* peerAddress,
    /* [out] */ IFileDescriptor** retFd)
{
    VALIDATE_NOT_NULL(retFd);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy);
    FAIL_RETURN(policy->OnNetwork());
    AutoPtr<IFileDescriptor> acceptFd;
    mOs->Accept(fd, peerAddress, (IFileDescriptor**)&acceptFd);
    if (IsInetSocket(acceptFd)) {
        return TagSocket(acceptFd, retFd);
    }
    *retFd = acceptFd;
    REFCOUNT_ADD(*retFd);
    return NOERROR;
}

ECode BlockGuardOs::Access(
    /* [in] */ const String& path,
    /* [in] */ Integer mode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    return mOs->Access(path, mode, result);
}

ECode BlockGuardOs::Chmod(
    /* [in] */ const String& path,
    /* [in] */ Integer mode)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Chmod(path, mode);
}

ECode BlockGuardOs::Chown(
    /* [in] */ const String& path,
    /* [in] */ Integer uid,
    /* [in] */ Integer gid)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Chown(path, uid, gid);
}

ECode BlockGuardOs::Close(
    /* [in] */ IFileDescriptor* fd)
{
    Boolean socket;
    if (fd->IsSocket(&socket), socket) {
        if (IsLingerSocket(fd)) {
            // If the fd is a socket with SO_LINGER set, we might block indefinitely.
            // We allow non-linger sockets so that apps can close their network
            // connections in methods like onDestroy which will run on the UI thread.
            AutoPtr<IBlockGuardPolicy> policy;
            BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy);
            FAIL_RETURN(policy->OnNetwork());
        }
        if (IsInetSocket(fd)) {
            UntagSocket(fd);
        }
    }
    return mOs->Close(fd);
}

Boolean BlockGuardOs::IsInetSocket(
    /* [in] */ IFileDescriptor* fd)
{
    Integer domain;
    ECode ec = Libcore::GetOs()->GetsockoptInt(fd, OsConstants::SOL_SOCKET_, OsConstants::SO_DOMAIN_, &domain);
    return SUCCEEDED(ec) && IsInetDomain(domain);
}

Boolean BlockGuardOs::IsInetDomain(
    /* [in] */ Integer domain)
{
    return (domain == OsConstants::AF_INET_) || (domain == OsConstants::AF_INET6_);
}

Boolean BlockGuardOs::IsLingerSocket(
    /* [in] */ IFileDescriptor* fd)
{
    AutoPtr<IStructLinger> linger;
    FAIL_RETURN(Libcore::GetOs()->GetsockoptLinger(
            fd, OsConstants::SOL_SOCKET_, OsConstants::SO_LINGER_, (IStructLinger**)&linger));
    Boolean on; Integer l;
    return (linger->IsOn(&on), on) && (linger->GetLinger(&l), l > 0);
}

}
}
