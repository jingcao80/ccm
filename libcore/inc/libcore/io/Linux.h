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

#ifndef __LIBCORE_IO_LINUX_H__
#define __LIBCORE_IO_LINUX_H__

#include "ccm/core/SyncObject.h"
#include "ccm.io.IByteBuffer.h"
#include "ccm.io.IFileDescriptor.h"
#include "ccm.net.IInetAddress.h"
#include "ccm.net.IInetSocketAddress.h"
#include "ccm.net.ISocketAddress.h"
#include "libcore.io.IOs.h"
#include "pisces.system.IStructAddrinfo.h"
#include "pisces.system.IStructCapUserData.h"
#include "pisces.system.IStructCapUserHeader.h"
#include "pisces.system.IStructFlock.h"
#include "pisces.system.IStructGroupReq.h"
#include "pisces.system.IStructGroupSourceReq.h"
#include "pisces.system.IStructIfaddrs.h"
#include "pisces.system.IStructLinger.h"
#include "pisces.system.IStructPasswd.h"
#include "pisces.system.IStructPollfd.h"
#include "pisces.system.IStructStat.h"
#include "pisces.system.IStructStatVfs.h"
#include "pisces.system.IStructTimeval.h"
#include "pisces.system.IStructUcred.h"
#include "pisces.system.IStructUtsname.h"

using ccm::core::SyncObject;
using ccm::io::IByteBuffer;
using ccm::io::IFileDescriptor;
using ccm::net::IInetAddress;
using ccm::net::IInetSocketAddress;
using ccm::net::ISocketAddress;
using pisces::system::IStructAddrinfo;
using pisces::system::IStructCapUserData;
using pisces::system::IStructCapUserHeader;
using pisces::system::IStructFlock;
using pisces::system::IStructGroupReq;
using pisces::system::IStructGroupSourceReq;
using pisces::system::IStructIfaddrs;
using pisces::system::IStructLinger;
using pisces::system::IStructPasswd;
using pisces::system::IStructPollfd;
using pisces::system::IStructStat;
using pisces::system::IStructStatVfs;
using pisces::system::IStructTimeval;
using pisces::system::IStructUcred;
using pisces::system::IStructUtsname;

