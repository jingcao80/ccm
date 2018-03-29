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

#include "Options.h"

#include <stdio.h>

namespace ccm {

Options::Options(
    /* [in] */ int argc,
    /* [in] */ char** argv)
    : mOptionNumber(argc)
    , mShowUsage(false)
    , mFormatError(false)
    , mOnlyCompile(false)
{
    Parse(argc, argv);
}

void Options::Parse(
    /* [in] */ int argc,
    /* [in] */ char** argv)
{
    mProgram = argv[0];

    int i = 1;
    while (i < argc) {
        const char* string = argv[i++];
        if (!strcmp("--help", string)) {
            mShowUsage = true;
        }
        else if (!strcmp("-c", string)) {
            mOnlyCompile = true;
        }
        else if (!strcmp("-o", string)) {
            mOutputFile = argv[i++];
        }
        else {
            mInputFile = string;
        }
    }
}

void Options::ShowUsage()
{
    printf("Usage: ccdl [options] file\n"
           "Options:\n"
           "  --help            Display command line options\n"
           "  -c                Compile only, not generate C++ codes\n"
           "  -o <file>         Place the metadata into <file>\n");
}

}
