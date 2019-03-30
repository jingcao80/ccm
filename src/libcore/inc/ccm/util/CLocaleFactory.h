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

#ifndef __CCM_UTIL_CLOCALEFACTORY_H__
#define __CCM_UTIL_CLOCALEFACTORY_H__

#include "ccm.util.ILocale.h"
#include "ccm.util.ILocaleFactory.h"
#include "_ccm_util_CLocaleFactory.h"
#include <ccmobject.h>

namespace ccm {
namespace util {

class CLocaleFactory
    : public Object
    , public ILocaleFactory
{
public:
    CCM_INTERFACE_DECL();

    CCM_OBJECT_DECL();

    ECode GetCANADA(
        /* [out] */  ILocale** l) override;

    ECode GetCANADA_FRENCH(
        /* [out] */  ILocale** l) override;

    ECode GetCHINA(
        /* [out] */  ILocale** l) override;

    ECode GetCHINESE(
        /* [out] */  ILocale** l) override;

    ECode GetDefault(
        /* [out] */  ILocale** l) override;

    ECode GetENGLISH(
        /* [out] */  ILocale** l) override;

    ECode GetFRANCE(
        /* [out] */  ILocale** l) override;

    ECode GetFRENCH(
        /* [out] */  ILocale** l) override;

    ECode GetGERMAN(
        /* [out] */  ILocale** l) override;

    ECode GetGERMANY(
        /* [out] */  ILocale** l) override;

    ECode GetITALIAN(
        /* [out] */  ILocale** l) override;

    ECode GetITALY(
        /* [out] */  ILocale** l) override;

    ECode GetJAPAN(
        /* [out] */  ILocale** l) override;

    ECode GetJAPANESE(
        /* [out] */  ILocale** l) override;

    ECode GetKOREA(
        /* [out] */  ILocale** l) override;

    ECode GetKOREAN(
        /* [out] */  ILocale** l) override;

    ECode GetPRC(
        /* [out] */  ILocale** l) override;

    ECode GetROOT(
        /* [out] */  ILocale** l) override;

    ECode GetSIMPLIFIED_CHINESE(
        /* [out] */  ILocale** l) override;

    ECode GetTAIWAN(
        /* [out] */  ILocale** l) override;

    ECode GetTRADITIONAL_CHINESE(
        /* [out] */  ILocale** l) override;

    ECode GetUK(
        /* [out] */  ILocale** l) override;

    ECode GetUS(
        /* [out] */  ILocale** l) override;
};

}
}

#endif // __CCM_UTIL_CLOCALEFACTORY_H__
