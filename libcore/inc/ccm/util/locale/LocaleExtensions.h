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

#ifndef __CCM_UTIL_LOCALE_LOCALEEXTENSIONS_H__
#define __CCM_UTIL_LOCALE_LOCALEEXTENSIONS_H__

#include "ccm/core/SyncObject.h"
#include "ccm/util/locale/Extension.h"
#include "ccm.util.IMap.h"
#include "ccm.util.ISet.h"
#include <ccmautoptr.h>

using ccm::core::SyncObject;

namespace ccm {
namespace util {
namespace locale {

class LocaleExtensions
    : public SyncObject
{
public:
    IInterface* Probe(
        /* [in] */ const InterfaceID& iid) override;

    static AutoPtr<LocaleExtensions> GetCALENDAR_JAPANESE();

    static AutoPtr<LocaleExtensions> GetNUMBER_THAI();

    LocaleExtensions(
        /* [in] */ IMap* extensions,
        /* [in] */ ISet* uattributes,
        /* [in] */ IMap* ukeywords);

    AutoPtr<ISet> GetKeys();

    AutoPtr<Extension> GetExtension(
        /* [in] */ Char key);

    String GetExtensionValue(
        /* [in] */ Char key);

    AutoPtr<ISet> GetUnicodeLocaleAttributes();

    AutoPtr<ISet> GetUnicodeLocaleKeys();

    String GetUnicodeLocaleType(
        /* [in] */ const String& unicodeLocaleKey);

    Boolean IsEmpty();

    static Boolean IsValidKey(
        /* [in] */ Char c);

    static Boolean IsValidUnicodeLocaleKey(
        /* [in] */ const String& ukey);

    ECode ToString(
        /* [out] */ String* desc) override;

    String GetID();

    ECode GetHashCode(
        /* [out] */ Integer* hash) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* same) override;

private:
    LocaleExtensions(
        /* [in] */ const String& id,
        /* [in] */ Char key,
        /* [in] */ Extension* value);

    static String ToID(
        /* [in] */ ISortedMap* map);

private:
    AutoPtr<IMap> mExtensionMap;
    String mId;
};

inline String LocaleExtensions::GetID()
{
    return mId;
}

}
}
}

#endif // __CCM_UTIL_LOCALE_LOCALEEXTENSIONS_H__
