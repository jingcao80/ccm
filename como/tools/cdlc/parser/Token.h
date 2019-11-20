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
    UNKNOWN = 0,            // 0)
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
    TYPEKIND,               // 17)
    // keywords
    CALLEE,                 // 18)
    COCLASS,                // 19)
    CONST,                  // 20)
    CONSTRUCTOR,            // 21)
    DELETE,                 // 22)
    DESCRIPTION,            // 23)
    ENUM,                   // 24)
    FALSE,                  // 25)
    IN,                     // 26)
    INCLUDE,                // 27)
    INTERFACE,              // 28)
    MODULE,                 // 29)
    NAMESPACE,              // 30)
    NULLPTR,                // 31)
    OUT,                    // 32)
    TRUE,                   // 33)
    URI,                    // 34)
    UUID,                   // 35)
    VERSION,                // 36)
    // symbols
    AMPERSAND,              // 37)  '&'
    ANGLE_BRACKETS_OPEN,    // 38)  '<'
    ANGLE_BRACKETS_CLOSE,   // 39)  '>'
    ASSIGNMENT,             // 40)  '='
    ASTERISK,               // 41)  '*'
    BRACES_OPEN,            // 42)  '{'
    BRACES_CLOSE,           // 43)  '}'
    BRACKETS_OPEN,          // 44)  '['
    BRACKETS_CLOSE,         // 45)  ']'
    COLON,                  // 46)  ':'
    COMMA,                  // 47)  ','
    COMPLIMENT,             // 48)  '~'
    DIVIDE,                 // 49)  '/'
    END_OF_LINE,            // 50)  '\n'
    EXCLUSIVE_OR,           // 51)  '^'
    INCLUSIVE_OR,           // 52)  '|'
    MINUS,                  // 53)  '-'
    MODULO,                 // 54)  '%'
    NOT,                    // 55)  '!'
    PARENTHESES_OPEN,       // 56)  '('
    PARENTHESES_CLOSE,      // 57)  ')'
    PERIOD,                 // 58)  '.'
    PLUS,                   // 59)  '+'
    SEMICOLON,              // 60)  ';'
    // others
    CHARACTER,              // 61)
    COMMENT_BLOCK,          // 62)
    COMMENT_LINE,           // 63)
    END_OF_FILE,            // 64)
    IDENTIFIER,             // 65)
    NUMBER_INTEGRAL,        // 66)
    NUMBER_FLOATINGPOINT,   // 67)
    SHIFT_LEFT,             // 68)  "<<"
    SHIFT_RIGHT,            // 69)  ">>"
    SHIFT_RIGHT_UNSIGNED,   // 70)  ">>>"
    STRING_LITERAL,         // 71)
    UUID_NUMBER,            // 72)
    VERSION_NUMBER,         // 73)
};

char Token2Char(
    /* [in] */ Token token);

}

#endif //__CDLC_TOKEN_H__
