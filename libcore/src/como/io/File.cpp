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

#include "como/core/CoreUtils.h"
#include "como/core/CRuntimePermission.h"
#include "como/core/Math.h"
#include "como/core/StringUtils.h"
#include "como/core/System.h"
#include "como/io/CFile.h"
#include "como/io/DefaultFileSystem.h"
#include "como/io/DeleteOnExitHook.h"
#include "como/io/File.h"
#include "como/util/CArrayList.h"
#include "como.core.ICharSequence.h"
#include "como.core.ILong.h"
#include "como.security.IPermission.h"
#include "como.util.IList.h"
#include <ccmlogger.h>

using como::core::CoreUtils;
using como::core::CRuntimePermission;
using como::core::IID_IComparable;
using como::core::ICharSequence;
using como::core::IID_ICharSequence;
using como::core::ILong;
using como::core::ISecurityManager;
using como::core::Math;
using como::core::StringUtils;
using como::core::System;
using como::io::IID_ISerializable;
using como::security::IPermission;
using como::security::IID_IPermission;
using como::util::CArrayList;
using como::util::IList;
using como::util::IID_IList;

namespace como {
namespace io {

COMO_INTERFACE_IMPL_3(File, SyncObject, IFile, ISerializable, IComparable);

AutoPtr<FileSystem> File::GetFS()
{
    static AutoPtr<FileSystem> FS = DefaultFileSystem::GetFileSystem();
    return FS;
}

Boolean File::IsInvalid()
{
    if (mStatus == PathStatus::UNKNOWN) {
        mStatus = (mPath.IndexOf(U'\0') < 0) ? PathStatus::CHECKED
                                            : PathStatus::INVALID;
    }
    return mStatus == PathStatus::INVALID;
}

ECode File::GetPrefixLength(
    /* [out] */ Integer* length)
{
    VALIDATE_NOT_NULL(length);

    *length = mPrefixLength;
    return NOERROR;
}

Char File::GetSeparatorChar()
{
    Char c;
    static const Char sSeparatorChar = (GetFS()->GetSeparator(&c), c);
    return sSeparatorChar;
}

ECode File::Constructor(
    /* [in] */ const String& pathname,
    /* [in] */ Integer prefixLength)
{
    mPath = pathname;
    mPrefixLength = prefixLength;
    return NOERROR;
}

ECode File::Constructor(
    /* [in] */ const String& child,
    /* [in] */ IFile* parent)
{
    String ppath;
    parent->GetPath(&ppath);
    GetFS()->Resolve(ppath, child, &mPath);
    parent->GetPrefixLength(&mPrefixLength);
    return NOERROR;
}

ECode File::Constructor(
    /* [in] */ const String& pathname)
{
    if (pathname.IsNull()) {
        return como::core::E_NULL_POINTER_EXCEPTION;
    }
    FileSystem* fs = GetFS();
    fs->Normalize(pathname, &mPath);
    fs->PrefixLength(mPath, &mPrefixLength);
    return NOERROR;
}

ECode File::Constructor(
    /* [in] */ const String& parent,
    /* [in] */ const String& child)
{
    if (child.IsNull()) {
        return como::core::E_NULL_POINTER_EXCEPTION;
    }
    FileSystem* fs = GetFS();
    if (!parent.IsNullOrEmpty()) {
        String normPpath, normCpath;
        fs->Normalize(parent, &normPpath);
        fs->Normalize(child, &normCpath);
        fs->Resolve(normPpath, normCpath, &mPath);
    }
    else {
        fs->Normalize(child, &mPath);
    }
    fs->PrefixLength(mPath, &mPrefixLength);
    return NOERROR;
}

ECode File::Constructor(
    /* [in] */ IFile* parent,
    /* [in] */ const String& child)
{
    if (child.IsNull()) {
        return como::core::E_NULL_POINTER_EXCEPTION;
    }
    FileSystem* fs = GetFS();
    if (parent != nullptr) {
        String ppath;
        parent->GetPath(&ppath);
        if (ppath.Equals("")) {
            String normPpath, normCpath;
            fs->GetDefaultParent(&normPpath);
            fs->Normalize(child, &normCpath);
            fs->Resolve(normPpath, normCpath, &mPath);
        }
        else {
            String normCpath;
            fs->Normalize(child, &normCpath);
            fs->Resolve(ppath, normCpath, &mPath);
        }
    }
    else {
        fs->Normalize(child, &mPath);
    }
    fs->PrefixLength(mPath, &mPrefixLength);
    return NOERROR;
}

ECode File::Constructor(
    /* [in] */ IURI* uri)
{
    Boolean absolute;
    if (uri->IsAbsolute(&absolute), !absolute) {
        Logger::E("File", "URI is not absolute");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean opaque;
    if (uri->IsOpaque(&opaque), opaque) {
        Logger::E("File", "URI is not hierarchical");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String scheme;
    uri->GetScheme(&scheme);
    if (scheme.IsNull() || !scheme.EqualsIgnoreCase("file")) {
        Logger::E("File", "URI scheme is not \"file\"");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String authority;
    if (uri->GetAuthority(&authority), !authority.IsNull()) {
        Logger::E("File", "URI has an authority component");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String fragment;
    if (uri->GetFragment(&fragment), !fragment.IsNull()) {
        Logger::E("File", "URI has a fragment component");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String query;
    if (uri->GetQuery(&query), !query.IsNull()) {
        Logger::E("File", "URI has a query component");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String p;
    uri->GetPath(&p);
    if (p.Equals("")) {
        Logger::E("File", "URI path component is empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FileSystem* fs = GetFS();
    fs->FromURIPath(p, &p);
    if (GetSeparatorChar() != U'/') {
        p = p.Replace(U'/', GetSeparatorChar());
    }
    fs->Normalize(p, &mPath);
    fs->PrefixLength(mPath, &mPrefixLength);
    return NOERROR;
}

ECode File::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    Integer index = mPath.LastIndexOf(GetSeparatorChar());
    if (index < mPrefixLength) {
        *name = mPath.Substring(mPrefixLength);
    }
    else {
        *name = mPath.Substring(index + 1);
    }
    return NOERROR;
}

ECode File::GetParent(
    /* [out] */ String* parent)
{
    VALIDATE_NOT_NULL(parent);

    Integer index = mPath.LastIndexOf(GetSeparatorChar());
    if (index < mPrefixLength) {
        if (mPrefixLength > 0 && mPath.GetLength() > mPrefixLength) {
            *parent = mPath.Substring(0, mPrefixLength);
            return NOERROR;
        }
        *parent = nullptr;
        return NOERROR;
    }
    else {
        *parent = mPath.Substring(0, index);
        return NOERROR;
    }
}

ECode File::GetParentFile(
    /* [out] */ IFile** parent)
{
    VALIDATE_NOT_NULL(parent);

    String p;
    GetParent(&p);
    if (p.IsNull()) {
        *parent = nullptr;
        return NOERROR;
    }
    return CFile::New(p, mPrefixLength, IID_IFile, (IInterface**)parent);
}

ECode File::GetPath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path);

    *path = mPath;
    return NOERROR;
}

ECode File::IsAbsolute(
    /* [out] */ Boolean* absolute)
{
    return GetFS()->IsAbsolute(this, absolute);
}

ECode File::GetAbsolutePath(
    /* [out] */ String* path)
{
    return GetFS()->Resolve(this, path);
}

ECode File::GetAbsoluteFile(
    /* [out] */ IFile** f)
{
    VALIDATE_NOT_NULL(f);

    String absPath;
    GetAbsolutePath(&absPath);
    Integer prefLen;
    GetFS()->PrefixLength(absPath, &prefLen);
    return CFile::New(absPath, prefLen, IID_IFile, (IInterface**)f);
}

ECode File::GetCanonicalPath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path);

    if (IsInvalid()) {
        Logger::E("File", "Invalid file path");
        return E_IO_EXCEPTION;
    }
    FileSystem* fs = GetFS();
    String p;
    fs->Resolve(this, &p);
    return fs->Canonicalize(p, path);
}

ECode File::GetCanonicalFile(
    /* [out] */ IFile** f)
{
    VALIDATE_NOT_NULL(f);

    String canonPath;
    FAIL_RETURN(GetCanonicalPath(&canonPath));
    Integer prefLen;
    GetFS()->PrefixLength(canonPath, &prefLen);
    return CFile::New(canonPath, prefLen, IID_IFile, (IInterface**)f);
}

String File::Slashify(
    /* [in] */ const String& path,
    /* [in] */ Boolean isDirectory)
{
    String p = path;
    if (GetSeparatorChar() != U'/') {
        p = p.Replace(GetSeparatorChar(), U'/');
    }
    if (!p.StartsWith("/")) {
        p = String("/") + p;
    }
    if (!p.EndsWith("/") && isDirectory) {
        p = p + "/";
    }
    return p;
}

ECode File::ToURL(
    /* [out] */ IURL** url)
{
    return NOERROR;
}

ECode File::ToURI(
    /* [out] */ IURI** id)
{
    return NOERROR;
}

ECode File::CanRead(
    /* [out] */ Boolean* readable)
{
    VALIDATE_NOT_NULL(readable);

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckRead(mPath));
    }
    if (IsInvalid()) {
        *readable = false;
        return NOERROR;
    }
    return GetFS()->CheckAccess(this, FileSystem::ACCESS_READ, readable);
}

ECode File::CanWrite(
    /* [out] */ Boolean* writeable)
{
    VALIDATE_NOT_NULL(writeable);

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckWrite(mPath));
    }
    if (IsInvalid()) {
        *writeable = false;
        return NOERROR;
    }
    return GetFS()->CheckAccess(this, FileSystem::ACCESS_WRITE, writeable);
}

ECode File::Exists(
    /* [out] */ Boolean* existed)
{
    VALIDATE_NOT_NULL(existed);

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckRead(mPath));
    }
    if (IsInvalid()) {
        *existed = false;
        return NOERROR;
    }
    return GetFS()->CheckAccess(this, FileSystem::ACCESS_OK, existed);
}

