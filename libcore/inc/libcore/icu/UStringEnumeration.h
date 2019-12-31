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

#ifndef __LIBCORE_ICU_USTRINGENUMERATION_H__
#define __LIBCORE_ICU_USTRINGENUMERATION_H__

#include <unicode/uenum.h>
#include <unicode/strenum.h>

namespace libcore {
namespace icu {

/**
 * A wrapper to make a UEnumeration into a StringEnumeration.  The
 * wrapper adopts the UEnumeration is wraps.
 */
class UStringEnumeration : public StringEnumeration {

public:
    /**
     * Constructor.  This constructor adopts its UEnumeration
     * argument.
     * @param uenum a UEnumeration object.  This object takes
     * ownership of 'uenum' and will close it in its destructor.  The
     * caller must not call uenum_close on 'uenum' after calling this
     * constructor.
     */
    UStringEnumeration(UEnumeration* uenum);

    /**
     * Destructor.  This closes the UEnumeration passed in to the
     * constructor.
     */
    virtual ~UStringEnumeration();

    /**
     * Return the number of elements that the iterator traverses.
     * @param status the error code.
     * @return number of elements in the iterator.
     */
    virtual int32_t count(UErrorCode& status) const;

    virtual const char* next(int32_t* resultLength, UErrorCode& status);

    /**
     * Returns the next element a UnicodeString*.  If there are no
     * more elements, returns NULL.
     * @param status the error code.
     * @return a pointer to the string, or NULL.
     */
    virtual const UnicodeString* snext(UErrorCode& status);

    /**
     * Resets the iterator.
     * @param status the error code.
     */
    virtual void reset(UErrorCode& status);

    static UStringEnumeration* fromUEnumeration(
            UEnumeration* enumToAdopt, UErrorCode& status);

private:
    UEnumeration* uenum; // owned
};

}
}

#endif // __LIBCORE_ICU_USTRINGENUMERATION_H__
