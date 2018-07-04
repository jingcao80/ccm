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

#include "ccm/core/System.h"
#include <ccmlogger.h>
#include <time.h>

namespace ccm {
namespace core {

INIT_PROI_2 AutoPtr<IProperties> System::sProps;
AutoPtr<IProperties> System::sUnchangeableProps;

static CONS_PROI_2 void StaticInitializeSystem()
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
    timespec now;
    now.tv_sec = now.tv_nsec = 0;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return static_cast<Long>(now.tv_sec) * 1000 + now.tv_nsec / 1000000;
}

Long System::GetNanoTime()
{
    timespec now;
    now.tv_sec = now.tv_nsec = 0;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return static_cast<Long>(now.tv_sec) * 1000000000LL + now.tv_nsec;
}

AutoPtr<IProperties> System::InitProperties()
{
    AutoPtr<IProperties> p = new PropertiesWithNonOverrideableDefaults(sUnchangeableProps);
    SetDefaultChangeableProperties(p);
    return p;
}

AutoPtr<IProperties> System::SetDefaultChangeableProperties(
    /* [in] */ IProperties* p)
{}

ECode System::GetProperty(
    /* [in] */ const String& key,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    FAIL_RETURN(CheckKey(key));
    AutoPtr<ISecurityManager> sm = GetSecurityManager();
    if (sm != nullptr) {
        sm->CheckPropertyAccess(key);
    }

    return NOERROR;
}

ECode System::GetProperty(
    /* [in] */ const String& key,
    /* [in] */ const String& def,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    *value = def;
    return NOERROR;
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

void System::StaticInitialize()
{
    sProps = InitProperties();
}

//----------------------------------------------------------------------

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
