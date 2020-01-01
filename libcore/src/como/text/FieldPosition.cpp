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

#include "como/text/FieldPosition.h"

namespace como {
namespace text {

COMO_INTERFACE_IMPL_1(FieldPosition, SyncObject, IFieldPosition);

ECode FieldPosition::Constructor(
    /* [in] */ Integer field)
{
    mField = field;
    return NOERROR;
}

ECode FieldPosition::Constructor(
    /* [in] */ IFormatField* attribute)
{
    return (attribute, -1);
}

ECode FieldPosition::Constructor(
    /* [in] */ IFormatField* attribute,
    /* [in] */ Integer field)
{
    mAttribute = attribute;
    mField = field;
    return NOERROR;
}

ECode FieldPosition::GetFieldAttribute(
    /* [out] */ IFormatField** attribute)
{
    VALIDATE_NOT_NULL(attribute);

    *attribute = mAttribute;
    REFCOUNT_ADD(*attribute);
    return NOERROR;
}

ECode FieldPosition::GetField(
    /* [out] */ Integer* field)
{
    VALIDATE_NOT_NULL(field);

    *field = mField;
    return NOERROR;
}

ECode FieldPosition::GetBeginIndex(
    /* [out] */ Integer* index)
{
    VALIDATE_NOT_NULL(index);

    *index = mBeginIndex;
    return NOERROR;
}

ECode FieldPosition::GetEndIndex(
    /* [out] */ Integer* index)
{
    VALIDATE_NOT_NULL(index);

    *index = mEndIndex;
    return NOERROR;
}

ECode FieldPosition::SetBeginIndex(
    /* [in] */ Integer index)
{
    mBeginIndex = index;
    return NOERROR;
}

ECode FieldPosition::SetEndIndex(
    /* [in] */ Integer index)
{
    mEndIndex= index;
    return NOERROR;
}

ECode FieldPosition::GetFieldDelegate(
    /* [out] */ IFormatFieldDelegate** delegate)
{
    VALIDATE_NOT_NULL(delegate);

    *delegate = new Delegate(this);
    REFCOUNT_ADD(*delegate);
    return NOERROR;
}

ECode FieldPosition::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    if (IFieldPosition::Probe(obj) == nullptr) {
        *same = false;
        return NOERROR;
    }

    FieldPosition* other = (FieldPosition*)IFieldPosition::Probe(obj);
    if (mAttribute == nullptr) {
        if (other->mAttribute != nullptr) {
            *same = false;
            return NOERROR;
        }
    }
    else if (!Object::Equals(mAttribute, other->mAttribute)) {
        *same = false;
        return NOERROR;
    }
    *same = (mBeginIndex == other->mBeginIndex) &&
            (mEndIndex == other->mEndIndex) &&
            (mField == other->mField);
    return NOERROR;
}

ECode FieldPosition::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    *hash = (mField << 24) | (mBeginIndex << 16) | mEndIndex;
    return NOERROR;
}

ECode FieldPosition::ToString(
    /* [out] */ String* desc)
{
    VALIDATE_NOT_NULL(desc);

    *desc = String::Format("%s[mField=%d,mAttribute=%s,mBeginIndex=%d,mEndIndex=%d]",
            Object::GetCoclassName((IFieldPosition*)this).string(), mField,
            Object::ToString(mAttribute).string(), mBeginIndex, mEndIndex);
    return NOERROR;
}

Boolean FieldPosition::MatchesField(
    /* [in]*/ IFormatField* attribute)
{
    if (mAttribute != nullptr) {
        return Object::Equals(mAttribute, attribute);
    }
    return false;
}

Boolean FieldPosition::MatchesField(
    /* [in] */ IFormatField* attribute,
    /* [in] */ Integer field)
{
    if (mAttribute != nullptr) {
        return Object::Equals(mAttribute, attribute);
    }
    return field == mField;
}

//---------------------------------------------------------------------------------

COMO_INTERFACE_IMPL_1(FieldPosition::Delegate, SyncObject, IFormatFieldDelegate);

ECode FieldPosition::Delegate::Formatted(
    /* [in] */ IFormatField* attr,
    /* [in] */ IInterface* value,
    /* [in] */ Integer start,
    /* [in] */ Integer end,
    /* [in] */ IStringBuffer* buffer)
{
    if (!mEncounteredField && mOwner->MatchesField(attr)) {
        mOwner->SetBeginIndex(start);
        mOwner->SetEndIndex(end);
        mEncounteredField = (start != end);
    }
    return NOERROR;
}

ECode FieldPosition::Delegate::Formatted(
    /* [in] */ Integer field,
    /* [in] */ IFormatField* attr,
    /* [in] */ IInterface* value,
    /* [in] */ Integer start,
    /* [in] */ Integer end,
    /* [in] */ IStringBuffer* buffer)
{
    if (!mEncounteredField && mOwner->MatchesField(attr, field)) {
        mOwner->SetBeginIndex(start);
        mOwner->SetEndIndex(end);
        mEncounteredField = (start != end);
    }
    return NOERROR;
}

}
}
