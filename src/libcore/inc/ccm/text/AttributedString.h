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

#ifndef __CCM_TEXT_ATTRIBUTEDSTRING_H__
#define __CCM_TEXT_ATTRIBUTEDSTRING_H__

#include "ccm/core/SyncObject.h"
#include "ccm/util/AbstractMap.h"
#include "ccm.core.ICloneable.h"
#include "ccm.core.IStringBuffer.h"
#include "ccm.text.IAttributedCharacterIterator.h"
#include "ccm.text.IAttributedCharacterIteratorAttribute.h"
#include "ccm.text.IAttributedString.h"
#include "ccm.text.ICharacterIterator.h"
#include "ccm.util.IMap.h"
#include "ccm.util.ISet.h"
#include "ccm.util.IVector.h"

using ccm::core::ICloneable;
using ccm::core::IStringBuffer;
using ccm::core::SyncObject;
using ccm::util::AbstractMap;
using ccm::util::IMap;
using ccm::util::ISet;
using ccm::util::IVector;

namespace ccm {
namespace text {

class AttributedString
    : public SyncObject
    , public IAttributedString
{
private:
    class AttributedStringIterator
        : public SyncObject
        , public IAttributedCharacterIterator
        , public ICharacterIterator
        , public ICloneable
    {
    public:
        CCM_INTERFACE_DECL();

        ECode Constructor(
            /* [in] */ AttributedString* owner,
            /* [in] */ const Array<IAttributedCharacterIteratorAttribute*>& attributes,
            /* [in] */ Integer beginIndex,
            /* [in] */ Integer endIndex);

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* same) override;

        ECode GetHashCode(
            /* [out] */ Integer* hash) override;

        ECode Clone(
            /* [in] */ const InterfaceID& iid,
            /* [out] */ IInterface** obj) override;

        ECode First(
            /* [out] */ Char* firstChar) override;

        ECode Last(
            /* [out] */ Char* lastChar) override;

        ECode Current(
            /* [out] */ Char* currChar) override;

        ECode Next(
            /* [out] */ Char* nextChar) override;

        ECode Previous(
            /* [out] */ Char* prevChar) override;

        ECode SetIndex(
            /* [in] */ Integer position,
            /* [out] */ Char* currChar) override;

        ECode GetBeginIndex(
            /* [out] */ Integer* beginIndex) override;

        ECode GetEndIndex(
            /* [out] */ Integer* endIndex) override;

        ECode GetIndex(
            /* [out] */ Integer* currIndex) override;

        ECode GetRunStart(
            /* [out] */ Integer* index) override;

        ECode GetRunStart(
            /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
            /* [out] */ Integer* index) override;

        ECode GetRunStart(
            /* [in] */ ISet* attributes,
            /* [out] */ Integer* index) override;

        ECode GetRunLimit(
            /* [out] */ Integer* index) override;

        ECode GetRunLimit(
            /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
            /* [out] */ Integer* index) override;

        ECode GetRunLimit(
            /* [in] */ ISet* attributes,
            /* [out] */ Integer* index) override;

        ECode GetAttributes(
            /* [out] */ IMap** attributes) override;

        ECode GetAllAttributeKeys(
            /* [out] */ ISet** keys) override;

        ECode GetAttribute(
            /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
            /* [out] */ IInterface** value) override;

    private:
        AutoPtr<AttributedString> GetString();

        Char InternalSetIndex(
            /* [in] */ Integer position);

        void UpdateRunInfo();

    private:
        Integer mBeginIndex = 0;
        Integer mEndIndex = 0;

        Array<IAttributedCharacterIteratorAttribute*> mRelevantAttributes;

        Integer mCurrentIndex = 0;

        Integer mCurrentRunIndex = 0;
        Integer mCurrentRunStart = 0;
        Integer mCurrentRunLimit = 0;

        AutoPtr<AttributedString> mOwner;
    };

    class AttributeMap
        : public AbstractMap
    {
    public:
        AttributeMap(
            /* [in] */ AttributedString* owner,
            /* [in] */ Integer runIndex,
            /* [in] */ Integer beginIndex,
            /* [in] */ Integer endIndex)
            : mRunIndex(runIndex)
            , mBeginIndex(beginIndex)
            , mEndIndex(endIndex)
            , mOwner(owner)
        {}

        ECode GetEntrySet(
            /* [out] */ ISet** entrySet) override;

        ECode Get(
            /* [in] */ IInterface* key,
            /* [out] */ IInterface** value) override;

    private:
        Integer mRunIndex;
        Integer mBeginIndex;
        Integer mEndIndex;

        AutoPtr<AttributedString> mOwner;
    };

public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ const Array<IAttributedCharacterIterator*>& iterators);

