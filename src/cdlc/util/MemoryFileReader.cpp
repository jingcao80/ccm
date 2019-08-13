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

#include "util/MemoryFileReader.h"
#include "util/Logger.h"

namespace cdlc {

const char* MemoryFileReader::TAG = "MemoryFileReader";

MemoryFileReader::MemoryFileReader(
    /* [in] */ const String& filePath)
{
    mFile = new File(filePath, File::READ);
}

bool MemoryFileReader::ReadIn(
    /* [in] */ bool recursive)
{
    bool ret = ReadInFile(*mFile, recursive);
    if (ret) {
        mData = mBuffer.GetArray();
        mDataSize = mBuffer.GetPosition();
        mDataPosition = 0;
    }
    return ret;
}

bool MemoryFileReader::ReadInFile(
    /* [in] */ const String& filePath,
    /* [in] */ bool recursive)
{
    File file(filePath, File::READ);
    return ReadInFile(file, recursive);
}

bool MemoryFileReader::ReadInFile(
    /* [in] */ File& file,
    /* [in] */ bool recursive)
{
    if (!file.IsValid()) {
        return false;
    }

    int lineNo = 0;
    while (!file.IsEof()) {
        String line = file.RawGetLine();
        lineNo++;
        if (line.StartsWith("include")) {
            String include = line.Substring(line.IndexOf('"') + 1, line.LastIndexOf('"'));
            if (recursive) {
                if (!ReadInFile(include, recursive)) {
                    return false;
                }
            }
            else {
                size_t beginPos = mBuffer.Put(line);
                FileInfo info;
                info.mBeginPos = beginPos;
                info.mEndPos = beginPos + line.GetLength();
                info.mFilePath = file.GetPath();
                info.mLineNo = lineNo;
                mFileInfos.push_back(info);
            }

        }
        else if (!line.IsNull()) {
            size_t beginPos = mBuffer.Put(line);
            FileInfo info;
            info.mBeginPos = beginPos;
            info.mEndPos = beginPos + line.GetLength();
            info.mFilePath = file.GetPath();
            info.mLineNo = lineNo;
            mFileInfos.push_back(info);
        }
    }

    return true;
}

char MemoryFileReader::PeekChar()
{
    return mData != nullptr ? mData[mDataPosition] : '\0';
}

char MemoryFileReader::GetChar()
{
    char c = PeekChar();
    if (mDataPosition < mDataSize) {
        mDataPosition++;
    }
    return c;
}

void MemoryFileReader::UnGetChar()
{
    if (mDataPosition > 0) {
        mDataPosition--;
    }
}

String MemoryFileReader::GetCurrentFilePath()
{
    if (mCurrentPosition != mDataPosition) {
        ResolveCurrentPosition();
    }
    return mCurrentFilePath;
}

int MemoryFileReader::GetCurrentLineNumber()
{
    if (mCurrentPosition != mDataPosition) {
        ResolveCurrentPosition();
    }
    return mCurrentLineNo;
}

int MemoryFileReader::GetCurrentColumnNumber()
{
    if (mCurrentPosition != mDataPosition) {
        ResolveCurrentPosition();
    }
    return mCurrentColumnNo;
}

void MemoryFileReader::ResolveCurrentPosition()
{
    size_t begin = 0;
    size_t end = mFileInfos.size();
    size_t half = (begin + end) / 2;
    while (begin < half && half < end) {
        FileInfo info = mFileInfos[half];
        if (mDataPosition < info.mBeginPos) {
            end = half;
            half = (begin + end) / 2;
        }
        else if (mDataPosition >= info.mEndPos) {
            begin = half;
            half = (begin + end) / 2;
        }
        else {
            mCurrentFilePath = info.mFilePath;
            mCurrentLineNo = info.mLineNo;
            mCurrentColumnNo = mDataPosition - info.mBeginPos + 1;
            mCurrentPosition = mDataPosition;
            return;
        }
    }

    FileInfo info = mFileInfos[end];
    mCurrentFilePath = info.mFilePath;
    mCurrentLineNo = info.mLineNo;
    mCurrentColumnNo = mDataPosition - info.mBeginPos + 1;
    mCurrentPosition = mDataPosition;
}

void MemoryFileReader::Mark()
{
    mMarkPosition = mDataPosition;
}

void MemoryFileReader::Reset()
{
    mDataPosition = mMarkPosition;
}

bool MemoryFileReader::IsEof()
{
    return mDataPosition == mDataSize;
}

void MemoryFileReader::Dump(
    /* [in] */ FILE* stream)
{
    FileInfo info = *mFileInfos.end();
    if (info.mEndPos > mBuffer.GetPosition()) {
        Logger::E(TAG, "The last end postion %lu is larger than buffer's postion %lu\n",
                info.mEndPos, mBuffer.GetPosition());
        return;
    }

    fprintf(stream, "==== Dump MemoryFileReader[%p] ====\n", this);
    const char* buffer = mBuffer.GetArray();
    for (FileInfo info : mFileInfos) {
        String line(buffer + info.mBeginPos, info.mEndPos - info.mBeginPos);
        int index = line.LastIndexOf('\n');
        line = line.Substring(0, index);
        line += String::Format("  // %s:%d\n", info.mFilePath.string(), info.mLineNo);
        fprintf(stream, "%s", line.string());
    }
    fprintf(stream, "==== Dump End ====\n");
}

}
