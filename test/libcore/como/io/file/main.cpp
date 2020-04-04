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

#include "como.io.CFile.h"
#include "como.io.IFile.h"
#include <comosp.h>
#include <gtest/gtest.h>

using namespace como;
using como::io::CFile;
using como::io::IFile;
using como::io::IID_IFile;

TEST(FileTest, TestCreateNewFile)
{
    AutoPtr<IFile> f;
    CFile::New(String("test.txt"), IID_IFile, (IInterface**)&f);
    Boolean succeeded;
    f->CreateNewFile(succeeded);
    EXPECT_TRUE(succeeded);
}

TEST(FileTest, TestExists)
{
    AutoPtr<IFile> f;
    CFile::New(String("test.txt"), IID_IFile, (IInterface**)&f);
    Boolean existed;
    f->Exists(existed);
    EXPECT_TRUE(existed);
}

TEST(FileTest, TestDelete)
{
    AutoPtr<IFile> f;
    CFile::New(String("test.txt"), IID_IFile, (IInterface**)&f);
    Boolean deleted;
    f->Delete(&deleted);
    EXPECT_TRUE(deleted);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