ECode File::IsDirectory(
    /* [out] */ Boolean* directory)
{
    VALIDATE_NOT_NULL(directory);

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckRead(mPath));
    }
    if (IsInvalid()) {
        *directory = false;
        return NOERROR;
    }
    Integer attrs;
    GetFS()->GetBooleanAttributes(this, &attrs);
    *directory = (attrs & FileSystem::BA_DIRECTORY) != 0;
    return NOERROR;
}

ECode File::IsFile(
    /* [out] */ Boolean* file)
{
    VALIDATE_NOT_NULL(file);

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckRead(mPath));
    }
    if (IsInvalid()) {
        *file = false;
        return NOERROR;
    }
    Integer attrs;
    GetFS()->GetBooleanAttributes(this, &attrs);
    *file = (attrs & FileSystem::BA_REGULAR) != 0;
    return NOERROR;
}

ECode File::IsHidden(
    /* [out] */ Boolean* hidden)
{
    VALIDATE_NOT_NULL(hidden);

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckRead(mPath));
    }
    if (IsInvalid()) {
        *hidden = false;
        return NOERROR;
    }
    Integer attrs;
    GetFS()->GetBooleanAttributes(this, &attrs);
    *hidden = (attrs & FileSystem::BA_HIDDEN) != 0;
    return NOERROR;
}

