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

#ifndef __LIBCORE_IO_BLOCKGUARDOS_H__
#define __LIBCORE_IO_BLOCKGUARDOS_H__

#include "libcore/io/ForwardingOs.h"

namespace libcore {
namespace io {

class BlockGuardOs
    : public ForwardingOs
{
public:
    BlockGuardOs(
        /* [in] */ IOs* os);

    ECode Accept(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ISocketAddress* peerAddress,
        /* [out] */ IFileDescriptor** retFd) override;

    ECode Access(
        /* [in] */ const String& path,
        /* [in] */ Integer mode,
        /* [out] */ Boolean* result) override;

    ECode Chmod(
        /* [in] */ const String& path,
        /* [in] */ Integer mode) override;

    ECode Chown(
        /* [in] */ const String& path,
        /* [in] */ Integer uid,
        /* [in] */ Integer gid) override;

    ECode Close(
        /* [in] */ IFileDescriptor* fd) override;

    ECode Connect(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IInetAddress* address,
        /* [in] */ Integer port) override;

    ECode Connect(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ISocketAddress* address) override;

    ECode Execv(
        /* [in] */ const String& filename,
        /* [in] */ const Array<String>& argv) override;

    ECode Execve(
        /* [in] */ const String& filename,
        /* [in] */ const Array<String>& argv,
        /* [in] */ const Array<String>& envp) override;

    ECode Fchmod(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer mode) override;

    ECode Fchown(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer uid,
        /* [in] */ Integer gid) override;

    ECode Fdatasync(
        /* [in] */ IFileDescriptor* fd) override;

    ECode Fstat(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ IStructStat** stat) override;

    ECode Fstatvfs(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ IStructStatVfs** statVfs) override;

    ECode Fsync(
        /* [in] */ IFileDescriptor* fd) override;

    ECode Ftruncate(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Long length) override;

    ECode Getxattr(
        /* [in] */ const String& path,
        /* [in] */ const String& name,
        /* [out, callee] */ Array<Byte>* attr) override;

    ECode Lchown(
        /* [in] */ const String& path,
        /* [in] */ Integer uid,
        /* [in] */ Integer gid) override;

    ECode Link(
        /* [in] */ const String& oldPath,
        /* [in] */ const String& newPath) override;

    ECode Lseek(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Long offset,
        /* [in] */ Integer whence,
        /* [out] */ Long* result) override;

    ECode Lstat(
        /* [in] */ const String& path,
        /* [out] */ IStructStat** stat) override;

    ECode Mkdir(
        /* [in] */ const String& path,
        /* [in] */ Integer mode) override;

    ECode Mkfifo(
        /* [in] */ const String& path,
        /* [in] */ Integer mode) override;

    ECode Msync(
        /* [in] */ HANDLE address,
        /* [in] */ Long byteCount,
        /* [in] */ Integer flags) override;

    ECode Open(
        /* [in] */ const String& path,
        /* [in] */ Integer flags,
        /* [in] */ Integer mode,
        /* [out] */ IFileDescriptor** fd) override;

    ECode Poll(
        /* [in] */ const Array<IStructPollfd*>& fds,
        /* [in] */ Integer timeoutMs,
        /* [out] */ Integer* result) override;

    ECode Posix_fallocate(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Long offset,
        /* [in] */ Long length) override;

    ECode Pread(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ Array<Byte>& bytes,
        /* [in] */ Integer byteOffset,
        /* [in] */ Integer byteCount,
        /* [in] */ Long offset,
        /* [out] */ Integer* num) override;

    ECode Pread(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Long offset,
        /* [out] */ Integer* num) override;

    ECode Pwrite(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ const Array<Byte>& bytes,
        /* [in] */ Integer byteOffset,
        /* [in] */ Integer byteCount,
        /* [in] */ Long offset,
        /* [out] */ Integer* num) override;

    ECode Pwrite(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Long offset,
        /* [out] */ Integer* num) override;

    ECode Read(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ Array<Byte>& bytes,
        /* [in] */ Integer byteOffset,
        /* [in] */ Integer byteCount,
        /* [out] */ Integer* num) override;

    ECode Read(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IByteBuffer* buffer,
        /* [out] */ Integer* num) override;

    ECode Readlink(
        /* [in] */ const String& path,
        /* [out] */ String* link) override;

    ECode Realpath(
        /* [in] */ const String& path,
        /* [out] */ String* realpath) override;

    ECode Readv(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ Array<IInterface*>& buffers,
        /* [out] */ Array<Integer>& offsets,
        /* [out] */ Array<Integer>& byteCounts,
        /* [out] */ Integer* num) override;

    ECode Recvfrom(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ Array<Byte>& bytes,
        /* [in] */ Integer byteOffset,
        /* [in] */ Integer byteCount,
        /* [in] */ Integer flags,
        /* [in] */ IInetSocketAddress* srcAddress,
        /* [out] */ Integer* num) override;

    ECode Recvfrom(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Integer flags,
        /* [in] */ IInetSocketAddress* srcAddress,
        /* [out] */ Integer* num) override;

    ECode Remove(
        /* [in] */ const String& path) override;

    ECode Rename(
        /* [in] */ const String& oldPath,
        /* [in] */ const String& newPath) override;

    ECode Removexattr(
        /* [in] */ const String& path,
        /* [in] */ const String& name) override;

    ECode Sendfile(
        /* [in] */ IFileDescriptor* outFd,
        /* [in] */ IFileDescriptor* inFd,
        /* [in, out] */ Long* inOffset,
        /* [in] */ Long byteCount,
        /* [out] */ Long* result) override;

    ECode Sendto(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ const Array<Byte>& bytes,
        /* [in] */ Integer byteOffset,
        /* [in] */ Integer byteCount,
        /* [in] */ Integer flags,
        /* [in] */ IInetAddress* inetAddress,
        /* [in] */ Integer port,
        /* [out] */ Integer* result) override;

    ECode Sendto(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ const Array<Byte>& bytes,
        /* [in] */ Integer byteOffset,
        /* [in] */ Integer byteCount,
        /* [in] */ Integer flags,
        /* [in] */ ISocketAddress* address,
        /* [out] */ Integer* result) override;

    ECode Sendto(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Integer flags,
        /* [in] */ IInetAddress* inetAddress,
        /* [in] */ Integer port,
        /* [out] */ Integer* result) override;

    ECode Setxattr(
        /* [in] */ const String& path,
        /* [in] */ const String& name,
        /* [in] */ const Array<Byte>& value,
        /* [in] */ Integer flags) override;

    ECode Socket(
        /* [in] */ Integer socketDomain,
        /* [in] */ Integer type,
        /* [in] */ Integer protocol,
        /* [out] */ IFileDescriptor** fd) override;

    ECode Socketpair(
        /* [in] */ Integer socketDomain,
        /* [in] */ Integer type,
        /* [in] */ Integer protocol,
        /* [in] */ IFileDescriptor* fd1,
        /* [in] */ IFileDescriptor* fd2) override;

    ECode Stat(
        /* [in] */ const String& path,
        /* [out] */ IStructStat** stat) override;

    ECode StatVfs(
        /* [in] */ const String& path,
        /* [out] */ IStructStatVfs** statfs) override;

    ECode Symlink(
        /* [in] */ const String& oldPath,
        /* [in] */ const String& newPath) override;

    ECode Unlink(
        /* [in] */ const String& pathname) override;

    ECode Write(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ const Array<Byte>& bytes,
        /* [in] */ Integer byteOffset,
        /* [in] */ Integer byteCount,
        /* [out] */ Integer* num) override;

    ECode Write(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IByteBuffer* buffer,
        /* [out] */ Integer* num) override;

    ECode Writev(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ const Array<IInterface*>& buffers,
        /* [in] */ const Array<Integer>& offsets,
        /* [in] */ const Array<Integer>& byteCounts,
        /* [out] */ Integer* result) override;

private:
    ECode TagSocket(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ IFileDescriptor** taggedFd = nullptr);

    ECode UntagSocket(
        /* [in] */ IFileDescriptor* fd);

    static Boolean IsInetSocket(
        /* [in] */ IFileDescriptor* fd);

    static Boolean IsInetDomain(
        /* [in] */ Integer domain);

    static Boolean IsLingerSocket(
        /* [in] */ IFileDescriptor* fd);
};

inline BlockGuardOs::BlockGuardOs(
    /* [in] */ IOs* os)
    : ForwardingOs(os)
{}

}
}

#endif // __LIBCORE_IO_BLOCKGUARDOS_H__
