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

#ifndef __CDLC_MEMORYFILEREADER_H__
#define __CDLC_MEMORYFILEREADER_H__

#include "util/AutoPtr.h"
#include "util/File.h"
#include "util/MemoryBuffer.h"
#include "util/Reader.h"
#include <cstddef>
#include <cstdio>
#include <vector>

namespace cdlc {

class MemoryFileReader
    : public Reader
{
public:
    class FileInfo
    {
    public:
        size_t mBeginPos;
        size_t mEndPos;
        String mFilePath;
        int mLineNo;
    };

public:
    MemoryFileReader(
        /* [in] */ const String& filePath);

    inline MemoryFileReader(
        /* [in] */ File* file);

    bool ReadIn(
        /* [in] */ bool recursive);

    char PeekChar() override;

    char GetChar() override;

    void UnGetChar() override;

    String GetCurrentFilePath() override;

    int GetCurrentLineNumber() override;

    int GetCurrentColumnNumber() override;

    void Mark() override;

    void Reset() override;

    bool IsEof() override;

    void Dump(
        /* [in] */ FILE* stream);

private:
    bool ReadInFile(
        /* [in] */ const String& filePath,
        /* [in] */ bool recursive);

    bool ReadInFile(
        /* [in] */ File& file,
        /* [in] */ bool recursive);

    void ResolveCurrentPosition();

private:
    static const char* TAG;

    AutoPtr<File> mFile;
    MemoryBuffer mBuffer;
    std::vector<FileInfo> mFileInfos;
    const char* mData = nullptr;
    size_t mDataSize = 0;
    size_t mDataPosition = 0;
    size_t mMarkPosition = 0;
    size_t mCurrentPosition = 0;
    String mCurrentFilePath;
    int mCurrentLineNo = 0;
    int mCurrentColumnNo = 0;
};

MemoryFileReader::MemoryFileReader(
    /* [in] */ File* file)
    : mFile(file)
{}

}

#endif // __CDLC_MEMORYFILEREADER_H__
