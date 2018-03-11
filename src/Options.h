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

#ifndef __CCM_OPTIONS_H__
#define __CCM_OPTIONS_H__

#include "String.h"

namespace ccm {

class Options
{
public:
    Options(
        /* [in] */ int argc,
        /* [in] */ char** argv);

    inline bool IsFormatError() { return mFormatError; }
    inline bool ShouldShowUsage() { return mShowUsage; }
    inline int GetOptionNumber() { return mOptionNumber; }
    inline String GetInputFile() { return mInputFile; }

    void ShowUsage();

private:
    void Parse(
        /* [in] */ int argc,
        /* [in] */ char** argv);

private:
    int mOptionNumber;
    String mProgram;
    String mInputFile;
    String mOutputFile;

    bool mShowUsage;
    bool mFormatError;
};

}

#endif //__CCM_OPTIONS_H__
