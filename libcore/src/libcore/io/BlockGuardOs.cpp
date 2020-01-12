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

#include "comort/system/BlockGuard.h"
#include "comort/system/SocketTagger.h"
#include "libcore/io/BlockGuardOs.h"
#include "libcore/io/Libcore.h"
#include "jing/system/OsConstants.h"
#include "comort.system.IBlockGuardPolicy.h"

using comort::system::BlockGuard;
using comort::system::SocketTagger;
using comort::system::IBlockGuardPolicy;
using jing::system::OsConstants;

namespace libcore {
namespace io {

ECode BlockGuardOs::TagSocket(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ IFileDescriptor** taggedFd)
{
    VALIDATE_NOT_NULL(taggedFd);

    ECode ec = SocketTagger::Get()->Tag(fd);
    if (FAILED(ec)) {
        return jing::system::E_ERRNO_EXCEPTION | (OsConstants::EINVAL_ & 0x000000ff);
    }
    *taggedFd = fd;
    REFCOUNT_ADD(*taggedFd);
    return NOERROR;
}

ECode BlockGuardOs::UntagSocket(
    /* [in] */ IFileDescriptor* fd)
{
    ECode ec = SocketTagger::Get()->Untag(fd);
    if (FAILED(ec)) {
        return jing::system::E_ERRNO_EXCEPTION | (OsConstants::EINVAL_ & 0x000000ff);
    }
    return NOERROR;
}

ECode BlockGuardOs::Accept(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ISocketAddress* peerAddress,
    /* [out] */ IFileDescriptor** retFd)
{
    VALIDATE_NOT_NULL(retFd);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnNetwork());
    AutoPtr<IFileDescriptor> acceptFd;
    mOs->Accept(fd, peerAddress, &acceptFd);
    if (IsInetSocket(acceptFd)) {
        return TagSocket(acceptFd, retFd);
    }
    acceptFd.MoveTo(retFd);
    return NOERROR;
}

ECode BlockGuardOs::Access(
    /* [in] */ const String& path,
    /* [in] */ Integer mode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    return mOs->Access(path, mode, result);
}

ECode BlockGuardOs::Chmod(
    /* [in] */ const String& path,
    /* [in] */ Integer mode)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Chmod(path, mode);
}

ECode BlockGuardOs::Chown(
    /* [in] */ const String& path,
    /* [in] */ Integer uid,
    /* [in] */ Integer gid)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
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
            BlockGuard::GetThreadPolicy(&policy);
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
            fd, OsConstants::SOL_SOCKET_, OsConstants::SO_LINGER_, &linger));
    Boolean on; Integer l;
    return (linger->IsOn(&on), on) && (linger->GetLinger(&l), l > 0);
}

ECode BlockGuardOs::Connect(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetAddress* address,
    /* [in] */ Integer port)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnNetwork());
    return mOs->Connect(fd, address, port);
}

ECode BlockGuardOs::Connect(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ISocketAddress* address)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnNetwork());
    return mOs->Connect(fd, address);
}

ECode BlockGuardOs::Execv(
    /* [in] */ const String& filename,
    /* [in] */ const Array<String>& argv)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    return mOs->Execv(filename, argv);
}

ECode BlockGuardOs::Execve(
    /* [in] */ const String& filename,
    /* [in] */ const Array<String>& argv,
    /* [in] */ const Array<String>& envp)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    return mOs->Execve(filename, argv, envp);
}

ECode BlockGuardOs::Fchmod(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer mode)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Fchmod(fd, mode);
}

ECode BlockGuardOs::Fchown(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer uid,
    /* [in] */ Integer gid)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Fchown(fd, uid, gid);
}

ECode BlockGuardOs::Fdatasync(
    /* [in] */ IFileDescriptor* fd)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Fdatasync(fd);
}

ECode BlockGuardOs::Fstat(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ IStructStat** stat)
{
    VALIDATE_NOT_NULL(stat);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    return mOs->Fstat(fd, stat);
}

ECode BlockGuardOs::Fstatvfs(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ IStructStatVfs** statVfs)
{
    VALIDATE_NOT_NULL(statVfs);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    return mOs->Fstatvfs(fd, statVfs);
}

ECode BlockGuardOs::Fsync(
    /* [in] */ IFileDescriptor* fd)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Fsync(fd);
}

ECode BlockGuardOs::Ftruncate(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Long length)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Ftruncate(fd, length);
}

ECode BlockGuardOs::Getxattr(
    /* [in] */ const String& path,
    /* [in] */ const String& name,
    /* [out, callee] */ Array<Byte>* attr)
{
    VALIDATE_NOT_NULL(attr);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    return mOs->Getxattr(path, name, attr);
}

ECode BlockGuardOs::Lchown(
    /* [in] */ const String& path,
    /* [in] */ Integer uid,
    /* [in] */ Integer gid)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Lchown(path, uid, gid);
}

ECode BlockGuardOs::Link(
    /* [in] */ const String& oldPath,
    /* [in] */ const String& newPath)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Link(oldPath, newPath);
}

