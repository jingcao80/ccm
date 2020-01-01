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

#ifndef __COMO_UTIL_LOCALEFACTORY_H__
#define __COMO_UTIL_LOCALEFACTORY_H__

#include "como.util.ILocale.h"

namespace como {
namespace util {

class COM_PUBLIC LocaleFactory
{
public:
    static ECode GetCANADA(
        /* [out] */ ILocale** l);

    static ECode GetCANADA_FRENCH(
        /* [out] */ ILocale** l);

    static ECode GetCHINA(
        /* [out] */ ILocale** l);

    static ECode GetCHINESE(
        /* [out] */ ILocale** l);

    static ECode GetDefault(
        /* [out] */ ILocale** l);

    static ECode GetENGLISH(
        /* [out] */ ILocale** l);

    static ECode GetFRANCE(
        /* [out] */ ILocale** l);

    static ECode GetFRENCH(
        /* [out] */ ILocale** l);

    static ECode GetGERMAN(
        /* [out] */ ILocale** l);

    static ECode GetGERMANY(
        /* [out] */ ILocale** l);

    static ECode GetITALIAN(
        /* [out] */ ILocale** l);

    static ECode GetITALY(
        /* [out] */ ILocale** l);

    static ECode GetJAPAN(
        /* [out] */ ILocale** l);

    static ECode GetJAPANESE(
        /* [out] */ ILocale** l);

    static ECode GetKOREA(
        /* [out] */ ILocale** l);

    static ECode GetKOREAN(
        /* [out] */ ILocale** l);

    static ECode GetPRC(
        /* [out] */ ILocale** l);

    static ECode GetROOT(
        /* [out] */ ILocale** l);

    static ECode GetSIMPLIFIED_CHINESE(
        /* [out] */ ILocale** l);

    static ECode GetTAIWAN(
        /* [out] */ ILocale** l);

    static ECode GetTRADITIONAL_CHINESE(
        /* [out] */ ILocale** l);

    static ECode GetUK(
        /* [out] */ ILocale** l);

    static ECode GetUS(
        /* [out] */ ILocale** l);

private:
    LocaleFactory();
};

}
}

#endif // __COMO_UTIL_LOCALEFACTORY_H__
