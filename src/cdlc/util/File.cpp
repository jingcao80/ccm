//=========================================================================
// Copyright (C) 2018 The C++ Component Model(COMO) Open Source Project
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

#include "util/File.h"
#include "util/Properties.h"
#include <cstdlib>
#include <unistd.h>

namespace cdlc {

File::File(
    /* [in] */ const String& path,
    /* [in] */ int mode)
    : mMode(mode)
{
    if (path.IsEmpty()) {
        return;
    }
    if (!path.StartsWith("/")) {
        for (String searchPath : Properties::Get().GetSearchPaths()) {
            String absolutePath = searchPath + "/" + path;
            if (access(absolutePath.string(), F_OK) == 0) {
                char* canonicalPath = realpath(absolutePath.string(), nullptr);
                mPath = canonicalPath;
                free(canonicalPath);
                break;
            }
        }
    }
    else {
        char* canonicalPath = realpath(path.string(), nullptr);
        mPath = canonicalPath;
        free(canonicalPath);
    }

    OpenFile();
}

File::~File()
{
    if (mFd != nullptr) {
        fclose(mFd);
    }
    if (mLine != nullptr) {
        free(mLine);
    }
}

void File::OpenFile()
{
    if (mMode & APPEND) {
        mFd = fopen(mPath, "a+");
    }
    else if (mMode & WRITE) {
        mFd = fopen(mPath, "w+");
    }
    else {
        mFd = fopen(mPath, "r");
    }
}

String File::RawGetLine()
{
    size_t n;
    if (getline(&mLine, &n, mFd) != -1) {
        return String(mLine);
    }
    else {
        if (feof(mFd) != 0) {
            mEof = true;
        }
        else {
            mError = ferror(mFd) != 0;
        }
        return nullptr;
    }
}

}
