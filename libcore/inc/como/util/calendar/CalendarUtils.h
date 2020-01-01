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

#ifndef __COMO_UTIL_CALENDAR_CALENDARUTILS_H__
#define __COMO_UTIL_CALENDAR_CALENDARUTILS_H__

#include "como.core.IStringBuffer.h"
#include "como.core.IStringBuilder.h"

using como::core::IStringBuffer;
using como::core::IStringBuilder;

namespace como {
namespace util {
namespace calendar {

class CalendarUtils
{
public:
    static Boolean IsGregorianLeapYear(
        /* [in] */ Integer gregorianYear);

    static Boolean IsJulianLeapYear(
        /* [in] */ Integer normalizedJulianYear);

    static Long FloorDivide(
        /* [in] */ Long n,
        /* [in] */ Long d);

    static Integer FloorDivide(
        /* [in] */ Integer n,
        /* [in] */ Integer d);

    COM_PUBLIC static Integer FloorDivide(
        /* [in] */ Integer n,
        /* [in] */ Integer d,
        /* [out] */ Array<Integer>& r);

    COM_PUBLIC static Integer FloorDivide(
        /* [in] */ Long n,
        /* [in] */ Integer d,
        /* [out] */ Array<Integer>& r);

    static Long Mod(
        /* [in] */ Long x,
        /* [in] */ Long y);

    static Integer Mod(
        /* [in] */ Integer x,
        /* [in] */ Integer y);

    static Integer Amod(
        /* [in] */ Integer x,
        /* [in] */ Integer y);

    static Long Amod(
        /* [in] */ Long x,
        /* [in] */ Long y);

    COM_PUBLIC static ECode Sprintf0d(
        /* [in] */ IStringBuilder* sb,
        /* [in] */ Integer value,
        /* [in] */ Integer width);

    COM_PUBLIC static ECode Sprintf0d(
        /* [in] */ IStringBuffer* sb,
        /* [in] */ Integer value,
        /* [in] */ Integer width);
};

inline Boolean CalendarUtils::IsGregorianLeapYear(
    /* [in] */ Integer gregorianYear)
{
    return (((gregorianYear % 4) == 0)
            && (((gregorianYear % 100) != 0) || ((gregorianYear % 400) == 0)));
}

inline Boolean CalendarUtils::IsJulianLeapYear(
    /* [in] */ Integer normalizedJulianYear)
{
    return (normalizedJulianYear % 4) == 0;
}

inline Long CalendarUtils::FloorDivide(
    /* [in] */ Long n,
    /* [in] */ Long d)
{
    return ((n >= 0) ?
            (n / d) : (((n + 1ll) / d) - 1ll));
}

inline Integer CalendarUtils::FloorDivide(
    /* [in] */ Integer n,
    /* [in] */ Integer d)
{
    return ((n >= 0) ?
            (n / d) : (((n + 1) / d) - 1));
}

inline Long CalendarUtils::Mod(
    /* [in] */ Long x,
    /* [in] */ Long y)
{
    return (x - y * FloorDivide(x, y));
}

inline Integer CalendarUtils::Mod(
    /* [in] */ Integer x,
    /* [in] */ Integer y)
{
    return (x - y * FloorDivide(x, y));
}

inline Integer CalendarUtils::Amod(
    /* [in] */ Integer x,
    /* [in] */ Integer y)
{
    Integer z = Mod(x, y);
    return (z == 0) ? y : z;
}

inline Long CalendarUtils::Amod(
    /* [in] */ Long x,
    /* [in] */ Long y)
{
    Long z = Mod(x, y);
    return (z == 0) ? y : z;
}

}
}
}

#endif // __COMO_UTIL_CALENDAR_CALENDARUTILS_H__
