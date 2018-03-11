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

#ifndef __CCM_PARSER_H__
#define __CCM_PARSER_H__

#include "File.h"
#include "Tokenizer.h"

#include <memory>

namespace ccm {

class Parser
{
public:
    Parser()
        : mFile(nullptr)
        , mStatus(NOERROR)
    {}

    int Parse(
        /* [in] */ const std::shared_ptr<File>& file);

private:
    int ParseFile();

    int ParseAttribute();

    int ParseComment(
        /* [in] */ Tokenizer::Token token);

    int ParseEnum();

    int ParseInterface();

public:
    static constexpr int NOERROR = 0x0;
    static constexpr int E_FILE_NOT_FOUND_EXCEPTION = 0x1;

private:
    static const String TAG;
    std::shared_ptr<File> mFile;
    Tokenizer mTokenizer;
    int mStatus;
};

}

#endif // __CCM_PARSER_H__