ECode File::LastModified(
    /* [out] */ Long* time)
{
    VALIDATE_NOT_NULL(time);

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckRead(mPath));
    }
    if (IsInvalid()) {
        *time = 0;
        return NOERROR;
    }
    return GetFS()->GetLastModifiedTime(this, time);
}

ECode File::GetLength(
    /* [out] */ Long* len)
{
    VALIDATE_NOT_NULL(len);

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckRead(mPath));
    }
    if (IsInvalid()) {
        *len = 0;
        return NOERROR;
    }
    return GetFS()->GetLength(this, len);
}

ECode File::CreateNewFile(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckWrite(mPath));
    }
    if (IsInvalid()) {
        Logger::E("File", "Invalid file path");
        return E_IO_EXCEPTION;
    }
    return GetFS()->CreateFileExclusively(mPath, succeeded);
}

ECode File::Delete(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckDelete(mPath));
    }
    if (IsInvalid()) {
        *succeeded = false;
        return NOERROR;
    }
    return GetFS()->Delete(this, succeeded);
}

ECode File::DeleteOnExit()
{
    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckDelete(mPath));
    }
    if (IsInvalid()) {
        return NOERROR;
    }
    DeleteOnExitHook::Add(mPath);
    return NOERROR;
}

ECode File::List(
    /* [out, callee] */ Array<String>* files)
{
    VALIDATE_NOT_NULL(files);

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckRead(mPath));
    }
    if (IsInvalid()) {
        *files = Array<String>::Null();
        return NOERROR;
    }
    return GetFS()->List(this, files);
}

