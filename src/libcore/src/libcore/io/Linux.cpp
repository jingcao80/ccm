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

#include "libcore/io/AsynchronousCloseMonitor.h"
#include "libcore/io/Linux.h"
#include <ccmlogger.h>
#include <errno.h>
#include <unistd.h>

namespace libcore {
namespace io {

/**
 * Used to retry system calls that can be interrupted with a signal. Unlike TEMP_FAILURE_RETRY, this
 * also handles the case where AsynchronousCloseMonitor::signalBlockedThreads(fd) is used to signal
 * a close() or Thread.interrupt(). Other signals that result in an EINTR result are ignored and the
 * system call is retried.
 *
 * Returns the result of the system call though a Java exception will be pending if the result is
 * -1: an IOException if the file descriptor is already closed, a InterruptedIOException if signaled
 * via AsynchronousCloseMonitor, or ErrnoException for other failures.
 */
#define IO_FAILURE_RETRY(outEc, return_type, syscall_name, fdObj, ...) ({ \
    return_type _rc = -1; \
    int _syscallErrno; \
    do { \
        bool _wasSignaled; \
        { \
            int _fd; \
            fdObj->GetInt(&_fd); \
            AsynchronousCloseMonitor _monitor(_fd); \
            _rc = syscall_name(_fd, __VA_ARGS__); \
            _syscallErrno = errno; \
            _wasSignaled = _monitor.WasSignaled(); \
        } \
        if (_wasSignaled) { \
            Logger::E("Linux", #syscall_name " interrupted"); \
            *outEc = ccm::io::E_INTERRUPTED_IO_EXCEPTION; \
            _rc = -1; \
            break; \
        } \
        if (_rc == -1 && _syscallErrno != EINTR) { \
            /* TODO: with a format string we could show the arguments too, like strace(1). */ \
            *outEc = pisces::system::E_ERRNO_EXCEPTION | (errno & 0x000000ff); \
            break; \
        } \
    } while (_rc == -1); /* && _syscallErrno == EINTR && !_wasSignaled */ \
    if (_rc == -1) { \
        /* If the syscall failed, re-set errno: throwing an exception might have modified it. */ \
        errno = _syscallErrno; \
    } \
    _rc; })

CCM_INTERFACE_IMPL_1(Linux, SyncObject, IOs);

ECode Linux::Accept(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ISocketAddress* peerAddress,
    /* [out] */ IFileDescriptor** retFd)
{
    return NOERROR;
}

ECode Linux::Access(
    /* [in] */ const String& path,
    /* [in] */ Integer mode,
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode Linux::Bind(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetAddress* address,
    /* [in] */ Integer port)
{
    return NOERROR;
}

ECode Linux::Bind(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ISocketAddress* address)
{
    return NOERROR;
}

ECode Linux::Capget(
    /* [in] */ IStructCapUserHeader* hdr,
    /* [out, callee] */ Array<IStructCapUserData*>* data)
{
    return NOERROR;
}

ECode Linux::Capset(
    /* [in] */ IStructCapUserHeader* hdr,
    /* [in] */ const Array<IStructCapUserData*>& data)
{
    return NOERROR;
}

ECode Linux::Chmod(
    /* [in] */ const String& path,
    /* [in] */ Integer mode)
{
    return NOERROR;
}

ECode Linux::Chown(
    /* [in] */ const String& path,
    /* [in] */ Integer uid,
    /* [in] */ Integer gid)
{
    return NOERROR;
}

ECode Linux::Close(
    /* [in] */ IFileDescriptor* fd)
{
    return NOERROR;
}

ECode Linux::Connect(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetAddress* address,
    /* [in] */ Integer port)
{
    return NOERROR;
}

ECode Linux::Connect(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ISocketAddress* address)
{
    return NOERROR;
}

ECode Linux::Dup(
    /* [in] */ IFileDescriptor* oldFd,
    /* [out] */ IFileDescriptor** retFd)
{
    return NOERROR;
}

ECode Linux::Dup2(
    /* [in] */ IFileDescriptor* oldFd,
    /* [in] */ Integer newFd,
    /* [out] */ IFileDescriptor** retFd)
{
    return NOERROR;
}

ECode Linux::Environ(
    /* [out, callee] */ Array<String>* env)
{
    return NOERROR;
}

ECode Linux::Execv(
    /* [in] */ const String& filename,
    /* [in] */ const Array<String>& argv)
{
    return NOERROR;
}

ECode Linux::Execve(
    /* [in] */ const String& filename,
    /* [in] */ const Array<String>& argv,
    /* [in] */ const Array<String>& envp)
{
    return NOERROR;
}

ECode Linux::Fchmod(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer mode)
{
    return NOERROR;
}

ECode Linux::Fchown(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer uid,
    /* [in] */ Integer gid)
{
    return NOERROR;
}

ECode Linux::FcntlFlock(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer cmd,
    /* [in] */ IStructFlock* arg,
    /* [out] */ Integer* result)
{
    return NOERROR;
}

ECode Linux::FcntlInt(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer cmd,
    /* [in] */ Integer arg,
    /* [out] */ Integer* result)
{
    return NOERROR;
}

ECode Linux::FcntlVoid(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer cmd,
    /* [out] */ Integer* result)
{
    return NOERROR;
}

ECode Linux::Fdatasync(
    /* [in] */ IFileDescriptor* fd)
{
    return NOERROR;
}

ECode Linux::Fstat(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ IStructStat** stat)
{
    return NOERROR;
}

ECode Linux::Fstatvfs(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ IStructStatVfs** statVfs)
{
    return NOERROR;
}

ECode Linux::Fsync(
    /* [in] */ IFileDescriptor* fd)
{
    return NOERROR;
}

ECode Linux::Ftruncate(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Long length)
{
    return NOERROR;
}

ECode Linux::Gai_Strerror(
    /* [in] */ Integer error,
    /* [out] */ String* strerror)
{
    return NOERROR;
}

ECode Linux::Getegid(
    /* [out] */ Integer* egid)
{
    return NOERROR;
}

ECode Linux::Geteuid(
    /* [out] */ Integer* euid)
{
    return NOERROR;
}

ECode Linux::Getgid(
    /* [out] */ Integer* gid)
{
    return NOERROR;
}

ECode Linux::Getenv(
    /* [in] */ const String& name,
    /* [out] */ String* value)
{
    return NOERROR;
}

ECode Linux::Getnameinfo(
    /* [in] */ IInetAddress* address,
    /* [in] */ Integer flags,
    /* [out] */ String* info)
{
    return NOERROR;
}

ECode Linux::Getpeername(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ ISocketAddress** name)
{
    return NOERROR;
}

ECode Linux::Getpgid(
    /* [in] */ Integer pid,
    /* [out] */ Integer* pgid)
{
    return NOERROR;
}

ECode Linux::Getpid(
    /* [out] */ Integer* pid)
{
    return NOERROR;
}

ECode Linux::Getppid(
    /* [out] */ Integer* ppid)
{
    return NOERROR;
}

ECode Linux::Getpwnam(
    /* [in] */ const String& name,
    /* [out] */ IStructPasswd** pwnam)
{
    return NOERROR;
}

ECode Linux::Getpwuid(
    /* [in] */ Integer uid,
    /* [out] */ IStructPasswd** pwuid)
{
    return NOERROR;
}

ECode Linux::Getsockname(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ ISocketAddress** socket)
{
    return NOERROR;
}

ECode Linux::GetsockoptByte(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [out] */ Integer* sockopt)
{
    return NOERROR;
}

ECode Linux::GetsockoptInAddr(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [out] */ IInetAddress** addr)
{
    return NOERROR;
}

ECode Linux::GetsockoptInt(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [out] */ Integer* sockopt)
{
    return NOERROR;
}

ECode Linux::GetsockoptLinger(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [out] */ IStructLinger** linger)
{
    return NOERROR;
}

ECode Linux::GetsockoptTimeval(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [out] */ IStructTimeval** timeval)
{
    return NOERROR;
}

ECode Linux::GetsockoptUcred(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [out] */ IStructUcred** ucred)
{
    return NOERROR;
}

ECode Linux::Gettid(
    /* [out] */ Integer* tid)
{
    return NOERROR;
}

ECode Linux::Getuid(
    /* [out] */ Integer* uid)
{
    return NOERROR;
}

ECode Linux::Getxattr(
    /* [in] */ const String& path,
    /* [in] */ const String& name,
    /* [out, callee] */ Array<Byte>* attr)
{
    return NOERROR;
}

ECode Linux::Getifaddrs(
    /* [out, callee] */ Array<IStructIfaddrs*>* addrs)
{
    return NOERROR;
}

ECode Linux::If_indextoname(
    /* [in] */ Integer index,
    /* [out] */ String* name)
{
    return NOERROR;
}

ECode Linux::If_nametoindex(
    /* [in] */ const String& name,
    /* [out] */ Integer* index)
{
    return NOERROR;
}

ECode Linux::Inet_pton(
    /* [in] */ Integer family,
    /* [in] */ const String& address,
    /* [out] */ IInetAddress** addr)
{
    return NOERROR;
}

ECode Linux::IoctlInetAddress(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer cmd,
    /* [in] */ const String& interfaceName,
    /* [out] */ IInetAddress** addr)
{
    return NOERROR;
}

ECode Linux::IoctlInt(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer cmd,
    /* [in, out] */ Integer* arg,
    /* [out] */ Integer* result)
{
    return NOERROR;
}

ECode Linux::IoctlMTU(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ const String& interfaceName,
    /* [out] */ Integer* mtu)
{
    return NOERROR;
}

ECode Linux::Isatty(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Boolean* isatty)
{
    return NOERROR;
}

ECode Linux::Kill(
    /* [in] */ Integer pid,
    /* [in] */ Integer signal)
{
    return NOERROR;
}

ECode Linux::Lchown(
    /* [in] */ const String& path,
    /* [in] */ Integer uid,
    /* [in] */ Integer gid)
{
    return NOERROR;
}

ECode Linux::Link(
    /* [in] */ const String& oldPath,
    /* [in] */ const String& newPath)
{
    return NOERROR;
}

ECode Linux::Listen(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer backlog)
{
    return NOERROR;
}

ECode Linux::Listxattr(
    /* [in] */ const String& path,
    /* [out, callee] */ Array<String>* attr)
{
    return NOERROR;
}

ECode Linux::Lseek(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Long offset,
    /* [in] */ Integer whence,
    /* [out] */ Long* result)
{
    return NOERROR;
}

ECode Linux::Lstat(
    /* [in] */ const String& path,
    /* [out] */ IStructStat** stat)
{
    return NOERROR;
}

ECode Linux::Mincore(
    /* [in] */ Long address,
    /* [in] */ Long byteCount,
    /* [in] */ const Array<Byte>& vector)
{
    return NOERROR;
}

ECode Linux::Mkdir(
    /* [in] */ const String& path,
    /* [in] */ Integer mode)
{
    return NOERROR;
}

ECode Linux::Mkfifo(
    /* [in] */ const String& path,
    /* [in] */ Integer mode)
{
    return NOERROR;
}

ECode Linux::Mlock(
    /* [in] */ Long address,
    /* [in] */ Long byteCount)
{
    return NOERROR;
}

ECode Linux::Mmap(
    /* [in] */ Long address,
    /* [in] */ Long byteCount,
    /* [in] */ Integer prot,
    /* [in] */ Integer flags,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Long offset,
    /* [out] */ Long* result)
{
    return NOERROR;
}

ECode Linux::Msync(
    /* [in] */ Long address,
    /* [in] */ Long byteCount,
    /* [in] */ Integer flags)
{
    return NOERROR;
}

ECode Linux::Munlock(
    /* [in] */ Long address,
    /* [in] */ Long byteCount)
{
    return NOERROR;
}

ECode Linux::Munmap(
    /* [in] */ Long address,
    /* [in] */ Long byteCount)
{
    return NOERROR;
}

ECode Linux::Open(
    /* [in] */ const String& path,
    /* [in] */ Integer flags,
    /* [in] */ Integer mode,
    /* [out] */ IFileDescriptor** fd)
{
    return NOERROR;
}

ECode Linux::Pipe2(
    /* [in] */ Integer flags,
    /* [out, callee] */ Array<IFileDescriptor*>* fds)
{
    return NOERROR;
}

ECode Linux::Pisces_Getaddrinfo(
    /* [in] */ const String& node,
    /* [in] */ IStructAddrinfo* hints,
    /* [in] */ Integer netId,
    /* [out, callee] */ Array<IInetAddress*>* infos)
{
    return NOERROR;
}

ECode Linux::Poll(
    /* [in] */ const Array<IStructPollfd*>& fds,
    /* [in] */ Integer timeoutMs,
    /* [out] */ Integer* result)
{
    return NOERROR;
}

ECode Linux::Posix_fallocate(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Long offset,
    /* [in] */ Long length)
{
    return NOERROR;
}

ECode Linux::Prctl(
    /* [in] */ Integer option,
    /* [in] */ Long arg2,
    /* [in] */ Long arg3,
    /* [in] */ Long arg4,
    /* [in] */ Long arg5,
    /* [out] */ Integer* prctl)
{
    return NOERROR;
}

ECode Linux::Pread(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Array<Byte>& bytes,
    /* [in] */ Integer byteOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ Long offset,
    /* [out] */ Integer* num)
{
    return NOERROR;
}

ECode Linux::Pread(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Long offset,
    /* [out] */ Integer* num)
{
    return NOERROR;
}

ECode Linux::Pwrite(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ const Array<Byte>& bytes,
    /* [in] */ Integer byteOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ Long offset,
    /* [out] */ Integer* num)
{
    return NOERROR;
}

ECode Linux::Pwrite(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Long offset,
    /* [out] */ Integer* num)
{
    return NOERROR;
}

ECode Linux::Read(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Array<Byte>& bytes,
    /* [in] */ Integer byteOffset,
    /* [in] */ Integer byteCount,
    /* [out] */ Integer* num)
{
    VALIDATE_NOT_NULL(num);

    ECode ec;
    *num = IO_FAILURE_RETRY(&ec, ssize_t, read, fd, bytes.GetPayload() + byteOffset, byteCount);
    return ec;
}

ECode Linux::Read(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ Integer* num)
{
    return NOERROR;
}

ECode Linux::Readlink(
    /* [in] */ const String& path,
    /* [out] */ String* link)
{
    return NOERROR;
}

ECode Linux::Realpath(
    /* [in] */ const String& path,
    /* [out] */ String* realpath)
{
    return NOERROR;
}

ECode Linux::Readv(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Array<IInterface*>& buffers,
    /* [out] */ Array<Integer>& offsets,
    /* [out] */ Array<Integer>& byteCounts,
    /* [out] */ Integer* num)
{
    return NOERROR;
}

ECode Linux::Recvfrom(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Array<Byte>& bytes,
    /* [in] */ Integer byteOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ Integer flags,
    /* [in] */ IInetSocketAddress* srcAddress,
    /* [out] */ Integer* num)
{
    return NOERROR;
}

ECode Linux::Recvfrom(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Integer flags,
    /* [in] */ IInetSocketAddress* srcAddress,
    /* [out] */ Integer* num)
{
    return NOERROR;
}

ECode Linux::Remove(
    /* [in] */ const String& path)
{
    return NOERROR;
}

ECode Linux::Removexattr(
    /* [in] */ const String& path,
    /* [in] */ const String& name)
{
    return NOERROR;
}

ECode Linux::Rename(
    /* [in] */ const String& oldPath,
    /* [in] */ const String& newPath)
{
    return NOERROR;
}

ECode Linux::Sendto(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ const Array<Byte>& bytes,
    /* [in] */ Integer byteOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ Integer flags,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Integer port,
    /* [out] */ Integer* result)
{
    return NOERROR;
}

ECode Linux::Sendto(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ const Array<Byte>& bytes,
    /* [in] */ Integer byteOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ Integer flags,
    /* [in] */ ISocketAddress* address,
    /* [out] */ Integer* result)
{
    return NOERROR;
}

ECode Linux::Sendto(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Integer flags,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Integer port,
    /* [out] */ Integer* result)
{
    return NOERROR;
}

ECode Linux::Sendfile(
    /* [in] */ IFileDescriptor* outFd,
    /* [in] */ IFileDescriptor* inFd,
    /* [in, out] */ Long* inOffset,
    /* [in] */ Long byteCount,
    /* [out] */ Long* result)
{
    return NOERROR;
}

ECode Linux::Setegid(
    /* [in] */ Integer egid)
{
    return NOERROR;
}

ECode Linux::Setenv(
    /* [in] */ const String& name,
    /* [in] */ const String& value,
    /* [in] */ Boolean overwrite)
{
    return NOERROR;
}

ECode Linux::Seteuid(
    /* [in] */ Integer euid)
{
    return NOERROR;
}

ECode Linux::Setgid(
    /* [in] */ Integer gid)
{
    return NOERROR;
}

ECode Linux::Setpgid(
    /* [in] */ Integer pid,
    /* [in] */ Integer pgid)
{
    return NOERROR;
}

ECode Linux::Setregid(
    /* [in] */ Integer rgid,
    /* [in] */ Integer egid)
{
    return NOERROR;
}

ECode Linux::Setreuid(
    /* [in] */ Integer ruid,
    /* [in] */ Integer euid)
{
    return NOERROR;
}

ECode Linux::Setsid(
    /* [out] */ Integer* sid)
{
    return NOERROR;
}

ECode Linux::SetsockoptByte(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [in] */ Integer value)
{
    return NOERROR;
}

ECode Linux::SetsockoptIfreq(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [in] */ const String& value)
{
    return NOERROR;
}

ECode Linux::SetsockoptInt(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [in] */ Integer value)
{
    return NOERROR;
}

ECode Linux::SetsockoptIpMreqn(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [in] */ Integer value)
{
    return NOERROR;
}

ECode Linux::SetsockoptGroupReq(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [in] */ IStructGroupReq* value)
{
    return NOERROR;
}

ECode Linux::SetsockoptGroupSourceReq(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [in] */ IStructGroupSourceReq* value)
{
    return NOERROR;
}

ECode Linux::SetsockoptLinger(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [in] */ IStructLinger* value)
{
    return NOERROR;
}

ECode Linux::SetsockoptTimeval(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [in] */ IStructTimeval* value)
{
    return NOERROR;
}

ECode Linux::Setuid(
    /* [in] */ Integer uid)
{
    return NOERROR;
}

ECode Linux::Setxattr(
    /* [in] */ const String& path,
    /* [in] */ const String& name,
    /* [in] */ const Array<Byte>& value,
    /* [in] */ Integer flags)
{
    return NOERROR;
}

ECode Linux::Shutdown(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer how)
{
    return NOERROR;
}

ECode Linux::Socket(
    /* [in] */ Integer socketDomain,
    /* [in] */ Integer type,
    /* [in] */ Integer protocol,
    /* [out] */ IFileDescriptor** fd)
{
    return NOERROR;
}

ECode Linux::Socketpair(
    /* [in] */ Integer socketDomain,
    /* [in] */ Integer type,
    /* [in] */ Integer protocol,
    /* [in] */ IFileDescriptor* fd1,
    /* [in] */ IFileDescriptor* fd2)
{
    return NOERROR;
}

ECode Linux::Stat(
    /* [in] */ const String& path,
    /* [out] */ IStructStat** stat)
{
    return NOERROR;
}

ECode Linux::StatVfs(
    /* [in] */ const String& path,
    /* [out] */ IStructStatVfs** statfs)
{
    return NOERROR;
}

ECode Linux::Strerror(
    /* [in] */ Integer errnum,
    /* [out] */ String* strerr)
{
    return NOERROR;
}

ECode Linux::Strsignal(
    /* [in] */ Integer signal,
    /* [out] */ String* strSignal)
{
    return NOERROR;
}

ECode Linux::Symlink(
    /* [in] */ const String& oldPath,
    /* [in] */ const String& newPath)
{
    return NOERROR;
}

ECode Linux::Sysconf(
    /* [in] */ Integer name,
    /* [out] */ Long* result)
{
    return NOERROR;
}

ECode Linux::Tcdrain(
    /* [in] */ IFileDescriptor* fd)
{
    return NOERROR;
}

ECode Linux::Tcsendbreak(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer duration)
{
    return NOERROR;
}

ECode Linux::Umask(
    /* [in] */ Integer mask,
    /* [out] */ Integer* result)
{
    return NOERROR;
}

ECode Linux::Uname(
    /* [out] */ IStructUtsname** uname)
{
    return NOERROR;
}

ECode Linux::Unlink(
    /* [in] */ const String& pathname)
{
    return NOERROR;
}

ECode Linux::Unsetenv(
    /* [in] */ const String& name)
{
    return NOERROR;
}

ECode Linux::Waitpid(
    /* [in] */ Integer pid,
    /* [in, out] */ Integer* status,
    /* [in] */ Integer options,
    /* [out] */ Integer* result)
{
    return NOERROR;
}

ECode Linux::Write(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ const Array<Byte>& bytes,
    /* [in] */ Integer byteOffset,
    /* [in] */ Integer byteCount,
    /* [out] */ Integer* num)
{
    return NOERROR;
}

ECode Linux::Write(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ Integer* num)
{
    return NOERROR;
}

ECode Linux::Writev(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ const Array<IInterface*>& buffers,
    /* [in] */ const Array<Integer>& offsets,
    /* [in] */ const Array<Integer>& byteCounts,
    /* [out] */ Integer* result)
{
    return NOERROR;
}

}
}