    ECode Constructor(
        /* [in] */ const String& text);

    ECode Constructor(
        /* [in] */ const String& text,
        /* [in] */ IMap* attributes);

    ECode Constructor(
        /* [in] */ IAttributedCharacterIterator* text);

    ECode Constructor(
        /* [in] */ IAttributedCharacterIterator* text,
        /* [in] */ Integer beginIndex,
        /* [in] */ Integer endIndex);

    ECode Constructor(
        /* [in] */ IAttributedCharacterIterator* text,
        /* [in] */ Integer beginIndex,
        /* [in] */ Integer endIndex,
        /* [in] */ const Array<IAttributedCharacterIteratorAttribute*>& attributes);

    ECode AddAttribute(
        /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
        /* [in] */ IInterface* value) override;

    ECode AddAttribute(
        /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
        /* [in] */ IInterface* value,
        /* [in] */ Integer beginIndex,
        /* [in] */ Integer endIndex) override;

    ECode AddAttributes(
        /* [in] */ IMap* attributes,
        /* [in] */ Integer beginIndex,
        /* [in] */ Integer endIndex) override;

    ECode GetIterator(
        /* [out] */ IAttributedCharacterIterator** it) override;

    ECode GetIterator(
        /* [in] */ const Array<IAttributedCharacterIteratorAttribute*>& attributes,
        /* [out] */ IAttributedCharacterIterator** it) override;

    ECode GetIterator(
        /* [in] */ const Array<IAttributedCharacterIteratorAttribute*>& attributes,
        /* [in] */ Integer beginIndex,
        /* [in] */ Integer endIndex,
        /* [out] */ IAttributedCharacterIterator** it) override;

    Integer GetLength();

    static AttributedString* From(
        /* [in] */ IAttributedString* obj);

private:
    void AddAttributeImpl(
        /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
        /* [in] */ IInterface* value,
        /* [in] */ Integer beginIndex,
        /* [in] */ Integer endIndex);

    void CreateRunAttributeDataVectors();

    Integer EnsureRunBreak(
        /* [in] */ Integer offset);

    Integer EnsureRunBreak(
        /* [in] */ Integer offset,
        /* [in] */ Boolean copyAttrs);

    void AddAttributeRunData(
        /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
        /* [in] */ IInterface* value,
        /* [in] */ Integer beginRunIndex,
        /* [in] */ Integer endRunIndex);

    Char CharAt(
        /* [in] */ Integer index);

    AutoPtr<IInterface> GetAttribute(
        /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
        /* [in] */ Integer runIndex);

    AutoPtr<IInterface> GetAttributeCheckRange(
        /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
        /* [in] */ Integer runIndex,
        /* [in] */ Integer beginIndex,
        /* [in] */ Integer endIndex);

    Boolean AttributeValuesMatch(
        /* [in] */ ISet* attributes,
        /* [in] */ Integer runIndex1,
        /* [in] */ Integer runIndex2);

    static Boolean ValuesMatch(
        /* [in] */ IInterface* value1,
        /* [in] */ IInterface* value2);

    void AppendContents(
        /* [in] */ IStringBuffer* buf,
        /* [in] */ ICharacterIterator* iterator);

    void SetAttributes(
        /* [in] */ IMap* attrs,
        /* [in] */ Integer offset);

    static Boolean MapsDiffer(
        /* [in] */ IMap* last,
        /* [in] */ IMap* attrs);

public:
    String mText;

    // current size of the arrays
    Integer mRunArraySize = 0;

    // actual number of runs, <= runArraySize
    Integer mRunCount = 0;

    // start index for each run
    Array<Integer> mRunStarts;

    // vector of attribute keys for each run
    Array<IVector*> mRunAttributes;

    // parallel vector of attribute values for each run
    Array<IVector*> mRunAttributeValues;

private:
    static constexpr Integer ARRAY_SIZE_INCREMENT = 10;
};

inline AttributedString* AttributedString::From(
    /* [in] */ IAttributedString* obj)
{
    return (AttributedString*)obj;
}

}
}

#endif // __CCM_TEXT_ATTRIBUTEDSTRING_H__
