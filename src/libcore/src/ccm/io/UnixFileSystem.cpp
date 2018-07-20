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

#include "ccm/core/System.h"
#include "ccm/io/CFile.h"
#include "ccm/io/UnixFileSystem.h"
#include "ccmrt/system/BlockGuard.h"
#include "ccm.core.ISecurityManager.h"
#include "ccmrt.system.IBlockGuardPolicy.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <stdio.h>
#include <unistd.h>

using ccm::core::ISecurityManager;
using ccm::core::System;
using ccm::io::CFile;
using ccm::io::IFile;
using ccmrt::system::BlockGuard;
using ccmrt::system::IBlockGuardPolicy;

extern int canonicalize(char *path, char *out, int len);
int handleOpen(const char *path, int oflag, int mode);

namespace ccm {
namespace io {

ECode UnixFileSystem::GetSeparator(
    /* [out] */ Char* separator)
{
    VALIDATE_NOT_NULL(separator);

    *separator = mSlash;
    return NOERROR;
}

ECode UnixFileSystem::GetPathSeparator(
    /* [out] */ Char* pathSeparator)
{
    VALIDATE_NOT_NULL(pathSeparator);

    *pathSeparator = mColon;
    return NOERROR;
}

ECode UnixFileSystem::Normalize(
    /* [in] */ const String& path,
    /* [out] */ String* normalizedPath)
{
    VALIDATE_NOT_NULL(normalizedPath);

    Integer n = path.GetLength();
    Array<Char> normalized = path.GetChars();
    Integer index = 0;
    Char prevChar = 0;
    for (Integer i = 0; i < n; i++) {
        Char current = normalized[i];
        // Remove duplicate slashes.
        if (!(current == '/' && prevChar == '/')) {
            normalized[index++] = current;
        }

        prevChar = current;
    }

    // Omit the trailing slash, except when pathname == "/".
    if (prevChar == '/' && n > 1) {
        index--;
    }

    if (index != n) {
        *normalizedPath = String(normalized, 0, index);
    }
    else {
        *normalizedPath = path;
    }
    return NOERROR;
}

ECode UnixFileSystem::PrefixLength(
    /* [in] */ const String& path,
    /* [out] */ Integer* length)
{
    VALIDATE_NOT_NULL(length);

    if (path.GetLength() == 0) {
        *length = 0;
        return NOERROR;
    }
    *length = path.GetChar(0) == '/' ? 1 : 0;
    return NOERROR;
}

ECode UnixFileSystem::Resolve(
    /* [in] */ const String& parent,
    /* [in] */ const String& child,
    /* [out] */ String* resolvedPath)
{
    VALIDATE_NOT_NULL(resolvedPath);

    if (child.IsEmpty() || child.Equals("/")) {
        *resolvedPath = parent;
        return NOERROR;
    }

    if (child.GetChar(0) == '/') {
        if (parent.Equals("/")) {
            *resolvedPath = child;
            return NOERROR;
        }
        *resolvedPath = parent + child;
        return NOERROR;
    }

    if (parent.Equals("/")) {
        *resolvedPath = parent + child;
        return NOERROR;
    }
    *resolvedPath = parent + "/" + child;
    return NOERROR;
}

ECode UnixFileSystem::GetDefaultParent(
    /* [out] */ String* parent)
{
    VALIDATE_NOT_NULL(parent);

    *parent = "/";
    return NOERROR;
}

ECode UnixFileSystem::FromURIPath(
    /* [in] */ const String& path,
    /* [out] */ String* resultPath)
{
    VALIDATE_NOT_NULL(resultPath);

    String p = path;
    if (p.EndsWith("/") && (p.GetLength() > 1)) {
        // "/foo/" --> "/foo", but "/" --> "/"
        p = p.Substring(0, p.GetLength() - 1);
    }
    *resultPath = p;
    return NOERROR;
}

ECode UnixFileSystem::IsAbsolute(
    /* [in] */ IFile* f,
    /* [out] */ Boolean* absolute)
{
    VALIDATE_NOT_NULL(absolute);

    Integer prefLen;
    f->GetPrefixLength(&prefLen);
    *absolute = prefLen != 0;
    return NOERROR;
}

ECode UnixFileSystem::Resolve(
    /* [in] */ IFile* f,
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path);

