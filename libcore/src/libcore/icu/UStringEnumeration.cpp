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

#include "libcore/icu/UStringEnumeration.h"
#include <ccmdef.h>

namespace libcore {
namespace icu {

UStringEnumeration* UStringEnumeration::fromUEnumeration(
    UEnumeration* uenumToAdopt, UErrorCode& status)
{
    if (U_FAILURE(status)) {
        uenum_close(uenumToAdopt);
        return nullptr;
    }
    UStringEnumeration* result = new UStringEnumeration(uenumToAdopt);
    if (result == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        uenum_close(uenumToAdopt);
        return nullptr;
    }
    return result;
}

UStringEnumeration::UStringEnumeration(
    UEnumeration* _uenum)
    : uenum(_uenum)
{
    CHECK(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration()
{
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(
    UErrorCode& status) const
{
    return uenum_count(uenum, &status);
}

const char* UStringEnumeration::next(
    int32_t* resultLength, UErrorCode& status)
{
    return uenum_next(uenum, resultLength, &status);
}

const UnicodeString* UStringEnumeration::snext(
    UErrorCode& status)
{
    int32_t length;
    const UChar* str = uenum_unext(uenum, &length, &status);
    if (str == 0 || U_FAILURE(status)) {
        return 0;
    }
    return &unistr.setTo(str, length);
}

void UStringEnumeration::reset(
    UErrorCode& status)
{
    uenum_reset(uenum, &status);
}

}
}
