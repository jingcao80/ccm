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

#ifndef __CCM_TEXT_BASEFORMAT_H__
#define __CCM_TEXT_BASEFORMAT_H__

#include "ccm/core/SyncObject.h"
#include "ccm/text/AttributedCharacterIteratorAttribute.h"
#include "ccm.core.ICloneable.h"
#include "ccm.io.ISerializable.h"
#include "ccm.text.IFormat.h"
#include "ccm.text.IFormatField.h"

using ccm::core::ICloneable;
using ccm::core::SyncObject;
using ccm::io::ISerializable;

namespace ccm {
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
        CCM_INTERFACE_DECL();

        ECode Constructor(
            /* [in] */ const String& fieldName);
    };

public:
    CCM_INTERFACE_DECL();

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
        /* [in] */ IAttributedCharacterIteratorAttribute* key,
        /* [in] */ IInterface* value,
        /* [out] */ IAttributedCharacterIterator** it);

    ECode CreateAttributedCharacterIterator(
        /* [in] */ IAttributedCharacterIterator* iterator,
        /* [in] */ IAttributedCharacterIteratorAttribute* key,
        /* [in] */ IInterface* value,
        /* [out] */ IAttributedCharacterIterator** it);

    using IFormat::Format;

    using IFormat::ParseObject;
};

}
}

#endif // __CCM_TEXT_BASEFORMAT_H__
