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

#include "pisces/system/CStructStat.h"

namespace pisces {
namespace system {

CCM_INTERFACE_IMPL_1(CStructStat, Object, IStructStat);

CCM_OBJECT_IMPL(CStructStat);

ECode CStructStat::Constructor(
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
    /* [in] */ Long st_blocks)
{
    mDev = st_dev;
    mIno = st_ino;
    mMode = st_mode;
    mNlink = st_nlink;
    mUid = st_uid;
    mGid = st_gid;
    mRdev = st_rdev;
    mSize = st_size;
    mAtime = st_atime;
    mMtime = st_mtime;
    mCtime = st_ctime;
    mBlksize = st_blksize;
    mBlocks = st_blocks;
    return NOERROR;
}

ECode CStructStat::GetAtime(
    /* [out] */ Long* atime)
{
    VALIDATE_NOT_NULL(atime);

    *atime = mAtime;
    return NOERROR;
}

ECode CStructStat::GetBlksize(
    /* [out] */ Long* blksize)
{
    VALIDATE_NOT_NULL(blksize);

    *blksize = mBlksize;
    return NOERROR;
}

ECode CStructStat::GetBlocks(
    /* [out] */ Long* blocks)
{
    VALIDATE_NOT_NULL(blocks);

    *blocks = mBlocks;
    return NOERROR;
}

ECode CStructStat::GetCtime(
    /* [out] */ Long* ctime)
{
    VALIDATE_NOT_NULL(ctime);

    *ctime = mCtime;
    return NOERROR;
}

ECode CStructStat::GetDev(
    /* [out] */ Long* dev)
{
    VALIDATE_NOT_NULL(dev);

    *dev = mDev;
    return NOERROR;
}

ECode CStructStat::GetGid(
    /* [out] */ Integer* gid)
{
    VALIDATE_NOT_NULL(gid);

    *gid = mGid;
    return NOERROR;
}

ECode CStructStat::GetIno(
    /* [out] */ Long* ino)
{
    VALIDATE_NOT_NULL(ino);

    *ino = mIno;
    return NOERROR;
}

ECode CStructStat::GetMode(
    /* [out] */ Integer* mode)
{
    VALIDATE_NOT_NULL(mode);

    *mode = mMode;
    return NOERROR;
}

ECode CStructStat::GetMtime(
    /* [out] */ Long* mtime)
{
    VALIDATE_NOT_NULL(mtime);

    *mtime = mMtime;
    return NOERROR;
}

ECode CStructStat::GetNlink(
    /* [out] */ Long* nlink)
{
    VALIDATE_NOT_NULL(nlink);

    *nlink = mNlink;
    return NOERROR;
}

ECode CStructStat::GetRdev(
    /* [out] */ Long* rdev)
{
    VALIDATE_NOT_NULL(rdev);

    *rdev = mRdev;
    return NOERROR;
}

ECode CStructStat::GetSize(
    /* [out] */ Long* size)
{
    VALIDATE_NOT_NULL(size);

    *size = mSize;
    return NOERROR;
}

ECode CStructStat::GetUid(
    /* [out] */ Integer* uid)
{
    VALIDATE_NOT_NULL(uid);

    *uid = mUid;
    return NOERROR;
}

}
}
