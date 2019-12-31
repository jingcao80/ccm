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

#ifndef __CCM_TEXT_DATEFORMAT_H__
#define __CCM_TEXT_DATEFORMAT_H__

#include "ccm/core/SyncObject.h"
#include "ccm/text/AttributedCharacterIteratorAttribute.h"
#include "ccm/text/Format.h"
#include "ccm.core.IBoolean.h"
#include "ccm.core.ICloneable.h"
#include "ccm.core.IStringBuffer.h"
#include "ccm.io.ISerializable.h"
#include "ccm.text.IDateFormat.h"
#include "ccm.text.IDateFormatField.h"
#include "ccm.text.IFieldPosition.h"
#include "ccm.text.IFormat.h"
#include "ccm.text.IFormatField.h"
#include "ccm.text.INumberFormat.h"
#include "ccm.text.IParsePosition.h"
#include "ccm.util.ICalendar.h"
#include "ccm.util.IDate.h"
#include "ccm.util.ILocale.h"
#include "ccm.util.ITimeZone.h"

using ccm::core::IBoolean;
using ccm::core::ICloneable;
using ccm::core::IStringBuffer;
using ccm::core::SyncObject;
using ccm::io::ISerializable;
using ccm::util::ICalendar;
using ccm::util::IDate;
using ccm::util::ILocale;
using ccm::util::ITimeZone;

namespace ccm {
namespace text {

class DateFormat
    : public BaseFormat
    , public IDateFormat
{
public:
    class Field
        : public AttributedCharacterIteratorAttribute
        , public IDateFormatField
        , public IFormatField
    {
    public:
        CCM_INTERFACE_DECL();

        static ECode OfCalendarField(
            /* [in] */ Integer calendarField,
            /* [out] */ IDateFormatField** field);

        ECode GetCalendarField(
            /* [out] */ Integer* calendarField) override;

        static const AutoPtr<IDateFormatField> GetERA();

        static const AutoPtr<IDateFormatField> GetYEAR();

        static const AutoPtr<IDateFormatField> GetMONTH();

        static const AutoPtr<IDateFormatField> GetDAY_OF_MONTH();

        static const AutoPtr<IDateFormatField> GetHOUR_OF_DAY1();

        static const AutoPtr<IDateFormatField> GetHOUR_OF_DAY0();

        static const AutoPtr<IDateFormatField> GetMINUTE();

        static const AutoPtr<IDateFormatField> GetSECOND();

        static const AutoPtr<IDateFormatField> GetMILLISECOND();

        static const AutoPtr<IDateFormatField> GetDAY_OF_WEEK();

        static const AutoPtr<IDateFormatField> GetDAY_OF_YEAR();

        static const AutoPtr<IDateFormatField> GetDAY_OF_WEEK_IN_MONTH();

        static const AutoPtr<IDateFormatField> GetWEEK_OF_YEAR();

        static const AutoPtr<IDateFormatField> GetWEEK_OF_MONTH();

        static const AutoPtr<IDateFormatField> GetAM_PM();

        static const AutoPtr<IDateFormatField> GetHOUR1();

        static const AutoPtr<IDateFormatField> GetHOUR0();

        static const AutoPtr<IDateFormatField> GetTIME_ZONE();

    protected:
        ECode Constructor(
            /* [in] */ const String& name,
            /* [in] */ Integer calendarField);

        ECode ReadResolve(
            /* [out] */ IInterface** obj) override;

    private:
        static AutoPtr<IMap> GetInstanceMap();

    private:
        static Array<IDateFormatField*> sCalendarToFieldMapping;

        Integer mCalendarField;
    };

public:
    CCM_INTERFACE_DECL();

    using IDateFormat::Format;

    ECode Format(
        /* [in] */ IInterface* obj,
        /* [in, out] */ IStringBuffer* toAppendTo,
        /* [in] */ IFieldPosition* pos) override final;

    ECode Format(
        /* [in] */ IDate* date,
        /* [out] */ String* result) override final;

    ECode Parse(
        /* [in] */ const String& source,
        /* [out] */ IDate** date) override;

    using IDateFormat::Parse;

    ECode ParseObject(
        /* [in] */ const String& source,
        /* [in] */ IParsePosition* pos,
        /* [out] */ IInterface** object) override;

    static ECode GetTimeInstance(
        /* [out] */ IDateFormat** instance);

    static ECode GetTimeInstance(
        /* [in] */ Integer style,
        /* [out] */ IDateFormat** instance);

    static ECode GetTimeInstance(
        /* [in] */ Integer style,
        /* [in] */ ILocale* locale,
        /* [out] */ IDateFormat** instance);

    static ECode GetDateInstance(
        /* [out] */ IDateFormat** instance);

    static ECode GetDateInstance(
        /* [in] */ Integer style,
        /* [out] */ IDateFormat** instance);

    static ECode GetDateInstance(
        /* [in] */ Integer style,
        /* [in] */ ILocale* locale,
        /* [out] */ IDateFormat** instance);

    static ECode GetDateTimeInstance(
        /* [out] */ IDateFormat** instance);

    static ECode GetDateTimeInstance(
        /* [in] */ Integer dateStyle,
        /* [in] */ Integer timeStyle,
        /* [out] */ IDateFormat** instance);

    static ECode GetDateTimeInstance(
        /* [in] */ Integer dateStyle,
        /* [in] */ Integer timeStyle,
        /* [in] */ ILocale* locale,
        /* [out] */ IDateFormat** instance);

    static ECode GetInstance(
        /* [out] */ IDateFormat** instance);

    static ECode Set24HourTimePref(
        /* [in] */ IBoolean* is24Hour);

    static ECode GetAvailableLocales(
        /* [out, callee] */ Array<ILocale*>* locales);

    ECode SetCalendar(
        /* [in] */ ICalendar* newCalendar) override;

    ECode GetCalendar(
        /* [out] */ ICalendar** calendar) override;

    ECode SetNumberFormat(
        /* [in] */ INumberFormat* newNumberFormat) override;

    ECode GetNumberFormat(
        /* [out] */ INumberFormat** format) override;

    ECode SetTimeZone(
        /* [in] */ ITimeZone* timezone) override;

    ECode GetTimeZone(
        /* [out] */ ITimeZone** timezone) override;

    ECode SetLenient(
        /* [in] */ Boolean lenient) override;

    ECode IsLenient(
        /* [out] */ Boolean* lenient) override;

    ECode GetHashCode(
        /* [out] */ Integer* hash) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* same) override;

protected:
    ECode CloneImpl(
        /* [in] */ DateFormat* newObj);

private:
    static ECode Get(
        /* [in] */ Integer timeStyle,
        /* [in] */ Integer dateStyle,
        /* [in] */ Integer flags,
        /* [in] */ ILocale* loc,
        /* [out] */ IDateFormat** instance);

public:
    static AutoPtr<IBoolean> sIs24Hour;

protected:
    AutoPtr<ICalendar> mCalendar;

    AutoPtr<INumberFormat> mNumberFormat;
};

}
}

#endif // __CCM_TEXT_DATEFORMAT_H__
