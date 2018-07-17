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

private:
    Char mSlash;
    Char mColon;
    String mCcmHome;
};

}
}

#endif // __CCM_IO_UNIXFILESYSTEM_H__
