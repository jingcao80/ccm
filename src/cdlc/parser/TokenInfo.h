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

#ifndef __CDLC_TOKENINFO_H__
#define __CDLC_TOKENINFO_H__

#include "parser/Token.h"
#include "util/String.h"
#include <utility>

namespace cdlc {

class TokenInfo
{
public:
    inline TokenInfo();

    inline TokenInfo(
        /* [in] */ Token token,
        /* [in] */ const String& filePath,
        /* [in] */ int lineNo = 0,
        /* [in] */ int columnNo = 0);

    TokenInfo(
        /* [in] */ const TokenInfo& other);

    TokenInfo(
        /* [in] */ TokenInfo&& other);

    TokenInfo& operator=(
        /* [in] */ TokenInfo&& other);

    inline bool IsBuildinType();

    static String Dump(
        /* [in] */ TokenInfo& tokenInfo);

public:
    Token mToken = Token::UNKNOWN;
    String mStringValue;
    int mCharValue = 0;
    long long int mIntegralValue = 0;
    double mFloatingPointValue = 0;
    int mBit = 0;
    int mRadix = 0;
    bool mScientificNotation = false;
    String mTokenFilePath;
    int mTokenLineNo = 0;
    int mTokenColumnNo = 0;
};

TokenInfo::TokenInfo()
{}

TokenInfo::TokenInfo(
    /* [in] */ Token token,
    /* [in] */ const String& filePath,
    /* [in] */ int lineNo,
    /* [in] */ int columnNo)
    : mToken(token)
    , mTokenFilePath(filePath)
    , mTokenLineNo(lineNo)
    , mTokenColumnNo(columnNo)
{}

bool TokenInfo::IsBuildinType()
{
    return Token::BOOLEAN <= mToken && mToken <= Token::TRIPLE;
}

}

#endif // __CDLC_TOKENINFO_H__
