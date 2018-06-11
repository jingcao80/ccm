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

#include "core/Character.h"
#include "core/StringUtils.h"
#include "ccm.core.IByte.h"
#include "ccm.core.IInteger.h"
#include "ccm.core.ILong.h"
#include "ccm.core.IShort.h"
#include <ccmlogger.h>

namespace ccm {
namespace core {

ECode StringUtils::ParseByte(
    /* [in] */ const String& s,
    /* [in] */ Integer radix,
    /* [out] */ Byte* value)
{
    VALIDATE_NOT_NULL(value);

    Integer i;
    FAIL_RETURN(ParseInt(s, radix, &i));
    if (i < IByte::MIN_VALUE || i > IByte::MAX_VALUE) {
        Logger::E("StringUtils", "Value out of range. Value:\"%s\" Radix:%d",
                s.string(), radix);
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    *value = (Byte)i;
    return NOERROR;
}

ECode StringUtils::ParseShort(
    /* [in] */ const String& s,
    /* [in] */ Integer radix,
    /* [out] */ Short* value)
{
    VALIDATE_NOT_NULL(value);

    Integer i;
    FAIL_RETURN(ParseInt(s, radix, &i));
    if (i < IShort::MIN_VALUE || i > IShort::MAX_VALUE) {
        Logger::E("StringUtils", "Value out of range. Value:\"%s\" Radix:%d",
                s.string(), radix);
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    *value = (Short)i;
    return NOERROR;
}

ECode StringUtils::ParseInt(
    /* [in] */ const String& s,
    /* [in] */ Integer radix,
    /* [out] */ Integer* value)
{
    VALIDATE_NOT_NULL(value);

    if (s.IsNull()) {
        Logger::E("StringUtils", "s == null");
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    if (radix < Character::MIN_RADIX) {
        Logger::E("StringUtils", "radix %d less than Character::MIN_RADIX", radix);
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    if (radix > Character::MAX_RADIX) {
        Logger::E("StringUtils", "radix %d greater than Character::MAX_RADIX", radix);
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    Integer result = 0;
    Boolean negative = false;
    Integer i = 0, len = s.GetLength();
    Integer limit = -IInteger::MAX_VALUE;
    Integer multmin;
    Integer digit;

    if (len > 0) {
        Char firstChar = s.GetChar(0);
        if (firstChar < '0') { // Possible leading "+" or "-"
            if (firstChar == '-') {
                negative = true;
                limit = IInteger::MIN_VALUE;
            }
            else if (firstChar != '+') {
                Logger::E("StringUtils", "s == %s", s.string());
                return E_NUMBER_FORMAT_EXCEPTION;
            }

            if (len == 1) { // Cannot have lone "+" or "-"
                Logger::E("StringUtils", "s == %s", s.string());
                return E_NUMBER_FORMAT_EXCEPTION;
            }
            i++;
        }
        multmin = limit / radix;
        while (i < len) {
            // Accumulating negatively avoids surprises near MAX_VALUE
            digit = Character::Digit(s.GetChar(i++), radix);
            if (digit < 0) {
                Logger::E("StringUtils", "s == %s", s.string());
                return E_NUMBER_FORMAT_EXCEPTION;
            }
            if (result < multmin) {
                Logger::E("StringUtils", "s == %s", s.string());
                return E_NUMBER_FORMAT_EXCEPTION;
            }
            result *= radix;
            if (result < limit + digit) {
                Logger::E("StringUtils", "s == %s", s.string());
                return E_NUMBER_FORMAT_EXCEPTION;
            }
            result -= digit;
        }
    }
    else {
        Logger::E("StringUtils", "s == %s", s.string());
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    return negative ? result : -result;
}

ECode StringUtils::ParseLong(
    /* [in] */ const String& s,
    /* [in] */ Integer radix,
    /* [out] */ Long* value)
{
    VALIDATE_NOT_NULL(value);

    if (s.IsNull()) {
        Logger::E("StringUtils", "s == null");
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    if (radix < Character::MIN_RADIX) {
        Logger::E("StringUtils", "radix %d less than Character::MIN_RADIX", radix);
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    if (radix > Character::MAX_RADIX) {
        Logger::E("StringUtils", "radix %d greater than Character::MAX_RADIX", radix);
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    Long result = 0;
    Boolean negative = false;
    Integer i = 0, len = s.GetLength();
    Long limit = -ILong::MAX_VALUE;
    Long multmin;
    Integer digit;

    if (len > 0) {
        Char firstChar = s.GetChar(0);
        if (firstChar < '0') { // Possible leading "+" or "-"
            if (firstChar == '-') {
                negative = true;
                limit = ILong::MIN_VALUE;
            }
            else if (firstChar != '+') {
                Logger::E("StringUtils", "s == %s", s.string());
                return E_NUMBER_FORMAT_EXCEPTION;
            }

            if (len == 1) { // Cannot have lone "+" or "-"
                Logger::E("StringUtils", "s == %s", s.string());
                return E_NUMBER_FORMAT_EXCEPTION;
            }
            i++;
        }
        multmin = limit / radix;
        while (i < len) {
            // Accumulating negatively avoids surprises near MAX_VALUE
            digit = Character::Digit(s.GetChar(i++), radix);
            if (digit < 0) {
                Logger::E("StringUtils", "s == %s", s.string());
                return E_NUMBER_FORMAT_EXCEPTION;
            }
            if (result < multmin) {
                Logger::E("StringUtils", "s == %s", s.string());
                return E_NUMBER_FORMAT_EXCEPTION;
            }
            result *= radix;
            if (result < limit + digit) {
                Logger::E("StringUtils", "s == %s", s.string());
                return E_NUMBER_FORMAT_EXCEPTION;
            }
            result -= digit;
        }
    }
    else {
        Logger::E("StringUtils", "s == %s", s.string());
        return E_NUMBER_FORMAT_EXCEPTION;
    }
    return negative ? result : -result;
}

}
}
