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

#ifndef __CCM_IO_FILE_H__
#define __CCM_IO_FILE_H__

#include "ccm/core/SyncObject.h"
#include "ccm/io/FileSystem.h"
#include "ccm.core.IComparable.h"
#include "ccm.io.IFile.h"
#include "ccm.io.ISerializable.h"
#include "ccm.net.IURI.h"
#include "ccm.net.IURL.h"
#include <ccmautoptr.h>

using ccm::core::IComparable;
using ccm::core::SyncObject;
using ccm::net::IURI;
using ccm::net::IURL;

namespace ccm {
namespace io {

class File
    : public SyncObject
    , public IFile
    , public ISerializable
    , public IComparable
{
public:
    CCM_INTERFACE_DECL();

    ECode GetPrefixLength(
        /* [out] */ Integer* length) override;

    static Char GetSeparatorChar();

    ECode Constructor(
        /* [in] */ const String& pathname,
        /* [in] */ Integer prefixLength);

    ECode Constructor(
        /* [in] */ const String& child,
        /* [in] */ IFile* parent);

    ECode Constructor(
        /* [in] */ const String& pathname);

    ECode Constructor(
        /* [in] */ const String& parent,
        /* [in] */ const String& child);

    ECode Constructor(
        /* [in] */ IFile* parent,
        /* [in] */ const String& child);

    ECode Constructor(
        /* [in] */ IURI* uri);

    ECode GetName(
        /* [out] */ String* name) override;

    ECode GetParent(
        /* [out] */ String* parent) override;

    ECode GetParentFile(
        /* [out] */ IFile** parent) override;

    ECode GetPath(
        /* [out] */ String* path) override;

    ECode IsAbsolute(
        /* [out] */ Boolean* absolute) override;

    ECode GetAbsolutePath(
        /* [out] */ String* path) override;

    ECode GetAbsoluteFile(
        /* [out] */ IFile** f) override;

    ECode GetCanonicalPath(
        /* [out] */ String* path) override;

    ECode GetCanonicalFile(
        /* [out] */ IFile** f) override;

    ECode ToURL(
        /* [out] */ IURL** url) override;

    ECode ToURI(
        /* [out] */ IURI** id) override;

    ECode CanRead(
        /* [out] */ Boolean* read) override;

    ECode CanWrite(
        /* [out] */ Boolean* write) override;

    ECode Exists(
        /* [out] */ Boolean* existed) override;

    ECode IsDirectory(
        /* [out] */ Boolean* directory) override;

    ECode IsFile(
        /* [out] */ Boolean* file) override;

    ECode IsHidden(
        /* [out] */ Boolean* hidden) override;

    ECode LastModified(
        /* [out] */ Long* time) override;

    ECode GetLength(
        /* [out] */ Long* len) override;

    static File* From(
        /* [in] */ IFile* f);









    ECode CompareTo(
        /* [in] */ IInterface* other,
        /* [out] */ Integer* result) override;

protected:
    Boolean IsInvalid();

private:
    static AutoPtr<FileSystem> GetFS();

    static String Slashify(
        /* [in] */ const String& path,
        /* [in] */ Boolean isDirectory);

private:
    String mPath;

    /**
     * The length of this abstract pathname's prefix, or zero if it has no
     * prefix.
     */
    Integer mPrefixLength;
};

inline File* File::From(
    /* [in] */ IFile* f)
{
    return (File*)f;
}

}
}

#endif // __CCM_IO_FILE_H__
