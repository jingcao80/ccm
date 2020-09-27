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

#include "como/text/CharacterIteratorFieldDelegate.h"
#include "como/core/Math.h"
#include "como/text/CAttributedString.h"
#include "como/util/CArrayList.h"
#include "como.text.IAttributedCharacterIterator.h"
#include "como.text.IAttributedString.h"

using como::core::Math;
using como::util::CArrayList;
using como::util::IID_IArrayList;

namespace como {
namespace text {

COMO_INTERFACE_IMPL_1(CharacterIteratorFieldDelegate, SyncObject, IFormatFieldDelegate);

CharacterIteratorFieldDelegate::CharacterIteratorFieldDelegate()
{
    CArrayList::New(IID_IArrayList, (IInterface**)&mAttributedStrings);
}

ECode CharacterIteratorFieldDelegate::Formatted(
    /* [in] */ IFormatField* attr,
    /* [in] */ IInterface* value,
    /* [in] */ Integer start,
    /* [in] */ Integer end,
    /* [in] */ IStringBuffer* buffer)
{
    if (start != end) {
        if (start < mSize) {
            Integer index = mSize;
            Integer asIndex;
            mAttributedStrings->GetSize(asIndex);
            asIndex -= 1;

            while (start < index) {
                AutoPtr<IInterface> v;
                mAttributedStrings->Get(asIndex--, v);
                IAttributedString* as = IAttributedString::Probe(v);
                Integer newIndex = index - CAttributedString::From(as)->GetLength();
                Integer aStart = Math::Max(0, start - newIndex);

                FAIL_RETURN(as->AddAttribute(IAttributedCharacterIterator::IAttribute::Probe(attr), value, aStart, Math::Min(
                        end - start, CAttributedString::From(as)->GetLength() - aStart) + aStart));
                index = newIndex;
            }
        }
        if (mSize < start) {
            String text;
            buffer->Substring(mSize, start, text);
            AutoPtr<IAttributedString> as;
            FAIL_RETURN(CAttributedString::New(text, IID_IAttributedString, (IInterface**)&as));
            mAttributedStrings->Add(as);
            mSize = start;
        }
        if (mSize < end) {
            Integer aStart = Math::Max(start, mSize);
            String text;
            buffer->Substring(aStart, end, text);
            AutoPtr<IAttributedString> as;
            FAIL_RETURN(CAttributedString::New(text, IID_IAttributedString, (IInterface**)&as));
            FAIL_RETURN(as->AddAttribute(IAttributedCharacterIterator::IAttribute::Probe(attr), value));
            mAttributedStrings->Add(as);
            mSize = end;
        }
    }
    return NOERROR;
}

ECode CharacterIteratorFieldDelegate::Formatted(
    /* [in] */ Integer fieldID,
    /* [in] */ IFormatField* attr,
    /* [in] */ IInterface* value,
    /* [in] */ Integer start,
    /* [in] */ Integer end,
    /* [in] */ IStringBuffer* buffer)
{
    return Formatted(attr, value, start, end, buffer);
}

ECode CharacterIteratorFieldDelegate::GetIterator(
    /* [in] */ const String& string,
    /* [out] */ AutoPtr<IAttributedCharacterIterator>& it)
{
    if (string.GetLength() > mSize) {
        AutoPtr<IAttributedString> as;
        FAIL_RETURN(CAttributedString::New(string.Substring(mSize), IID_IAttributedString, (IInterface**)&as));
        mAttributedStrings->Add(as);
        mSize = string.GetLength();
    }
    Integer iCount;
    mAttributedStrings->GetSize(iCount);
    Array<IAttributedCharacterIterator*> iterators(iCount);

    for (Integer counter = 0; counter < iCount; counter++) {
        AutoPtr<IInterface> as;
        mAttributedStrings->Get(counter, as);
        AutoPtr<IAttributedCharacterIterator> ait;
        FAIL_RETURN(IAttributedString::Probe(as)->GetIterator(ait));
        iterators.Set(counter, ait);
    }
    AutoPtr<IAttributedString> astring;
    FAIL_RETURN(CAttributedString::New(iterators, IID_IAttributedString, (IInterface**)&astring));
    return astring->GetIterator(it);
}

}
}
