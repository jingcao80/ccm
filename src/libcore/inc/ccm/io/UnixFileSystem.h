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

#ifndef __CCM_IO_UNIXFILESYSTEM_H__
#define __CCM_IO_UNIXFILESYSTEM_H__

#include "ccm/io/ExpiringCache.h"
#include "ccm/io/FileSystem.h"

namespace ccm {
namespace io {

class UnixFileSystem
    : public FileSystem
{
public:
    UnixFileSystem();

    ECode GetSeparator(
        /* [out] */ Char* separator) override;

    ECode GetPathSeparator(
        /* [out] */ Char* pathSeparator) override;

    ECode Normalize(
        /* [in] */ const String& path,
        /* [out] */ String* normalizedPath) override;

    ECode PrefixLength(
        /* [in] */ const String& path,
        /* [out] */ Integer* length) override;

    ECode Resolve(
        /* [in] */ const String& parent,
        /* [in] */ const String& child,
        /* [out] */ String* resolvedPath) override;

    ECode GetDefaultParent(
        /* [out] */ String* parent) override;

    ECode FromURIPath(
        /* [in] */ const String& path,
        /* [out] */ String* resultPath) override;

    ECode IsAbsolute(
        /* [in] */ IFile* f,
        /* [out] */ Boolean* absolute) override;

    ECode Resolve(
        /* [in] */ IFile* f,
        /* [out] */ String* path) override;

    ECode Canonicalize(
        /* [in] */ const String& path,
        /* [out] */ String* canonicalizedPath) override;

    ECode GetBooleanAttributes(
        /* [in] */ IFile* f,
        /* [out] */ Integer* attr) override;

    ECode CheckAccess(
        /* [in] */ IFile* f,
        /* [in] */ Integer access,
        /* [out] */ Boolean* result) override;

    ECode GetLastModifiedTime(
        /* [in] */ IFile* f,
        /* [out] */ Long* time) override;

    ECode GetLength(
        /* [in] */ IFile* f,
        /* [out] */ Long* length) override;

    ECode SetPermission(
        /* [in] */ IFile* f,
        /* [in] */ Integer access,
        /* [in] */ Boolean enable,
        /* [in] */ Boolean owneronly,
        /* [out] */ Boolean* succeeded) override;

    ECode CreateFileExclusively(
        /* [in] */ const String& pathname,
        /* [out] */ Boolean* succeeded) override;

    ECode Delete(
        /* [in] */ IFile* f,
        /* [out] */ Boolean* succeeded) override;

    ECode List(
        /* [in] */ IFile* f,
        /* [out, callee] */ Array<String>* elements) override;

    ECode CreateDirectory(
        /* [in] */ IFile* f,
        /* [out] */ Boolean* succeeded) override;

    ECode Rename(
        /* [in] */ IFile* f1,
        /* [in] */ IFile* f2,
        /* [out] */ Boolean* succeeded) override;

    ECode SetLastModifiedTime(
        /* [in] */ IFile* f,
        /* [in] */ Long time,
        /* [out] */ Boolean* succeeded) override;

    ECode SetReadOnly(
        /* [in] */ IFile* f,
        /* [out] */ Boolean* succeeded) override;

    ECode ListRoots(
        /* [out, callee] */ Array<IFile*>* roots) override;

    ECode GetSpace(
        /* [in] */ IFile* f,
        /* [in] */ Integer t,
        /* [out] */ Long* space) override;

    ECode Compare(
        /* [in] */ IFile* f1,
        /* [in] */ IFile* f2,
        /* [out] */ Integer* result) override;

    ECode GetHashCode(
        /* [in] */ IFile* f,
        /* [out] */ Integer* hash) override;

protected:
    static String ParentOrNull(
        /* [in] */ const String& path);

private:
    ECode Canonicalize0(
        /* [in] */ const String& path,
        /* [out] */ String* canonicalizedPath);

    Integer GetBooleanAttributes0(
        /* [in] */ const String& abspath);

    Boolean CheckAccess0(
        /* [in] */ IFile* f,
        /* [in] */ Integer access);

    Long GetLastModifiedTime0(
        /* [in] */ IFile* f);

    Long GetLength0(
        /* [in] */ IFile* f);

    Boolean SetPermission0(
        /* [in] */ IFile* f,
        /* [in] */ Integer access,
        /* [in] */ Boolean enable,
        /* [in] */ Boolean owneronly);

    ECode CreateFileExclusively0(
        /* [in] */ const String& path,
        /* [out] */ Boolean* succeeded);

    Boolean Delete0(
        /* [in] */ IFile* f);

    ECode List0(
        /* [in] */ IFile* f,
        /* [out, callee] */ Array<String>* elements);

    Boolean CreateDirectory0(
        /* [in] */ IFile* f);

    Boolean Rename0(
        /* [in] */ IFile* f1,
        /* [in] */ IFile* f2);

    Boolean SetLastModifiedTime0(
        /* [in] */ IFile* f,
        /* [in] */ Long time);

    Boolean SetReadOnly0(
        /* [in] */ IFile* f);

    Long GetSpace0(
        /* [in] */ IFile* f,
        /* [in] */ Integer t);

private:
    Char mSlash;
    Char mColon;
    String mCcmHome;
    ExpiringCache mCache;
    ExpiringCache mCcmHomePrefixCache;
};

inline UnixFileSystem::UnixFileSystem()
{}

}
}

#endif // __CCM_IO_UNIXFILESYSTEM_H__
