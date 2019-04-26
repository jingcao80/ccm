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
#include <ccmobject.h>
#include <ccmlogger.h>

using ccm::core::E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
using ccm::core::Math;

namespace ccm {
namespace util {

ECode Arrays::RangeCheck(
    /* [in] */ Integer arrayLength,
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex)
{
    if (fromIndex > toIndex) {
        Logger::E("Arrays", "fromIndex(%d) > toIndex(%d)", fromIndex, toIndex);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (fromIndex < 0) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (toIndex > arrayLength) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

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

Integer Arrays::BinarySearch(
    /* [in] */ const Array<String>& a,
    /* [in] */ const String& key)
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
        Integer mid = ((unsigned Integer)(low + high)) >> 1;
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

Integer Arrays::BinarySearch0(
    /* [in] */ const Array<String>& a,
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex,
    /* [in] */ String key)
{
    Integer low = fromIndex;
    Integer high = toIndex - 1;

    while (low <= high) {
        Integer mid = ((unsigned Integer)(low + high)) >> 1;
        Integer cmp = a[mid].Compare(key);

        if (cmp < 0) {
            low = mid + 1;
        }
        else if (cmp > 0) {
            high = mid - 1;
        }
        else {
            return mid; // key found
        }
    }
    return -(low + 1);  // key not found.
}

Boolean Arrays::Equals(
    /* [in] */ const Array<Long>& a,
    /* [in] */ const Array<Long>& a2)
{
    if (a == a2) {
        return true;
    }
    if (a.IsNull() || a2.IsNull()) {
        return false;
    }

    Integer length = a.GetLength();
    if (a2.GetLength() != length) {
        return false;
    }

    for (Integer i = 0; i < length; i++) {
        if (a[i] != a2[i]) {
            return false;
        }
    }

    return true;
}

Boolean Arrays::Equals(
    /* [in] */ const Array<Integer>& a,
    /* [in] */ const Array<Integer>& a2)
{
    if (a == a2) {
        return true;
    }
    if (a.IsNull() || a2.IsNull()) {
        return false;
    }

    Integer length = a.GetLength();
    if (a2.GetLength() != length) {
        return false;
    }

    for (Integer i = 0; i < length; i++) {
        if (a[i] != a2[i]) {
            return false;
        }
    }

    return true;
}

Boolean Arrays::Equals(
    /* [in] */ const Array<Short>& a,
    /* [in] */ const Array<Short>& a2)
{
    if (a == a2) {
        return true;
    }
    if (a.IsNull() || a2.IsNull()) {
        return false;
    }

    Integer length = a.GetLength();
    if (a2.GetLength() != length) {
        return false;
    }

    for (Integer i = 0; i < length; i++) {
        if (a[i] != a2[i]) {
            return false;
        }
    }

    return true;
}

Boolean Arrays::Equals(
    /* [in] */ const Array<Char>& a,
    /* [in] */ const Array<Char>& a2)
{
    if (a == a2) {
        return true;
    }
    if (a.IsNull() || a2.IsNull()) {
        return false;
    }

    Integer length = a.GetLength();
    if (a2.GetLength() != length) {
        return false;
    }

    for (Integer i = 0; i < length; i++) {
        if (a[i] != a2[i]) {
            return false;
        }
    }

    return true;
}

Boolean Arrays::Equals(
    /* [in] */ const Array<Byte>& a,
    /* [in] */ const Array<Byte>& a2)
{
    if (a == a2) {
        return true;
    }
    if (a.IsNull() || a2.IsNull()) {
        return false;
    }

    Integer length = a.GetLength();
    if (a2.GetLength() != length) {
        return false;
    }

    for (Integer i = 0; i < length; i++) {
        if (a[i] != a2[i]) {
            return false;
        }
    }

    return true;
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
    /* [out] */ Array<Char>& a,
    /* [in] */ Integer fromIndex,
    /* [in] */ Integer toIndex,
    /* [in] */ Char value)
{
    FAIL_RETURN(RangeCheck(a.GetLength(), fromIndex, toIndex));
    for (Integer i = fromIndex; i < toIndex; i++) {
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
    Integer N = Math::Min((Integer)original.GetLength(), newLength);
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
    Integer N = Math::Min((Integer)original.GetLength(), newLength);
    for (Integer i = 0; i < N; i++) {
        copy.Set(i, original[i]);
    }
    *newArray = copy;
    return NOERROR;
}

Integer Arrays::GetHashCode(
    /* [in] */ const Array<Long>& a)
{
    if (a.IsNull()) {
        return 0;
    }

    Integer result = 1;
    for (Long element : a) {
        Integer elementHash = (Integer)(element ^ (((unsigned Long)element) >> 32));
        result = 31 * result + elementHash;
    }

    return result;
}

Integer Arrays::GetHashCode(
    /* [in] */ const Array<Integer>& a)
{
    if (a.IsNull()) {
        return 0;
    }

    Integer result = 1;
    for (Integer element : a) {
        result = 31 * result + element;
    }

    return result;
}

Integer Arrays::GetHashCode(
    /* [in] */ const Array<Short>& a)
{
    if (a.IsNull()) {
        return 0;
    }

    Integer result = 1;
    for (Short element : a) {
        result = 31 * result + element;
    }

    return result;
}

Integer Arrays::GetHashCode(
    /* [in] */ const Array<Char>& a)
{
    if (a.IsNull()) {
        return 0;
    }

    Integer result = 1;
    for (Char element : a) {
        result = 31 * result + element;
    }

    return result;
}

Integer Arrays::GetHashCode(
    /* [in] */ const Array<Byte>& a)
{
    if (a.IsNull()) {
        return 0;
    }

    Integer result = 1;
    for (Byte element : a) {
        result = 31 * result + element;
    }

    return result;
}

Integer Arrays::GetHashCode(
    /* [in] */ const Array<Boolean>& a)
{
    if (a.IsNull()) {
        return 0;
    }

    Integer result = 1;
    for (Boolean element : a) {
        result = 31 * result + (element ? 1231 : 1237);
    }

    return result;
}

Integer Arrays::GetHashCode(
    /* [in] */ const Array<Float>& a)
{
    if (a.IsNull()) {
        return 0;
    }

    Integer result = 1;
    for (Float element : a) {
        result = 31 * result + Math::FloatToIntegerBits(element);
    }

    return result;
}

Integer Arrays::GetHashCode(
    /* [in] */ const Array<Double>& a)
{
    if (a.IsNull()) {
        return 0;
    }

    Integer result = 1;
    for (Double element : a) {
        Long bits = Math::DoubleToLongBits(element);
        result = 31 * result + (Integer)(bits ^ (((unsigned Long)bits) >> 32));
    }

    return result;
}

Integer Arrays::GetHashCode(
    /* [in] */ const Array<IInterface*>& a)
{
    if (a.IsNull()) {
        return 0;
    }

    Integer result = 1;
    for (IInterface* element : a) {
        result = 31 * result + (element == nullptr ?
                0 : Object::GetHashCode(element));
    }

    return result;
}

}
}
