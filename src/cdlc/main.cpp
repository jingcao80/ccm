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

#include "ast/Module.h"
#include "parser/Parser.h"
#include "util/Logger.h"
#include "util/Options.h"

using namespace cdlc;

static const char* TAG = "cdlc";

int main(int argc, char** argv)
{
    Options options(argc, argv);

    if (options.DoShowUsage()) {
        options.ShowUsage();
        return 0;
    }

    if (options.HasErrors()) {
        options.ShowErrors();
        return 0;
    }

    if (options.DoCompile()) {
        Parser parser;
        if (!parser.Parse(options.GetSourceFile())) {
            Logger::E(TAG, "Parsing failed.");
            return -1;
        }

        if (options.DoDumpAST()) {
            AutoPtr<Module> module = parser.GetCompiledModule();
            printf("%s", module->Dump("").string());
        }
    }

    return 0;
}
