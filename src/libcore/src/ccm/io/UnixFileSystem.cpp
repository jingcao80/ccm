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
#include "ccm/io/UnixFileSystem.h"

using ccm::core::System;

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
        }
        *canonicalizedPath = res;
        return NOERROR;
    }
}

ECode UnixFileSystem::GetBooleanAttributes(
    /* [in] */ IFile* f,
    /* [out] */ Integer* attr)
{
    return NOERROR;
}

}
}
