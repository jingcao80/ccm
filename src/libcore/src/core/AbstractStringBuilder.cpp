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

#include "core/AbstractStringBuilder.h"
#include "core/CString.h"
#include <ccmlogger.h>

namespace ccm {
namespace core {

CCM_INTERFACE_IMPL_2(AbstractStringBuilder, SyncObject, IAppendable, ICharSequence);

AbstractStringBuilder::~AbstractStringBuilder()
{
    if (mValue != nullptr) {
        free(mValue);
    }
}

ECode AbstractStringBuilder::constructor(
    /* [in] */ Integer capacity)
{
    mValue = (char*)malloc(capacity);
    if (mValue == nullptr) {
        Logger::E("AbstractStringBuilder", "Malloc mValue failed.");
    }
    mCapacity = capacity;
    return NOERROR;
}

ECode AbstractStringBuilder::GetLength(
    /* [out] */ Integer* number)
{
    VALIDATE_NOT_NULL(number);

    *number = mCount;
    return NOERROR;
}

ECode AbstractStringBuilder::GetCapacity(
    /* [out] */ Integer* capacity)
{
    VALIDATE_NOT_NULL(capacity);

    *capacity = mCapacity;
    return NOERROR;
}

ECode AbstractStringBuilder::EnsureCapacity(
    /* [in] */ Integer minimumCapacity)
{
    if (minimumCapacity > 0) {
        EnsureCapacityInternal(minimumCapacity);
    }
    return NOERROR;
}

void AbstractStringBuilder::EnsureCapacityInternal(
    /* [in] */ Integer minimumCapacity)
{
    if (minimumCapacity - mCapacity > 0) {
        char* oldValue = mValue;
        Integer newCapacity = NewCapacity(minimumCapacity);
        mValue = (char*)malloc(newCapacity);
        if (mValue == nullptr) {
            Logger::E("AbstractStringBuilder", "Malloc mValue failed.");
            return;
        }
        memcpy(mValue, oldValue, mCapacity);
        mCapacity = newCapacity;
        free(oldValue);
    }
}

Integer AbstractStringBuilder::NewCapacity(
    /* [in] */ Integer minCapacity)
{
    Integer newCapacity = (mCapacity << 1) + 2;
    if (newCapacity - minCapacity < 0) {
        newCapacity = minCapacity;
    }
    if (MAX_ARRAY_SIZE - newCapacity < 0) {
        Logger::E("AbstractStringBuilder", "Out of memory.");
        return MAX_ARRAY_SIZE;
    }
    return newCapacity;
}

ECode AbstractStringBuilder::TrimToSize()
{
    if (mByteCount < mCapacity) {
        char* oldValue = mValue;
        mValue = (char*)malloc(mByteCount);
        if (mValue == nullptr) {
            Logger::E("AbstractStringBuilder", "Malloc mValue failed.");
            return E_OUT_OF_MEMORY_ERROR;
        }
        memcpy(mValue, oldValue, mByteCount);
        mCapacity = mByteCount;
        free(oldValue);
    }
    return NOERROR;
}

ECode AbstractStringBuilder::SetLength(
    /* [in] */ Integer newLength)
{
    if (newLength < 0) {
        return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    EnsureCapacityInternal(newLength - mCount + mCapacity);

    if (mCount < newLength) {
        memset(mValue + mByteCount, 0, newLength - mCount);
        mByteCount += newLength - mCount;
    }

    mCount = newLength;
    return NOERROR;
}

ECode AbstractStringBuilder::GetCharAt(
    /* [in] */ Integer index,
    /* [out] */ Char* c)
{
    VALIDATE_NOT_NULL(c);

    if (index < 0 || index >= mCount) {
        return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Integer byteSize;
    const char* p = mValue;
    const char* end = mValue + mByteCount;
    while (*p && p < end) {
        Char unicode = GetCharInternal(p, &byteSize);
        if (byteSize == 0 || p + byteSize >= end) {
            *c = String::INVALID_CHAR;
            return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        }
        if (index == 0) {
            *c = unicode;
            break;
        }
        p += byteSize;
        index -= 1;
    }
    return NOERROR;
}

Char AbstractStringBuilder::GetCharInternal(
    /* [in] */ const char* cur,
    /* [in] */ Integer* byteSize)
{
    if (IsASCII(*cur)) {
        *byteSize = 1;
        return *cur;
    }

    const char first_char = *cur++;
    Char result = first_char;
    Char mask, to_ignore_mask;
    Integer num_to_read = 0;
    for (num_to_read = 1, mask = 0x40, to_ignore_mask = 0xFFFFFF80;
         (first_char & mask);
         num_to_read++, to_ignore_mask |= mask, mask >>= 1) {
        // 0x3F == 00111111
        result = (result << 6) + (*cur++ & 0x3F);
    }
    to_ignore_mask |= mask;
    result &= ~(to_ignore_mask << (6 * (num_to_read - 1)));
    *byteSize = num_to_read;
    return result;
}

ECode AbstractStringBuilder::GetChars(
    /* [in] */ Integer srcStart,
    /* [in] */ Integer srcEnd,
    /* [out] */ Array<Char>& dst,
    /* [in] */ Integer dstStart)
{
    if (srcStart < 0 || srcEnd < 0 || srcEnd > mCount) {
        return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (srcStart > srcEnd) {
        Logger::E("AbstractStringBuilder", "start > end.");
        return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Integer byteSize, i = 0;
    const char* p = mValue;
    const char* end = mValue + mByteCount;
    while (*p && p < end) {
        Char unicode = GetCharInternal(p, &byteSize);
        if (byteSize == 0 || p + byteSize >= end) {
            return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        }
        if (i >= srcStart && i <= srcEnd - 1) {
            dst[dstStart + i - srcStart] = unicode;
            if (i == srcEnd - 1) {
                break;
            }
        }
        p += byteSize;
        i++;
    }
    return NOERROR;
}

ECode AbstractStringBuilder::SetCharAt(
    /* [in] */ Integer index,
    /* [in] */ Char ch)
{
    if (index < 0 || index >= mCount) {
        return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Integer chByteSize = String::GetByteSize(ch);
    Integer byteSize;
    const char* p = mValue;
    const char* end = mValue + mByteCount;
    while (*p && p < end) {
        byteSize = String::UTF8SequenceLength(*p);
        if (byteSize == 0 || p + byteSize >= end) {
            return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        }
        if (index == 0) {
            if (chByteSize > byteSize) {
                ptrdiff_t offset = p - mValue;
                EnsureCapacityInternal(mByteCount + chByteSize - byteSize);
                p = mValue + offset;
            }
            else if (chByteSize < byteSize) {
                memmove(const_cast<char*>(p) + chByteSize, p + byteSize,
                        mByteCount - (p - mValue + byteSize));
            }
            String::WriteUTF8Bytes(const_cast<char*>(p), ch, chByteSize);
            break;
        }
        p += byteSize;
        index -= 1;
    }

    return NOERROR;
}

ECode AbstractStringBuilder::Append(
    /* [in] */ const String& str)
{
    if (str.IsNull()) {
        return AppendNull();
    }
    Integer len = str.GetByteLength();
    EnsureCapacityInternal(mByteCount + len);
    memcpy(mValue + mByteCount, str.string(), len);
    mCount += str.GetLength();
    mByteCount += len;
    return NOERROR;
}

ECode AbstractStringBuilder::Append(
    /* [in] */ IStringBuffer* sb)
{
    if (sb == nullptr) {
        return AppendNull();
    }
    String str;
    ICharSequence::Probe(sb)->ToString(&str);
    return Append(str);
}

ECode AbstractStringBuilder::Append(
    /* [in] */ ICharSequence* s)
{
    if (s == nullptr) {
        return AppendNull();
    }

    Integer len;
    s->GetLength(&len);
    return Append(s, 0, len);
}

ECode AbstractStringBuilder::AppendNull()
{
    Integer c = mByteCount;
    EnsureCapacityInternal(c + 4);
    mValue[c++] = 'n';
    mValue[c++] = 'u';
    mValue[c++] = 'l';
    mValue[c++] = 'l';
    mByteCount = c;
    mCount += 4;
    return NOERROR;
}

ECode AbstractStringBuilder::Append(
    /* [in] */ ICharSequence* s,
    /* [in] */ Integer start,
    /* [in] */ Integer end)
{
    if (s == nullptr) {
        return AppendNull();
    }
    Integer len;
    s->GetLength(&len);
    if (start < 0 || start > end || end > len) {
        Logger::E("AbstractStringBuilder", "start %d, end %d, length %d",
                start, end, len);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    String str;
    s->ToString(&str);
    String substr = str.Substring(start, end);
    return Append(substr);
}

ECode AbstractStringBuilder::Append(
    /* [in] */ const Array<Char>& str)
{
    Integer len = str.GetLength();
    EnsureCapacityInternal(mByteCount + len * 4);
    for (Integer i = 0; i < len; i++) {
        Append(str[i]);
    }
    return NOERROR;
}

ECode AbstractStringBuilder::Append(
    /* [in] */ const Array<Char>& str,
    /* [in] */ Integer offset,
    /* [in] */ Integer len)
{
    if (offset < 0 || offset >= str.GetLength() || len < 0 ||
        (offset + len >= str.GetLength())) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    EnsureCapacityInternal(mByteCount + len * 4);
    for (Integer i = offset; i < offset + len; i++) {
        Append(str[i]);
    }
    return NOERROR;
}

ECode AbstractStringBuilder::Append(
    /* [in] */ Boolean b)
{
    if (b) {
        EnsureCapacityInternal(mByteCount + 4);
        mValue[mByteCount++] = 't';
        mValue[mByteCount++] = 'r';
        mValue[mByteCount++] = 'u';
        mValue[mByteCount++] = 'e';
        mCount += 4;
    }
    else {
        EnsureCapacityInternal(mByteCount + 5);
        mValue[mByteCount++] = 'f';
        mValue[mByteCount++] = 'a';
        mValue[mByteCount++] = 'l';
        mValue[mByteCount++] = 's';
        mValue[mByteCount++] = 'e';
        mCount += 5;
    }
    return NOERROR;
}

ECode AbstractStringBuilder::Append(
    /* [in] */ Char c)
{
    Integer byteSize = String::GetByteSize(c);
    EnsureCapacityInternal(mByteCount + byteSize);
    String::WriteUTF8Bytes(mValue + mByteCount, c, byteSize);
    mCount++;
    mByteCount += byteSize;
    return NOERROR;
}

ECode AbstractStringBuilder::Append(
    /* [in] */ Integer i)
{
    if (i == INTEGER_MIN_VALUE) {
        return Append(String("-2147483648"));
    }
    return Append(String::Format("%d", i));
}

ECode AbstractStringBuilder::Append(
    /* [in] */ Long l)
{
    if (l == LONG_MIN_VALUE) {
        return Append(String("-9223372036854775808"));
    }
    return Append(String::Format("%lld", l));
}

ECode AbstractStringBuilder::Append(
    /* [in] */ Float f)
{
    return Append(String::Format("%f", f));
}

ECode AbstractStringBuilder::Append(
    /* [in] */ Double d)
{
    return Append(String::Format("%f", d));
}

ECode AbstractStringBuilder::Delete(
    /* [in] */ Integer start,
    /* [in] */ Integer end)
{
    if (start < 0) {
        return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (end > mCount) {
        end = mCount;
    }
    if (start > end) {
        return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    Integer len = end - start;
    if (len > 0) {
        Integer byteSize, i = 0;
        char* charStart;
        char* charEnd;
        const char* p = mValue;
        const char* pend = mValue + mByteCount;
        while (*p && p < pend) {
            Char unicode = GetCharInternal(p, &byteSize);
            if (byteSize == 0 || p + byteSize >= pend) {
                return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
            }
            if (i == start) {
                charStart = const_cast<char*>(p);
            }
            if (i == end) {
                charEnd = const_cast<char*>(p);
                break;
            }
            p += byteSize;
            i++;
        }
        memmove(const_cast<char*>(charStart), charEnd, mByteCount - (charEnd - mValue));
        mCount -= len;
        mByteCount -= charEnd - charStart;
    }
    return NOERROR;
}

ECode AbstractStringBuilder::DeleteCharAt(
    /* [in] */ Integer index)
{
    if (index < 0 || index > mCount) {
        return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Integer byteSize;
    const char* p = mValue;
    const char* end = mValue + mByteCount;
    while (*p && p < end) {
        byteSize = String::UTF8SequenceLength(*p);
        if (byteSize == 0 || p + byteSize >= end) {
            return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        }
        if (index == 0) {
            memmove(const_cast<char*>(p), p + byteSize, mByteCount - (p + byteSize - mValue));
            mCount--;
            mByteCount -= byteSize;
            break;
        }
        p += byteSize;
        index -= 1;
    }
    return NOERROR;
}

ECode AbstractStringBuilder::Replace(
    /* [in] */ Integer start,
    /* [in] */ Integer end,
    /* [in] */ const String& str)
{
    if (start < 0 || start > mCount || start > end) {
        return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (end > mCount) {
        end = mCount;
    }

    Integer byteSize, i = 0, byteStart = 0, byteEnd = 0;
    const char* p = mValue;
    const char* pend = mValue + mByteCount;
    while (*p && p < pend) {
        Char unicode = GetCharInternal(p, &byteSize);
        if (byteSize == 0 || p + byteSize >= pend) {
            return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        }
        if (i == start) {
            byteStart = p - mValue;
        }
        if (i == end) {
            byteEnd = p - mValue;
            break;
        }
        p += byteSize;
        i++;
    }

    Integer len = str.GetByteLength();
    Integer newByteCount = mByteCount + len - (byteEnd - byteStart);
    EnsureCapacityInternal(newByteCount);

    memmove(mValue + byteStart + len, mValue + byteEnd, mByteCount - byteEnd);
    memcpy(mValue + byteStart, str.string(), len);
    mCount = mCount + str.GetLength() - (end - start);
    mByteCount = newByteCount;
    return NOERROR;
}

ECode AbstractStringBuilder::Substring(
    /* [in] */ Integer start,
    /* [out] */ String* str)
{
    return Substring(start, mCount, str);
}

ECode AbstractStringBuilder::SubSequence(
    /* [in] */ Integer start,
    /* [in] */ Integer end,
    /* [out] */ ICharSequence** subcsq)
{
    String str;
    Substring(start, end, &str);
    return CString::New(str, IID_ICharSequence, (IInterface**)subcsq);
}

ECode AbstractStringBuilder::Substring(
    /* [in] */ Integer start,
    /* [in] */ Integer end,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    if (start < 0 || end > mCount || start > end) {
        return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Integer byteSize, i = 0, byteStart = 0, byteEnd = 0;
    const char* p = mValue;
    const char* pend = mValue + mByteCount;
    while (*p && p < pend) {
        Char unicode = GetCharInternal(p, &byteSize);
        if (byteSize == 0 || p + byteSize >= pend) {
            return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        }
        if (i == start) {
            byteStart = p - mValue;
        }
        if (i == end) {
            byteEnd = p - mValue;
            break;
        }
        p += byteSize;
        i++;
    }
    *str = String(mValue + byteStart, byteEnd - byteStart);
    return NOERROR;
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer index,
    /* [in] */ const Array<Char>& str,
    /* [in] */ Integer offset,
    /* [in] */ Integer len)
{
    if (index < 0 || index > mCount) {
        return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (offset < 0 || len < 0 || (offset > str.GetLength() - len)) {
        Logger::E("AbstractStringBuilder", "offset %d, len %d, str.length %d",
                offset, len, str.GetLength());
        return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    Integer totalByteSize = 0;
    for (Integer i = 0; i < len; i++) {
        totalByteSize = String::GetByteSize(str[offset + i]);
    }
    EnsureCapacityInternal(mByteCount + totalByteSize);

    Integer byteSize;
    const char* p = mValue;
    const char* end = mValue + mByteCount;
    while (*p && p < end) {
        byteSize = String::UTF8SequenceLength(*p);
        if (byteSize == 0 || p + byteSize >= end) {
            return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        }
        if (index == 0) {
            break;
        }
        p += byteSize;
        index -= 1;
    }

    memmove(const_cast<char*>(p) + totalByteSize, p, mByteCount - (p - mValue));
    for (Integer i = 0; i < len; i++) {
        Char c = str[offset + i];
        byteSize = String::GetByteSize(c);
        String::WriteUTF8Bytes(const_cast<char*>(p), c, byteSize);
        p += byteSize;
    }
    mCount += len;
    mByteCount += totalByteSize;
    return NOERROR;
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ IInterface* obj)
{
    return Insert(offset, Object::ToString(obj));
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ const String& str)
{
    if (offset < 0 || offset > mCount) {
        return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    String string = str;
    if (string.IsNull()) {
        string = "null";
    }
    Integer len = string.GetByteLength();
    EnsureCapacityInternal(mByteCount + len);

    Integer byteSize;
    const char* p = mValue;
    const char* end = mValue + mByteCount;
    while (*p && p < end) {
        byteSize = String::UTF8SequenceLength(*p);
        if (byteSize == 0 || p + byteSize >= end) {
            return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        }
        if (offset == 0) {
            break;
        }
        p += byteSize;
        offset -= 1;
    }

    memmove(const_cast<char*>(p) + len, p, mByteCount - (p - mValue));
    memcpy(const_cast<char*>(p), string.string(), len);
    mCount += str.GetLength();
    mByteCount += len;
    return NOERROR;
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ const Array<Char>& str)
{
    return Insert(offset, str, 0, str.GetLength());
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer dstOffset,
    /* [in] */ ICharSequence* s)
{
    String str("null");
    if (s != nullptr) {
        s->ToString(&str);
    }
    return Insert(dstOffset, str);
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer dstOffset,
    /* [in] */ ICharSequence* s,
    /* [in] */ Integer start,
    /* [in] */ Integer end)
{
    if (dstOffset < 0 || dstOffset > mCount) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    String str("null");
    if (s != nullptr) {
        s->ToString(&str);
    }
    if (start < 0 || end < 0 || start > end || end > str.GetLength()) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    return Insert(dstOffset, str.Substring(start, end));
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Boolean b)
{
    return Insert(offset, b ? String("true") : String("false"));
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Char c)
{
    Integer totalByteSize = String::GetByteSize(c);
    EnsureCapacityInternal(mByteCount + totalByteSize);

    Integer byteSize;
    const char* p = mValue;
    const char* end = mValue + mByteCount;
    while (*p && p < end) {
        byteSize = String::UTF8SequenceLength(*p);
        if (byteSize == 0 || p + byteSize >= end) {
            return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        }
        if (offset == 0) {
            break;
        }
        p += byteSize;
        offset -= 1;
    }

    memmove(const_cast<char*>(p) + totalByteSize, p, mByteCount - (p - mValue));
    String::WriteUTF8Bytes(const_cast<char*>(p), c, totalByteSize);
    mCount += 1;
    mByteCount += totalByteSize;
    return NOERROR;
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Integer i)
{
    return Insert(offset, String::Format("%d", i));
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Long l)
{
    return Insert(offset, String::Format("%lld", l));
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Float f)
{
    return Insert(offset, String::Format("%f", f));
}

ECode AbstractStringBuilder::Insert(
    /* [in] */ Integer offset,
    /* [in] */ Double d)
{
    return Insert(offset, String::Format("%f", d));
}

ECode AbstractStringBuilder::IndexOf(
    /* [in] */ const String& str,
    /* [out] */ Integer* idx)
{
    return IndexOf(str, 0, idx);
}

ECode AbstractStringBuilder::IndexOf(
    /* [in] */ const String& str,
    /* [in] */ Integer fromIndex,
    /* [out] */ Integer* idx)
{
    VALIDATE_NOT_NULL(idx);

    Integer i = 0;
    Integer byteSize;
    const char* p = mValue;
    const char* end = mValue + mByteCount;
    const char* psub = nullptr;
    while (*p && p < end) {
        byteSize = String::UTF8SequenceLength(*p);
        if (byteSize == 0 || p + byteSize >= end) {
            return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        }
        if (i >= fromIndex && psub == nullptr) {
            psub = strstr(p, str.string());
            if (psub == nullptr) {
                *idx = -1;
                break;
            }
        }
        if (p == psub) {
            *idx = i;
            break;
        }
        p += byteSize;
        i++;
    }
    return NOERROR;
}

ECode AbstractStringBuilder::LastIndexOf(
    /* [in] */ const String& str,
    /* [out] */ Integer* idx)
{
    return LastIndexOf(str, mCount, idx);
}

ECode AbstractStringBuilder::LastIndexOf(
    /* [in] */ const String& str,
    /* [in] */ Integer fromIndex,
    /* [out] */ Integer* idx)
{
    VALIDATE_NOT_NULL(idx);

    *idx = String(mValue, mByteCount).LastIndexOf(fromIndex);
    return NOERROR;
}

ECode AbstractStringBuilder::Reverse()
{
    if (mByteCount > 0) {
        char* tmpStr = (char*)malloc(mByteCount);
        if (tmpStr == nullptr) {
            Logger::E("AbstractStringBuilder", "Out of memory.");
            return E_OUT_OF_MEMORY_ERROR;
        }
        memcpy(tmpStr, mValue, mByteCount);

        Integer byteSize;
        const char* p = tmpStr;
        const char* end = tmpStr + mByteCount;
        const char* t = mValue + mByteCount;
        while (*p && p < end) {
            byteSize = String::UTF8SequenceLength(*p);
            if (byteSize == 0 || p + byteSize >= end) {
                break;
            }
            t -= byteSize;
            memcpy(const_cast<char*>(t), p, byteSize);
            p += byteSize;
        }
        free(tmpStr);
    }
    return NOERROR;
}

}
}
