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

#include "ccm.io.CFileInputStream.h"
#include "ccm.io.IFileInputStream.h"
#include "ccm.io.IInputStream.h"
#include <ccmautoptr.h>
#include <ccmtypes.h>
#include <test/gtest.h>

using namespace ccm;
using ccm::io::CFileInputStream;
using ccm::io::IFileInputStream;
using ccm::io::IID_IFileInputStream;
using ccm::io::IInputStream;
using ccm::io::IID_IInputStream;

static const char* LICENSE[] = {
    "//=========================================================================",
    "// Copyright (C) 2018 The C++ Component Model(CCM) Open Source Project",
    "//",
    "// Licensed under the Apache License, Version 2.0 (the \"License\");",
    "// you may not use this file except in compliance with the License.",
    "// You may obtain a copy of the License at",
    "//",
    "//      http://www.apache.org/licenses/LICENSE-2.0",
    "//",
    "// Unless required by applicable law or agreed to in writing, software",
    "// distributed under the License is distributed on an \"AS IS\" BASIS,",
    "// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.",
    "// See the License for the specific language governing permissions and",
    "// limitations under the License.",
    "//=========================================================================",
};

TEST(FileInputStreamTest, FileInputStreamReadTest)
{
    AutoPtr<IInputStream> input;
    CFileInputStream::New(String("testfile"), IID_IInputStream, (IInterface**)&input);
    EXPECT_TRUE(input != nullptr);
    Integer number;
    input->Available(&number);
    EXPECT_EQ(number, 781);
    Array<Byte> buffer(100);
    input->Read(buffer, 0, 75, &number);
    EXPECT_EQ(number, strlen(LICENSE[0]));
    EXPECT_STREQ(LICENSE[0], String(buffer.GetPayload(), number).string());
    input->Close();
}

ECode Main(Array<String>& args)
{
    testing::InitGoogleTest(args);
    ECode ec = RUN_ALL_TESTS() == 0 ? NOERROR : E_FAILED_EXCEPTION;
    return ec;
}