    Boolean absolute;
    if (IsAbsolute(f, &absolute), absolute) {
        return f->GetPath(path);
    }
    String userDirPath, fPath;
    System::GetProperty(String("user.dir"), &userDirPath);
    f->GetPath(&fPath);
    return Resolve(userDirPath, fPath, path);
}

ECode UnixFileSystem::Canonicalize(
    /* [in] */ const String& path,
    /* [out] */ String* canonicalizedPath)
{
    VALIDATE_NOT_NULL(canonicalizedPath);

    if (!sUseCanonCaches) {
        return Canonicalize0(path, canonicalizedPath);
    }
    else {
        String res = mCache.Get(path);
        if (res.IsNull()) {
            String dir;
            String resDir;
            if (sUseCanonPrefixCache) {
                // Note that this can cause symlinks that should
                // be resolved to a destination directory to be
                // resolved to the directory they're contained in
                dir = ParentOrNull(path);
                if (!dir.IsNull()) {
                    resDir = mCcmHomePrefixCache.Get(dir);
                    if (!resDir.IsNull()) {
                        // Hit only in prefix cache; full path is canonical
                        String filename = path.Substring(1 + dir.GetLength());
                        res = resDir + mSlash + filename;
                        mCache.Put(dir + mSlash + filename, res);
                    }
                }
            }
            if (res.IsNull()) {
                AutoPtr<IBlockGuardPolicy> policy;
                BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy);
                FAIL_RETURN(policy->OnReadFromDisk());
                FAIL_RETURN(Canonicalize0(path, &res));
                mCache.Put(path, res);
                if (sUseCanonPrefixCache &&
                        !dir.IsNull() && dir.StartsWith(mCcmHome)) {
                    resDir = ParentOrNull(res);
                    // Note that we don't allow a resolved symlink
                    // to elsewhere in java.home to pollute the
                    // prefix cache (java.home prefix cache could
                    // just as easily be a set at this point)
                    if (!resDir.IsNull() && resDir.Equals(dir)) {
                        AutoPtr<IFile> f;
                        CFile::New(res, IID_IFile, (IInterface**)&f);
                        Boolean result;
                        if ((f->Exists(&result), result) &&
                            (f->IsDirectory(&result), !result)) {
                            mCcmHomePrefixCache.Put(dir, resDir);
                        }
                    }
                }
            }
        }
        *canonicalizedPath = res;
        return NOERROR;
    }
}

ECode UnixFileSystem::Canonicalize0(
    /* [in] */ const String& path,
    /* [out] */ String* canonicalizedPath)
{
    char canonicalPath[MAXPATHLEN];
    if (canonicalize(const_cast<char*>(path.string()), canonicalPath, MAXPATHLEN) < 0)
    {
        Logger::E("UnixFileSystem", "Bad pathname: %s", path.string());
        return E_IO_EXCEPTION;
    }
    else {
        *canonicalizedPath = canonicalPath;
        return NOERROR;
    }
}

String UnixFileSystem::ParentOrNull(
    /* [in] */ const String& path)
{
    if (path.IsNull()) return String();
    Char sep = File::GetSeparatorChar();
    Integer last = path.GetLength() - 1;
    Integer idx = last;
    Integer adjacentDots = 0;
    Integer nonDotCount = 0;
    while (idx > 0) {
        Char c = path.GetChar(idx);
        if (c == '.') {
            if (++adjacentDots >= 2) {
                return String();
            }
        }
        else if (c == sep) {
            if (adjacentDots == 1 && nonDotCount == 0) {
                return String();
            }
            if (idx == 0 || idx >= last - 1 ||
                    path.GetChar(idx - 1) == sep) {
                return String();
            }
            return path.Substring(0, idx);
        }
        else {
            ++nonDotCount;
            adjacentDots = 0;
        }
        --idx;
    }
    return String();
}

static Boolean StatMode(const char *path, int *mode)
{
    struct stat64 sb;
    if (stat64(path, &sb) == 0) {
        *mode = sb.st_mode;
        return true;
    }
    return false;
}

