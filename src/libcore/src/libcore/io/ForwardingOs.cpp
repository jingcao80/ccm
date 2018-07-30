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

#include "libcore/io/ForwardingOs.h"

namespace libcore {
namespace io {

CCM_INTERFACE_IMPL_1(ForwardingOs, SyncObject, IOs);

ECode ForwardingOs::Accept(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ISocketAddress* peerAddress,
    /* [out] */ IFileDescriptor** retFd)
{
    return mOs->Accept(fd, peerAddress, retFd);
}

ECode ForwardingOs::Access(
    /* [in] */ const String& path,
    /* [in] */ Integer mode,
    /* [out] */ Boolean* result)
{
    return mOs->Access(path, mode, result);
}

ECode ForwardingOs::Bind(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetAddress* address,
    /* [in] */ Integer port)
{
    return mOs->Bind(fd, address, port);
}

ECode ForwardingOs::Bind(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ISocketAddress* address)
{
    return mOs->Bind(fd, address);
}

ECode ForwardingOs::Capget(
    /* [in] */ IStructCapUserHeader* hdr,
    /* [out, callee] */ Array<IStructCapUserData*>* data)
{
    return mOs->Capget(hdr, data);
}

ECode ForwardingOs::Capset(
    /* [in] */ IStructCapUserHeader* hdr,
    /* [in] */ const Array<IStructCapUserData*>& data)
{
    return mOs->Capset(hdr, data);
}

ECode ForwardingOs::Chmod(
    /* [in] */ const String& path,
    /* [in] */ Integer mode)
{
    return mOs->Chmod(path, mode);
}

ECode ForwardingOs::Chown(
    /* [in] */ const String& path,
    /* [in] */ Integer uid,
    /* [in] */ Integer gid)
{
    return mOs->Chown(path, uid, gid);
}

ECode ForwardingOs::Close(
    /* [in] */ IFileDescriptor* fd)
{
    return mOs->Close(fd);
}

ECode ForwardingOs::Connect(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IInetAddress* address,
    /* [in] */ Integer port)
{
    return mOs->Connect(fd, address, port);
}

ECode ForwardingOs::Connect(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ISocketAddress* address)
{
    return mOs->Connect(fd, address);
}

ECode ForwardingOs::Dup(
    /* [in] */ IFileDescriptor* oldFd,
    /* [out] */ IFileDescriptor** retFd)
{
    return mOs->Dup(oldFd, retFd);
}

ECode ForwardingOs::Dup2(
    /* [in] */ IFileDescriptor* oldFd,
    /* [in] */ Integer newFd,
    /* [out] */ IFileDescriptor** retFd)
{
    return mOs->Dup2(oldFd, newFd, retFd);
}

ECode ForwardingOs::Environ(
    /* [out, callee] */ Array<String>* env)
{
    return mOs->Environ(env);
}

ECode ForwardingOs::Execv(
    /* [in] */ const String& filename,
    /* [in] */ const Array<String>& argv)
{
    return mOs->Execv(filename, argv);
}

ECode ForwardingOs::Execve(
    /* [in] */ const String& filename,
    /* [in] */ const Array<String>& argv,
    /* [in] */ const Array<String>& envp)
{
    return mOs->Execve(filename, argv, envp);
}

ECode ForwardingOs::Fchmod(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer mode)
{
    return mOs->Fchmod(fd, mode);
}

ECode ForwardingOs::Fchown(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer uid,
    /* [in] */ Integer gid)
{
    return mOs->Fchown(fd, uid, gid);
}

ECode ForwardingOs::FcntlFlock(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer cmd,
    /* [in] */ IStructFlock* arg,
    /* [out] */ Integer* result)
{
    return mOs->FcntlFlock(fd, cmd, arg, result);
}

ECode ForwardingOs::FcntlInt(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer cmd,
    /* [in] */ Integer arg,
    /* [out] */ Integer* result)
{
    return mOs->FcntlInt(fd, cmd, arg, result);
}

ECode ForwardingOs::FcntlVoid(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer cmd,
    /* [out] */ Integer* result)
{
    return mOs->FcntlVoid(fd, cmd, result);
}

ECode ForwardingOs::Fdatasync(
    /* [in] */ IFileDescriptor* fd)
{
    return mOs->Fdatasync(fd);
}

ECode ForwardingOs::Fstat(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ IStructStat** stat)
{
    return mOs->Fstat(fd, stat);
}

ECode ForwardingOs::Fstatvfs(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ IStructStatVfs** statVfs)
{
    return mOs->Fstatvfs(fd, statVfs);
}

ECode ForwardingOs::Fsync(
    /* [in] */ IFileDescriptor* fd)
{
    return mOs->Fsync(fd);
}

ECode ForwardingOs::Ftruncate(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Long length)
{
    return mOs->Ftruncate(fd, length);
}

ECode ForwardingOs::Gai_Strerror(
    /* [in] */ Integer error,
    /* [out] */ String* strerror)
{
    return mOs->Gai_Strerror(error, strerror);
}

ECode ForwardingOs::Getegid(
    /* [out] */ Integer* egid)
{
    return mOs->Getegid(egid);
}

ECode ForwardingOs::Geteuid(
    /* [out] */ Integer* euid)
{
    return mOs->Geteuid(euid);
}

ECode ForwardingOs::Getgid(
    /* [out] */ Integer* gid)
{
    return mOs->Getgid(gid);
}

ECode ForwardingOs::Getenv(
    /* [in] */ const String& name,
    /* [out] */ String* value)
{
    return mOs->Getenv(name, value);
}

ECode ForwardingOs::Getnameinfo(
    /* [in] */ IInetAddress* address,
    /* [in] */ Integer flags,
    /* [out] */ String* info)
{
    return mOs->Getnameinfo(address, flags, info);
}

ECode ForwardingOs::Getpeername(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ ISocketAddress** name)
{
    return mOs->Getpeername(fd, name);
}

ECode ForwardingOs::Getpgid(
    /* [in] */ Integer pid,
    /* [out] */ Integer* pgid)
{
    return mOs->Getpgid(pid, pgid);
}

ECode ForwardingOs::Getpid(
    /* [out] */ Integer* pid)
{
    return mOs->Getpid(pid);
}

ECode ForwardingOs::Getppid(
    /* [out] */ Integer* ppid)
{
    return mOs->Getppid(ppid);
}

ECode ForwardingOs::Getpwnam(
    /* [in] */ const String& name,
    /* [out] */ IStructPasswd** pwnam)
{
    return mOs->Getpwnam(name, pwnam);
}

ECode ForwardingOs::Getpwuid(
    /* [in] */ Integer uid,
    /* [out] */ IStructPasswd** pwuid)
{
    return mOs->Getpwuid(uid, pwuid);
}

ECode ForwardingOs::Getsockname(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ ISocketAddress** socket)
{
    return mOs->Getsockname(fd, socket);
}

ECode ForwardingOs::GetsockoptByte(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [out] */ Integer* sockopt)
{
    return mOs->GetsockoptByte(fd, level, option, sockopt);
}

ECode ForwardingOs::GetsockoptInAddr(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [out] */ IInetAddress** addr)
{
    return mOs->GetsockoptInAddr(fd, level, option, addr);
}

ECode ForwardingOs::GetsockoptInt(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [out] */ Integer* sockopt)
{
    return mOs->GetsockoptInt(fd, level, option, sockopt);
}

ECode ForwardingOs::GetsockoptLinger(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [out] */ IStructLinger** linger)
{
    return mOs->GetsockoptLinger(fd, level, option, linger);
}

ECode ForwardingOs::GetsockoptTimeval(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [out] */ IStructTimeval** timeval)
{
    return mOs->GetsockoptTimeval(fd, level, option, timeval);
}

ECode ForwardingOs::GetsockoptUcred(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [out] */ IStructUcred** ucred)
{
    return mOs->GetsockoptUcred(fd, level, option, ucred);
}

ECode ForwardingOs::Gettid(
    /* [out] */ Integer* tid)
{
    return mOs->Gettid(tid);
}

ECode ForwardingOs::Getuid(
    /* [out] */ Integer* uid)
{
    return mOs->Getuid(uid);
}

ECode ForwardingOs::Getxattr(
    /* [in] */ const String& path,
    /* [in] */ const String& name,
    /* [out, callee] */ Array<Byte>* attr)
{
    return mOs->Getxattr(path, name, attr);
}

ECode ForwardingOs::Getifaddrs(
    /* [out, callee] */ Array<IStructIfaddrs*>* addrs)
{
    return mOs->Getifaddrs(addrs);
}

ECode ForwardingOs::If_indextoname(
    /* [in] */ Integer index,
    /* [out] */ String* name)
{
    return mOs->If_indextoname(index, name);
}

ECode ForwardingOs::If_nametoindex(
    /* [in] */ const String& name,
    /* [out] */ Integer* index)
{
    return mOs->If_nametoindex(name, index);
}

ECode ForwardingOs::Inet_pton(
    /* [in] */ Integer family,
    /* [in] */ const String& address,
    /* [out] */ IInetAddress** addr)
{
    return mOs->Inet_pton(family, address, addr);
}

ECode ForwardingOs::IoctlInetAddress(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer cmd,
    /* [in] */ const String& interfaceName,
    /* [out] */ IInetAddress** addr)
{
    return mOs->IoctlInetAddress(fd, cmd, interfaceName, addr);
}

ECode ForwardingOs::IoctlInt(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer cmd,
    /* [in, out] */ Integer* arg,
    /* [out] */ Integer* result)
{
    return mOs->IoctlInt(fd, cmd, arg, result);
}

ECode ForwardingOs::IoctlMTU(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ const String& interfaceName,
    /* [out] */ Integer* mtu)
{
    return mOs->IoctlMTU(fd, interfaceName, mtu);
}

ECode ForwardingOs::Isatty(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Boolean* isatty)
{
    return mOs->Isatty(fd, isatty);
}

ECode ForwardingOs::Kill(
    /* [in] */ Integer pid,
    /* [in] */ Integer signal)
{
    return mOs->Kill(pid, signal);
}

ECode ForwardingOs::Lchown(
    /* [in] */ const String& path,
    /* [in] */ Integer uid,
    /* [in] */ Integer gid)
{
    return mOs->Lchown(path, uid, gid);
}

ECode ForwardingOs::Link(
    /* [in] */ const String& oldPath,
    /* [in] */ const String& newPath)
{
    return mOs->Link(oldPath, newPath);
}

ECode ForwardingOs::Listen(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer backlog)
{
    return mOs->Listen(fd, backlog);
}

ECode ForwardingOs::Listxattr(
    /* [in] */ const String& path,
    /* [out, callee] */ Array<String>* attr)
{
    return mOs->Listxattr(path, attr);
}

ECode ForwardingOs::Lseek(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Long offset,
    /* [in] */ Integer whence,
    /* [out] */ Long* result)
{
    return mOs->Lseek(fd, offset, whence, result);
}

ECode ForwardingOs::Lstat(
    /* [in] */ const String& path,
    /* [out] */ IStructStat** stat)
{
    return mOs->Lstat(path, stat);
}

ECode ForwardingOs::Mincore(
    /* [in] */ Long address,
    /* [in] */ Long byteCount,
    /* [in] */ const Array<Byte>& vector)
{
    return mOs->Mincore(address, byteCount, vector);
}

ECode ForwardingOs::Mkdir(
    /* [in] */ const String& path,
    /* [in] */ Integer mode)
{
    return mOs->Mkdir(path, mode);
}

ECode ForwardingOs::Mkfifo(
    /* [in] */ const String& path,
    /* [in] */ Integer mode)
{
    return mOs->Mkfifo(path, mode);
}

ECode ForwardingOs::Mlock(
    /* [in] */ Long address,
    /* [in] */ Long byteCount)
{
    return mOs->Mlock(address, byteCount);
}

ECode ForwardingOs::Mmap(
    /* [in] */ Long address,
    /* [in] */ Long byteCount,
    /* [in] */ Integer prot,
    /* [in] */ Integer flags,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Long offset,
    /* [out] */ Long* result)
{
    return mOs->Mmap(address, byteCount, prot, flags, fd, offset, result);
}

ECode ForwardingOs::Msync(
    /* [in] */ Long address,
    /* [in] */ Long byteCount,
    /* [in] */ Integer flags)
{
    return mOs->Msync(address, byteCount, flags);
}

ECode ForwardingOs::Munlock(
    /* [in] */ Long address,
    /* [in] */ Long byteCount)
{
    return mOs->Munlock(address, byteCount);
}

ECode ForwardingOs::Munmap(
    /* [in] */ Long address,
    /* [in] */ Long byteCount)
{
    return mOs->Munmap(address, byteCount);
}

ECode ForwardingOs::Open(
    /* [in] */ const String& path,
    /* [in] */ Integer flags,
    /* [in] */ Integer mode,
    /* [out] */ IFileDescriptor** fd)
{
    return mOs->Open(path, flags, mode, fd);
}

ECode ForwardingOs::Pipe2(
    /* [in] */ Integer flags,
    /* [out, callee] */ Array<IFileDescriptor*>* fds)
{
    return mOs->Pipe2(flags, fds);
}

ECode ForwardingOs::Pisces_Getaddrinfo(
    /* [in] */ const String& node,
    /* [in] */ IStructAddrinfo* hints,
    /* [in] */ Integer netId,
    /* [out, callee] */ Array<IInetAddress*>* infos)
{
    return mOs->Pisces_Getaddrinfo(node, hints, netId, infos);
}

ECode ForwardingOs::Poll(
    /* [in] */ const Array<IStructPollfd*>& fds,
    /* [in] */ Integer timeoutMs,
    /* [out] */ Integer* result)
{
    return mOs->Poll(fds, timeoutMs, result);
}

ECode ForwardingOs::Posix_fallocate(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Long offset,
    /* [in] */ Long length)
{
    return mOs->Posix_fallocate(fd, offset, length);
}

ECode ForwardingOs::Prctl(
    /* [in] */ Integer option,
    /* [in] */ Long arg2,
    /* [in] */ Long arg3,
    /* [in] */ Long arg4,
    /* [in] */ Long arg5,
    /* [out] */ Integer* prctl)
{
    return mOs->Prctl(option, arg2, arg3, arg4, arg5, prctl);
}

ECode ForwardingOs::Pread(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Array<Byte>& bytes,
    /* [in] */ Integer byteOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ Long offset,
    /* [out] */ Integer* num)
{
    return mOs->Pread(fd, bytes, byteOffset, byteCount, offset, num);
}

ECode ForwardingOs::Pread(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Long offset,
    /* [out] */ Integer* num)
{
    return mOs->Pread(fd, buffer, offset, num);
}

ECode ForwardingOs::Pwrite(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ const Array<Byte>& bytes,
    /* [in] */ Integer byteOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ Long offset,
    /* [out] */ Integer* num)
{
    return mOs->Pwrite(fd, bytes, byteOffset, byteCount, offset, num);
}

ECode ForwardingOs::Pwrite(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Long offset,
    /* [out] */ Integer* num)
{
    return mOs->Pwrite(fd, buffer, offset, num);
}

ECode ForwardingOs::Read(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Array<Byte>& bytes,
    /* [in] */ Integer byteOffset,
    /* [in] */ Integer byteCount,
    /* [out] */ Integer* num)
{
    return mOs->Read(fd, bytes, byteOffset, byteCount, num);
}

ECode ForwardingOs::Read(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ Integer* num)
{
    return mOs->Read(fd, buffer, num);
}

ECode ForwardingOs::Readlink(
    /* [in] */ const String& path,
    /* [out] */ String* link)
{
    return mOs->Readlink(path, link);
}

ECode ForwardingOs::Realpath(
    /* [in] */ const String& path,
    /* [out] */ String* realpath)
{
    return mOs->Realpath(path, realpath);
}

ECode ForwardingOs::Readv(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Array<IInterface*>& buffers,
    /* [out] */ Array<Integer>& offsets,
    /* [out] */ Array<Integer>& byteCounts,
    /* [out] */ Integer* num)
{
    return mOs->Readv(fd, buffers, offsets, byteCounts, num);
}

ECode ForwardingOs::Recvfrom(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Array<Byte>& bytes,
    /* [in] */ Integer byteOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ Integer flags,
    /* [in] */ IInetSocketAddress* srcAddress,
    /* [out] */ Integer* num)
{
    return mOs->Recvfrom(fd, bytes, byteOffset, byteCount, flags, srcAddress, num);
}

ECode ForwardingOs::Recvfrom(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Integer flags,
    /* [in] */ IInetSocketAddress* srcAddress,
    /* [out] */ Integer* num)
{
    return mOs->Recvfrom(fd, buffer, flags, srcAddress, num);
}

ECode ForwardingOs::Remove(
    /* [in] */ const String& path)
{
    return mOs->Remove(path);
}

ECode ForwardingOs::Removexattr(
    /* [in] */ const String& path,
    /* [in] */ const String& name)
{
    return mOs->Removexattr(path, name);
}

ECode ForwardingOs::Rename(
    /* [in] */ const String& oldPath,
    /* [in] */ const String& newPath)
{
    return mOs->Rename(oldPath, newPath);
}

ECode ForwardingOs::Sendfile(
    /* [in] */ IFileDescriptor* outFd,
    /* [in] */ IFileDescriptor* inFd,
    /* [in, out] */ Long* inOffset,
    /* [in] */ Long byteCount,
    /* [out] */ Long* result)
{
    return mOs->Sendfile(outFd, inFd, inOffset, byteCount, result);
}

ECode ForwardingOs::Sendto(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ const Array<Byte>& bytes,
    /* [in] */ Integer byteOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ Integer flags,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Integer port,
    /* [out] */ Integer* result)
{
    return mOs->Sendto(fd, bytes, byteOffset, byteCount, flags, inetAddress, port, result);
}

ECode ForwardingOs::Sendto(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ const Array<Byte>& bytes,
    /* [in] */ Integer byteOffset,
    /* [in] */ Integer byteCount,
    /* [in] */ Integer flags,
    /* [in] */ ISocketAddress* address,
    /* [out] */ Integer* result)
{
    return mOs->Sendto(fd, bytes, byteOffset, byteCount, flags, address, result);
}

ECode ForwardingOs::Sendto(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Integer flags,
    /* [in] */ IInetAddress* inetAddress,
    /* [in] */ Integer port,
    /* [out] */ Integer* result)
{
    return mOs->Sendto(fd, buffer, flags, inetAddress, port, result);
}

ECode ForwardingOs::Setegid(
    /* [in] */ Integer egid)
{
    return mOs->Setegid(egid);
}

ECode ForwardingOs::Setenv(
    /* [in] */ const String& name,
    /* [in] */ const String& value,
    /* [in] */ Boolean overwrite)
{
    return mOs->Setenv(name, value, overwrite);
}

ECode ForwardingOs::Seteuid(
    /* [in] */ Integer euid)
{
    return mOs->Seteuid(euid);
}

ECode ForwardingOs::Setgid(
    /* [in] */ Integer gid)
{
    return mOs->Setgid(gid);
}

ECode ForwardingOs::Setpgid(
    /* [in] */ Integer pid,
    /* [in] */ Integer pgid)
{
    return mOs->Setpgid(pid, pgid);
}

ECode ForwardingOs::Setregid(
    /* [in] */ Integer rgid,
    /* [in] */ Integer egid)
{
    return mOs->Setregid(rgid, egid);
}

ECode ForwardingOs::Setreuid(
    /* [in] */ Integer ruid,
    /* [in] */ Integer euid)
{
    return mOs->Setreuid(ruid, euid);
}

ECode ForwardingOs::Setsid(
    /* [out] */ Integer* sid)
{
    return mOs->Setsid(sid);
}

ECode ForwardingOs::SetsockoptByte(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [in] */ Integer value)
{
    return mOs->SetsockoptByte(fd, level, option, value);
}

ECode ForwardingOs::SetsockoptIfreq(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [in] */ const String& value)
{
    return mOs->SetsockoptIfreq(fd, level, option, value);
}

ECode ForwardingOs::SetsockoptInt(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [in] */ Integer value)
{
    return mOs->SetsockoptInt(fd, level, option, value);
}

ECode ForwardingOs::SetsockoptIpMreqn(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [in] */ Integer value)
{
    return mOs->SetsockoptIpMreqn(fd, level, option, value);
}

ECode ForwardingOs::SetsockoptGroupReq(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [in] */ IStructGroupReq* value)
{
    return mOs->SetsockoptGroupReq(fd, level, option, value);
}

ECode ForwardingOs::SetsockoptGroupSourceReq(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [in] */ IStructGroupSourceReq* value)
{
    return mOs->SetsockoptGroupSourceReq(fd, level, option, value);
}

ECode ForwardingOs::SetsockoptLinger(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [in] */ IStructLinger* value)
{
    return mOs->SetsockoptLinger(fd, level, option, value);
}

ECode ForwardingOs::SetsockoptTimeval(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer level,
    /* [in] */ Integer option,
    /* [in] */ IStructTimeval* value)
{
    return mOs->SetsockoptTimeval(fd, level, option, value);
}

ECode ForwardingOs::Setuid(
    /* [in] */ Integer uid)
{
    return mOs->Setuid(uid);
}

ECode ForwardingOs::Setxattr(
    /* [in] */ const String& path,
    /* [in] */ const String& name,
    /* [in] */ const Array<Byte>& value,
    /* [in] */ Integer flags)
{
    return mOs->Setxattr(path, name, value, flags);
}

ECode ForwardingOs::Shutdown(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer how)
{
    return mOs->Shutdown(fd, how);
}

ECode ForwardingOs::Socket(
    /* [in] */ Integer socketDomain,
    /* [in] */ Integer type,
    /* [in] */ Integer protocol,
    /* [out] */ IFileDescriptor** fd)
{
    return mOs->Socket(socketDomain, type, protocol, fd);
}

ECode ForwardingOs::Socketpair(
    /* [in] */ Integer socketDomain,
    /* [in] */ Integer type,
    /* [in] */ Integer protocol,
    /* [in] */ IFileDescriptor* fd1,
    /* [in] */ IFileDescriptor* fd2)
{
    return mOs->Socketpair(socketDomain, type, protocol, fd1, fd2);
}

ECode ForwardingOs::Stat(
    /* [in] */ const String& path,
    /* [out] */ IStructStat** stat)
{
    return mOs->Stat(path, stat);
}

ECode ForwardingOs::StatVfs(
    /* [in] */ const String& path,
    /* [out] */ IStructStatVfs** statfs)
{
    return mOs->StatVfs(path, statfs);
}

ECode ForwardingOs::Strerror(
    /* [in] */ Integer errnum,
    /* [out] */ String* strerr)
{
    return mOs->Strerror(errnum, strerr);
}

ECode ForwardingOs::Strsignal(
    /* [in] */ Integer signal,
    /* [out] */ String* strSignal)
{
    return mOs->Strsignal(signal, strSignal);
}

ECode ForwardingOs::Symlink(
    /* [in] */ const String& oldPath,
    /* [in] */ const String& newPath)
{
    return mOs->Symlink(oldPath, newPath);
}

ECode ForwardingOs::Sysconf(
    /* [in] */ Integer name,
    /* [out] */ Long* result)
{
    return mOs->Sysconf(name, result);
}

ECode ForwardingOs::Tcdrain(
    /* [in] */ IFileDescriptor* fd)
{
    return mOs->Tcdrain(fd);
}

ECode ForwardingOs::Tcsendbreak(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Integer duration)
{
    return mOs->Tcsendbreak(fd, duration);
}

ECode ForwardingOs::Umask(
    /* [in] */ Integer mask,
    /* [out] */ Integer* result)
{
    return mOs->Umask(mask, result);
}

ECode ForwardingOs::Uname(
    /* [out] */ IStructUtsname** uname)
{
    return mOs->Uname(uname);
}

ECode ForwardingOs::Unlink(
    /* [in] */ const String& pathname)
{
    return mOs->Unlink(pathname);
}

ECode ForwardingOs::Unsetenv(
    /* [in] */ const String& name)
{
    return mOs->Unsetenv(name);
}

ECode ForwardingOs::Waitpid(
    /* [in] */ Integer pid,
    /* [in, out] */ Integer* status,
    /* [in] */ Integer options,
    /* [out] */ Integer* result)
{
    return mOs->Waitpid(pid, status, options, result);
}

ECode ForwardingOs::Write(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ const Array<Byte>& bytes,
    /* [in] */ Integer byteOffset,
    /* [in] */ Integer byteCount,
    /* [out] */ Integer* num)
{
    return mOs->Write(fd, bytes, byteOffset, byteCount, num);
}

ECode ForwardingOs::Write(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ Integer* num)
{
    return mOs->Write(fd, buffer, num);
}

ECode ForwardingOs::Writev(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ const Array<IInterface*>& buffers,
    /* [in] */ const Array<Integer>& offsets,
    /* [in] */ const Array<Integer>& byteCounts,
    /* [out] */ Integer* result)
{
    return mOs->Writev(fd, buffers, offsets, byteCounts, result);
}

}
}
