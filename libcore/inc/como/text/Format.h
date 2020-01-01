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

#ifndef __COMO_TEXT_BASEFORMAT_H__
#define __COMO_TEXT_BASEFORMAT_H__

#include "como/core/SyncObject.h"
#include "como/text/AttributedCharacterIteratorAttribute.h"
#include "como.core.ICloneable.h"
#include "como.io.ISerializable.h"
#include "como.text.IFormat.h"
#include "como.text.IFormatField.h"

using como::core::ICloneable;
using como::core::SyncObject;
using como::io::ISerializable;

namespace como {
namespace text {

class BaseFormat
    : public SyncObject
    , public IFormat
    , public ISerializable
    , public ICloneable
{
public:
    class Field
        : public AttributedCharacterIteratorAttribute
        , public IFormatField
    {
    protected:
        COMO_INTERFACE_DECL();

        ECode Constructor(
            /* [in] */ const String& fieldName);
    };

public:
    COMO_INTERFACE_DECL();

    ECode Format(
        /* [in] */ IInterface* obj,
        /* [out] */ String* string) override;

    ECode FormatToCharacterIterator(
        /* [in] */ IInterface* obj,
        /* [out] */ IAttributedCharacterIterator** it) override;

    ECode ParseObject(
        /* [in] */ const String& source,
        /* [out] */ IInterface** object) override;

    ECode CreateAttributedCharacterIterator(
        /* [in] */ const String& s,
        /* [out] */ IAttributedCharacterIterator** it);

    ECode CreateAttributedCharacterIterator(
        /* [in] */ const Array<IAttributedCharacterIterator*>& iterators,
        /* [out] */ IAttributedCharacterIterator** it);

    ECode CreateAttributedCharacterIterator(
        /* [in] */ const String& string,
        /* [in] */ IAttributedCharacterIterator::IAttribute* key,
        /* [in] */ IInterface* value,
        /* [out] */ IAttributedCharacterIterator** it);

    ECode CreateAttributedCharacterIterator(
        /* [in] */ IAttributedCharacterIterator* iterator,
        /* [in] */ IAttributedCharacterIterator::IAttribute* key,
        /* [in] */ IInterface* value,
        /* [out] */ IAttributedCharacterIterator** it);

    using IFormat::Format;

    using IFormat::ParseObject;
};

}
}

#endif // __COMO_TEXT_BASEFORMAT_H__