Integer UnixFileSystem::GetBooleanAttributes0(
    /* [in] */ const String& abspath)
{
    Integer rv = 0;
    int mode;
    if (StatMode(abspath.string(), &mode)) {
        int fmt = mode & S_IFMT;
        rv = (Integer) (BA_EXISTS
              | ((fmt == S_IFREG) ? BA_REGULAR : 0)
              | ((fmt == S_IFDIR) ? BA_DIRECTORY : 0));
    }
    return rv;
}

ECode UnixFileSystem::GetBooleanAttributes(
    /* [in] */ IFile* f,
    /* [out] */ Integer* attr)
{
    VALIDATE_NOT_NULL(attr);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy);
    FAIL_RETURN(policy->OnReadFromDisk());

    String path;
    f->GetPath(&path);
    Integer value = GetBooleanAttributes0(path);
    String name;
    f->GetName(&name);
    Boolean hidden = (name.GetLength() > 0) && (name.GetChar(0) == '.');
    *attr = value | (hidden ? BA_HIDDEN : 0);
    return NOERROR;
}

ECode UnixFileSystem::CheckAccess(
    /* [in] */ IFile* f,
    /* [in] */ Integer access,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    *result = CheckAccess0(f, access);
    return NOERROR;
}

Boolean UnixFileSystem::CheckAccess0(
    /* [in] */ IFile* f,
    /* [in] */ Integer a)
{
    Boolean rv = false;
    Integer mode = 0;
    switch (a) {
        case ACCESS_OK:
            mode = F_OK;
            break;
        case ACCESS_READ:
            mode = R_OK;
            break;
        case ACCESS_WRITE:
            mode = W_OK;
            break;
        case ACCESS_EXECUTE:
            mode = X_OK;
            break;
        default:
            CHECK(0);
    }
    String path;
    f->GetPath(&path);
    if (access(path.string(), mode) == 0) {
        rv = true;
    }
    return rv;
}

ECode UnixFileSystem::GetLastModifiedTime(
    /* [in] */ IFile* f,
    /* [out] */ Long* time)
{
    VALIDATE_NOT_NULL(time);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    *time = GetLastModifiedTime0(f);
    return NOERROR;
}

Long UnixFileSystem::GetLastModifiedTime0(
    /* [in] */ IFile* f)
{
    Long rv = 0;

    String path;
    f->GetPath(&path);
    struct stat64 sb;
    if (stat64(path.string(), &sb) == 0) {
        rv = 1000 * (Long)sb.st_mtime;
    }
    return rv;
}

ECode UnixFileSystem::GetLength(
    /* [in] */ IFile* f,
    /* [out] */ Long* length)
{
    VALIDATE_NOT_NULL(length);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    *length = GetLength0(f);
    return NOERROR;
}

Long UnixFileSystem::GetLength0(
    /* [in] */ IFile* f)
{
    Long rv = 0;

    String path;
    f->GetPath(&path);
    struct stat64 sb;
    if (stat64(path, &sb) == 0) {
        rv = sb.st_size;
    }
    return rv;
}

ECode UnixFileSystem::SetPermission(
    /* [in] */ IFile* f,
    /* [in] */ Integer access,
    /* [in] */ Boolean enable,
    /* [in] */ Boolean owneronly,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    *succeeded = SetPermission0(f, access, enable, owneronly);
    return NOERROR;
}

Boolean UnixFileSystem::SetPermission0(
    /* [in] */ IFile* f,
    /* [in] */ Integer access,
    /* [in] */ Boolean enable,
    /* [in] */ Boolean owneronly)
{
    Boolean rv = false;

    int amode = 0;
    int mode;
    switch (access) {
    case ACCESS_READ:
        if (owneronly) {
            amode = S_IRUSR;
        }
        else {
            amode = S_IRUSR | S_IRGRP | S_IROTH;
        }
        break;
    case ACCESS_WRITE:
        if (owneronly) {
            amode = S_IWUSR;
        }
        else {
            amode = S_IWUSR | S_IWGRP | S_IWOTH;
        }
        break;
    case ACCESS_EXECUTE:
        if (owneronly) {
            amode = S_IXUSR;
        }
        else {
            amode = S_IXUSR | S_IXGRP | S_IXOTH;
        }
        break;
    default:
        CHECK(0);
    }
    String path;
    f->GetPath(&path);
    if (StatMode(path, &mode)) {
        if (enable) {
            mode |= amode;
        }
        else {
            mode &= ~amode;
        }
        if (chmod(path, mode) >= 0) {
            rv = true;
        }
    }
    return rv;
}

