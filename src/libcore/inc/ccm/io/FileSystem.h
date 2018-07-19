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

#ifndef __CCM_IO_FILESYSTEM_H__
#define __CCM_IO_FILESYSTEM_H__

#include "ccm/core/SyncObject.h"
#include "ccm.io.IFile.h"

using ccm::core::SyncObject;

namespace ccm {
namespace io {

static CONS_PROI_5 void StaticInitializeFileSystem();

class FileSystem
    : public SyncObject
{
public:
    virtual ECode GetSeparator(
        /* [out] */ Char* separator) = 0;

    virtual ECode GetPathSeparator(
        /* [out] */ Char* pathSeparator) = 0;

    virtual ECode Normalize(
        /* [in] */ const String& path,
        /* [out] */ String* normalizedPath) = 0;

    virtual ECode PrefixLength(
        /* [in] */ const String& path,
        /* [out] */ Integer* length) = 0;

    virtual ECode Resolve(
        /* [in] */ const String& parent,
        /* [in] */ const String& child,
        /* [out] */ String* resolvedPath) = 0;

    virtual ECode GetDefaultParent(
        /* [out] */ String* parent) = 0;

    virtual ECode FromURIPath(
        /* [in] */ const String& path,
        /* [out] */ String* resultPath) = 0;

    virtual ECode IsAbsolute(
        /* [in] */ IFile* f,
        /* [out] */ Boolean* absolute) = 0;

    virtual ECode Resolve(
        /* [in] */ IFile* f,
        /* [out] */ String* path) = 0;

    virtual ECode Canonicalize(
        /* [in] */ const String& path,
        /* [out] */ String* canonicalizedPath) = 0;

    virtual ECode GetBooleanAttributes(
        /* [in] */ IFile* f,
        /* [out] */ Integer* attr) = 0;

    virtual ECode CheckAccess(
        /* [in] */ IFile* f,
        /* [in] */ Integer access,
        /* [out] */ Boolean* result) = 0;

    virtual ECode SetPermission(
        /* [in] */ IFile* f,
        /* [in] */ Integer access,
        /* [in] */ Boolean enable,
        /* [in] */ Boolean owneronly,
        /* [out] */ Boolean* succeeded) = 0;

    virtual ECode GetLastModifiedTime(
        /* [in] */ IFile* f,
        /* [out] */ Long* time) = 0;

    virtual ECode GetLength(
        /* [in] */ IFile* f,
        /* [out] */ Long* length) = 0;

    virtual ECode CreateFileExclusively(
        /* [in] */ const String& pathname,
        /* [out] */ Boolean* succeeded) = 0;

    virtual ECode Delete(
        /* [in] */ IFile* f,
        /* [out] */ Boolean* succeeded) = 0;

    virtual ECode List(
        /* [in] */ IFile* f,
        /* [out, callee] */ Array<String>* elements) = 0;

    virtual ECode CreateDirectory(
        /* [in] */ IFile* f,
        /* [out] */ Boolean* succeeded) = 0;

    virtual ECode Rename(
        /* [in] */ IFile* f1,
        /* [in] */ IFile* f2,
        /* [out] */ Boolean* succeeded) = 0;

    virtual ECode SetLastModifiedTime(
        /* [in] */ IFile* f,
        /* [in] */ Long time,
        /* [out] */ Boolean* succeeded) = 0;

    virtual ECode SetReadOnly(
        /* [in] */ IFile* f,
        /* [out] */ Boolean* succeeded) = 0;

    virtual ECode ListRoots(
        /* [out, callee] */ Array<IFile*>* roots) = 0;

    virtual ECode GetSpace(
        /* [in] */ IFile* f,
        /* [in] */ Integer t,
        /* [out] */ Long* space) = 0;

    virtual ECode Compare(
        /* [in] */ IFile* f1,
        /* [in] */ IFile* f2,
        /* [out] */ Integer* result) = 0;

    virtual ECode GetHashCode(
        /* [in] */ IFile* f,
        /* [out] */ Integer* hash) = 0;

private:
    static Boolean GetBooleanProperty(
        /* [in] */ const String& prop,
        /* [in] */ Boolean defaultVal);

    static ECode StaticInitialize();

public:
    static constexpr Integer BA_EXISTS    = 0x01;
    static constexpr Integer BA_REGULAR   = 0x02;
    static constexpr Integer BA_DIRECTORY = 0x04;
    static constexpr Integer BA_HIDDEN    = 0x08;

    static constexpr Integer ACCESS_READ    = 0x04;
    static constexpr Integer ACCESS_WRITE   = 0x02;
    static constexpr Integer ACCESS_EXECUTE = 0x01;
    static constexpr Integer ACCESS_OK      = 0x08;

    static constexpr Integer SPACE_TOTAL  = 0;
    static constexpr Integer SPACE_FREE   = 1;
    static constexpr Integer SPACE_USABLE = 2;

protected:
    static Boolean sUseCanonCaches;
    static Boolean sUseCanonPrefixCache;

    friend void StaticInitializeFileSystem();
};

}
}

#endif // __CCM_IO_FILESYSTEM_H__