namespace libcore {
namespace io {

class Linux
    : public SyncObject
    , public IOs
{
public:
    Linux();

    CCM_INTERFACE_DECL();

    ECode Accept(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ISocketAddress* peerAddress,
        /* [out] */ IFileDescriptor** retFd) override;

    ECode Access(
        /* [in] */ const String& path,
        /* [in] */ Integer mode,
        /* [out] */ Boolean* result) override;

    ECode Bind(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IInetAddress* address,
        /* [in] */ Integer port) override;

    ECode Bind(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ISocketAddress* address) override;

    ECode Capget(
        /* [in] */ IStructCapUserHeader* hdr,
        /* [out, callee] */ Array<IStructCapUserData*>* data) override;

    ECode Capset(
        /* [in] */ IStructCapUserHeader* hdr,
        /* [in] */ const Array<IStructCapUserData*>& data) override;

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

    ECode Dup(
        /* [in] */ IFileDescriptor* oldFd,
        /* [out] */ IFileDescriptor** retFd) override;

    ECode Dup2(
        /* [in] */ IFileDescriptor* oldFd,
        /* [in] */ Integer newFd,
        /* [out] */ IFileDescriptor** retFd) override;

    ECode Environ(
        /* [out, callee] */ Array<String>* env) override;

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

    ECode FcntlFlock(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer cmd,
        /* [in] */ IStructFlock* arg,
        /* [out] */ Integer* result) override;

    ECode FcntlInt(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer cmd,
        /* [in] */ Integer arg,
        /* [out] */ Integer* result) override;

    ECode FcntlVoid(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer cmd,
        /* [out] */ Integer* result) override;

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

    ECode Gai_Strerror(
        /* [in] */ Integer error,
        /* [out] */ String* strerror) override;

    ECode Getegid(
        /* [out] */ Integer* egid) override;

    ECode Geteuid(
        /* [out] */ Integer* euid) override;

    ECode Getgid(
        /* [out] */ Integer* gid) override;

    ECode Getenv(
        /* [in] */ const String& name,
        /* [out] */ String* value) override;

    ECode Getnameinfo(
        /* [in] */ IInetAddress* address,
        /* [in] */ Integer flags,
        /* [out] */ String* info) override;

    ECode Getpeername(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ ISocketAddress** name) override;

    ECode Getpgid(
        /* [in] */ Integer pid,
        /* [out] */ Integer* pgid) override;

    ECode Getpid(
        /* [out] */ Integer* pid) override;

    ECode Getppid(
        /* [out] */ Integer* ppid) override;

    ECode Getpwnam(
        /* [in] */ const String& name,
        /* [out] */ IStructPasswd** pwnam) override;

    ECode Getpwuid(
        /* [in] */ Integer uid,
        /* [out] */ IStructPasswd** pwuid) override;

    ECode Getsockname(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ ISocketAddress** socket) override;

    ECode GetsockoptByte(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer level,
        /* [in] */ Integer option,
        /* [out] */ Integer* sockopt) override;

    ECode GetsockoptInAddr(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer level,
        /* [in] */ Integer option,
        /* [out] */ IInetAddress** addr) override;

    ECode GetsockoptInt(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer level,
        /* [in] */ Integer option,
        /* [out] */ Integer* sockopt) override;

    ECode GetsockoptLinger(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer level,
        /* [in] */ Integer option,
        /* [out] */ IStructLinger** linger) override;

    ECode GetsockoptTimeval(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer level,
        /* [in] */ Integer option,
        /* [out] */ IStructTimeval** timeval) override;

    ECode GetsockoptUcred(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer level,
        /* [in] */ Integer option,
        /* [out] */ IStructUcred** ucred) override;

    ECode Gettid(
        /* [out] */ Integer* tid) override;

    ECode Getuid(
        /* [out] */ Integer* uid) override;

    ECode Getxattr(
        /* [in] */ const String& path,
        /* [in] */ const String& name,
        /* [out, callee] */ Array<Byte>* attr) override;

    ECode Getifaddrs(
        /* [out, callee] */ Array<IStructIfaddrs*>* addrs) override;

    ECode If_indextoname(
        /* [in] */ Integer index,
        /* [out] */ String* name) override;

    ECode If_nametoindex(
        /* [in] */ const String& name,
        /* [out] */ Integer* index) override;

    ECode Inet_pton(
        /* [in] */ Integer family,
        /* [in] */ const String& address,
        /* [out] */ IInetAddress** addr) override;

    ECode IoctlInetAddress(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer cmd,
        /* [in] */ const String& interfaceName,
        /* [out] */ IInetAddress** addr) override;

    ECode IoctlInt(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer cmd,
        /* [in, out] */ Integer* arg,
        /* [out] */ Integer* result) override;

    ECode IoctlMTU(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ const String& interfaceName,
        /* [out] */ Integer* mtu) override;

    ECode Isatty(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ Boolean* isatty) override;

    ECode Kill(
        /* [in] */ Integer pid,
        /* [in] */ Integer signal) override;

    ECode Lchown(
        /* [in] */ const String& path,
        /* [in] */ Integer uid,
        /* [in] */ Integer gid) override;

    ECode Link(
        /* [in] */ const String& oldPath,
        /* [in] */ const String& newPath) override;

    ECode Listen(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer backlog) override;

    ECode Listxattr(
        /* [in] */ const String& path,
        /* [out, callee] */ Array<String>* attr) override;

    ECode Lseek(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Long offset,
        /* [in] */ Integer whence,
        /* [out] */ Long* result) override;

    ECode Lstat(
        /* [in] */ const String& path,
        /* [out] */ IStructStat** stat) override;

    ECode Mincore(
        /* [in] */ HANDLE address,
        /* [in] */ Long byteCount,
        /* [in] */ const Array<Byte>& vector) override;

    ECode Mkdir(
        /* [in] */ const String& path,
        /* [in] */ Integer mode) override;

    ECode Mkfifo(
        /* [in] */ const String& path,
        /* [in] */ Integer mode) override;

    ECode Mlock(
        /* [in] */ HANDLE address,
        /* [in] */ Long byteCount) override;

    ECode Mmap(
        /* [in] */ HANDLE address,
        /* [in] */ Long byteCount,
        /* [in] */ Integer prot,
        /* [in] */ Integer flags,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Long offset,
        /* [out] */ HANDLE* result) override;

    ECode Msync(
        /* [in] */ HANDLE address,
        /* [in] */ Long byteCount,
        /* [in] */ Integer flags) override;

    ECode Munlock(
        /* [in] */ HANDLE address,
        /* [in] */ Long byteCount) override;

    ECode Munmap(
        /* [in] */ HANDLE address,
        /* [in] */ Long byteCount) override;

    ECode Open(
        /* [in] */ const String& path,
        /* [in] */ Integer flags,
        /* [in] */ Integer mode,
        /* [out] */ IFileDescriptor** fd) override;

    ECode Pipe2(
        /* [in] */ Integer flags,
        /* [out, callee] */ Array<IFileDescriptor*>* fds) override;

    ECode Pisces_Getaddrinfo(
        /* [in] */ const String& node,
        /* [in] */ IStructAddrinfo* hints,
        /* [in] */ Integer netId,
        /* [out, callee] */ Array<IInetAddress*>* infos) override;

    ECode Poll(
        /* [in] */ const Array<IStructPollfd*>& fds,
        /* [in] */ Integer timeoutMs,
        /* [out] */ Integer* result) override;

    ECode Posix_fallocate(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Long offset,
        /* [in] */ Long length) override;

    ECode Prctl(
        /* [in] */ Integer option,
        /* [in] */ Long arg2,
        /* [in] */ Long arg3,
        /* [in] */ Long arg4,
        /* [in] */ Long arg5,
        /* [out] */ Integer* prctl) override;

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

    ECode Removexattr(
        /* [in] */ const String& path,
        /* [in] */ const String& name) override;

    ECode Rename(
        /* [in] */ const String& oldPath,
        /* [in] */ const String& newPath) override;

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

    ECode Sendfile(
        /* [in] */ IFileDescriptor* outFd,
        /* [in] */ IFileDescriptor* inFd,
        /* [in, out] */ Long* inOffset,
        /* [in] */ Long byteCount,
        /* [out] */ Long* result) override;

    ECode Setegid(
        /* [in] */ Integer egid) override;

    ECode Setenv(
        /* [in] */ const String& name,
        /* [in] */ const String& value,
        /* [in] */ Boolean overwrite) override;

    ECode Seteuid(
        /* [in] */ Integer euid) override;

    ECode Setgid(
        /* [in] */ Integer gid) override;

    ECode Setpgid(
        /* [in] */ Integer pid,
        /* [in] */ Integer pgid) override;

    ECode Setregid(
        /* [in] */ Integer rgid,
        /* [in] */ Integer egid) override;

    ECode Setreuid(
        /* [in] */ Integer ruid,
        /* [in] */ Integer euid) override;

    ECode Setsid(
        /* [out] */ Integer* sid) override;

    ECode SetsockoptByte(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer level,
        /* [in] */ Integer option,
        /* [in] */ Integer value) override;

    ECode SetsockoptIfreq(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer level,
        /* [in] */ Integer option,
        /* [in] */ const String& value) override;

    ECode SetsockoptInt(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer level,
        /* [in] */ Integer option,
        /* [in] */ Integer value) override;

    ECode SetsockoptIpMreqn(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer level,
        /* [in] */ Integer option,
        /* [in] */ Integer value) override;

    ECode SetsockoptGroupReq(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer level,
        /* [in] */ Integer option,
        /* [in] */ IStructGroupReq* value) override;

    ECode SetsockoptGroupSourceReq(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer level,
        /* [in] */ Integer option,
        /* [in] */ IStructGroupSourceReq* value) override;

    ECode SetsockoptLinger(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer level,
        /* [in] */ Integer option,
        /* [in] */ IStructLinger* value) override;

    ECode SetsockoptTimeval(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer level,
        /* [in] */ Integer option,
        /* [in] */ IStructTimeval* value) override;

    ECode Setuid(
        /* [in] */ Integer uid) override;

    ECode Setxattr(
        /* [in] */ const String& path,
        /* [in] */ const String& name,
        /* [in] */ const Array<Byte>& value,
        /* [in] */ Integer flags) override;

    ECode Shutdown(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer how) override;

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

    ECode Strerror(
        /* [in] */ Integer errnum,
        /* [out] */ String* strerr) override;

    ECode Strsignal(
        /* [in] */ Integer signal,
        /* [out] */ String* strSignal) override;

    ECode Symlink(
        /* [in] */ const String& oldPath,
        /* [in] */ const String& newPath) override;

    ECode Sysconf(
        /* [in] */ Integer name,
        /* [out] */ Long* result) override;

    ECode Tcdrain(
        /* [in] */ IFileDescriptor* fd) override;

    ECode Tcsendbreak(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Integer duration) override;

    ECode Umask(
        /* [in] */ Integer mask,
        /* [out] */ Integer* result) override;

    ECode Uname(
        /* [out] */ IStructUtsname** uname) override;

    ECode Unlink(
        /* [in] */ const String& pathname) override;

    ECode Unsetenv(
        /* [in] */ const String& name) override;

    ECode Waitpid(
        /* [in] */ Integer pid,
        /* [in, out] */ Integer* status,
        /* [in] */ Integer options,
        /* [out] */ Integer* result) override;

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
};

inline Linux::Linux()
{}

}
}

#endif // __LIBCORE_IO_LINUX_H__
