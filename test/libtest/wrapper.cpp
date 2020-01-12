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

#include "gtest.h"

namespace como {

template<>
struct DeleteFunc<char*, false>
{
    inline void operator()(
        /* [in] */ char** data,
        /* [in] */ void* id)
    {}
};

}

namespace testing {

GTEST_API_ void InitGoogleTest(Array<String>& args)
{
    int argc = args.GetLength();
    Array<char*> argv(argc);
    for (Integer i = 0; i < argc; i++) {
        argv[i] = const_cast<char*>(args[i].string());
    }
    testing::InitGoogleTest(&argc, argv.GetPayload());
}

}
