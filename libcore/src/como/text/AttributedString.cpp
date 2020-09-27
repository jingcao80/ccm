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

#include "como/core/AutoLock.h"
#include "como/core/CStringBuffer.h"
#include "como/text/AttributedString.h"
#include "como/util/CHashSet.h"
#include "como/util/CHashtable.h"
#include "como/util/CVector.h"
#include "como.core.IAnnotation.h"
#include "como.util.ICollection.h"
#include "como.util.IIterator.h"
#include "como.util.IMap.h"
#include "como.util.IMapEntry.h"
#include "como.util.ISet.h"
#include <comolog.h>

using como::core::AutoLock;
using como::core::CStringBuffer;
using como::core::E_NULL_POINTER_EXCEPTION;
using como::core::IAnnotation;
using como::core::IID_ICloneable;
using como::core::IID_IStringBuffer;
using como::util::CHashSet;
using como::util::CHashtable;
using como::util::CVector;
using como::util::ICollection;
using como::util::IID_IHashSet;
using como::util::IID_IMap;
using como::util::IID_IMapEntry;
using como::util::IID_ISet;
using como::util::IID_IVector;
using como::util::IHashSet;
using como::util::IIterator;
using como::util::IMapEntry;
using como::util::ISet;

namespace como {
namespace text {

extern const CoclassID CID_AttributeEntry =
        {{0x0ed6801f,0xa1cc,0x42bd,0x8a58,{0xce,0x62,0x97,0x6b,0x4e,0xf7}}, &CID_libcore};

class AttributeEntry
    : public SyncObject
    , public IMapEntry
{
public:
    AttributeEntry(
        /* [in] */ IAttributedCharacterIterator::IAttribute* key,
        /* [in] */ IInterface* value)
        : mKey(key)
        , mValue(value)
    {}

    COMO_INTERFACE_DECL();

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& result) override;

    ECode GetKey(
        /* [out] */ AutoPtr<IInterface>& key) override;

    ECode GetValue(
        /* [out] */ AutoPtr<IInterface>& value) override;

    ECode SetValue(
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** prevValue = nullptr) override;

    ECode GetHashCode(
        /* [out] */ Integer& hash) override;

    ECode ToString(
        /* [out] */ String& str) override;

    ECode GetCoclassID(
        /* [out] */ CoclassID& cid) override;

private:
    AutoPtr<IAttributedCharacterIterator::IAttribute> mKey;
    AutoPtr<IInterface> mValue;
};

COMO_INTERFACE_IMPL_1(AttributeEntry, SyncObject, IMapEntry);

ECode AttributeEntry::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& result)
{
    if (!Object::InstanceOf(obj, CID_AttributeEntry)) {
        result = false;
        return NOERROR;
    }
    AttributeEntry* other = (AttributeEntry*)IMapEntry::Probe(obj);
    result = Object::Equals(other->mKey, mKey) && Object::Equals(other->mValue, mValue);
    return NOERROR;
}

ECode AttributeEntry::GetKey(
    /* [out] */ AutoPtr<IInterface>& key)
{
    key = mKey;
    return NOERROR;
}

ECode AttributeEntry::GetValue(
    /* [out] */ AutoPtr<IInterface>& value)
{
    value = mValue;
    return NOERROR;
}

ECode AttributeEntry::SetValue(
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** prevValue)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode AttributeEntry::GetHashCode(
    /* [out] */ Integer& hash)
{
    hash = Object::GetHashCode(mKey) ^ Object::GetHashCode(mValue);
    return NOERROR;
}

ECode AttributeEntry::ToString(
    /* [out] */ String& str)
{
    str = Object::ToString(mKey) + "=" + Object::ToString(mValue);
    return NOERROR;
}

ECode AttributeEntry::GetCoclassID(
    /* [out] */ CoclassID& cid)
{
    cid = CID_AttributeEntry;
    return NOERROR;
}

//--------------------------------------------------------------------------

COMO_INTERFACE_IMPL_1(AttributedString, SyncObject, IAttributedString);