ECode BlockGuardOs::Lseek(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Long offset,
    /* [in] */ Integer whence,
    /* [out] */ Long* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    return mOs->Lseek(fd, offset, whence, result);
}

ECode BlockGuardOs::Lstat(
    /* [in] */ const String& path,
    /* [out] */ IStructStat** stat)
{
    VALIDATE_NOT_NULL(stat);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    return mOs->Lstat(path, stat);
}

ECode BlockGuardOs::Mkdir(
    /* [in] */ const String& path,
    /* [in] */ Integer mode)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Mkdir(path, mode);
}

ECode BlockGuardOs::Mkfifo(
    /* [in] */ const String& path,
    /* [in] */ Integer mode)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Mkfifo(path, mode);
}

ECode BlockGuardOs::Msync(
    /* [in] */ HANDLE address,
    /* [in] */ Long byteCount,
    /* [in] */ Integer flags)
{
    if ((flags & OsConstants::MS_SYNC_) != 0) {
        AutoPtr<IBlockGuardPolicy> policy;
        BlockGuard::GetThreadPolicy(&policy);
        FAIL_RETURN(policy->OnWriteToDisk());
    }
    return mOs->Msync(address, byteCount, flags);
}

ECode BlockGuardOs::Open(
    /* [in] */ const String& path,
    /* [in] */ Integer flags,
    /* [in] */ Integer mode,
    /* [out] */ IFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    if ((flags & OsConstants::O_ACCMODE_) != OsConstants::O_RDONLY_) {
        FAIL_RETURN(policy->OnWriteToDisk());
    }
    return mOs->Open(path, flags, mode, fd);
}

ECode BlockGuardOs::Poll(
    /* [in] */ const Array<IStructPollfd*>& fds,
    /* [in] */ Integer timeoutMs,
    /* [out] */ Integer* result)
{
    VALIDATE_NOT_NULL(result);

    // Greater than 0 is a timeout in milliseconds and -1 means "block forever",
    // but 0 means "poll and return immediately", which shouldn't be subject to BlockGuard.
    if (timeoutMs != 0) {
        AutoPtr<IBlockGuardPolicy> policy;
        BlockGuard::GetThreadPolicy(&policy);
        FAIL_RETURN(policy->OnNetwork());
    }
    return mOs->Poll(fds, timeoutMs, result);
}

ECode BlockGuardOs::Posix_fallocate(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Long offset,
    /* [in] */ Long length)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Posix_fallocate(fd, offset, length);
}

ECode BlockGuardOs::Pread(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Array<Byte>& bytes,
    /* [in] */ Integer byteOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ Long offset,
    /* [out] */ Integer* num)
{
    VALIDATE_NOT_NULL(num);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    return mOs->Pread(fd, bytes, byteOffset, byteCount, offset, num);
}

ECode BlockGuardOs::Pread(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Long offset,
    /* [out] */ Integer* num)
{
    VALIDATE_NOT_NULL(num);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    return mOs->Pread(fd, buffer, offset, num);
}

ECode BlockGuardOs::Pwrite(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ const Array<Byte>& bytes,
    /* [in] */ Integer byteOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ Long offset,
    /* [out] */ Integer* num)
{
    VALIDATE_NOT_NULL(num);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Pwrite(fd, bytes, byteOffset, byteCount, offset, num);
}

ECode BlockGuardOs::Pwrite(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Long offset,
    /* [out] */ Integer* num)
{
    VALIDATE_NOT_NULL(num);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Pwrite(fd, buffer, offset, num);
}

ECode BlockGuardOs::Read(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Array<Byte>& bytes,
    /* [in] */ Integer byteOffset,
    /* [in] */ Integer byteCount,
    /* [out] */ Integer* num)
{
    VALIDATE_NOT_NULL(num);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    return mOs->Read(fd, bytes, byteOffset, byteCount, num);
}

ECode BlockGuardOs::Read(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ Integer* num)
{
    VALIDATE_NOT_NULL(num);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    return mOs->Read(fd, buffer, num);
}

ECode BlockGuardOs::Readlink(
    /* [in] */ const String& path,
    /* [out] */ String* link)
{
    VALIDATE_NOT_NULL(link);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    return mOs->Readlink(path, link);
}

ECode BlockGuardOs::Realpath(
    /* [in] */ const String& path,
    /* [out] */ String* realpath)
{
    VALIDATE_NOT_NULL(realpath);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    return mOs->Realpath(path, realpath);
}

ECode BlockGuardOs::Readv(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Array<IInterface*>& buffers,
    /* [out] */ Array<Integer>& offsets,
    /* [out] */ Array<Integer>& byteCounts,
    /* [out] */ Integer* num)
{
    VALIDATE_NOT_NULL(num);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    return mOs->Readv(fd, buffers, offsets, byteCounts, num);
}

ECode BlockGuardOs::Recvfrom(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Array<Byte>& bytes,
    /* [in] */ Integer byteOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ Integer flags,
    /* [in] */ IInetSocketAddress* srcAddress,
    /* [out] */ Integer* num)
{
    VALIDATE_NOT_NULL(num);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnNetwork());
    return mOs->Recvfrom(fd, bytes, byteOffset, byteCount, flags, srcAddress, num);
}

