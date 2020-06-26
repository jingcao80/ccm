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

#ifndef __COMO_CORE_STRINGTOREAL_H__
#define __COMO_CORE_STRINGTOREAL_H__

#include <comotypes.h>

namespace como {
namespace core {

class StringToReal
{
private:
    class StringExponentPair
    {
    public:
        Float SpecialValue();

    public:
        String mS;
        Long mE = 0;
        Boolean mNegative = false;

        Boolean mInfinity = false;
        Boolean mZero = false;
    };

public:
    static ECode ParseFloat(
        /* [in] */ const String& s,
        /* [out] */ Float& value);

    static ECode ParseDouble(
        /* [in] */ const String& s,
        /* [out] */ Double& value);

private:
    static Float ParseFltImpl(
        /* [in] */ const String& s,
        /* [in] */ Integer e);

    static Double ParseDblImpl(
        /* [in] */ const String& s,
        /* [in] */ Integer e);

    static ECode InitialParse(
        /* [in] */ const String& s,
        /* [in] */ Integer length,
        /* [in] */ Boolean isDouble,
        /* [out] */ StringExponentPair& pair);

    // Parses "+Nan", "NaN", "-Nan", "+Infinity", "Infinity", and "-Infinity", case-insensitively.
    static ECode ParseName(
        /* [in] */ const String& name,
        /* [in] */ Boolean isDouble,
        /* [out] */ Float& value);
};

}
}

#endif // __COMO_CORE_STRINGTOREAL_H__
