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

#include "ccm/util/CLocaleFactory.h"
#include "ccm/util/Locale.h"

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_1(CLocaleFactory, Object, ILocaleFactory);

CCM_OBJECT_IMPL(CLocaleFactory);

ECode CLocaleFactory::GetCANADA(
    /* [out] */  ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetCANADA().MoveTo(l);
    return NOERROR;
}

ECode CLocaleFactory::GetCANADA_FRENCH(
    /* [out] */  ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetCANADA_FRENCH().MoveTo(l);
    return NOERROR;
}

ECode CLocaleFactory::GetCHINA(
    /* [out] */  ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetCHINA().MoveTo(l);
    return NOERROR;
}

ECode CLocaleFactory::GetCHINESE(
    /* [out] */  ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetCHINESE().MoveTo(l);
    return NOERROR;
}

ECode CLocaleFactory::GetDefault(
    /* [out] */  ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetDefault().MoveTo(l);
    return NOERROR;
}

ECode CLocaleFactory::GetENGLISH(
    /* [out] */  ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetENGLISH().MoveTo(l);
    return NOERROR;
}

ECode CLocaleFactory::GetFRANCE(
    /* [out] */  ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetFRANCE().MoveTo(l);
    return NOERROR;
}

ECode CLocaleFactory::GetFRENCH(
    /* [out] */  ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetFRENCH().MoveTo(l);
    return NOERROR;
}

ECode CLocaleFactory::GetGERMAN(
    /* [out] */  ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetGERMAN().MoveTo(l);
    return NOERROR;
}

ECode CLocaleFactory::GetGERMANY(
    /* [out] */  ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetGERMANY().MoveTo(l);
    return NOERROR;
}

ECode CLocaleFactory::GetITALIAN(
    /* [out] */  ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetITALIAN().MoveTo(l);
    return NOERROR;
}

ECode CLocaleFactory::GetITALY(
    /* [out] */  ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetITALY().MoveTo(l);
    return NOERROR;
}

ECode CLocaleFactory::GetJAPAN(
    /* [out] */  ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetJAPAN().MoveTo(l);
    return NOERROR;
}

ECode CLocaleFactory::GetJAPANESE(
    /* [out] */  ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetJAPANESE().MoveTo(l);
    return NOERROR;
}

ECode CLocaleFactory::GetKOREA(
    /* [out] */  ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetKOREA().MoveTo(l);
    return NOERROR;
}

ECode CLocaleFactory::GetKOREAN(
    /* [out] */  ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetKOREAN().MoveTo(l);
    return NOERROR;
}

ECode CLocaleFactory::GetPRC(
    /* [out] */  ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetPRC().MoveTo(l);
    return NOERROR;
}

ECode CLocaleFactory::GetROOT(
    /* [out] */  ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetROOT().MoveTo(l);
    return NOERROR;
}

ECode CLocaleFactory::GetSIMPLIFIED_CHINESE(
    /* [out] */  ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetSIMPLIFIED_CHINESE().MoveTo(l);
    return NOERROR;
}

ECode CLocaleFactory::GetTAIWAN(
    /* [out] */  ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetTAIWAN().MoveTo(l);
    return NOERROR;
}

ECode CLocaleFactory::GetTRADITIONAL_CHINESE(
    /* [out] */  ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetTRADITIONAL_CHINESE().MoveTo(l);
    return NOERROR;
}

ECode CLocaleFactory::GetUK(
    /* [out] */  ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetUK().MoveTo(l);
    return NOERROR;
}

ECode CLocaleFactory::GetUS(
    /* [out] */  ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetUS().MoveTo(l);
    return NOERROR;
}

}
}
