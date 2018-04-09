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

#ifndef __CCDL_OPTIONS_H__
#define __CCDL_OPTIONS_H__

#include "util/String.h"

namespace ccdl {

class Options
{
public:
    Options(
        /* [in] */ int argc,
        /* [in] */ char** argv);

    inline bool IsFormatError()
    { return mFormatError; }

    inline bool ShouldCompile()
    { return mShouldCompile; }

    inline bool ShouldSaveMetadata()
    { return mShouldSaveMetadata; }

    inline bool ShouldGenerate()
    { return mShouldGenerate; }

    inline bool ShouldShowUsage()
    { return mShowUsage; }

    inline int GetOptionNumber()
    { return mOptionNumber; }

    inline String GetInputFile()
    { return mInputFile; }

    inline String GetMetadataOutputFile()
    { return mMetadataOuputFile; }

    inline String GetOutputDir()
    { return mOutputDir; }

    inline bool IsFromSoFile()
    { return mMetadataFileType == SO_FILE; }

    inline bool IsFromMetadataFile()
    { return mMetadataFileType == METADATA_FILE; }

    void ShowUsage();

private:
    void Parse(
        /* [in] */ int argc,
        /* [in] */ char** argv);

private:
    static constexpr int METADATA_FILE = 1;
    static constexpr int SO_FILE = 2;

    int mOptionNumber;
    String mProgram;
    String mInputFile;
    String mMetadataOuputFile;
    String mOutputDir;

    bool mShowUsage;
    bool mFormatError;
    bool mShouldCompile;
    bool mShouldSaveMetadata;
    bool mShouldGenerate;
    int mMetadataFileType;
};

}

#endif //__CCDL_OPTIONS_H__
