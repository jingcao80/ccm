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

#include "ccm/math/Division.h"

namespace ccm {
namespace math {

Integer Division::DivideArrayByInteger(
    /* [in] */ Array<Integer>& quotient,
    /* [in] */ Array<Integer>& dividend,
    /* [in] */ Integer dividendLength,
    /* [in] */ Integer divisor)
{
    Long rem = 0;
    Long bLong = divisor & 0xffffffffLL;

    for (Integer i = dividendLength - 1; i >= 0; i--) {
        Long temp = (rem << 32) | (dividend[i] & 0xffffffffLL);
        Long quot;
        if (temp >= 0) {
            quot = (temp / bLong);
            rem = (temp % bLong);
        }
        else {
            /*
             * make the dividend positive shifting it right by 1 bit then
             * get the quotient an remainder and correct them properly
             */
            Long aPos = ((unsigned Long)temp) >> 1;
            Long bPos = ((unsigned Integer)divisor) >> 1;
            quot = aPos / bPos;
            rem = aPos % bPos;
            // double the remainder and add 1 if a is odd
            rem = (rem << 1) + (temp & 1);
            if ((divisor & 1) != 0) {
                // the divisor is odd
                if (quot <= rem) {
                    rem -= quot;
                }
                else {
                    if (quot - rem <= bLong) {
                        rem += bLong - quot;
                        quot -= 1;
                    }
                    else {
                        rem += (bLong << 1) - quot;
                        quot -= 2;
                    }
                }
            }
        }
        quotient[i] = (Integer)(quot & 0xffffffffLL);
    }
    return (Integer)rem;
}

}
}
