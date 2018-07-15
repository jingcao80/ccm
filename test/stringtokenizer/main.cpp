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

#include "ccm.util.CStringTokenizer.h"
#include "ccm.util.IStringTokenizer.h"
#include <ccmautoptr.h>
#include <gtest/gtest.h>

using namespace ccm;
using ccm::util::CStringTokenizer;
using ccm::util::IStringTokenizer;
using ccm::util::IID_IStringTokenizer;

TEST(StringTokenizerTest, StringTokenizerTest)
{
    AutoPtr<IStringTokenizer> tokenizer;
    CStringTokenizer::New(String("hello world hello shanghai"),
            String(" "), IID_IStringTokenizer, (IInterface**)&tokenizer);
    Integer count;
    tokenizer->CountTokens(&count);
    EXPECT_EQ(4, count);
    String token1, token2, token3, token4;
    tokenizer->GetNextToken(&token1);
    EXPECT_STREQ("hello", token1.string());
    tokenizer->GetNextToken(&token2);
    EXPECT_STREQ("world", token2.string());
    tokenizer->GetNextToken(&token3);
    EXPECT_STREQ("hello", token3.string());
    tokenizer->GetNextToken(&token4);
    EXPECT_STREQ("shanghai", token4.string());
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
