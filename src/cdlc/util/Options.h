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

#ifndef __CDLC_OPTIONS_H__
#define __CDLC_OPTIONS_H__

#include "util/String.h"

namespace cdlc {

class Options
{
public:
    inline Options(
        /* [in] */ int argc,
        /* [in] */ char** argv);

    inline String GetCodegenDirectory() const;

    inline int GetCodegenMode() const;

    inline String GetMetadataFile() const;

    inline int GetMetadataFileType() const;

    inline String GetSaveFile() const;

    inline String GetSourceFile() const;

    inline bool DoCompile() const;

    inline bool DoDumpAST() const;

    inline bool DoDumpMetadata() const;

    inline bool DoGenerateCode() const;

    inline bool DoSaveMetadata() const;

    inline bool HasErrors() const;

    inline bool DoShowUsage() const;

    void ShowErrors() const;

    void ShowUsage() const;

private:
    void Parse(
        /* [in] */ int argc,
        /* [in] */ char** argv);

private:
    String mProgram;
    String mCodegenDir;
    int mCodegenMode = 0;
    String mMetadataFile;
    int mMetadataFileType = 0;
    String mSaveFile;
    String mSourceFile;
    String mIllegalOptions;

    bool mDoCompile = false;
    bool mDoDumpAST = false;
    bool mDoDumpMetadata = false;
    bool mDoGenerateCode = false;
    bool mDoSaveMetadata = false;
    bool mShowUsage = false;
    bool mShowVersion = false;
};

Options::Options(
    /* [in] */ int argc,
    /* [in] */ char** argv)
{
    Parse(argc, argv);
}

int Options::GetCodegenMode() const
{
    return mCodegenMode;
}

String Options::GetCodegenDirectory() const
{
    return mCodegenDir;
}

String Options::GetMetadataFile() const
{
    return mMetadataFile;
}

int Options::GetMetadataFileType() const
{
    return mMetadataFileType;
}

String Options::GetSaveFile() const
{
    return mSaveFile;
}

String Options::GetSourceFile() const
{
    return mSourceFile;
}

bool Options::DoCompile() const
{
    return mDoCompile;
}

bool Options::DoDumpAST() const
{
    return mDoDumpAST;
}

bool Options::DoDumpMetadata() const
{
    return mDoDumpMetadata;
}

bool Options::DoGenerateCode() const
{
    return mDoGenerateCode;
}

bool Options::DoSaveMetadata() const
{
    return mDoSaveMetadata;
}

bool Options::HasErrors() const
{
    return !mIllegalOptions.IsEmpty() || mSourceFile.IsEmpty();
}

bool Options::DoShowUsage() const
{
    return mShowUsage;
}

}

#endif // __CDLC_OPTIONS_H__
