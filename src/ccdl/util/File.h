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

#ifndef __CCDL_FILE_H__
#define __CCDL_FILE_H__

#include "String.h"

#include <stdio.h>

namespace ccdl {

class File
{
private:
    class BufferedReader
    {
    public:
        BufferedReader(
            /* [in] */ File* owner);

        int Peek();

        int Read();

        void Unread(
            /* [in] */ int c);

        int GetColumnNo();

        int GetLineNo();

    private:
        int FRead();

    private:
        static constexpr int BUFFER_SIZE = 512;

        File* mOwner;
        int mColumnNo;
        int mPrevColumnNo;
        int mLineNo;
        char mBuffer[BUFFER_SIZE];
        int mPosition;
        int mCapacity;
        bool mEof;
        bool mError;
    };

public:
    File(
        /* [in] */ const char* path,
        /* [in] */ int mode);

    File(
        /* [in] */ const String& path,
        /* [in] */ int mode);

    ~File();

    inline bool IsValid();

    inline String GetPath();

    inline int Peek();

    inline int Read();

    inline void Unread(
        /* [in] */ int c);

    void Write(
        /* [in] */ const void* data,
        /* [in] */ size_t size);

    void Flush();

    void Close();

    inline int GetColumnNo();

    inline int GetLineNo();

public:
    static constexpr int READ = 0x1;
    static constexpr int WRITE = 0x2;
    static constexpr int APPEND = 0x4;

private:
    String mPath;
    FILE* mFd;
    int mMode;
    BufferedReader mReader;
};

bool File::IsValid()
{
    return mFd != nullptr;
}

String File::GetPath()
{
    return mPath;
}

int File::Peek()
{
    return mReader.Peek();
}

int File::Read()
{
    return mReader.Read();
}

void File::Unread(
    /* [in] */ int c)
{
    mReader.Unread(c);
}

int File::GetColumnNo()
{
    return mReader.GetColumnNo();
}

int File::GetLineNo()
{
    return mReader.GetLineNo();
}

}

#endif //__CCDL_FILE_H__
