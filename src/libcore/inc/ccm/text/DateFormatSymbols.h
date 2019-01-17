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

#ifndef __CCM_TEXT_DATEFORMATSYMBOLS_H__
#define __CCM_TEXT_DATEFORMATSYMBOLS_H__

#include "ccm/core/SyncObject.h"
#include "ccm.core.ICloneable.h"
#include "ccm.io.ISerializable.h"
#include "ccm.text.IDateFormatSymbols.h"
#include "ccm.util.ILocale.h"
#include <ccmautoptr.h>

using ccm::core::SyncObject;
using ccm::core::ICloneable;
using ccm::io::ISerializable;
using ccm::util::ILocale;

namespace ccm {
namespace text {

class DateFormatSymbols
    : public SyncObject
    , public IDateFormatSymbols
    , public ISerializable
    , public ICloneable
{
public:
    CCM_INTERFACE_DECL();

    static AutoPtr<IDateFormatSymbols> GetInstance(
        /* [in] */ ILocale* locale)
    {
        return nullptr;
    }

    static AutoPtr<IDateFormatSymbols> GetInstanceRef(
        /* [in] */ ILocale* locale)
    {
        return nullptr;
    }

    Array<String> GetTinyMonths()
    {
        return Array<String>::Null();
    }

    Array<String> GetStandAloneMonths()
    {
        return Array<String>::Null();
    }

    Array<String> GetShortStandAloneMonths()
    {
        return Array<String>::Null();
    }

    Array<String> GetTinyStandAloneMonths()
    {
        return Array<String>::Null();
    }

    Array<String> GetTinyWeekdays()
    {
        return Array<String>::Null();
    }

    Array<String> GetStandAloneWeekdays()
    {
        return Array<String>::Null();
    }

    Array<String> GetShortStandAloneWeekdays()
    {
        return Array<String>::Null();
    }

    Array<String> GetTinyStandAloneWeekdays()
    {
        return Array<String>::Null();
    }

    Integer GetZoneIndex(
        /* [in] */ const String& ID)
    {
        return -1;
    }

    Array<Array<String>> GetZoneStringsWrapper()
    {
        return Array<Array<String>>::Null();
    }

    static DateFormatSymbols* From(
        /* [in] */ IDateFormatSymbols* obj);

public:
    /**
     * Indicates that zoneStrings is set externally with setZoneStrings() method.
     */
    Boolean mIsZoneStringsSet = false;

    /**
     * Unlocalized date-time pattern characters. For example: 'y', 'd', etc.
     * All locales use the same these unlocalized pattern characters.
     *
     * Pretend to support 'L' and 'c' for now. It's meant for standalone weekday and
     * month names, but we just use the non-standalone versions for now.
     */
    static const String sPatternChars;

    static const Integer PATTERN_ERA = 0; // G
    static const Integer PATTERN_YEAR = 1; // y
    static const Integer PATTERN_MONTH = 2; // M
    static const Integer PATTERN_DAY_OF_MONTH = 3; // d
    static const Integer PATTERN_HOUR_OF_DAY1 = 4; // k
    static const Integer PATTERN_HOUR_OF_DAY0 = 5; // H
    static const Integer PATTERN_MINUTE = 6; // m
    static const Integer PATTERN_SECOND = 7; // s
    static const Integer PATTERN_MILLISECOND = 8; // S
    static const Integer PATTERN_DAY_OF_WEEK = 9; // E
    static const Integer PATTERN_DAY_OF_YEAR = 10; // D
    static const Integer PATTERN_DAY_OF_WEEK_IN_MONTH = 11; // F
    static const Integer PATTERN_WEEK_OF_YEAR = 12; // w
    static const Integer PATTERN_WEEK_OF_MONTH = 13; // W
    static const Integer PATTERN_AM_PM = 14; // a
    static const Integer PATTERN_HOUR1 = 15; // h
    static const Integer PATTERN_HOUR0 = 16; // K
    static const Integer PATTERN_ZONE_NAME = 17; // z
    static const Integer PATTERN_ZONE_VALUE = 18; // Z
    static const Integer PATTERN_WEEK_YEAR = 19; // Y
    static const Integer PATTERN_ISO_DAY_OF_WEEK = 20; // u
    static const Integer PATTERN_ISO_ZONE = 21; // X
    static const Integer PATTERN_MONTH_STANDALONE = 22; // L
    static const Integer PATTERN_STANDALONE_DAY_OF_WEEK = 23; // c
};

inline DateFormatSymbols* DateFormatSymbols::From(
    /* [in] */ IDateFormatSymbols* obj)
{
    return (DateFormatSymbols*)obj;
}

}
}

#endif // __CCM_TEXT_DATEFORMATSYMBOLS_H__
