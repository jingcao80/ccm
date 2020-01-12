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

#ifndef __COMO_TEXT_FIELDPOSITION_H__
#define __COMO_TEXT_FIELDPOSITION_H__

#include "como/core/SyncObject.h"
#include "como.core.IStringBuffer.h"
#include "como.text.IFieldPosition.h"
#include "como.text.IFormatField.h"
#include "como.text.IFormatFieldDelegate.h"

using como::core::IStringBuffer;
using como::core::SyncObject;

namespace como {
namespace text {

class FieldPosition
    : public SyncObject
    , public IFieldPosition
{
private:
    class Delegate
        : public SyncObject
        , public IFormatFieldDelegate
    {
    public:
        Delegate(
            /* [in] */ FieldPosition* owner)
            : mOwner(owner)
        {}

        COMO_INTERFACE_DECL();

        ECode Formatted(
            /* [in] */ IFormatField* attr,
            /* [in] */ IInterface* value,
            /* [in] */ Integer start,
            /* [in] */ Integer end,
            /* [in] */ IStringBuffer* buffer) override;

        ECode Formatted(
            /* [in] */ Integer field,
            /* [in] */ IFormatField* attr,
            /* [in] */ IInterface* value,
            /* [in] */ Integer start,
            /* [in] */ Integer end,
            /* [in] */ IStringBuffer* buffer) override;

    private:
        AutoPtr<FieldPosition> mOwner;
        Boolean mEncounteredField = false;
    };

public:
    COMO_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Integer field);

    ECode Constructor(
        /* [in] */ IFormatField* attribute);

    ECode Constructor(
        /* [in] */ IFormatField* attribute,
        /* [in] */ Integer field);

    ECode GetFieldAttribute(
        /* [out] */ IFormatField** attribute) override;

    ECode GetField(
        /* [out] */ Integer* field) override;

    ECode GetBeginIndex(
        /* [out] */ Integer* index) override;

    ECode GetEndIndex(
        /* [out] */ Integer* index) override;

    ECode SetBeginIndex(
        /* [in] */ Integer index) override;

    ECode SetEndIndex(
        /* [in] */ Integer index) override;

    ECode GetFieldDelegate(
        /* [out] */ IFormatFieldDelegate** delegate) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& same) override;

    ECode GetHashCode(
        /* [out] */ Integer& hash) override;

    ECode ToString(
        /* [out] */ String& desc) override;

private:
    Boolean MatchesField(
        /* [in]*/ IFormatField* attribute);

    Boolean MatchesField(
        /* [in] */ IFormatField* attribute,
        /* [in] */ Integer field);

protected:
    Integer mField = 0;

    Integer mEndIndex = 0;

    Integer mBeginIndex = 0;

private:
    AutoPtr<IFormatField> mAttribute;
};

}
}

#endif // __COMO_TEXT_FIELDPOSITION_H__
