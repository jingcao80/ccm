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

#include "ccm/core/CArray.h"
#include "ccm/core/CoreUtils.h"
#include "ccm/util/CHashMap.h"
#include "ccm/util/locale/LanguageTag.h"
#include "ccm.core.IArray.h"
#include "ccm.core.ICharSequence.h"
#include "ccm.core.IString.h"
#include "ccm.util.IMap.h"

using ccm::core::CArray;
using ccm::core::CoreUtils;
using ccm::core::IArray;
using ccm::core::ICharSequence;
using ccm::core::IID_IArray;
using ccm::core::IID_IString;

namespace ccm {
namespace util {
namespace locale {

CCM_INTERFACE_IMPL_1(LanguageTag, SyncObject, ILanguageTag);

static void PutStringArray(
    /* [in] */ IMap* map,
    /* [in] */ const String& tag,
    /* [in] */ const String& prefered)
{
    AutoPtr<ICharSequence> key = CoreUtils::Box(tag);
    AutoPtr<ICharSequence> value = CoreUtils::Box(prefered);
    AutoPtr<IArray> strArray;
    CArray::New(IID_IString, 2, IID_IArray, (IInterface**)&strArray);
    strArray->Set(0, key);
    strArray->Set(1, value);
    map->Put(key, value);
}

static AutoPtr<IMap> CreateGRANDFATHERED()
{
    AutoPtr<IMap> map;
    CHashMap::New(IID_IMap, (IInterface**)&map);
    PutStringArray(map, String("art-lojban"), String("jbo"));
    PutStringArray(map, String("cel-gaulish"), String("xtg-x-cel-gaulish"));   // fallback
    PutStringArray(map, String("en-GB-oed"), String("en-GB-x-oed"));         // fallback
    PutStringArray(map, String("i-ami"), String("ami"));
    PutStringArray(map, String("i-bnn"), String("bnn"));
    PutStringArray(map, String("i-default"), String("en-x-i-default"));      // fallback
    PutStringArray(map, String("i-enochian"), String("und-x-i-enochian"));    // fallback
    PutStringArray(map, String("i-hak"), String("hak"));
    PutStringArray(map, String("i-klingon"), String("tlh"));
    PutStringArray(map, String("i-lux"), String("lb"));
    PutStringArray(map, String("i-mingo"), String("see-x-i-mingo"));       // fallback
    PutStringArray(map, String("i-navajo"), String("nv"));
    PutStringArray(map, String("i-pwn"), String("pwn"));
    PutStringArray(map, String("i-tao"), String("tao"));
    PutStringArray(map, String("i-tay"), String("tay"));
    PutStringArray(map, String("i-tsu"), String("tsu"));
    PutStringArray(map, String("no-bok"), String("nb"));
    PutStringArray(map, String("no-nyn"), String("nn"));
    PutStringArray(map, String("sgn-BE-FR"), String("sfb"));
    PutStringArray(map, String("sgn-BE-NL"), String("vgt"));
    PutStringArray(map, String("sgn-CH-DE"), String("sgg"));
    PutStringArray(map, String("zh-guoyu"), String("cmn"));
    PutStringArray(map, String("zh-hakka"), String("hak"));
    PutStringArray(map, String("zh-min"), String("nan-x-zh-min"));        // fallback
    PutStringArray(map, String("zh-min-nan"), String("nan"));
    PutStringArray(map, String("zh-xiang"), String("hsn"));
    return map;
}

AutoPtr<IMap> LanguageTag::GetGRANDFATHERED()
{
    static AutoPtr<IMap> GRANDFATHERED = CreateGRANDFATHERED();
    return GRANDFATHERED;
}

}
}
}
