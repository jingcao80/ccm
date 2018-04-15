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

    inline bool DoCompile();

    inline bool DoSaveMetadata();

    inline bool DoGenerateCode();

    inline bool DoShowUsage();

    inline int GetOptionNumber();

    inline String GetInputFile();

    inline String GetMetadataOutputFile();

    inline String GetCodeGenDirectory();

    inline int GetMode();

    inline int GetMetadataInputType();

    void ShowUsage();

private:
    void Parse(
        /* [in] */ int argc,
        /* [in] */ char** argv);

public:
    static constexpr int MODE_CCMRT = 1;
    static constexpr int MODE_COMPONENT = 2;
    static constexpr int MODE_USER = 3;

    static constexpr int TYPE_METADATA_FILE = 1;
    static constexpr int TYPE_SO_FILE = 2;

private:
    bool mShowUsage;
    bool mDoCompile;
    bool mDoGenerate;
    bool mDoSaveMetadata;

    int mOptionNumber;
    int mMode;
    int mMetadataInputType;

    String mProgram;
    String mCodeGenDir;
    String mMetadataOutputFile;
    String mInputFile;
};

bool Options::DoCompile()
{
    return mDoCompile;
}

bool Options::DoSaveMetadata()
{
    return mDoSaveMetadata;
}

bool Options::DoGenerateCode()
{
    return mDoGenerate;
}

bool Options::DoShowUsage()
{
    return mShowUsage;
}

int Options::GetOptionNumber()
{
    return mOptionNumber;
}

String Options::GetInputFile()
{
    return mInputFile;
}

String Options::GetMetadataOutputFile()
{
    return mMetadataOutputFile;
}

String Options::GetCodeGenDirectory()
{
    return mCodeGenDir;
}

int Options::GetMode()
{
    return mMode;
}

int Options::GetMetadataInputType()
{
    return mMetadataInputType;
}

}

#endif //__CCDL_OPTIONS_H__
