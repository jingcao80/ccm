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

#include "ccm/core/CBoolean.h"
#include "ccm/core/CChar.h"
#include "ccm/core/CDouble.h"
#include "ccm/core/CInteger.h"
#include "ccm/core/CoreUtils.h"
#include "ccm/core/CString.h"
#include "ccm/core/System.h"
#include "ccm.core.ICloneable.h"

namespace ccm {
namespace core {

AutoPtr<IChar> CoreUtils::Box(
    /* [in] */ Char c)
{
    AutoPtr<IChar> ch;
    CChar::New(c, IID_IChar, (IInterface**)&ch);
    return ch;
}

AutoPtr<IBoolean> CoreUtils::Box(
    /* [in] */ Boolean b)
{
    AutoPtr<IBoolean> bo;
    CBoolean::New(b, IID_IBoolean, (IInterface**)&bo);
    return bo;
}

AutoPtr<IInteger> CoreUtils::Box(
    /* [in] */ Integer i)
{
    AutoPtr<IInteger> io;
    CInteger::New(i, IID_IInteger, (IInterface**)&io);
    return io;
}

AutoPtr<IDouble> CoreUtils::Box(
    /* [in] */ Double d)
{
    AutoPtr<IDouble> dobj;
    CDouble::New(d, IID_IDouble, (IInterface**)&dobj);
    return dobj;
}

AutoPtr<ICharSequence> CoreUtils::Box(
    /* [in] */ const String& str)
{
    AutoPtr<ICharSequence> seq;
    CString::New(str, IID_ICharSequence, (IInterface**)&seq);
    return seq;
}

Array<ICharSequence*> CoreUtils::Box(
    /* [in] */ const Array<String>& strArray)
{
    if (strArray.IsEmpty()) {
        return Array<ICharSequence*>::Null();
    }

    Long size = strArray.GetLength();
    Array<ICharSequence*> seqArray(size);
    for (Long i = 0; i < size; i++) {
        seqArray.Set(i, Box(strArray[i]));
    }
    return seqArray;
}

Char CoreUtils::Unbox(
    /* [in] */ IChar* ch)
{
    Char c;
    ch->GetValue(&c);
    return c;
}

Boolean CoreUtils::Unbox(
    /* [in] */ IBoolean* bo)
{
    Boolean b;
    bo->GetValue(&b);
    return b;
}

Integer CoreUtils::Unbox(
    /* [in] */ IInteger* io)
{
    Integer i;
    io->GetValue(&i);
    return i;
}

Double CoreUtils::Unbox(
    /* [in] */ IDouble* dobj)
{
    Double d;
    dobj->GetValue(&d);
    return d;
}

String CoreUtils::Unbox(
    /* [in] */ ICharSequence* seq)
{
    String str;
    if (seq != nullptr) {
        seq->ToString(&str);
    }
    return str;
}

Array<String> CoreUtils::Unbox(
    /* [in] */ const Array<ICharSequence*>& seqArray)
{
    if (seqArray.IsEmpty()) {
        return Array<String>::Null();
    }

    Long size = seqArray.GetLength();
    Array<String> strArray(size);
    for (Long i = 0; i < size; i++) {
        strArray[i] = Unbox(seqArray[i]);
    }
    return strArray;
}

Boolean CoreUtils::GetBoolean(
    /* [in] */ const String& name)
{
    Boolean result = false;
    String value;
    ECode ec = System::GetProperty(name, &value);
    if (SUCCEEDED(ec)) {
        result = (!value.IsNull() && value.EqualsIgnoreCase("true"));
    }
    return result;
}

AutoPtr<IInterface> CoreUtils::Clone(
    /* [in] */ IInterface* obj,
    /* [in] */ const InterfaceID& iid)
{
    ICloneable* cloneable = ICloneable::Probe(obj);
    if (cloneable == nullptr) {
        return nullptr;
    }
    AutoPtr<IInterface> clone;
    cloneable->Clone(iid, &clone);
    return clone;
}

}
}