ECode File::List(
    /* [in] */ IFilenameFilter* filter,
    /* [out, callee] */ Array<String>* files)
{
    VALIDATE_NOT_NULL(files);

    Array<String> names;
    FAIL_RETURN(List(&names));
    if (names.IsNull() || filter == nullptr) {
        *files = names;
        return NOERROR;
    }
    AutoPtr<IList> v;
    CArrayList::New(IID_IList, (IInterface**)&v);
    for (Integer i = 0; i < names.GetLength(); i++) {
        Boolean accepted;
        if (filter->Accept(this, names[i], &accepted), accepted) {
            v->Add(CoreUtils::Box(names[i]));
        }
    }
    Array<ICharSequence*> seqs;
    v->ToArray(IID_ICharSequence, (Array<IInterface*>*)&seqs);
    *files = CoreUtils::Unbox(seqs);
    return NOERROR;
}

ECode File::ListFiles(
    /* [out, callee] */ Array<IFile*>* files)
{
    VALIDATE_NOT_NULL(files);

    Array<String> ss;
    FAIL_RETURN(List(&ss));
    if (ss.IsNull()) {
        *files = Array<IFile*>::Null();
        return NOERROR;
    }
    Integer n = ss.GetLength();
    Array<IFile*> fs(n);
    for (Integer i = 0; i < n; i++) {
        AutoPtr<IFile> f;
        CFile::New(ss[i], this, IID_IFile, (IInterface**)&f);
        fs.Set(i, f);
    }
    *files = fs;
    return NOERROR;
}

ECode File::ListFiles(
    /* [in] */ IFilenameFilter* filter,
    /* [out, callee] */ Array<IFile*>* files)
{
    VALIDATE_NOT_NULL(files);

    Array<String> ss;
    FAIL_RETURN(List(&ss));
    if (ss.IsNull()) {
        *files = Array<IFile*>::Null();
        return NOERROR;
    }
    AutoPtr<IList> v;
    CArrayList::New(IID_IList, (IInterface**)&v);
    for (Integer i = 0; i < ss.GetLength(); i++) {
        Boolean accepted;
        if (filter == nullptr ||
                (filter->Accept(this, ss[i], &accepted), accepted)) {
            AutoPtr<IFile> f;
            CFile::New(ss[i], this, IID_IFile, (IInterface**)&f);
            v->Add(f);
        }
    }
    return v->ToArray((Array<IInterface*>*)files);
}

ECode File::ListFiles(
    /* [in] */ IFileFilter* filter,
    /* [out, callee] */ Array<IFile*>* files)
{
    VALIDATE_NOT_NULL(files);

    Array<String> ss;
    FAIL_RETURN(List(&ss));
    if (ss.IsNull()) {
        *files = Array<IFile*>::Null();
        return NOERROR;
    }
    AutoPtr<IList> v;
    CArrayList::New(IID_IList, (IInterface**)&v);
    for (Integer i = 0; i < ss.GetLength(); i++) {
        AutoPtr<IFile> f;
        CFile::New(ss[i], this, IID_IFile, (IInterface**)&f);
        Boolean accepted;
        if (filter == nullptr ||
                (filter->Accept(f, &accepted), accepted)) {
            v->Add(f);
        }
    }
    return v->ToArray((Array<IInterface*>*)files);
}

ECode File::Mkdir(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckWrite(mPath));
    }
    if (IsInvalid()) {
        *succeeded = false;
        return NOERROR;
    }
    return GetFS()->CreateDirectory(this, succeeded);
}

ECode File::Mkdirs(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    Boolean existed;
    FAIL_RETURN(Exists(&existed));
    if (existed) {
        *succeeded = false;
        return NOERROR;
    }
    FAIL_RETURN(Mkdir(succeeded));
    if (*succeeded) {
        return NOERROR;
    }
    AutoPtr<IFile> canonFile;
    ECode ec = GetCanonicalFile(&canonFile);
    if (FAILED(ec)) {
        *succeeded = false;
        return NOERROR;
    }

    AutoPtr<IFile> parent;
    canonFile->GetParentFile(&parent);
    if (parent == nullptr) {
        *succeeded = false;
        return NOERROR;
    }
    FAIL_RETURN(parent->Mkdirs(succeeded));
    if (!*succeeded) {
        FAIL_RETURN(parent->Exists(&existed));
        if (!existed) {
            *succeeded = false;
            return NOERROR;
        }
    }
    return canonFile->Mkdir(succeeded);
}

