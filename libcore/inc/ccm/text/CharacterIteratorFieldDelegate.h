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

#ifndef __CCM_TEXT_CHARACTERITERATORFIELDDELEGATE_H__
#define __CCM_TEXT_CHARACTERITERATORFIELDDELEGATE_H__

#include "ccm/core/SyncObject.h"
#include "ccm.core.IStringBuffer.h"
#include "ccm.text.IFormatField.h"
#include "ccm.text.IFormatFieldDelegate.h"
#include "ccm.util.IArrayList.h"

using ccm::core::IStringBuffer;
using ccm::core::SyncObject;
using ccm::util::IArrayList;

namespace ccm {
namespace text {

class CharacterIteratorFieldDelegate
    : public SyncObject
    , public IFormatFieldDelegate
{
public:
    CharacterIteratorFieldDelegate();

    CCM_INTERFACE_DECL();

    ECode Formatted(
        /* [in] */ IFormatField* attr,
        /* [in] */ IInterface* value,
        /* [in] */ Integer start,
        /* [in] */ Integer end,
        /* [in] */ IStringBuffer* buffer) override;

    ECode Formatted(
        /* [in] */ Integer fieldID,
        /* [in] */ IFormatField* attr,
        /* [in] */ IInterface* value,
        /* [in] */ Integer start,
        /* [in] */ Integer end,
        /* [in] */ IStringBuffer* buffer) override;

    ECode GetIterator(
        /* [in] */ const String& string,
        /* [out] */ IAttributedCharacterIterator** it);

private:
    /**
     * Array of AttributeStrings. Whenever <code>formatted</code> is invoked
     * for a region > size, a new instance of AttributedString is added to
     * attributedStrings. Subsequent invocations of <code>formatted</code>
     * for existing regions result in invoking addAttribute on the existing
     * AttributedStrings.
     */
    AutoPtr<IArrayList> mAttributedStrings;

    /**
     * Running count of the number of characters that have
     * been encountered.
     */
    Integer mSize = 0;
};

}
}

#endif // __CCM_TEXT_CHARACTERITERATORFIELDDELEGATE_H__
