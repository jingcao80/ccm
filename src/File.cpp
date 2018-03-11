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

namespace ccm {

File::File(
    /* [in] */ const char* path)
    : mReader(this)
{
    if (path == nullptr || path[0] == '\0') {
        mFd = NULL;
        return;
    }

    mFd = fopen(path, "r");
}

File::File(
    /* [in] */ const String& path)
    : mReader(this)
{
    if (path.IsNullOrEmpty()) {
        mFd = NULL;
        return;
    }

    mFd = fopen(path, "r");
}

File::~File()
{
    if (mFd != nullptr) {
        fclose(mFd);
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