ECode File::RenameTo(
    /* [in] */ IFile* dest,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    if (dest == nullptr) {
        return como::core::E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckWrite(mPath));
        String dPath;
        dest->GetPath(&dPath);
        FAIL_RETURN(security->CheckWrite(dPath));
    }
    if (IsInvalid() || From(dest)->IsInvalid()) {
        *succeeded = false;
        return NOERROR;
    }
    return GetFS()->Rename(this, dest, succeeded);
}

ECode File::SetLastModified(
    /* [in] */ Long time,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    if (time < 0) {
        Logger::E("File", "Negative time");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckWrite(mPath));
    }
    if (IsInvalid()) {
        *succeeded = false;
        return NOERROR;
    }
    return GetFS()->SetLastModifiedTime(this, time, succeeded);
}

ECode File::SetReadOnly(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckWrite(mPath));
    }
    if (IsInvalid()) {
        *succeeded = false;
        return NOERROR;
    }
    return GetFS()->SetReadOnly(this, succeeded);
}

ECode File::SetWritable(
    /* [in] */ Boolean writable,
    /* [in] */ Boolean ownerOnly,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckWrite(mPath));
    }
    if (IsInvalid()) {
        *succeeded = false;
        return NOERROR;
    }
    return GetFS()->SetPermission(this,
            FileSystem::ACCESS_WRITE, writable, ownerOnly, succeeded);
}

ECode File::SetWritable(
    /* [in] */ Boolean writable,
    /* [out] */ Boolean* succeeded)
{
    return SetWritable(writable, true, succeeded);
}

ECode File::SetReadable(
    /* [in] */ Boolean readable,
    /* [in] */ Boolean ownerOnly,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckWrite(mPath));
    }
    if (IsInvalid()) {
        *succeeded = false;
        return NOERROR;
    }
    return GetFS()->SetPermission(this,
            FileSystem::ACCESS_READ, readable, ownerOnly, succeeded);
}

ECode File::SetReadable(
    /* [in] */ Boolean readable,
    /* [out] */ Boolean* succeeded)
{
    return SetReadable(readable, true, succeeded);
}

ECode File::SetExecutable(
    /* [in] */ Boolean executable,
    /* [in] */ Boolean ownerOnly,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckWrite(mPath));
    }
    if (IsInvalid()) {
        *succeeded = false;
        return NOERROR;
    }
    return GetFS()->SetPermission(this,
            FileSystem::ACCESS_EXECUTE, executable, ownerOnly, succeeded);
}

ECode File::SetExecutable(
    /* [in] */ Boolean executable,
    /* [out] */ Boolean* succeeded)
{
    return SetExecutable(executable, true, succeeded);
}

ECode File::CanExecute(
    /* [out] */ Boolean* executable)
{
    VALIDATE_NOT_NULL(executable);

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckExec(mPath));
    }
    if (IsInvalid()) {
        *executable = false;
        return NOERROR;
    }
    return GetFS()->CheckAccess(this, FileSystem::ACCESS_EXECUTE, executable);
}

ECode File::ListRoots(
    /* [out, callee] */ Array<IFile*>* roots)
{
    return GetFS()->ListRoots(roots);
}

ECode File::GetTotalSpace(
    /* [out] */ Long* space)
{
    VALIDATE_NOT_NULL(space);

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        AutoPtr<IPermission> perm;
        CRuntimePermission::New(String("getFileSystemAttributes"), IID_IPermission, (IInterface**)&perm);
        FAIL_RETURN(security->CheckPermission(perm));
        FAIL_RETURN(security->CheckRead(mPath));
    }
    if (IsInvalid()) {
        *space = 0;
        return NOERROR;
    }
    return GetFS()->GetSpace(this, FileSystem::SPACE_TOTAL, space);
}

