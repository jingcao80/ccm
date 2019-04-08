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

#include "ccm/core/CoreUtils.h"
#include "ccm/core/HardcodedSystemProperties.h"
#include "ccm/core/System.h"
#include "ccm/io/CPrintWriter.h"
#include "ccm/io/CStringWriter.h"
#include "ccm/util/CLocale.h"
#include "ccm/util/CProperties.h"
#include "ccm.io.IPrintWriter.h"
#include "ccm.io.IStringWriter.h"
#include "ccm.util.IHashtable.h"
#include "libcore/io/Libcore.h"
#include <ccmlogger.h>
#include <time.h>
#include <sys/time.h>

using ccm::io::CPrintWriter;
using ccm::io::CStringWriter;
using ccm::io::IPrintWriter;
using ccm::io::IStringWriter;
using ccm::io::IID_IPrintWriter;
using ccm::io::IID_IStringWriter;
using ccm::util::CLocale;
using ccm::util::CProperties;
using ccm::util::IID_IProperties;
using ccm::util::IHashtable;
using ccm::util::IID_IHashtable;
using libcore::io::Libcore;

namespace ccm {
namespace core {

INIT_PROI_3 AutoPtr<IProperties> System::sProps;
INIT_PROI_3 AutoPtr<IProperties> System::sUnchangeableProps;
INIT_PROI_3 String System::sLineSeparator;

static CONS_PROI_4 void StaticInitializeSystem()
{
    System::StaticInitialize();
}

AutoPtr<IPrintStream> System::GetOut()
{
    static AutoPtr<IPrintStream> sOut;
    return sOut;
}

AutoPtr<ISecurityManager> System::GetSecurityManager()
{
    return nullptr;
}

Long System::GetCurrentTimeMillis()
{
    struct timeval tv;
    gettimeofday(&tv, (struct timezone *) nullptr);
    Long when = tv.tv_sec * 1000LL + tv.tv_usec / 1000;
    return when;
}

Long System::GetNanoTime()
{
    timespec now;
    now.tv_sec = now.tv_nsec = 0;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return static_cast<Long>(now.tv_sec) * 1000000000LL + now.tv_nsec;
}

AutoPtr<IProperties> System::InitUnchangeableSystemProperties()
{
    AutoPtr<IHashtable> p;
    CProperties::New(IID_IHashtable, (IInterface**)&p);

    p->Put(CoreUtils::Box(String("ccm.class.path")),
           CoreUtils::Box(String(getenv("CLASS_PATH"))));

    Integer N = ArrayLength(HardcodedSystemProperties::STATIC_PROPERTIES);
    for (Integer i = 0; i < N; i++) {
        String pair[2] = HardcodedSystemProperties::STATIC_PROPERTIES[i];
        Boolean contains;
        if (p->ContainsKey(CoreUtils::Box(pair[0]), &contains), contains) {
            LogE(String("Ignoring command line argument: -D") + pair[0]);
        }
        if (pair[1].IsNull()) {
            p->Remove(CoreUtils::Box(pair[0]));
        }
        else {
            p->Put(CoreUtils::Box(pair[0]), CoreUtils::Box(pair[1]));
        }
    }

    return IProperties::Probe(p);
}

AutoPtr<IProperties> System::InitProperties()
{
    AutoPtr<PropertiesWithNonOverrideableDefaults> p = new PropertiesWithNonOverrideableDefaults();
    p->Constructor(sUnchangeableProps);
    SetDefaultChangeableProperties(p);
    return p.Get();
}

AutoPtr<IProperties> System::SetDefaultChangeableProperties(
    /* [in] */ IProperties* p)
{
    Boolean contains;
    String tmpdirProp("ccm.io.tmpdir");
    if (IHashtable::Probe(sUnchangeableProps)->ContainsKey(
            CoreUtils::Box(tmpdirProp), &contains), !contains) {
        p->SetProperty(tmpdirProp, String("/tmp"));
    }

    String uhProp("user.home");
    if (IHashtable::Probe(sUnchangeableProps)->ContainsKey(
            CoreUtils::Box(uhProp), &contains), !contains) {
        p->SetProperty(uhProp, String(""));
    }

    return p;
}

ECode System::SetUnchangeableSystemProperty(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    FAIL_RETURN(CheckKey(key));
    IHashtable::Probe(sUnchangeableProps)->Put(CoreUtils::Box(key), CoreUtils::Box(value));
    return NOERROR;
}

ECode System::AddLegacyLocaleSystemProperties()
{
    String locale;
    FAIL_RETURN(GetProperty(String("user.locale"), String(""), &locale));
    if (!locale.IsEmpty()) {
        AutoPtr<ILocale> l = CLocale::ForLanguageTag(locale);
        String language, country, variant;
        l->GetLanguage(&language);
        l->GetCountry(&country);
        l->GetVariant(&variant);
        SetUnchangeableSystemProperty(String("user.language"), language);
        SetUnchangeableSystemProperty(String("user.region"), country);
        SetUnchangeableSystemProperty(String("user.variant"), variant);
    }
    else {
        // If "user.locale" isn't set we fall back to our old defaults of
        // language="en" and region="US" (if unset) and don't attempt to set it.
        // The Locale class will fall back to using user.language and
        // user.region if unset.
        String language, region;
        FAIL_RETURN(GetProperty(String("user.language"), String(""), &language));
        FAIL_RETURN(GetProperty(String("user.region"), String(""), &region));

        if (language.IsEmpty()) {
            SetUnchangeableSystemProperty(String("user.language"), String("en"));
        }

        if (region.IsEmpty()) {
            SetUnchangeableSystemProperty(String("user.region"), String("US"));
        }
    }
}

ECode System::GetProperty(
    /* [in] */ const String& key,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    FAIL_RETURN(CheckKey(key));
    AutoPtr<ISecurityManager> sm = GetSecurityManager();
    if (sm != nullptr) {
        FAIL_RETURN(sm->CheckPropertyAccess(key));
    }

    return sProps->GetProperty(key, value);
}

ECode System::GetProperty(
    /* [in] */ const String& key,
    /* [in] */ const String& def,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    FAIL_RETURN(CheckKey(key));
    AutoPtr<ISecurityManager> sm = GetSecurityManager();
    if (sm != nullptr) {
        FAIL_RETURN(sm->CheckPropertyAccess(key));
    }

    return sProps->GetProperty(key, def, value);
}

ECode System::CheckKey(
    /* [in] */ const String& key)
{
    if (key.IsNullOrEmpty()) {
        Logger::E("System", "key can't be null or empty.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode System::GetEnv(
    /* [in] */ const String& name,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    if (name.IsNull()) {
        Logger::E("System", "name == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    return Libcore::GetOs()->Getenv(name, value);
}

ECode System::Log(
    /* [in] */ Char type,
    /* [in] */ const String& message,
    /* [in] */ IStackTrace* st)
{
    Integer level;
    switch(type) {
        case U'D': case U'd': level = Logger::DEBUG; break;
        case U'E': case U'e': level = Logger::ERROR; break;
        case U'V': case U'v': level = Logger::VERBOSE; break;
        case U'W': case U'w': level = Logger::WARNING; break;
        default: level = Logger::VERBOSE; break;
    }

    Logger::Log(level, "System", "%s", message.string());
    if (st != nullptr) {
        AutoPtr<IStringWriter> sw;
        CStringWriter::New(IID_IStringWriter, (IInterface**)&sw);
        AutoPtr<IPrintWriter> pw;
        CPrintWriter::New(IWriter::Probe(sw), IID_IPrintWriter, (IInterface**)&pw);
        st->PrintStackTrace(pw);
        String backtrace;
        sw->ToString(&backtrace);
        Logger::Log(level, "System", "\n%s", backtrace.string());
    }

    return NOERROR;
}

ECode System::StaticInitialize()
{
    sUnchangeableProps = InitUnchangeableSystemProperties();
    sProps = InitProperties();
    AddLegacyLocaleSystemProperties();

    sProps->GetProperty(String("line.separator"), &sLineSeparator);

    return NOERROR;
}

//----------------------------------------------------------------------

ECode System::PropertiesWithNonOverrideableDefaults::Constructor(
    /* [in] */ IProperties* defaults)
{
    return Properties::Constructor(defaults);
}

ECode System::PropertiesWithNonOverrideableDefaults::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    return NOERROR;
}

ECode System::PropertiesWithNonOverrideableDefaults::ToString(
    /* [in] */ String* str)
{
    return NOERROR;
}

}
}
