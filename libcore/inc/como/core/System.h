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

#ifndef __COMO_CORE_SYSTEM_H__
#define __COMO_CORE_SYSTEM_H__

#include "como/util/Properties.h"
#include "como.core.ISecurityManager.h"
#include "como.core.IStackTrace.h"
#include "como.io.IPrintStream.h"
#include "como.util.IProperties.h"
#include <comosp.h>
#include <comotypes.h>

using como::io::IPrintStream;
using como::util::IProperties;
using como::util::Properties;

namespace como {
namespace core {

class System
{
protected:
    class PropertiesWithNonOverrideableDefaults
        : public Properties
    {
    public:
        ECode Constructor(
            /* [in] */ IProperties* defaults);

        ECode Clone(
            /* [in] */ const InterfaceID& iid,
            /* [out] */ IInterface** obj) override;

        ECode ToString(
            /* [in] */ String& str) override;
    };

public:
    static AutoPtr<IPrintStream> GetOut();

    static AutoPtr<ISecurityManager> GetSecurityManager();

    static Long GetCurrentTimeMillis();

    static Long GetNanoTime();

    static ECode SetUnchangeableSystemProperty(
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    static String GetLineSeparator();

    static ECode GetProperty(
        /* [in] */ const String& key,
        /* [out] */ String* value);

    static ECode GetProperty(
        /* [in] */ const String& key,
        /* [in] */ const String& def,
        /* [out] */ String* value);

    static ECode GetEnv(
        /* [in] */ const String& name,
        /* [out] */ String* value);

    static ECode LogE(
        /* [in] */ const String& message);

    static ECode LogW(
        /* [in] */ const String& message,
        /* [in] */ IStackTrace* st);

    static ECode StaticInitialize();

private:
    System();

    static AutoPtr<IProperties> InitUnchangeableSystemProperties();

    static AutoPtr<IProperties> InitProperties();

    static AutoPtr<IProperties> SetDefaultChangeableProperties(
        /* [in] */ IProperties* p);

    static ECode AddLegacyLocaleSystemProperties();

    static ECode CheckKey(
        /* [in] */ const String& key);

    static ECode Log(
        /* [in] */ Char type,
        /* [in] */ const String& message,
        /* [in] */ IStackTrace* st);

private:
    static AutoPtr<IProperties> sProps;

    static AutoPtr<IProperties> sUnchangeableProps;

    static String sLineSeparator;

    friend void StaticInitializeSystem();
};

inline String System::GetLineSeparator()
{
    return sLineSeparator;
}

inline ECode System::LogE(
    /* [in] */ const String& message)
{
    return Log(U'E', message, nullptr);
}

inline ECode System::LogW(
    /* [in] */ const String& message,
    /* [in] */ IStackTrace* st)
{
    return Log(U'W', message, st);
}

}
}

#endif // __COMO_CORE_SYSTEM_H__
