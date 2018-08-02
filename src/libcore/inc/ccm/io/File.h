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
#include "ccm.io.IFileFilter.h"
#include "ccm.io.IFilenameFilter.h"
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
private:
    enum class PathStatus
    {
        UNKNOWN,
        INVALID,
        CHECKED,
    };

    class TempDirectory
    {
    public:
        static ECode GenerateFile(
            /* [in] */ const String& prefix,
            /* [in] */ const String& suffix,
            /* [in] */ IFile* dir,
            /* [out] */ IFile** temp);

    private:
        TempDirectory();
    };

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
        /* [out] */ Boolean* readable) override;

    ECode CanWrite(
        /* [out] */ Boolean* writeable) override;

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

    ECode CreateNewFile(
        /* [out] */ Boolean* succeeded) override;

    ECode Delete(
        /* [out] */ Boolean* succeeded = nullptr) override;

    ECode DeleteOnExit() override;

    ECode List(
        /* [out, callee] */ Array<String>* files) override;

    ECode List(
        /* [in] */ IFilenameFilter* filter,
        /* [out, callee] */ Array<String>* files) override;

    ECode ListFiles(
        /* [out, callee] */ Array<IFile*>* files) override;

    ECode ListFiles(
        /* [in] */ IFilenameFilter* filter,
        /* [out, callee] */ Array<IFile*>* files) override;

    ECode ListFiles(
        /* [in] */ IFileFilter* filter,
        /* [out, callee] */ Array<IFile*>* files) override;

    ECode Mkdir(
        /* [out] */ Boolean* succeeded) override;

    ECode Mkdirs(
        /* [out] */ Boolean* succeeded) override;

    ECode RenameTo(
        /* [in] */ IFile* dest,
        /* [out] */ Boolean* succeeded) override;

    ECode SetLastModified(
        /* [in] */ Long time,
        /* [out] */ Boolean* succeeded) override;

    ECode SetReadOnly(
        /* [out] */ Boolean* succeeded) override;

    ECode SetWritable(
        /* [in] */ Boolean writable,
        /* [in] */ Boolean ownerOnly,
        /* [out] */ Boolean* succeeded) override;

    ECode SetWritable(
        /* [in] */ Boolean writable,
        /* [out] */ Boolean* succeeded) override;

    ECode SetReadable(
        /* [in] */ Boolean readable,
        /* [in] */ Boolean ownerOnly,
        /* [out] */ Boolean* succeeded) override;

    ECode SetReadable(
        /* [in] */ Boolean readable,
        /* [out] */ Boolean* succeeded) override;

    ECode SetExecutable(
        /* [in] */ Boolean executable,
        /* [in] */ Boolean ownerOnly,
        /* [out] */ Boolean* succeeded) override;

    ECode SetExecutable(
        /* [in] */ Boolean executable,
        /* [out] */ Boolean* succeeded) override;

    ECode CanExecute(
        /* [out] */ Boolean* executable) override;

    static ECode ListRoots(
        /* [out, callee] */ Array<IFile*>* roots);

    ECode GetTotalSpace(
        /* [out] */ Long* space) override;

    ECode GetFreeSpace(
        /* [out] */ Long* space) override;

    ECode GetUsableSpace(
        /* [out] */ Long* space) override;

    static ECode CreateTempFile(
        /* [in] */ const String& prefix,
        /* [in] */ const String& suffix,
        /* [in] */ IFile* directory,
        /* [out] */ IFile** temp);

    static ECode CreateTempFile(
        /* [in] */ const String& prefix,
        /* [in] */ const String& suffix,
        /* [out] */ IFile** temp);

    ECode CompareTo(
        /* [in] */ IInterface* other,
        /* [out] */ Integer* result) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* same) override;

    ECode GetHashCode(
        /* [out] */ Integer* hash) override;

    ECode ToString(
        /* [out] */ String* desc) override;

    static File* From(
        /* [in] */ IFile* f);

protected:
    Boolean IsInvalid();

private:
    static AutoPtr<FileSystem> GetFS();

    static String Slashify(
        /* [in] */ const String& path,
        /* [in] */ Boolean isDirectory);

private:
    String mPath;

    PathStatus mStatus = PathStatus::UNKNOWN;

    /**
     * The length of this abstract pathname's prefix, or zero if it has no
     * prefix.
     */
    Integer mPrefixLength;

    friend class FileInputStream;
    friend class FileOutputStream;
};

inline File* File::From(
    /* [in] */ IFile* f)
{
    return (File*)f;
}

}
}

#endif // __CCM_IO_FILE_H__
