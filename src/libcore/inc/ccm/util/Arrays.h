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

#ifndef __CCM_UTIL_ARRAYS_H__
#define __CCM_UTIL_ARRAYS_H__

#include "ccm.util.IList.h"

namespace ccm {
namespace util {

class Arrays
{
public:
    static ECode CheckOffsetAndCount(
        /* [in] */ Integer arrayLength,
        /* [in] */ Integer offset,
        /* [in] */ Integer count);

    static Integer BinarySearch(
        /* [in] */ const Array<Long>& a,
        /* [in] */ Long key);

    static ECode Fill(
        /* [out] */ Array<Char>& a,
        /* [in] */ Char value);

    static ECode Fill(
        /* [out] */ Array<Char>& a,
        /* [in] */ Integer fromIndex,
        /* [in] */ Integer toIndex,
        /* [in] */ Char value);

    static ECode Fill(
        /* [out] */ Array<Boolean>& a,
        /* [in] */ Boolean value);

    static ECode CopyOf(
        /* [in] */ const Array<String> & original,
        /* [in] */ Integer newLength,
        /* [out, callee] */ Array<String>* newArray);

    static ECode CopyOf(
        /* [in] */ const Array<IInterface*>& original,
        /* [in] */ Integer newLength,
        /* [out, callee] */ Array<IInterface*>* newArray);

    static ECode AsList(
        /* [in] */ const Array<IInterface*>& a,
        /* [out] */ IList** list)
    {
        return NOERROR;
    }

private:
    Arrays()
    {}

    static ECode RangeCheck(
        /* [in] */ Integer arrayLength,
        /* [in] */ Integer fromIndex,
        /* [in] */ Integer toIndex);

    static Integer BinarySearch0(
        /* [in] */ const Array<Long>& a,
        /* [in] */ Integer fromIndex,
        /* [in] */ Integer toIndex,
        /* [in] */ Long key);
};

}
}

#endif // __CCM_UTIL_ARRAYS_H__