ECode BlockGuardOs::Recvfrom(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Integer flags,
    /* [in] */ IInetSocketAddress* srcAddress,
    /* [out] */ Integer* num)
{
    VALIDATE_NOT_NULL(num);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnNetwork());
    return mOs->Recvfrom(fd, buffer, flags, srcAddress, num);
}

ECode BlockGuardOs::Remove(
    /* [in] */ const String& path)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Remove(path);
}

ECode BlockGuardOs::Removexattr(
    /* [in] */ const String& path,
    /* [in] */ const String& name)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Removexattr(path, name);
}

ECode BlockGuardOs::Rename(
    /* [in] */ const String& oldPath,
    /* [in] */ const String& newPath)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Rename(oldPath, newPath);
}

ECode BlockGuardOs::Sendfile(
    /* [in] */ IFileDescriptor* outFd,
    /* [in] */ IFileDescriptor* inFd,
    /* [in, out] */ Long* inOffset,
    /* [in] */ Long byteCount,
    /* [out] */ Long* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Sendfile(outFd, inFd, inOffset, byteCount, result);
}

ECode BlockGuardOs::Sendto(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ const Array<Byte>& bytes,
    /* [in] */ Integer byteOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ Integer flags,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Integer port,
    /* [out] */ Integer* result)
{
    VALIDATE_NOT_NULL(result);

    if (inetAddress != nullptr) {
        AutoPtr<IBlockGuardPolicy> policy;
        BlockGuard::GetThreadPolicy(&policy);
        FAIL_RETURN(policy->OnNetwork());
    }
    return mOs->Sendto(fd, bytes, byteOffset, byteCount, flags, inetAddress, port, result);
}

ECode BlockGuardOs::Sendto(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ const Array<Byte>& bytes,
    /* [in] */ Integer byteOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ Integer flags,
    /* [in] */ ISocketAddress* address,
    /* [out] */ Integer* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnNetwork());
    return mOs->Sendto(fd, bytes, byteOffset, byteCount, flags, address, result);
}

ECode BlockGuardOs::Sendto(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Integer flags,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Integer port,
    /* [out] */ Integer* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnNetwork());
    return mOs->Sendto(fd, buffer, flags, inetAddress, port, result);
}

ECode BlockGuardOs::Setxattr(
    /* [in] */ const String& path,
    /* [in] */ const String& name,
    /* [in] */ const Array<Byte>& value,
    /* [in] */ Integer flags)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Setxattr(path, name, value, flags);
}

ECode BlockGuardOs::Socket(
    /* [in] */ Integer socketDomain,
    /* [in] */ Integer type,
    /* [in] */ Integer protocol,
    /* [out] */ IFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd);

    FAIL_RETURN(mOs->Socket(socketDomain, type, protocol, fd));
    if (IsInetDomain(socketDomain)) {
        TagSocket(*fd);
    }
    return NOERROR;
}

ECode BlockGuardOs::Socketpair(
    /* [in] */ Integer socketDomain,
    /* [in] */ Integer type,
    /* [in] */ Integer protocol,
    /* [in] */ IFileDescriptor* fd1,
    /* [in] */ IFileDescriptor* fd2)
{
    FAIL_RETURN(mOs->Socketpair(socketDomain, type, protocol, fd1, fd2));
    if (IsInetDomain(socketDomain)) {
        TagSocket(fd1);
        TagSocket(fd2);
    }
    return NOERROR;
}

ECode BlockGuardOs::Stat(
    /* [in] */ const String& path,
    /* [out] */ IStructStat** stat)
{
    VALIDATE_NOT_NULL(stat);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    return mOs->Stat(path, stat);
}

ECode BlockGuardOs::StatVfs(
    /* [in] */ const String& path,
    /* [out] */ IStructStatVfs** statfs)
{
    VALIDATE_NOT_NULL(statfs);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    return mOs->StatVfs(path, statfs);
}

ECode BlockGuardOs::Symlink(
    /* [in] */ const String& oldPath,
    /* [in] */ const String& newPath)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Symlink(oldPath, newPath);
}

ECode BlockGuardOs::Unlink(
    /* [in] */ const String& pathname)
{
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Unlink(pathname);
}

ECode BlockGuardOs::Write(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ const Array<Byte>& bytes,
    /* [in] */ Integer byteOffset,
    /* [in] */ Integer byteCount,
    /* [out] */ Integer* num)
{
    VALIDATE_NOT_NULL(num);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Write(fd, bytes, byteOffset, byteCount, num);
}

ECode BlockGuardOs::Write(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ Integer* num)
{
    VALIDATE_NOT_NULL(num);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Write(fd, buffer, num);
}

ECode BlockGuardOs::Writev(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ const Array<IInterface*>& buffers,
    /* [in] */ const Array<Integer>& offsets,
    /* [in] */ const Array<Integer>& byteCounts,
    /* [out] */ Integer* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy(&policy);
    FAIL_RETURN(policy->OnWriteToDisk());
    return mOs->Writev(fd, buffers, offsets, byteCounts, result);
}

}
}
