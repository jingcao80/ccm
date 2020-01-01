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

#include "como/util/Locale.h"
#include "como/util/LocaleFactory.h"

namespace como {
namespace util {

ECode LocaleFactory::GetCANADA(
    /* [out] */ ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetCANADA().MoveTo(l);
    return NOERROR;
}

ECode LocaleFactory::GetCANADA_FRENCH(
    /* [out] */ ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetCANADA_FRENCH().MoveTo(l);
    return NOERROR;
}

ECode LocaleFactory::GetCHINA(
    /* [out] */ ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetCHINA().MoveTo(l);
    return NOERROR;
}

ECode LocaleFactory::GetCHINESE(
    /* [out] */ ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetCHINESE().MoveTo(l);
    return NOERROR;
}

ECode LocaleFactory::GetDefault(
    /* [out] */ ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetDefault().MoveTo(l);
    return NOERROR;
}

ECode LocaleFactory::GetENGLISH(
    /* [out] */ ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetENGLISH().MoveTo(l);
    return NOERROR;
}

ECode LocaleFactory::GetFRANCE(
    /* [out] */ ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetFRANCE().MoveTo(l);
    return NOERROR;
}

ECode LocaleFactory::GetFRENCH(
    /* [out] */ ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetFRENCH().MoveTo(l);
    return NOERROR;
}

ECode LocaleFactory::GetGERMAN(
    /* [out] */ ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetGERMAN().MoveTo(l);
    return NOERROR;
}

ECode LocaleFactory::GetGERMANY(
    /* [out] */ ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetGERMANY().MoveTo(l);
    return NOERROR;
}

ECode LocaleFactory::GetITALIAN(
    /* [out] */ ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetITALIAN().MoveTo(l);
    return NOERROR;
}

ECode LocaleFactory::GetITALY(
    /* [out] */ ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetITALY().MoveTo(l);
    return NOERROR;
}

ECode LocaleFactory::GetJAPAN(
    /* [out] */ ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetJAPAN().MoveTo(l);
    return NOERROR;
}

ECode LocaleFactory::GetJAPANESE(
    /* [out] */ ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetJAPANESE().MoveTo(l);
    return NOERROR;
}

ECode LocaleFactory::GetKOREA(
    /* [out] */ ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetKOREA().MoveTo(l);
    return NOERROR;
}

ECode LocaleFactory::GetKOREAN(
    /* [out] */ ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetKOREAN().MoveTo(l);
    return NOERROR;
}

ECode LocaleFactory::GetPRC(
    /* [out] */ ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetPRC().MoveTo(l);
    return NOERROR;
}

ECode LocaleFactory::GetROOT(
    /* [out] */ ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetROOT().MoveTo(l);
    return NOERROR;
}

ECode LocaleFactory::GetSIMPLIFIED_CHINESE(
    /* [out] */ ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetSIMPLIFIED_CHINESE().MoveTo(l);
    return NOERROR;
}

ECode LocaleFactory::GetTAIWAN(
    /* [out] */ ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetTAIWAN().MoveTo(l);
    return NOERROR;
}

ECode LocaleFactory::GetTRADITIONAL_CHINESE(
    /* [out] */ ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetTRADITIONAL_CHINESE().MoveTo(l);
    return NOERROR;
}

ECode LocaleFactory::GetUK(
    /* [out] */ ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetUK().MoveTo(l);
    return NOERROR;
}

ECode LocaleFactory::GetUS(
    /* [out] */ ILocale** l)
{
    VALIDATE_NOT_NULL(l);

    Locale::GetUS().MoveTo(l);
    return NOERROR;
}

}
}
