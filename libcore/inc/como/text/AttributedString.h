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

#ifndef __COMO_TEXT_ATTRIBUTEDSTRING_H__
#define __COMO_TEXT_ATTRIBUTEDSTRING_H__

#include "como/core/SyncObject.h"
#include "como/util/AbstractMap.h"
#include "como.core.ICloneable.h"
#include "como.core.IStringBuffer.h"
#include "como.text.IAttributedCharacterIterator.h"
#include "como.text.IAttributedString.h"
#include "como.text.ICharacterIterator.h"
#include "como.util.IMap.h"
#include "como.util.ISet.h"
#include "como.util.IVector.h"

using como::core::ICloneable;
using como::core::IStringBuffer;
using como::core::SyncObject;
using como::util::AbstractMap;
using como::util::IMap;
using como::util::ISet;
using como::util::IVector;

namespace como {
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
        COMO_INTERFACE_DECL();

        ECode Constructor(
            /* [in] */ AttributedString* owner,
            /* [in] */ const Array<IAttributedCharacterIterator::IAttribute*>& attributes,
            /* [in] */ Integer beginIndex,
            /* [in] */ Integer endIndex);

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean& same) override;

        ECode GetHashCode(
            /* [out] */ Integer& hash) override;

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
            /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
            /* [out] */ Integer* index) override;

        ECode GetRunStart(
            /* [in] */ ISet* attributes,
            /* [out] */ Integer* index) override;

        ECode GetRunLimit(
            /* [out] */ Integer* index) override;

        ECode GetRunLimit(
            /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
            /* [out] */ Integer* index) override;

        ECode GetRunLimit(
            /* [in] */ ISet* attributes,
            /* [out] */ Integer* index) override;

        ECode GetAttributes(
            /* [out] */ IMap** attributes) override;

        ECode GetAllAttributeKeys(
            /* [out] */ ISet** keys) override;

        ECode GetAttribute(
            /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
            /* [out] */ IInterface** value) override;

    private:
        AutoPtr<AttributedString> GetString();

        Char InternalSetIndex(
            /* [in] */ Integer position);

        void UpdateRunInfo();

    private:
        Integer mBeginIndex = 0;
        Integer mEndIndex = 0;

        Array<IAttributedCharacterIterator::IAttribute*> mRelevantAttributes;

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
    COMO_INTERFACE_DECL();

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
        /* [in] */ const Array<IAttributedCharacterIterator::IAttribute*>& attributes);

    ECode AddAttribute(
        /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
        /* [in] */ IInterface* value) override;

    ECode AddAttribute(
        /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
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
        /* [in] */ const Array<IAttributedCharacterIterator::IAttribute*>& attributes,
        /* [out] */ IAttributedCharacterIterator** it) override;

    ECode GetIterator(
        /* [in] */ const Array<IAttributedCharacterIterator::IAttribute*>& attributes,
        /* [in] */ Integer beginIndex,
        /* [in] */ Integer endIndex,
        /* [out] */ IAttributedCharacterIterator** it) override;

    Integer GetLength();

    static AttributedString* From(
        /* [in] */ IAttributedString* obj);

private:
    void AddAttributeImpl(
        /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
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
        /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
        /* [in] */ IInterface* value,
        /* [in] */ Integer beginRunIndex,
        /* [in] */ Integer endRunIndex);

    Char CharAt(
        /* [in] */ Integer index);

    AutoPtr<IInterface> GetAttribute(
        /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
        /* [in] */ Integer runIndex);

    AutoPtr<IInterface> GetAttributeCheckRange(
        /* [in] */ IAttributedCharacterIterator::IAttribute* attribute,
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

#endif // __COMO_TEXT_ATTRIBUTEDSTRING_H__
