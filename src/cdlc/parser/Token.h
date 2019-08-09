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

#ifndef __CDLC_TOKEN_H__
#define __CDLC_TOKEN_H__

namespace cdlc {

enum class Token
{
    UNKNOW = 0,             // 0)
    // types
    ARRAY,                  // 1)
    BOOLEAN,                // 2)
    BYTE,                   // 3)
    CHAR,                   // 4)
    COCLASSID,              // 5)
    COMPONENTID,            // 6)
    DOUBLE,                 // 7)
    ECODE,                  // 8)
    FLOAT,                  // 9)
    HANDLE,                 // 10)
    INTEGER,                // 11)
    INTERFACEID,            // 12)
    LONG,                   // 13)
    SHORT,                  // 14)
    STRING,                 // 15)
    TRIPLE,                 // 16)
    // keywords
    CALLEE,                 // 17)
    COCLASS,                // 18)
    CONST,                  // 19)
    CONSTRUCTOR,            // 20)
    DELETE,                 // 21)
    DESCRIPTION,            // 22)
    ENUM,                   // 23)
    FALSE,                  // 24)
    IN,                     // 25)
    INCLUDE,                // 26)
    INTERFACE,              // 27)
    MODULE,                 // 28)
    NAMESPACE,              // 29)
    NULLPTR,                // 30)
    OUT,                    // 31)
    TRUE,                   // 32)
    URL,                    // 33)
    UUID,                   // 34)
    VERSION,                // 35)
    // symbols
    AMPERSAND,              // 36)  '&'
    ANGLE_BRACKETS_OPEN,    // 37)  '<'
    ANGLE_BRACKETS_CLOSE,   // 38)  '>'
    ASSIGNMENT,             // 39)  '='
    ASTERISK,               // 40)  '*'
    BRACES_OPEN,            // 41)  '{'
    BRACES_CLOSE,           // 42)  '}'
    BRACKETS_OPEN,          // 43)  '['
    BRACKETS_CLOSE,         // 44)  ']'
    COLON,                  // 45)  ':'
    COMMA,                  // 46)  ','
    COMPLIMENT,             // 47)  '~'
    DIVIDE,                 // 48)  '/'
    END_OF_LINE,            // 49)  '\n'
    EXCLUSIVE_OR,           // 50)  '^'
    INCLUSIVE_OR,           // 51)  '|'
    MINUS,                  // 52)  '-'
    MODULO,                 // 53)  '%'
    NOT,                    // 54)  '!'
    PARENTHESES_OPEN,       // 55)  '('
    PARENTHESES_CLOSE,      // 56)  ')'
    PERIOD,                 // 57)  '.'
    PLUS,                   // 58)  '+'
    SEMICOLON,              // 59)  ';'
    // others
    CHARACTER,              // 60)
    COMMENT_BLOCK,          // 61)
    COMMENT_LINE,           // 62)
    END_OF_FILE,            // 63)
    IDENTIFIER,             // 64)
    NUMBER_INTEGRAL,        // 65)
    NUMBER_FLOATINGPOINT,   // 66)
    SHIFT_LEFT,             // 67)  "<<"
    SHIFT_RIGHT,            // 68)  ">>"
    SHIFT_RIGHT_UNSIGNED,   // 69)  ">>>"
    STRING_LITERAL,         // 70)
    UUID_NUMBER,            // 71)
    VERSION_NUMBER,         // 72)
};

char Token2Char(
    /* [in] */ Token token);

}

#endif //__CDLC_TOKEN_H__