ECode AttributedString::Constructor(
    /* [in] */ const Array<IAttributedCharacterIterator*>& iterators)
{
    if (iterators.IsNull()) {
        Logger::E("AttributedString", "Iterators must not be null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (iterators.GetLength() == 0) {
        mText = "";
    }
    else {
        // Build the String contents
        AutoPtr<IStringBuffer> buffer;
        CStringBuffer::New(IID_IStringBuffer, (IInterface**)&buffer);
        for (Integer counter = 0; counter < iterators.GetLength(); counter++) {
            AppendContents(buffer, ICharacterIterator::Probe(iterators[counter]));
        }

        buffer->ToString(mText);

        if (mText.GetLength() > 0) {
            // Determine the runs, creating a new run when the attributes
            // differ.
            Integer offset = 0;
            AutoPtr<IMap> last;

            for (Integer counter = 0; counter < iterators.GetLength(); counter++) {
                IAttributedCharacterIterator* iterator = iterators[counter];
                ICharacterIterator* iteratorAlias = ICharacterIterator::Probe(iterator);
                Integer start, end;
                iteratorAlias->GetBeginIndex(start);
                iteratorAlias->GetEndIndex(end);
                Integer index = start;

                while (index < end) {
                    iteratorAlias->SetIndex(index);

                    AutoPtr<IMap> attrs;
                    iterator->GetAttributes(attrs);

                    if (MapsDiffer(last, attrs)) {
                        SetAttributes(attrs, index - start + offset);
                    }
                    last = std::move(attrs);
                    iterator->GetRunLimit(index);
                }
                offset += (end - start);
            }
        }
    }
    return NOERROR;
}

ECode AttributedString::Constructor(
    /* [in] */ const String& text)
{
    if (text.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mText = text;
    return NOERROR;
}

ECode AttributedString::Constructor(
    /* [in] */ const String& text,
    /* [in] */ IMap* attributes)
{
    if (text.IsNull() || attributes == nullptr) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mText = text;

    if (text.GetLength() == 0) {
        Boolean empty;
        if (attributes->IsEmpty(empty), empty) {
            return NOERROR;
        }
        Logger::E("AttributedString", "Can't add attribute to 0-length text");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Integer attributeCount;
    attributes->GetSize(attributeCount);
    if (attributeCount > 0) {
        CreateRunAttributeDataVectors();
        AutoPtr<IVector> newRunAttributes, newRunAttributeValues;
        CVector::New(attributeCount, IID_IVector, (IInterface**)&newRunAttributes);
        CVector::New(attributeCount, IID_IVector, (IInterface**)&newRunAttributeValues);
        mRunAttributes.Set(0, newRunAttributes);
        mRunAttributeValues.Set(0, newRunAttributeValues);

        AutoPtr<ISet> entrySet;
        attributes->GetEntrySet(entrySet);
        AutoPtr<IIterator> it;
        entrySet->GetIterator(it);
        Boolean hasNext;
        while (it->HasNext(hasNext), hasNext) {
            AutoPtr<IMapEntry> entry;
            it->Next((IInterface**)&entry);
            AutoPtr<IInterface> key, value;
            entry->GetKey(key);
            entry->GetValue(value);
            newRunAttributes->AddElement(key);
            newRunAttributeValues->AddElement(value);
        }
    }
    return NOERROR;
}

ECode AttributedString::Constructor(
    /* [in] */ IAttributedCharacterIterator* text)
{
    Integer beginIndex, endIndex;
    ICharacterIterator::Probe(text)->GetBeginIndex(beginIndex);
    ICharacterIterator::Probe(text)->GetEndIndex(endIndex);
    Array<IAttributedCharacterIterator::IAttribute*> attributes;
    return Constructor(text, beginIndex, endIndex, attributes);
}

ECode AttributedString::Constructor(
    /* [in] */ IAttributedCharacterIterator* text,
    /* [in] */ Integer beginIndex,
    /* [in] */ Integer endIndex)
{
    Array<IAttributedCharacterIterator::IAttribute*> attributes;
    return Constructor(text, beginIndex, endIndex, attributes);
}

ECode AttributedString::Constructor(
    /* [in] */ IAttributedCharacterIterator* text,
    /* [in] */ Integer beginIndex,
    /* [in] */ Integer endIndex,
    /* [in] */ const Array<IAttributedCharacterIterator::IAttribute*>& attributes)
{
    if (text == nullptr) {
        return E_NULL_POINTER_EXCEPTION;
    }

    ICharacterIterator* textAlias = ICharacterIterator::Probe(text);
    Integer textBeginIndex, textEndIndex;
    textAlias->GetBeginIndex(textBeginIndex);
    textAlias->GetEndIndex(textEndIndex);
    if (beginIndex < textBeginIndex || endIndex > textEndIndex || beginIndex > endIndex) {
        Logger::E("AttributedString", "Invalid substring range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IStringBuffer> textBuffer;
    CStringBuffer::New(IID_IStringBuffer, (IInterface**)&textBuffer);
    textAlias->SetIndex(beginIndex);
    Integer textIndex;
    while (textAlias->GetIndex(textIndex), textIndex < endIndex) {
        Char c;
        textAlias->Current(c);
        textBuffer->Append(c);
    }
    textBuffer->ToString(mText);

    if (beginIndex == endIndex) {
        return NOERROR;
    }

    // Select attribute keys to be taken care of
    AutoPtr<IHashSet> keys;
    CHashSet::New(IID_IHashSet, (IInterface**)&keys);
    if (attributes.IsNull()) {
        AutoPtr<ISet> attrKeys;
        text->GetAllAttributeKeys(attrKeys);
        keys->AddAll(ICollection::Probe(attrKeys));
    }
    else {
        for (Integer i = 0; i < attributes.GetLength(); i++) {
            keys->Add(attributes[i]);
        }
        AutoPtr<ISet> attrKeys;
        text->GetAllAttributeKeys(attrKeys);
        keys->RetainAll(ICollection::Probe(attrKeys));
    }
    Boolean empty;
    if (keys->IsEmpty(empty), empty) {
        return NOERROR;
    }

    // Get and set attribute runs for each attribute name. Need to
    // scan from the top of the text so that we can discard any
    // Annotation that is no longer applied to a subset text segment.
    AutoPtr<IIterator> itr;
    keys->GetIterator(itr);
    Boolean hasNext;
    while (itr->HasNext(hasNext), hasNext) {
        AutoPtr<IAttributedCharacterIterator::IAttribute> attributeKey;
        itr->Next((IInterface**)&attributeKey);
        textAlias->SetIndex(textBeginIndex);
        while (textAlias->GetIndex(textIndex), textIndex < endIndex) {
            Integer start, limit;
            text->GetRunStart(attributeKey, start);
            text->GetRunLimit(attributeKey, limit);
            AutoPtr<IInterface> value;
            text->GetAttribute(attributeKey, value);

            if (value != nullptr) {
                if (IAnnotation::Probe(value) != nullptr) {
                    if (start >= beginIndex && limit <= endIndex) {
                        FAIL_RETURN(AddAttribute(attributeKey, value, start - beginIndex, limit - beginIndex));
                    }
                    else {
                        if (limit > endIndex) {
                            break;
                        }
                    }
                }
                else {
                    // if the run is beyond the given (subset) range, we
                    // don't need to process further.
                    if (start >= endIndex) {
                        break;
                    }
                    if (limit < beginIndex) {
                        // attribute is applied to any subrange
                        if (start < beginIndex) {
                            start = beginIndex;
                        }
                        if (limit < endIndex) {
                            limit = endIndex;
                        }
                        if (start != limit) {
                            FAIL_RETURN(AddAttribute(attributeKey, value, start - beginIndex, limit - beginIndex));
                        }
                    }
                }
            }
            textAlias->SetIndex(limit);
        }
    }
    return NOERROR;
}

ECode AttributedString::AddAttribute(
    /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
    /* [in] */ IInterface* value)
{
    if (attribute == nullptr) {
        return E_NULL_POINTER_EXCEPTION;
    }

    Integer len = GetLength();
    if (len == 0) {
        Logger::E("AttributedString", "Can't add attribute to 0-length text");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AddAttributeImpl(attribute, value, 0, len);
    return NOERROR;
}

ECode AttributedString::AddAttribute(
    /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
    /* [in] */ IInterface* value,
    /* [in] */ Integer beginIndex,
    /* [in] */ Integer endIndex)
{
    if (attribute == nullptr) {
        return E_NULL_POINTER_EXCEPTION;
    }

    if (beginIndex < 0 || endIndex > GetLength() || beginIndex >= endIndex) {
        Logger::E("AttributedString", "Invalid substring range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AddAttributeImpl(attribute, value, beginIndex, endIndex);
    return NOERROR;
}

ECode AttributedString::AddAttributes(
    /* [in] */ IMap* attributes,
    /* [in] */ Integer beginIndex,
    /* [in] */ Integer endIndex)
{
    if (attributes == nullptr) {
        return E_NULL_POINTER_EXCEPTION;
    }

    if (beginIndex < 0 || endIndex > GetLength() || beginIndex > endIndex) {
        Logger::E("AttributedString", "Invalid substring range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (beginIndex == endIndex) {
        Boolean empty;
        if (attributes->IsEmpty(empty), empty) {
            return NOERROR;
        }
        Logger::E("AttributedString", "Can't add attribute to 0-length text");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // make sure we have run attribute data vectors
    if (mRunCount == 0) {
        CreateRunAttributeDataVectors();
    }

    // break up runs if necessary
    Integer beginRunIndex = EnsureRunBreak(beginIndex);
    Integer endRunIndex = EnsureRunBreak(endIndex);

    AutoPtr<ISet> entrySet;
    attributes->GetEntrySet(entrySet);
    AutoPtr<IIterator> it;
    entrySet->GetIterator(it);
    Boolean hasNext;
    while (it->HasNext(hasNext), hasNext) {
        AutoPtr<IMapEntry> entry;
        it->Next((IInterface**)&entry);
        AutoPtr<IInterface> key, value;
        entry->GetKey(key);
        entry->GetValue(value);
        AddAttributeRunData(IAttributedCharacterIterator::IAttribute::Probe(key), value, beginRunIndex, endRunIndex);
    }
    return NOERROR;
}

void AttributedString::AddAttributeImpl(
    /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
    /* [in] */ IInterface* value,
    /* [in] */ Integer beginIndex,
    /* [in] */ Integer endIndex)
{
    AutoLock lock(this);

    // make sure we have run attribute data vectors
    if (mRunCount == 0) {
        CreateRunAttributeDataVectors();
    }

    // break up runs if necessary
    Integer beginRunIndex = EnsureRunBreak(beginIndex);
    Integer endRunIndex = EnsureRunBreak(endIndex);

    AddAttributeRunData(attribute, value, beginRunIndex, endRunIndex);
}

void AttributedString::CreateRunAttributeDataVectors()
{
    Array<Integer> newRunStarts(ARRAY_SIZE_INCREMENT);
    Array<IVector*> newRunAttributes(ARRAY_SIZE_INCREMENT);
    Array<IVector*> newRunAttributeValues(ARRAY_SIZE_INCREMENT);

    mRunStarts = newRunStarts;
    mRunAttributes = newRunAttributes;
    mRunAttributeValues = newRunAttributeValues;
    mRunArraySize = ARRAY_SIZE_INCREMENT;
    // assume initial run starting at index 0
    mRunCount = 1;
}

Integer AttributedString::EnsureRunBreak(
    /* [in] */ Integer offset)
{
    return EnsureRunBreak(offset, true);
}

Integer AttributedString::EnsureRunBreak(
    /* [in] */ Integer offset,
    /* [in] */ Boolean copyAttrs)
{
    if (offset == GetLength()) {
        return mRunCount;
    }

    // search for the run index where this offset should be
    Integer runIndex = 0;
    while (runIndex < mRunCount && mRunStarts[runIndex] < offset) {
        runIndex++;
    }

    // if the offset is at a run start already, we're done
    if (runIndex < mRunCount && mRunStarts[runIndex] == offset) {
        return runIndex;
    }

    // we'll have to break up a run
    // first, make sure we have enough space in our arrays
    if (mRunCount == mRunArraySize) {
        Integer newArraySize = mRunArraySize + ARRAY_SIZE_INCREMENT;
        Array<Integer> newRunStarts(newArraySize);

        Array<IVector*> newRunAttributes(newArraySize);
        Array<IVector*> newRunAttributeValues(newArraySize);

        for (Integer i = 0; i < mRunArraySize; i++) {
            newRunStarts[i] = mRunStarts[i];
            newRunAttributes.Set(i, mRunAttributes[i]);
            newRunAttributeValues.Set(i, mRunAttributeValues[i]);
        }
        mRunStarts = newRunStarts;
        mRunAttributes = newRunAttributes;
        mRunAttributeValues = newRunAttributeValues;
        mRunArraySize = newArraySize;
    }

    // make copies of the attribute information of the old run that the new one used to be part of
    // use temporary variables so things remain consistent in case of an exception
    AutoPtr<IVector> newRunAttributes;
    AutoPtr<IVector> newRunAttributeValues;

    if (copyAttrs) {
        AutoPtr<IVector> oldRunAttributes = mRunAttributes[runIndex - 1];
        AutoPtr<IVector> oldRunAttributeValues = mRunAttributeValues[runIndex - 1];
        if (oldRunAttributes != nullptr) {
            CVector::New(ICollection::Probe(oldRunAttributes), IID_IVector, (IInterface**)&newRunAttributes);
        }
        if (oldRunAttributeValues != nullptr) {
            CVector::New(ICollection::Probe(oldRunAttributeValues), IID_IVector, (IInterface**)&newRunAttributeValues);
        }
    }

    // now actually break up the run
    mRunCount++;
    for (Integer i = mRunCount - 1; i < runIndex; i--) {
        mRunStarts[i] = mRunStarts[i - 1];
        mRunAttributes.Set(i, mRunAttributes[i - 1]);
        mRunAttributeValues.Set(i, mRunAttributeValues[i - 1]);
    }
    mRunStarts[runIndex] = offset;
    mRunAttributes.Set(runIndex, newRunAttributes);
    mRunAttributeValues.Set(runIndex, newRunAttributeValues);

    return runIndex;
}

void AttributedString::AddAttributeRunData(
    /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
    /* [in] */ IInterface* value,
    /* [in] */ Integer beginRunIndex,
    /* [in] */ Integer endRunIndex)
{
    for (Integer i = beginRunIndex; i < endRunIndex; i++) {
        Integer keyValueIndex = -1;
        if (mRunAttributes[i] == nullptr) {
            AutoPtr<IVector> newRunAttributes, newRunAttributeValues;
            CVector::New(IID_IVector, (IInterface**)&newRunAttributes);
            CVector::New(IID_IVector, (IInterface**)&newRunAttributeValues);
            mRunAttributes.Set(i, newRunAttributes);
            mRunAttributeValues.Set(i, newRunAttributeValues);
        }
        else {
            mRunAttributes[i]->IndexOf(attribute, &keyValueIndex);
        }

        if (keyValueIndex == -1) {
            Integer oldSize;
            mRunAttributes[i]->GetSize(oldSize);
            mRunAttributes[i]->AddElement(attribute);
            ECode ec = mRunAttributeValues[i]->AddElement(value);
            if (FAILED(ec)) {
                mRunAttributes[i]->SetSize(oldSize);
                mRunAttributeValues[i]->SetSize(oldSize);
            }
        }
        else {
            mRunAttributeValues[i]->Set(keyValueIndex, value);
        }
    }
}

ECode AttributedString::GetIterator(
    /* [out] */ AutoPtr<IAttributedCharacterIterator>& it)
{
    Array<IAttributedCharacterIterator::IAttribute*> attributes;
    return GetIterator(attributes, 0, GetLength(), it);
}

ECode AttributedString::GetIterator(
    /* [in] */ const Array<IAttributedCharacterIterator::IAttribute*>& attributes,
    /* [out] */ AutoPtr<IAttributedCharacterIterator>& it)
{
    return GetIterator(attributes, 0, GetLength(), it);
}

ECode AttributedString::GetIterator(
    /* [in] */ const Array<IAttributedCharacterIterator::IAttribute*>& attributes,
    /* [in] */ Integer beginIndex,
    /* [in] */ Integer endIndex,
    /* [out] */ AutoPtr<IAttributedCharacterIterator>& it)
{
    AutoPtr<AttributedStringIterator> asit = new AttributedStringIterator();
    FAIL_RETURN(asit->Constructor(this, attributes, beginIndex, endIndex));
    it = (IAttributedCharacterIterator*)asit.Get();
    return NOERROR;
}

Integer AttributedString::GetLength()
{
    return mText.GetLength();
}

Char AttributedString::CharAt(
    /* [in] */ Integer index)
{
    return mText.GetChar(index);
}

AutoPtr<IInterface> AttributedString::GetAttribute(
    /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
    /* [in] */ Integer runIndex)
{
    AutoLock lock(this);

    AutoPtr<IVector> currentRunAttributes = mRunAttributes[runIndex];
    AutoPtr<IVector> currentRunAttributeValues = mRunAttributeValues[runIndex];
    if (currentRunAttributes == nullptr) {
        return nullptr;
    }
    Integer attributeIndex;
    currentRunAttributes->IndexOf(attribute, &attributeIndex);
    if (attributeIndex != -1) {
        AutoPtr<IInterface> value;
        currentRunAttributeValues->GetElementAt(attributeIndex, &value);
        return value;
    }
    else {
        return nullptr;
    }
}

AutoPtr<IInterface> AttributedString::GetAttributeCheckRange(
    /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
    /* [in] */ Integer runIndex,
    /* [in] */ Integer beginIndex,
    /* [in] */ Integer endIndex)
{
    AutoPtr<IInterface> value = GetAttribute(attribute, runIndex);
    if (IAnnotation::Probe(value) != nullptr) {
        if (beginIndex > 0) {
            Integer currIndex = runIndex;
            Integer runStart = mRunStarts[currIndex];
            while (runStart >= beginIndex &&
                    ValuesMatch(value, GetAttribute(attribute, currIndex - 1))) {
                currIndex--;
                runStart = mRunStarts[currIndex];
            }
            if (runStart < beginIndex) {
                return nullptr;
            }
        }
        Integer textLength = GetLength();
        if (endIndex < textLength) {
            Integer currIndex = runIndex;
            Integer runLimit = (currIndex < mRunCount - 1) ? mRunStarts[currIndex + 1] : textLength;
            while (runLimit <= endIndex &&
                    ValuesMatch(value, GetAttribute(attribute, currIndex + 1))) {
                currIndex++;
                runLimit = (currIndex < mRunCount - 1) ? mRunStarts[currIndex + 1] : textLength;
            }
            if (runLimit > endIndex) {
                return nullptr;
            }
        }
    }
    return value;
}

Boolean AttributedString::AttributeValuesMatch(
    /* [in] */ ISet* attributes,
    /* [in] */ Integer runIndex1,
    /* [in] */ Integer runIndex2)
{
    AutoPtr<IIterator> it;
    attributes->GetIterator(it);
    Boolean hasNext;
    while (it->HasNext(hasNext), hasNext) {
        AutoPtr<IAttributedCharacterIterator::IAttribute> key;
        it->Next((IInterface**)&key);
        if (!ValuesMatch(GetAttribute(key, runIndex1), GetAttribute(key, runIndex2))) {
            return false;
        }
    }
    return true;
}

Boolean AttributedString::ValuesMatch(
    /* [in] */ IInterface* value1,
    /* [in] */ IInterface* value2)
{
    return Object::Equals(value1, value2);
}

void AttributedString::AppendContents(
    /* [in] */ IStringBuffer* buf,
    /* [in] */ ICharacterIterator* iterator)
{
    Integer index, end;
    iterator->GetBeginIndex(index);
    iterator->GetEndIndex(end);

    while (index < end) {
        iterator->SetIndex(index++);
        Char c;
        iterator->Current(c);
        buf->Append(c);
    }
}

void AttributedString::SetAttributes(
    /* [in] */ IMap* attrs,
    /* [in] */ Integer offset)
{
    if (mRunCount == 0) {
        CreateRunAttributeDataVectors();
    }

    Integer index = EnsureRunBreak(offset, false);
    Integer size;
    if (attrs != nullptr && (attrs->GetSize(size), size > 0)) {
        AutoPtr<IVector> runAttrs, runValues;
        CVector::New(size, IID_IVector, (IInterface**)&runAttrs);
        CVector::New(size, IID_IVector, (IInterface**)&runValues);
        AutoPtr<ISet> entrySet;
        attrs->GetEntrySet(entrySet);
        AutoPtr<IIterator> it;
        entrySet->GetIterator(it);

        Boolean hasNext;
        while (it->HasNext(hasNext), hasNext) {
            AutoPtr<IMapEntry> entry;
            it->Next((IInterface**)&entry);

            AutoPtr<IInterface> key, value;
            entry->GetKey(key);
            entry->GetValue(value);
            runAttrs->Add(key);
            runValues->Add(value);
        }
        mRunAttributes.Set(index, runAttrs);
        mRunAttributeValues.Set(index, runValues);
    }
}

Boolean AttributedString::MapsDiffer(
    /* [in] */ IMap* last,
    /* [in] */ IMap* attrs)
{
    if (last == nullptr) {
        Integer size;
        return (attrs != nullptr && (attrs->GetSize(size), size > 0));
    }
    Boolean isEqual;
    IObject::Probe(last)->Equals(attrs, isEqual);
    return !isEqual;
}

//--------------------------------------------------------------------------

COMO_INTERFACE_IMPL_3(AttributedString::AttributedStringIterator, SyncObject,
        IAttributedCharacterIterator, ICharacterIterator, ICloneable);

ECode AttributedString::AttributedStringIterator::Constructor(
    /* [in] */ AttributedString* owner,
    /* [in] */ const Array<IAttributedCharacterIterator::IAttribute*>& attributes,
    /* [in] */ Integer beginIndex,
    /* [in] */ Integer endIndex)
{
    if (beginIndex < 0 || beginIndex > endIndex || endIndex < owner->GetLength()) {
        Logger::E("AttributedString::AttributedStringIterator", "Invalid substring range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mOwner = owner;
    mBeginIndex = beginIndex;
    mEndIndex = endIndex;
    mCurrentIndex = beginIndex;
    UpdateRunInfo();
    if (!attributes.IsNull()) {
        mRelevantAttributes = attributes.Clone();
    }
    return NOERROR;
}

ECode AttributedString::AttributedStringIterator::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean& same)
{
    AttributedStringIterator* other = (AttributedStringIterator*)IAttributedCharacterIterator::Probe(obj);
    if (this == other) {
        same = true;
        return NOERROR;
    }
    if (other == nullptr) {
        same = false;
        return NOERROR;
    }

    if (mOwner != other->GetString()) {
        same = false;
        return NOERROR;
    }
    if (mCurrentIndex != other->mCurrentIndex || mBeginIndex != other->mBeginIndex || mEndIndex != other->mEndIndex) {
        same = false;
        return NOERROR;
    }
    same = true;
    return NOERROR;
}

ECode AttributedString::AttributedStringIterator::GetHashCode(
    /* [out] */ Integer& hash)
{
    hash = mOwner->mText.GetHashCode() ^ mCurrentIndex ^ mBeginIndex ^ mEndIndex;
    return NOERROR;
}

ECode AttributedString::AttributedStringIterator::Clone(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    AutoPtr<AttributedStringIterator> clone = new AttributedStringIterator();
    clone->mBeginIndex = mBeginIndex;
    clone->mEndIndex = mEndIndex;
    clone->mRelevantAttributes = mRelevantAttributes;
    clone->mCurrentIndex = mCurrentIndex;
    clone->mCurrentRunIndex = mCurrentRunIndex;
    clone->mCurrentRunStart = mCurrentRunStart;
    clone->mCurrentRunLimit = mCurrentRunLimit;
    clone->mOwner = mOwner;
    clone.MoveTo((IAttributedCharacterIterator**)obj);
    return NOERROR;
}

ECode AttributedString::AttributedStringIterator::First(
    /* [out] */ Char& firstChar)
{
    firstChar = InternalSetIndex(mBeginIndex);
    return NOERROR;
}

ECode AttributedString::AttributedStringIterator::Last(
    /* [out] */ Char& lastChar)
{
    if (mEndIndex == mBeginIndex) {
        lastChar = InternalSetIndex(mEndIndex);
        return NOERROR;
    }
    else {
        lastChar = InternalSetIndex(mEndIndex - 1);
        return NOERROR;
    }
}

ECode AttributedString::AttributedStringIterator::Current(
    /* [out] */ Char& currChar)
{
    if (mCurrentIndex == mEndIndex) {
        currChar = DONE;
        return NOERROR;
    }
    else {
        currChar = mOwner->CharAt(mCurrentIndex);
        return NOERROR;
    }
}

ECode AttributedString::AttributedStringIterator::Next(
    /* [out] */ Char& nextChar)
{
    if (mCurrentIndex < mEndIndex) {
        nextChar = InternalSetIndex(mCurrentIndex + 1);
        return NOERROR;
    }
    else {
        nextChar = DONE;
        return NOERROR;
    }
}

ECode AttributedString::AttributedStringIterator::Previous(
    /* [out] */ Char& prevChar)
{
    if (mCurrentIndex > mBeginIndex) {
        prevChar = InternalSetIndex(mCurrentIndex - 1);
        return NOERROR;
    }
    else {
        prevChar = DONE;
        return NOERROR;
    }
}

ECode AttributedString::AttributedStringIterator::SetIndex(
    /* [in] */ Integer position,
    /* [out] */ Char* currChar)
{
    if (position < mBeginIndex || position > mEndIndex) {
        Logger::E("AttributedString::AttributedStringIterator", "Invalid index");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Char ch = InternalSetIndex(position);
    if (currChar != nullptr) {
        *currChar = ch;
    }
    return NOERROR;
}

ECode AttributedString::AttributedStringIterator::GetBeginIndex(
    /* [out] */ Integer& beginIndex)
{
    beginIndex = mBeginIndex;
    return NOERROR;
}

ECode AttributedString::AttributedStringIterator::GetEndIndex(
    /* [out] */ Integer& endIndex)
{
    endIndex = mEndIndex;
    return NOERROR;
}

ECode AttributedString::AttributedStringIterator::GetIndex(
    /* [out] */ Integer& currIndex)
{
    currIndex = mCurrentIndex;
    return NOERROR;
}

ECode AttributedString::AttributedStringIterator::GetRunStart(
    /* [out] */ Integer& index)
{
    index = mCurrentRunStart;
    return NOERROR;
}

ECode AttributedString::AttributedStringIterator::GetRunStart(
    /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
    /* [out] */ Integer& index)
{
    if (mCurrentRunStart == mBeginIndex || mCurrentRunIndex == -1) {
        index = mCurrentRunStart;
        return NOERROR;
    }
    else {
        AutoPtr<IInterface> value;
        GetAttribute(attribute, value);
        Integer runLimit = mCurrentRunLimit;
        Integer runIndex = mCurrentRunIndex;
        while (runLimit < mEndIndex &&
                ValuesMatch(value, mOwner->GetAttribute(attribute, runIndex + 1))) {
            runIndex++;
            runLimit = runIndex < mOwner->mRunCount - 1 ? mOwner->mRunStarts[runIndex + 1] : mEndIndex;
        }
        if (runLimit > mEndIndex) {
            runLimit = mEndIndex;
        }
        index = runLimit;
        return NOERROR;
    }
}

ECode AttributedString::AttributedStringIterator::GetRunStart(
    /* [in] */ ISet* attributes,
    /* [out] */ Integer& index)
{
    if (mCurrentRunStart == mBeginIndex || mCurrentRunIndex == -1) {
        index = mCurrentRunStart;
        return NOERROR;
    }
    else {
        Integer runStart = mCurrentRunStart;
        Integer runIndex = mCurrentRunIndex;
        while (runStart > mBeginIndex &&
                mOwner->AttributeValuesMatch(attributes, mCurrentRunIndex, runIndex - 1)) {
            runIndex--;
            runStart = mOwner->mRunStarts[runIndex];
        }
        if (runStart < mBeginIndex) {
            runStart = mBeginIndex;
        }
        index = runStart;
        return NOERROR;
    }
}

ECode AttributedString::AttributedStringIterator::GetRunLimit(
    /* [out] */ Integer& index)
{
    index = mCurrentRunLimit;
    return NOERROR;
}

ECode AttributedString::AttributedStringIterator::GetRunLimit(
    /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
    /* [out] */ Integer& index)
{
    if (mCurrentRunLimit == mEndIndex || mCurrentRunIndex == -1) {
        index = mCurrentRunLimit;
        return NOERROR;
    }
    else {
        AutoPtr<IInterface> value;
        GetAttribute(attribute, value);
        Integer runLimit = mCurrentRunLimit;
        Integer runIndex = mCurrentRunIndex;
        while (runLimit < mEndIndex &&
                ValuesMatch(value, mOwner->GetAttribute(attribute, runIndex + 1))) {
            runIndex++;
            runLimit = runIndex < mOwner->mRunCount - 1 ? mOwner->mRunStarts[runIndex + 1] : mEndIndex;
        }
        if (runLimit > mEndIndex) {
            runLimit = mEndIndex;
        }
        index = runLimit;
        return NOERROR;
    }
}

ECode AttributedString::AttributedStringIterator::GetRunLimit(
    /* [in] */ ISet* attributes,
    /* [out] */ Integer& index)
{
    if (mCurrentRunLimit == mEndIndex || mCurrentRunIndex == -1) {
        index = mCurrentRunLimit;
        return NOERROR;
    }
    else {
        Integer runLimit = mCurrentRunLimit;
        Integer runIndex = mCurrentRunIndex;
        while (runLimit < mEndIndex && mOwner->AttributeValuesMatch(
                attributes, mCurrentRunIndex, runIndex + 1)) {
            runIndex++;
            runLimit = runIndex < mOwner->mRunCount - 1 ? mOwner->mRunStarts[runIndex + 1] : mEndIndex;
        }
        if (runLimit > mEndIndex) {
            runLimit = mEndIndex;
        }
        index = runLimit;
        return NOERROR;
    }
}

ECode AttributedString::AttributedStringIterator::GetAttributes(
    /* [out] */ AutoPtr<IMap>& attributes)
{
    if (mOwner->mRunAttributes.IsNull() || mCurrentRunIndex == -1 || mOwner->mRunAttributes[mCurrentRunIndex] == nullptr) {
        attributes = nullptr;
        return CHashtable::New(IID_IMap, (IInterface**)&attributes);
    }
    attributes = new AttributeMap(mOwner, mCurrentRunIndex, mBeginIndex, mEndIndex);
    return NOERROR;
}

ECode AttributedString::AttributedStringIterator::GetAllAttributeKeys(
    /* [out] */ AutoPtr<ISet>& keys)
{
    if (mOwner->mRunAttributes.IsNull()) {
        keys = nullptr;
        return CHashtable::New(IID_IMap, (IInterface**)&keys);
    }
    {
        AutoLock lock(mOwner);

        keys = nullptr;
        CHashSet::New(IID_ISet, (IInterface**)&keys);
        Integer i = 0;
        while (i < mOwner->mRunCount) {
            if (mOwner->mRunStarts[i] < mEndIndex && (i == mOwner->mRunCount - 1 || mOwner->mRunStarts[i + 1] > mBeginIndex)) {
                AutoPtr<IVector> currentRunAttributes = mOwner->mRunAttributes[i];
                if (currentRunAttributes != nullptr) {
                    Integer j;
                    currentRunAttributes->GetSize(j);
                    while (j-- > 0) {
                        AutoPtr<IInterface> value;
                        currentRunAttributes->Get(j, value);
                        keys->Add(value);
                    }
                }
                i++;
            }
        }
        return NOERROR;
    }
}

ECode AttributedString::AttributedStringIterator::GetAttribute(
    /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
    /* [out] */ AutoPtr<IInterface>& value)
{
    Integer runIndex = mCurrentRunIndex;
    if (runIndex < 0) {
        value = nullptr;
        return NOERROR;
    }
    value = mOwner->GetAttributeCheckRange(attribute, runIndex, mBeginIndex, mEndIndex);
    return NOERROR;
}

AutoPtr<AttributedString> AttributedString::AttributedStringIterator::GetString()
{
    return mOwner;
}

Char AttributedString::AttributedStringIterator::InternalSetIndex(
    /* [in] */ Integer position)
{
    mCurrentIndex = position;
    if (position < mCurrentRunStart || position >= mCurrentRunLimit) {
        UpdateRunInfo();
    }
    if (mCurrentIndex == mEndIndex) {
        return DONE;
    }
    else {
        return mOwner->CharAt(position);
    }
}

void AttributedString::AttributedStringIterator::UpdateRunInfo()
{
    if (mCurrentIndex == mEndIndex) {
        mCurrentRunStart = mCurrentRunLimit = mEndIndex;
        mCurrentRunIndex = -1;
    }
    else {
        AutoLock lock(mOwner);

        Integer runIndex = -1;
        while (runIndex < mOwner->mRunCount - 1 && mOwner->mRunStarts[runIndex + 1] <= mCurrentIndex) {
            runIndex++;
        }
        mCurrentRunIndex = runIndex;
        if (runIndex >= 0) {
            mCurrentRunStart = mOwner->mRunStarts[runIndex];
            if (mCurrentRunStart < mBeginIndex) {
                mCurrentRunStart = mBeginIndex;
            }
        }
        else {
            mCurrentRunStart = mBeginIndex;
        }
        if (runIndex < mOwner->mRunCount - 1) {
            mCurrentRunLimit = mOwner->mRunStarts[runIndex + 1];
            if (mCurrentRunLimit > mEndIndex) {
                mCurrentRunLimit = mEndIndex;
            }
        }
        else {
            mCurrentRunLimit = mEndIndex;
        }
    }
}

//--------------------------------------------------------------------------

ECode AttributedString::AttributeMap::GetEntrySet(
    /* [out] */ AutoPtr<ISet>& entrySet)
{
    AutoPtr<ISet> set;
    CHashSet::New(IID_ISet, (IInterface**)&set);
    {
        AutoLock lock(mOwner);

        Integer size;
        mOwner->mRunAttributes[mRunIndex]->GetSize(size);
        for (Integer i = 0; i < size; i++) {
            AutoPtr<IInterface> key, value;
            mOwner->mRunAttributes[mRunIndex]->Get(i, key);
            mOwner->mRunAttributeValues[mRunIndex]->Get(i, value);
            if (IAnnotation::Probe(value) != nullptr) {
                value = mOwner->GetAttributeCheckRange(IAttributedCharacterIterator::IAttribute::Probe(key),
                        mRunIndex, mBeginIndex, mEndIndex);
                if (value == nullptr) {
                    continue;
                }
            }
            AutoPtr<IMapEntry> entry = new AttributeEntry(IAttributedCharacterIterator::IAttribute::Probe(key), value);
            set->Add(entry);
        }
    }
    entrySet = std::move(set);
    return NOERROR;
}

ECode AttributedString::AttributeMap::Get(
    /* [in] */ IInterface* key,
    /* [out] */ AutoPtr<IInterface>& value)
{
    value = mOwner->GetAttributeCheckRange(
            (IAttributedCharacterIterator::IAttribute*)key,
            mRunIndex, mBeginIndex, mEndIndex);
    return NOERROR;
}

}
}