ECode UnixFileSystem::CreateFileExclusively(
    /* [in] */ const String& pathname,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    return CreateFileExclusively0(pathname, succeeded);
}

ECode UnixFileSystem::CreateFileExclusively0(
    /* [in] */ const String& path,
    /* [out] */ Boolean* succeeded)
{
    Boolean rv = false;

    int fd;
    if (strcmp(path, "/")) {
        fd = handleOpen(path, O_RDWR | O_CREAT | O_EXCL, 0666);
        if (fd < 0) {
            if (errno != EEXIST) {
                return E_IO_EXCEPTION;
            }
        }
        else {
            if (close(fd) == -1) {
                return E_IO_EXCEPTION;
            }
            rv = true;
        }
    }
    *succeeded = rv;
    return NOERROR;
}

ECode UnixFileSystem::Delete(
    /* [in] */ IFile* f,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    mCache.Clear();
    mCcmHomePrefixCache.Clear();
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    *succeeded = Delete0(f);
    return NOERROR;
}

Boolean UnixFileSystem::Delete0(
    /* [in] */ IFile* f)
{
    Boolean rv = false;

    String path;
    f->GetPath(&path);
    if (remove(path) == 0) {
        rv = true;
    }
    return rv;
}

ECode UnixFileSystem::List(
    /* [in] */ IFile* f,
    /* [out, callee] */ Array<String>* elements)
{
    VALIDATE_NOT_NULL(elements);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    return List0(f, elements);
}

ECode UnixFileSystem::List0(
    /* [in] */ IFile* f,
    /* [out, callee] */ Array<String>* elements)
{
    DIR* dir = nullptr;

    String path;
    f->GetPath(&path);
    dir = opendir(path.string());
    if (dir == nullptr) {
        *elements = Array<String>::Null();
        return NOERROR;
    }

    struct dirent64* ptr = (struct dirent64*)malloc(sizeof(struct dirent64) + (PATH_MAX + 1));
    if (ptr == nullptr) {
        closedir(dir);
        *elements = Array<String>::Null();
        return E_OUT_OF_MEMORY_ERROR;
    }

    Integer len = 0;
    Integer maxLen = 16;
    struct dirent64* result;
    Array<String> rv(maxLen);
    while ((readdir64_r(dir, ptr, &result) == 0)  && (result != NULL)) {
        if (!strcmp(ptr->d_name, ".") || !strcmp(ptr->d_name, "..")) {
            continue;
        }
        if (len == maxLen) {
            Array<String> old = rv;
            rv = Array<String>(maxLen <<= 1);
            rv.Copy(old, len);
        }
        String name(ptr->d_name);
        rv[len++] = name;
    }
    closedir(dir);
    free(ptr);

    Array<String> old = rv;
    rv = Array<String>(len);
    rv.Copy(old, len);
    *elements = rv;
    return NOERROR;
}

ECode UnixFileSystem::CreateDirectory(
    /* [in] */ IFile* f,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    *succeeded = CreateDirectory0(f);
    return NOERROR;
}

Boolean UnixFileSystem::CreateDirectory0(
    /* [in] */ IFile* f)
{
    Boolean rv = false;

    String path;
    f->GetPath(&path);
    if (mkdir(path, 0777) == 0) {
        rv = true;
    }
    return rv;
}

ECode UnixFileSystem::Rename(
    /* [in] */ IFile* f1,
    /* [in] */ IFile* f2,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    mCache.Clear();
    mCcmHomePrefixCache.Clear();
    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    *succeeded = Rename0(f1, f2);
    return NOERROR;
}

