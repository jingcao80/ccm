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

#ifndef __CDLC_FILE_H__
#define __CDLC_FILE_H__

#include "util/LightRefBase.h"
#include "util/String.h"
#include <cstdio>

namespace cdlc {

class File
    : public LightRefBase
{
public:
    File(
        /* [in] */ const String& absolutePath,
        /* [in] */ int mode);

    ~File();

    inline bool IsValid() const;

    inline String GetPath() const;

    String RawGetLine();

    inline bool IsEof() const;

private:
    void OpenFile();

public:
    static constexpr int READ = 0x1;
    static constexpr int WRITE = 0x2;
    static constexpr int APPEND = 0x4;

private:
    String mPath;
    FILE* mFd = nullptr;
    int mMode = READ;
    bool mEof = false;
    bool mError = false;

    char* mLine = nullptr;
};

bool File::IsValid() const
{
    return mFd != nullptr;
}

String File::GetPath() const
{
    return mPath;
}

bool File::IsEof() const
{
    return mEof;
}

}

#endif // __CDLC_FILE_H__
