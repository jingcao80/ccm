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

#ifndef __COMO_UTIL_LOCALE_EXTENSION_H__
#define __COMO_UTIL_LOCALE_EXTENSION_H__

#include "como.util.locale.ILanguageTag.h"
#include <ccmobject.h>

namespace como {
namespace util {
namespace locale {

class Extension
    : public Object
{
public:
    Extension(
        /* [in] */ Char key);

    Extension(
        /* [in] */ Char key,
        /* [in] */ const String& value);

    Char GetKey();

    String GetValue();

    String GetID();

    ECode ToString(
        /* [out] */ String* desc) override;

protected:
    void SetValue(
        /* [in] */ const String& value);

private:
    Char mKey;
    String mValue;
    String mId;
};

inline Extension::Extension(
    /* [in] */ Char key)
    : mKey(key)
{}

inline Extension::Extension(
    /* [in] */ Char key,
    /* [in] */ const String& value)
    : mKey(key)
{
    SetValue(value);
}

inline void Extension::SetValue(
    /* [in] */ const String& value)
{
    mValue = value;
    mId = String::Format("%c%s%s", mKey, ILanguageTag::SEP.string(), mValue.string());
}

inline Char Extension::GetKey()
{
    return mKey;
}

inline String Extension::GetValue()
{
    return mValue;
}

inline String Extension::GetID()
{
    return mId;
}

}
}
}

#endif // __COMO_UTIL_LOCALE_EXTENSION_H__
