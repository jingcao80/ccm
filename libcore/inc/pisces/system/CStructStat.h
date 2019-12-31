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

#ifndef __PISCES_SYSTEM_CSTRUCTSTAT_H__
#define __PISCES_SYSTEM_CSTRUCTSTAT_H__

#include "pisces.system.IStructStat.h"
#include "_pisces_system_CStructStat.h"
#include <ccmobject.h>

namespace pisces {
namespace system {

Coclass(CStructStat)
    , public Object
    , public IStructStat
{
public:
    CCM_INTERFACE_DECL();

    CCM_OBJECT_DECL();

    ECode Constructor(
        /* [in] */ Long st_dev,
        /* [in] */ Long st_ino,
        /* [in] */ Integer st_mode,
        /* [in] */ Long st_nlink,
        /* [in] */ Integer st_uid,
        /* [in] */ Integer st_gid,
        /* [in] */ Long st_rdev,
        /* [in] */ Long st_size,
        /* [in] */ Long st_atime,
        /* [in] */ Long st_mtime,
        /* [in] */ Long st_ctime,
        /* [in] */ Long st_blksize,
        /* [in] */ Long st_blocks);

    ECode GetAtime(
        /* [out] */ Long* atime) override;

    ECode GetBlksize(
        /* [out] */ Long* blksize) override;

    ECode GetBlocks(
        /* [out] */ Long* blocks) override;

    ECode GetCtime(
        /* [out] */ Long* ctime) override;

    ECode GetDev(
        /* [out] */ Long* dev) override;

    ECode GetGid(
        /* [out] */ Integer* gid) override;

    ECode GetIno(
        /* [out] */ Long* ino) override;

    ECode GetMode(
        /* [out] */ Integer* mode) override;

    ECode GetMtime(
        /* [out] */ Long* mtime) override;

    ECode GetNlink(
        /* [out] */ Long* nlink) override;

    ECode GetRdev(
        /* [out] */ Long* rdev) override;

    ECode GetSize(
        /* [out] */ Long* size) override;

    ECode GetUid(
        /* [out] */ Integer* uid) override;

private:

    /** Device ID of device containing file. */
    Long mDev;

    /** File serial number (inode). */
    Long mIno;

    /** Mode (permissions) of file. */
    Integer mMode;

    /** Number of hard links to the file. */
    Long mNlink;

    /** User ID of file. */
    Integer mUid;

    /** Group ID of file. */
    Integer mGid;

    /** Device ID (if file is character or block special). */
    Long mRdev;

    /**
     * For regular files, the file size in bytes.
     * For symbolic links, the length in bytes of the pathname contained in the symbolic link.
     * For a shared memory object, the length in bytes.
     * For a typed memory object, the length in bytes.
     * For other file types, the use of this field is unspecified.
     */
    Long mSize;

    /** Time of last access. */
    Long mAtime;

    /** Time of last data modification. */
    Long mMtime;

    /** Time of last status change. */
    Long mCtime;

    /**
     * A file system-specific preferred I/O block size for this object.
     * For some file system types, this may vary from file to file.
     */
    Long mBlksize;

    /** Number of blocks allocated for this object. */
    Long mBlocks;
};

}
}

#endif // __PISCES_SYSTEM_CSTRUCTSTAT_H__
