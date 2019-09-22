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

#include "util/Options.h"
#include "util/Properties.h"
#include "util/StringBuilder.h"
#include <cstdio>

namespace cdlc {

void Options::Parse(
    /* [in] */ int argc,
    /* [in] */ char** argv)
{
    StringBuilder errors;
    mProgram = argv[0];

    int i = 1;
    while (i < argc) {
        String option(argv[i++]);
        if (option.Equals("-dump-ast")) {
            mDoDumpAST = true;
        }
        else if (option.Equals("-dump-metadata")) {
            mDoDumpMetadata = true;
        }
        else if (option.Equals("--help")) {
            mShowUsage = true;
        }
        else if (option.Equals("--version")) {
            mShowVersion = true;
        }
        else if (option.Equals("-c")) {
            mDoCompile = true;
        }
        else if (option.Equals("-i")) {
            Properties::Get().AddSearchPath(argv[i++]);
        }
        else if (!option.StartsWith("-")) {
            mSourceFile = option;
        }
        else {
            errors.Append(option);
            errors.Append(":");
        }
    }

    mIllegalOptions = errors.ToString();
}

void Options::ShowErrors() const
{
    if (!mIllegalOptions.IsEmpty()) {
        String options = mIllegalOptions;
        int index;
        while ((index = options.IndexOf(":")) != -1) {
            printf("The Option \"%s\" is illegal.\n", options.Substring(0, index).string());
            options = options.Substring(index + 1);
        }
    }
    printf("Use \"--help\" to show usage.\n");
}

void Options::ShowUsage() const
{
    printf("Compile a .cdl file and generate metadata, or generate C++ codes from the metadata.\n"
            "Usage: cdlc [options] file\n"
            "Options:\n"
            "  --help           Display command line options\n"
            "  --version        Display version information\n"
            "  -dump-ast        Display the AST of the .cdl file\n"
            "  -dump-metadata   Display the metadata generated from the .cdl file\n"
            "  -c               Compile the .cdl file\n"
            "  -i <directory>   Add <directory> to the .cdl files search paths\n"
            "  -o <file>        Save the metadata into <file>\n");
}

}
