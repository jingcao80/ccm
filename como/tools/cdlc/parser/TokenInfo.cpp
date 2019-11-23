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

#include "parser/TokenInfo.h"

namespace cdlc {

TokenInfo::TokenInfo(
    /* [in] */ const TokenInfo& other)
    : mToken(other.mToken)
    , mStringValue(other.mStringValue)
    , mCharValue(other.mCharValue)
    , mIntegralValue(other.mIntegralValue)
    , mFloatingPointValue(other.mFloatingPointValue)
    , mBit(other.mBit)
    , mRadix(other.mRadix)
    , mScientificNotation(other.mScientificNotation)
    , mTokenFilePath(other.mTokenFilePath)
    , mTokenLineNo(other.mTokenLineNo)
    , mTokenColumnNo(other.mTokenColumnNo)
{}

TokenInfo::TokenInfo(
    /* [in] */ TokenInfo&& other)
    : mToken(other.mToken)
    , mStringValue(std::move(other.mStringValue))
    , mCharValue(other.mCharValue)
    , mIntegralValue(other.mIntegralValue)
    , mFloatingPointValue(other.mFloatingPointValue)
    , mBit(other.mBit)
    , mRadix(other.mRadix)
    , mScientificNotation(other.mScientificNotation)
    , mTokenFilePath(std::move(other.mTokenFilePath))
    , mTokenLineNo(other.mTokenLineNo)
    , mTokenColumnNo(other.mTokenColumnNo)
{}

TokenInfo& TokenInfo::operator=(
    /* [in] */ TokenInfo&& other)
{
    mToken = other.mToken;
    mStringValue = std::move(other.mStringValue);
    mCharValue = other.mCharValue;
    mIntegralValue = other.mIntegralValue;
    mFloatingPointValue = other.mFloatingPointValue;
    mBit = other.mBit;
    mRadix = other.mRadix;
    mScientificNotation = other.mScientificNotation;
    mTokenFilePath = std::move(other.mTokenFilePath);
    mTokenLineNo = other.mTokenLineNo;
    mTokenColumnNo = other.mTokenColumnNo;
    return *this;
}

String TokenInfo::Dump(
    /* [in] */ TokenInfo& tokenInfo)
{
    switch (tokenInfo.mToken) {
        case Token::AMPERSAND:
            return "&";
        case Token::ANGLE_BRACKETS_OPEN:
            return "<";
        case Token::ANGLE_BRACKETS_CLOSE:
            return ">";
        case Token::ARRAY:
            return "Array";
        case Token::ASSIGNMENT:
            return "=";
        case Token::ASTERISK:
            return "*";
        case Token::BOOLEAN:
            return "Boolean";
        case Token::BRACES_OPEN:
            return "{";
        case Token::BRACES_CLOSE:
            return "}";
        case Token::BRACKETS_OPEN:
            return "[";
        case Token::BRACKETS_CLOSE:
            return "]";
        case Token::BYTE:
            return "Byte";
        case Token::CALLEE:
            return "callee";
        case Token::CHAR:
            return "Char";
        case Token::CHARACTER:
            return tokenInfo.mStringValue;
        case Token::COCLASS:
            return "coclass";
        case Token::COCLASSID:
            return "CoclassID";
        case Token::COLON:
            return ":";
        case Token::COMMA:
            return ",";
        case Token::COMMENT_BLOCK:
        case Token::COMMENT_LINE:
            return tokenInfo.mStringValue;
        case Token::COMPLIMENT:
            return "~";
        case Token::COMPONENTID:
            return "ComponentID";
        case Token::CONST:
            return "const";
        case Token::CONSTRUCTOR:
            return "Constructor";
        case Token::DELETE:
            return "Delete";
        case Token::DESCRIPTION:
            return "description";
        case Token::DIVIDE:
            return "/";
        case Token::DOUBLE:
            return "Double";
        case Token::ECODE:
            return "ECode";
        case Token::END_OF_LINE:
            return "\n";
        case Token::ENUM:
            return "enum";
        case Token::EXCLUSIVE_OR:
            return "^";
        case Token::FALSE:
            return "false";
        case Token::FLOAT:
            return "Float";
        case Token::HANDLE:
            return "HANDLE";
        case Token::IDENTIFIER:
            return tokenInfo.mStringValue;
        case Token::IN:
            return "in";
        case Token::INCLUDE:
            return "include";
        case Token::INCLUSIVE_OR:
            return "|";
        case Token::INTEGER:
            return "Integer";
        case Token::INTERFACE:
            return "interface";
        case Token::INTERFACEID:
            return "InterfaceID";
        case Token::LONG:
            return "Long";
        case Token::MINUS:
            return "-";
        case Token::MODULE:
            return "module";
        case Token::MODULO:
            return "%";
        case Token::NAMESPACE:
            return "namespace";
        case Token::NOT:
            return "!";
        case Token::NULLPTR:
            return "nullptr";
        case Token::NUMBER_INTEGRAL:
        case Token::NUMBER_FLOATINGPOINT:
            return tokenInfo.mStringValue;
        case Token::OUT:
            return "out";
        case Token::PARENTHESES_OPEN:
            return "(";
        case Token::PARENTHESES_CLOSE:
            return ")";
        case Token::PERIOD:
            return ".";
        case Token::PLUS:
            return "+";
        case Token::SEMICOLON:
            return ";";
        case Token::SHIFT_LEFT:
            return "<<";
        case Token::SHIFT_RIGHT:
            return ">>";
        case Token::SHIFT_RIGHT_UNSIGNED:
            return ">>>";
        case Token::SHORT:
            return "Short";
        case Token::STRING:
            return "String";
        case Token::STRING_LITERAL:
            return tokenInfo.mStringValue;
        case Token::TRIPLE:
            return "Triple";
        case Token::TRUE:
            return "true";
        case Token::TYPEKIND:
            return "TypeKind";
        case Token::URI:
            return "uri";
        case Token::UUID:
            return "uuid";
        case Token::UUID_NUMBER:
            return tokenInfo.mStringValue;
        case Token::VERSION:
            return "version";
        case Token::VERSION_NUMBER:
            return tokenInfo.mStringValue;
        default:
            return "";
    }
}

}
