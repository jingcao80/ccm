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

    inline String GetSourceFile() const;

    inline bool DoCompile() const;

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
    String mSourceFile;
    String mIllegalOptions;

    bool mDoCompile = false;
    bool mShowUsage = false;
    bool mShowVersion = false;
};

Options::Options(
    /* [in] */ int argc,
    /* [in] */ char** argv)
{
    Parse(argc, argv);
}

String Options::GetSourceFile() const
{
    return mSourceFile;
}

bool Options::DoCompile() const
{
    return mDoCompile;
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
