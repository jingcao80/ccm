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
#include "ccm/io/DefaultFileSystem.h"
#include "ccm/io/File.h"
#include <ccmlogger.h>

using ccm::core::IID_IComparable;
using ccm::core::ISecurityManager;
using ccm::core::System;
using ccm::io::IID_ISerializable;

namespace ccm {
namespace io {

CCM_INTERFACE_IMPL_3(File, SyncObject, IFile, ISerializable, IComparable);

AutoPtr<FileSystem> File::GetFS()
{
    static AutoPtr<FileSystem> FS = DefaultFileSystem::GetFileSystem();
    return FS;
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
        return ccm::core::E_NULL_POINTER_EXCEPTION;
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
        return ccm::core::E_NULL_POINTER_EXCEPTION;
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
        return ccm::core::E_NULL_POINTER_EXCEPTION;
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
    if (GetSeparatorChar() != '/') {
        p = p.Replace('/', GetSeparatorChar());
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
    if (GetSeparatorChar() != '/') {
        p = p.Replace(GetSeparatorChar(), '/');
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
    /* [out] */ Boolean* read)
{
    VALIDATE_NOT_NULL(read);

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckRead(mPath));
    }
    if (IsInvalid()) {
        *read = false;
        return NOERROR;
    }
    return GetFS()->CheckAccess(this, FileSystem::ACCESS_READ, read);
}

ECode File::CanWrite(
    /* [out] */ Boolean* write)
{
    VALIDATE_NOT_NULL(write);

    AutoPtr<ISecurityManager> security = System::GetSecurityManager();
    if (security != nullptr) {
        FAIL_RETURN(security->CheckWrite(mPath));
    }
    if (IsInvalid()) {
        *write = false;
        return NOERROR;
    }
    return GetFS()->CheckAccess(this, FileSystem::ACCESS_WRITE, write);
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


ECode File::CompareTo(
    /* [in] */ IInterface* other,
    /* [out] */ Integer* result)
{
    return NOERROR;
}

Boolean File::IsInvalid()
{
    return false;
}

}
}
