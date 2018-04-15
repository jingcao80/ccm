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

namespace ccdl {

Options::Options(
    /* [in] */ int argc,
    /* [in] */ char** argv)
    : mShowUsage(false)
    , mDoCompile(false)
    , mDoGenerate(false)
    , mDoSaveMetadata(false)
    , mOptionNumber(argc)
    , mMode(0)
    , mMetadataInputType(0)
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
            mDoCompile = true;
        }
        else if (!strcmp("-d", string)) {
            mCodeGenDir = argv[i++];
        }
        else if (!strcmp("-g", string)) {
            mDoGenerate = true;
            mMode = MODE_COMPONENT;
            mMetadataInputType = TYPE_SO_FILE;
        }
        else if (!strcmp("-k", string)) {
            mMode = MODE_COMPONENT;
        }
        else if (!strcmp("-m", string)) {
            mMetadataInputType = TYPE_METADATA_FILE;
        }
        else if (!strcmp("-o", string)) {
            mDoSaveMetadata = true;
            mMetadataOutputFile = argv[i++];
        }
        else if (!strcmp("-r", string)) {
            mMode = MODE_CCMRT;
        }
        else if (!strcmp("-s", string)) {
            mMetadataInputType = TYPE_SO_FILE;
        }
        else if (!strcmp("-u", string)) {
            mMode = MODE_USER;
        }
        else {
            mInputFile = string;
        }
    }
}

void Options::ShowUsage()
{
    printf("Compile a .cdl file to metadata, or generate C++ codes from metadata."
           "Usage: ccdl [options] file\n"
           "Options:\n"
           "  --help            Display command line options\n"
           "  -c                Compile the .cdl file to metadata\n"
           "  -d <directory>    Place generated C++ codes into <directory>\n"
           "  -g                Generate C++ codes\n"
           "  -k                Set \"componet\" mode\n"
           "  -m                Set \"metadata\" as metadata input file type\n"
           "  -o <file>         Place the metadata into <file>\n"
           "  -r                Set \"ccmrt\" mode\n"
           "  -s                Set \"so\" as metadata input file type\n"
           "  -u                Set \"user\" mode\n");
}

}
