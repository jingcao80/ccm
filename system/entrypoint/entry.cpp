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

#include "como/core/RuntimeFactory.h"
#include "como.core.IRuntime.h"
#include <comosp.h>

using namespace como;
using como::core::RuntimeFactory;
using como::core::IRuntime;

extern ECode Main(Array<String>& args);

int main(int argv, char** argc)
{
    Array<String> args(argv);

    for (int i = 0; i < argv; i++) {
        args[i] = argc[i];
    }

    AutoPtr<IRuntime> runtime;
    RuntimeFactory::GetRuntime(&runtime);

    ECode ec = Main(args);

    runtime->Exit(SUCCEEDED(ec) ? 0 : 1);

    return 0;
}
