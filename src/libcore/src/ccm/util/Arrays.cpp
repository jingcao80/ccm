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

#include "ccm/core/Math.h"
#include "ccm/util/Arrays.h"
#include "libcore.h"

using ccm::core::Math;

namespace ccm {
namespace util {

ECode Arrays::CheckOffsetAndCount(
    /* [in] */ Integer arrayLength,
    /* [in] */ Integer offset,
    /* [in] */ Integer count)
{
    if ((offset | count) < 0 || offset > arrayLength || arrayLength - offset < count) {
        return ccm::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

Integer Arrays::BinarySearch(
    /* [in] */ const Array<Long>& a,
    /* [in] */ Long key)
{
    return BinarySearch0(a, 0, a.GetLength(), key);
}

Integer Arrays::BinarySearch0(
    /* [in] */ const Array<Long>& a,
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex,
    /* [in] */ Long key)
{
    Integer low = fromIndex;
    Integer high = toIndex - 1;

    while (low <= high) {
        Integer mid = (low + high) >> 1;
        Long midVal = a[mid];

        if (midVal < key) {
            low = mid + 1;
        }
        else if (midVal > key) {
            high = mid - 1;
        }
        else {
            return mid; // key found
        }
    }
    return -(low + 1);  // key not found.
}

ECode Arrays::Fill(
    /* [in] */ Array<Char>& a,
    /* [in] */ Char value)
{
    for (Integer i = 0, len = a.GetLength(); i < len; i++) {
        a[i] = value;
    }
    return NOERROR;
}

ECode Arrays::Fill(
    /* [in] */ Array<Boolean>& a,
    /* [in] */ Boolean value)
{
    for (Integer i = 0, len = a.GetLength(); i < len; i++) {
        a[i] = value;
    }
    return NOERROR;
}

ECode Arrays::CopyOf(
    /* [in] */ const Array<String> & original,
    /* [in] */ Integer newLength,
    /* [out, callee] */ Array<String>* newArray)
{
    VALIDATE_NOT_NULL(newArray);

    if (newLength < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Array<String> copy = Array<String>(newLength);
    Integer N = Math::Min(original.GetLength(), newLength);
    for (Integer i = 0; i < N; i++) {
        copy[i] = original[i];
    }
    *newArray = copy;
    return NOERROR;
}

ECode Arrays::CopyOf(
    /* [in] */ const Array<IInterface*>& original,
    /* [in] */ Integer newLength,
    /* [out, callee] */ Array<IInterface*>* newArray)
{
    VALIDATE_NOT_NULL(newArray);

    if (newLength < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Array<IInterface*> copy = Array<IInterface*>(newLength);
    Integer N = Math::Min(original.GetLength(), newLength);
    for (Integer i = 0; i < N; i++) {
        copy.Set(i, original[i]);
    }
    *newArray = copy;
    return NOERROR;
}

}
}