Boolean UnixFileSystem::Rename0(
    /* [in] */ IFile* f1,
    /* [in] */ IFile* f2)
{
    Boolean rv = false;

    String fromPath, toPath;
    f1->GetPath(&fromPath);
    f2->GetPath(&toPath);
    if (rename(fromPath.string(), toPath.string()) == 0) {
        rv = true;
    }
    return rv;
}

ECode UnixFileSystem::SetLastModifiedTime(
    /* [in] */ IFile* f,
    /* [in] */ Long time,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    *succeeded = SetLastModifiedTime0(f, time);
    return NOERROR;
}

Boolean UnixFileSystem::SetLastModifiedTime0(
    /* [in] */ IFile* f,
    /* [in] */ Long time)
{
    Boolean rv = false;

    String path;
    f->GetPath(&path);
    struct stat64 sb;

    if (stat64(path, &sb) == 0) {
        struct timeval tv[2];

        /* Preserve access time */
        tv[0].tv_sec = sb.st_atime;
        tv[0].tv_usec = 0;

        /* Change last-modified time */
        tv[1].tv_sec = time / 1000;
        tv[1].tv_usec = (time % 1000) * 1000;

        if (utimes(path, tv) == 0) {
            rv = true;
        }
    }

    return rv;
}

ECode UnixFileSystem::SetReadOnly(
    /* [in] */ IFile* f,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    *succeeded = SetReadOnly0(f);
    return NOERROR;
}

Boolean UnixFileSystem::SetReadOnly0(
    /* [in] */ IFile* f)
{
    Boolean rv = false;

    String path;
    f->GetPath(&path);
    int mode;
    if (StatMode(path, &mode)) {
        if (chmod(path, mode & ~(S_IWUSR | S_IWGRP | S_IWOTH)) >= 0) {
            rv = true;
        }
    }
    return rv;
}

ECode UnixFileSystem::ListRoots(
    /* [out, callee] */ Array<IFile*>* roots)
{
    VALIDATE_NOT_NULL(roots);

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        ECode ec = security->CheckRead(String("/"));
        if (FAILED(ec)) {
            *roots = Array<IFile*>(0);
            return NOERROR;
        }
    }
    AutoPtr<IFile> f;
    CFile::New(String("/"), IID_IFile, (IInterface**)&f);
    Array<IFile*> rv(1);
    rv.Set(0, f);
    *roots = rv;
    return NOERROR;
}

ECode UnixFileSystem::GetSpace(
    /* [in] */ IFile* f,
    /* [in] */ Integer t,
    /* [out] */ Long* space)
{
    VALIDATE_NOT_NULL(space);

    AutoPtr<IBlockGuardPolicy> policy;
    BlockGuard::GetThreadPolicy((IBlockGuardPolicy**)&policy);
    FAIL_RETURN(policy->OnReadFromDisk());
    *space = GetSpace0(f, t);
    return NOERROR;
}

Long UnixFileSystem::GetSpace0(
    /* [in] */ IFile* f,
    /* [in] */ Integer t)
{
    Long rv = 0;

    String path;
    f->GetPath(&path);
    struct statvfs64 fsstat;
    memset(&fsstat, 0, sizeof(fsstat));
    if (statvfs64(path, &fsstat) == 0) {
        switch(t) {
            case SPACE_TOTAL:
                rv = (Long)fsstat.f_frsize * (Long)fsstat.f_blocks;
                break;
            case SPACE_FREE:
                rv = (Long)fsstat.f_frsize * (Long)fsstat.f_bfree;
                break;
            case SPACE_USABLE:
                rv = (Long)fsstat.f_frsize * (Long)fsstat.f_bavail;
                break;
            default:
                CHECK(0);
        }
    }
    return rv;
}

ECode UnixFileSystem::Compare(
    /* [in] */ IFile* f1,
    /* [in] */ IFile* f2,
    /* [out] */ Integer* result)
{
    VALIDATE_NOT_NULL(result);

    String path1, path2;
    f1->GetPath(&path1);
    f2->GetPath(&path2);
    *result = path1.Compare(path2);
    return NOERROR;
}

ECode UnixFileSystem::GetHashCode(
    /* [in] */ IFile* f,
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    String path;
    f->GetPath(&path);
    *hash = path.GetHashCode() ^ 1234321;
    return NOERROR;
}

}
}
