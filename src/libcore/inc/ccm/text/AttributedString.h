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
#include "ccm.text.IAttributedCharacterIterator.h"
#include "ccm.text.IAttributedCharacterIteratorAttribute.h"
#include "ccm.text.IAttributedString.h"
#include "ccm.util.IMap.h"

using ccm::core::SyncObject;
using ccm::util::IMap;

namespace ccm {
namespace text {

class AttributedString
    : public SyncObject
    , public IAttributedString
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Array<IAttributedCharacterIterator*> iterators)
    {
        return NOERROR;
    }

    ECode Constructor(
        /* [in] */ const String& text)
    {
        return NOERROR;
    }

    ECode Constructor(
        /* [in] */ const String& text,
        /* [in] */ IMap* attributes)
    {
        return NOERROR;
    }

    ECode Constructor(
        /* [in] */ IAttributedCharacterIterator* text)
    {
        return NOERROR;
    }

    ECode Constructor(
        /* [in] */ IAttributedCharacterIterator* text,
        /* [in] */ Integer beginIndex,
        /* [in] */ Integer endIndex)
    {
        return NOERROR;
    }

    ECode Constructor(
        /* [in] */ IAttributedCharacterIterator* text,
        /* [in] */ Integer beginIndex,
        /* [in] */ Integer endIndex,
        /* [in] */ Array<IAttributedCharacterIteratorAttribute*> attributes)
    {
        return NOERROR;
    }

    ECode AddAttribute(
        /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
        /* [in] */ IInterface* value) override
    {
        return NOERROR;
    }

    ECode AddAttribute(
        /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
        /* [in] */ IInterface* value,
        /* [in] */ Integer beginIndex,
        /* [in] */ Integer endIndex) override
    {
        return NOERROR;
    }

    ECode GetIterator(
        /* [out] */ IAttributedCharacterIterator** it) override
    {
        return NOERROR;
    }

    Integer GetLength()
    {
        return NOERROR;
    }

    static AttributedString* From(
        /* [in] */ IAttributedString* obj);
};

inline AttributedString* AttributedString::From(
    /* [in] */ IAttributedString* obj)
{
    return (AttributedString*)obj;
}

}
}

#endif // __CCM_TEXT_ATTRIBUTEDSTRING_H__
