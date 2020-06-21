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

#include "como/core/CStringBuffer.h"
#include "como/text/CAttributedString.h"
#include "como/text/CFieldPosition.h"
#include "como/text/CParsePosition.h"
#include "como/text/Format.h"
#include <comolog.h>

using como::core::CStringBuffer;
using como::core::IID_ICloneable;
using como::core::IID_IStringBuffer;
using como::core::IStringBuffer;
using como::io::IID_ISerializable;

namespace como {
namespace text {

COMO_INTERFACE_IMPL_3(BaseFormat, SyncObject, IFormat, ISerializable, ICloneable);

ECode BaseFormat::Format(
    /* [in] */ IInterface* obj,
    /* [out] */ String& string)
{
    AutoPtr<IStringBuffer> sb;
    CStringBuffer::New(IID_IStringBuffer, (IInterface**)&sb);
    AutoPtr<IFieldPosition> fp;
    CFieldPosition::New(0, IID_IFieldPosition, (IInterface**)&fp);
    FAIL_RETURN(Format(obj, sb, fp));
    return sb->ToString(string);
}

ECode BaseFormat::FormatToCharacterIterator(
    /* [in] */ IInterface* obj,
    /* [out] */ AutoPtr<IAttributedCharacterIterator>& it)
{
    String string;
    FAIL_RETURN(Format(obj, string));
    return CreateAttributedCharacterIterator(string, it);
}

ECode BaseFormat::ParseObject(
    /* [in] */ const String& source,
    /* [out] */ AutoPtr<IInterface>& object)
{
    AutoPtr<IParsePosition> pos;
    CParsePosition::New(0, IID_IParsePosition, (IInterface**)&pos);
    FAIL_RETURN(ParseObject(source, pos, object));
    Integer index;
    if (pos->GetIndex(index), index == 0) {
        Logger::E("BaseFormat", "ParseObject(String) failed");
        return E_PARSE_EXCEPTION;
    }
    return NOERROR;
}

ECode BaseFormat::CreateAttributedCharacterIterator(
    /* [in] */ const String& s,
    /* [out] */ AutoPtr<IAttributedCharacterIterator>& it)
{
    AutoPtr<IAttributedString> as;
    CAttributedString::New(s, IID_IAttributedString, (IInterface**)&as);
    return as->GetIterator(it);
}

ECode BaseFormat::CreateAttributedCharacterIterator(
    /* [in] */ const Array<IAttributedCharacterIterator*>& iterators,
    /* [out] */ AutoPtr<IAttributedCharacterIterator>& it)
{
    AutoPtr<IAttributedString> as;
    CAttributedString::New(iterators, IID_IAttributedString, (IInterface**)&as);
    return as->GetIterator(it);
}

ECode BaseFormat::CreateAttributedCharacterIterator(
    /* [in] */ const String& string,
    /* [in] */ IAttributedCharacterIterator::IAttribute* key,
    /* [in] */ IInterface* value,
    /* [out] */ AutoPtr<IAttributedCharacterIterator>& it)
{
    AutoPtr<IAttributedString> as;
    CAttributedString::New(string, IID_IAttributedString, (IInterface**)&as);
    as->AddAttribute(key, value);
    return as->GetIterator(it);
}

ECode BaseFormat::CreateAttributedCharacterIterator(
    /* [in] */ IAttributedCharacterIterator* iterator,
    /* [in] */ IAttributedCharacterIterator::IAttribute* key,
    /* [in] */ IInterface* value,
    /* [out] */ AutoPtr<IAttributedCharacterIterator>& it)
{
    AutoPtr<IAttributedString> as;
    CAttributedString::New(iterator, IID_IAttributedString, (IInterface**)&as);
    as->AddAttribute(key, value);
    return as->GetIterator(it);
}

//-----------------------------------------------------------------------------

COMO_INTERFACE_IMPL_1(BaseFormat::Field, AttributedCharacterIteratorAttribute, IFormatField);

ECode BaseFormat::Field::Constructor(
    /* [in] */ const String& fieldName)
{
    return AttributedCharacterIteratorAttribute::Constructor(fieldName);
}

}
}