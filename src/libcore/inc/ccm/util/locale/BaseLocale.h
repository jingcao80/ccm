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

#ifndef __CCM_UTIL_LOCALE_BASELOCALE_H__
#define __CCM_UTIL_LOCALE_BASELOCALE_H__

#include "ccm.core.IComparable.h"
#include <ccmautoptr.h>
#include <ccmobject.h>

using ccm::core::IComparable;

namespace ccm {
namespace util {
namespace locale {

class BaseLocale
    : public Object
{
private:
    class Key
        : public Object
        , public IComparable
    {
    public:
        CCM_INTERFACE_DECL();

        inline Key(
            /* [in] */ const String& language,
            /* [in] */ const String& script,
            /* [in] */ const String& region,
            /* [in] */ const String& variant)
            : Key(language, script, region, variant, false)
        {}

    private:
        Key(
            /* [in] */ const String& language,
            /* [in] */ const String& region);

        Key(
            /* [in] */ const String& language,
            /* [in] */ const String& script,
            /* [in] */ const String& region,
            /* [in] */ const String& variant,
            /* [in] */ Boolean normalized);

    private:
        String mLang;
        String mScrt;
        String mRegn;
        String mVart;
        Boolean mNormalized;
        Integer mHash;
    };

    class Cache
    {

    };

public:
    static AutoPtr<BaseLocale> CreateInstance(
        /* [in] */ const String& language,
        /* [in] */ const String& region);

private:
    static Cache* Get_CACHE();

    BaseLocale(
        /* [in] */ const String& language,
        /* [in] */ const String& region);

    BaseLocale(
        /* [in] */ const String& language,
        /* [in] */ const String& script,
        /* [in] */ const String& region,
        /* [in] */ const String& variant);

private:
    String mLanguage;
    String mScript;
    String mRegion;
    String mVariant;
};

}
}
}

#endif // __CCM_UTIL_LOCALE_BASELOCALE_H__