ECode File::GetFreeSpace(
    /* [out] */ Long* space)
{
    VALIDATE_NOT_NULL(space);

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        AutoPtr<IPermission> perm;
        CRuntimePermission::New(String("getFileSystemAttributes"), IID_IPermission, (IInterface**)&perm);
        FAIL_RETURN(security->CheckPermission(perm));
        FAIL_RETURN(security->CheckRead(mPath));
    }
    if (IsInvalid()) {
        *space = 0;
        return NOERROR;
    }
    return GetFS()->GetSpace(this, FileSystem::SPACE_FREE, space);
}

ECode File::GetUsableSpace(
    /* [out] */ Long* space)
{
    VALIDATE_NOT_NULL(space);

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        AutoPtr<IPermission> perm;
        CRuntimePermission::New(String("getFileSystemAttributes"), IID_IPermission, (IInterface**)&perm);
        FAIL_RETURN(security->CheckPermission(perm));
        FAIL_RETURN(security->CheckRead(mPath));
    }
    if (IsInvalid()) {
        *space = 0;
        return NOERROR;
    }
    return GetFS()->GetSpace(this, FileSystem::SPACE_USABLE, space);
}

ECode File::CreateTempFile(
    /* [in] */ const String& prefix,
    /* [in] */ const String& _suffix,
    /* [in] */ IFile* directory,
    /* [out] */ IFile** temp)
{
    VALIDATE_NOT_NULL(temp);

    if (prefix.GetLength() < 3) {
        Logger::E("File", "Prefix string too short");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String suffix = _suffix;
    if (suffix.IsNull()) {
        suffix = ".tmp";
    }

    AutoPtr<IFile> tmpdir = directory;
    if (tmpdir == nullptr) {
        String dir;
        FAIL_RETURN(System::GetProperty(String("como.io.tmpdir"), String("."), &dir));
        CFile::New(dir, IID_IFile, (IInterface**)&tmpdir);
    }

    AutoPtr<IFile> f;
    Integer attrs;
    do {
        f = nullptr;
        TempDirectory::GenerateFile(prefix, suffix, tmpdir, &f);
    } while (GetFS()->GetBooleanAttributes(f, &attrs),
            (attrs & FileSystem::BA_EXISTS) != 0);

    String path;
    f->GetPath(&path);
    Boolean succeeded;
    if (GetFS()->CreateFileExclusively(path, &succeeded), !succeeded) {
        Logger::E("File", "Unable to create temporary file");
        return E_IO_EXCEPTION;
    }

    f.MoveTo(temp);
    return NOERROR;
}

ECode File::CreateTempFile(
    /* [in] */ const String& prefix,
    /* [in] */ const String& suffix,
    /* [out] */ IFile** temp)
{
    return CreateTempFile(prefix, suffix, nullptr, temp);
}

ECode File::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Integer* result)
{
    return GetFS()->Compare(this, IFile::Probe(other), result);
}

ECode File::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    if (obj != nullptr && IFile::Probe(obj) != nullptr) {
        Integer result;
        CompareTo(obj, &result);
        *same = result == 0;
        return NOERROR;
    }
    *same = false;
    return NOERROR;
}

ECode File::GetHashCode(
    /* [out] */ Integer* hash)
{
    return GetFS()->GetHashCode(this, hash);
}

ECode File::ToString(
    /* [out] */ String* desc)
{
    return GetPath(desc);
}

//-------------------------------------------------------------------------

ECode File::TempDirectory::GenerateFile(
    /* [in] */ const String& prefix,
    /* [in] */ const String& suffix,
    /* [in] */ IFile* dir,
    /* [out] */ IFile** temp)
{
    Long n = Math::RandomLongInternal();
    if (n == ILong::MIN_VALUE) {
        n = 0;      // corner case
    }
    else {
        n = Math::Abs(n);
    }

    String name = prefix + StringUtils::ToString(n) + suffix;
    AutoPtr<IFile> f;
    CFile::New(dir, name, IID_IFile, (IInterface**)&f);
    String fName; Boolean invalid;
    if ((f->GetName(&fName), !name.Equals(fName)) || File::From(f)->IsInvalid()) {
        Logger::E("File", "Unable to create temporary file");
        return E_IO_EXCEPTION;
    }
    f.MoveTo(temp);
    return NOERROR;
}

}
}
