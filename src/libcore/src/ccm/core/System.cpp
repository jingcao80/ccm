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
#include "ccm/core/System.h"
#include "ccm/util/CProperties.h"
#include "ccm.util.IHashtable.h"
#include <ccmlogger.h>
#include <time.h>
#include <sys/time.h>

using ccm::util::CProperties;
using ccm::util::IID_IProperties;
using ccm::util::IHashtable;

namespace ccm {
namespace core {

INIT_PROI_3 AutoPtr<IProperties> System::sProps;
INIT_PROI_3 AutoPtr<IProperties> System::sUnchangeableProps;

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
    gettimeofday(&tv, (struct timezone *) NULL);
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
    AutoPtr<IProperties> p;
    CProperties::New(IID_IProperties, (IInterface**)&p);

    p->SetProperty(String("ccm.class.path"), String(getenv("CLASS_PATH")));

    return p;
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

ECode System::StaticInitialize()
{
    sUnchangeableProps = InitUnchangeableSystemProperties();
    sProps = InitProperties();
    return NOERROR;
}

//----------------------------------------------------------------------

ECode System::PropertiesWithNonOverrideableDefaults::Constructor(
    /* [in] */ IProperties* defaults)
{
    return Properties::Constructor(defaults);
}

ECode System::PropertiesWithNonOverrideableDefaults::Clone(
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
