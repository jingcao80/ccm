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

#include "File.h"

namespace ccdl {

File::BufferedReader::BufferedReader(
    /* [in] */ File* owner)
    : mOwner(owner)
    , mColumnNo(0)
    , mPrevColumnNo(0)
    , mLineNo(1)
    , mPosition(-1)
    , mCapacity(-1)
    , mEof(false)
    , mError(false)
{}

int File::BufferedReader::GetColumnNo()
{
    return mColumnNo;
}

int File::BufferedReader::GetLineNo()
{
    return mLineNo;
}

//-----------------------------------------------------------------

File::File(
    /* [in] */ const char* path,
    /* [in] */ int mode)
    : mMode(mode)
    , mReader(this)
{
    if (path == nullptr || path[0] == '\0') {
        mFd = nullptr;
        return;
    }

    if (mode & APPEND) {
        mFd = fopen(path, "a+");
    }
    else if (mode & WRITE) {
        mFd = fopen(path, "w+");
    }
    else {
        mFd = fopen(path, "r");
    }

    mPath = path;
}

File::File(
    /* [in] */ const String& path,
    /* [in] */ int mode)
    : mMode(mode)
    , mReader(this)
{
    if (path.IsNullOrEmpty()) {
        mFd = nullptr;
        return;
    }

    if (mode & APPEND) {
        mFd = fopen(path, "a+");
    }
    else if (mode & WRITE) {
        mFd = fopen(path, "w+");
    }
    else {
        mFd = fopen(path, "r");
    }

    mPath = path;
}

File::~File()
{
    if (mFd != nullptr) {
        fclose(mFd);
        mFd = nullptr;
    }
}

void File::Write(
    /* [in] */ const void* data,
    /* [in] */ size_t size)
{
    if (data == nullptr || size == 0) return;

    if (mMode & (WRITE | APPEND) && mFd != nullptr) {
        fwrite(data, size, 1, mFd);
    }
}

void File::Flush()
{
    if (mMode & (WRITE | APPEND) && mFd != nullptr) {
        fflush(mFd);
    }
}

void File::Close()
{
    if (mFd != nullptr) {
        fclose(mFd);
        mFd = nullptr;
    }
}


int File::BufferedReader::Peek()
{
    if (mPosition >= mCapacity) {
        int ret = FRead();
        if (ret == -1) return -1;
    }

    return mBuffer[mPosition];
}

int File::BufferedReader::Read()
{
    int ret = Peek();
    if (ret == -1) return -1;

    mPosition++;
    if (ret != '\n') {
        mColumnNo++;
    }
    else {
        mPrevColumnNo = mColumnNo;
        mColumnNo = 0;
        mLineNo++;
    }
    return ret;
}

void File::BufferedReader::Unread(
    /* [in] */ int c)
{
    if (mPosition <= 0) return;

    mBuffer[--mPosition] = c;
    if (c != '\n') {
        mColumnNo--;
    }
    else {
        mLineNo--;
        mColumnNo = mPrevColumnNo;
    }
}

int File::BufferedReader::FRead()
{
    if (mEof || mError) return -1;

    memset(mBuffer, 0, BUFFER_SIZE);
    size_t count = fread(mBuffer, 1, BUFFER_SIZE, mOwner->mFd);
    if (count < BUFFER_SIZE) {
        if (feof(mOwner->mFd) != 0) {
            mEof = true;
        }
        else mError = ferror(mOwner->mFd) != 0;
    }
    mCapacity = count;
    mPosition = 0;
    return count == 0? -1 : count;
}

}
