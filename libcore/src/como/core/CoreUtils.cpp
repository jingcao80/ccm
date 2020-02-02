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

#include "como/core/CArray.h"
#include "como/core/CBoolean.h"
#include "como/core/CChar.h"
#include "como/core/CDouble.h"
#include "como/core/CInteger.h"
#include "como/core/CLong.h"
#include "como/core/CoreUtils.h"
#include "como/core/CString.h"
#include "como/core/System.h"
#include "como.core.ICloneable.h"

namespace como {
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

AutoPtr<ILong> CoreUtils::Box(
    /* [in] */ Long l)
{
    AutoPtr<ILong> lo;
    CLong::New(l, IID_ILong, (IInterface**)&lo);
    return lo;
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

AutoPtr<IArray> CoreUtils::Box(
    /* [in] */ const Array<IInterface*>& objArray)
{
    if (objArray.IsEmpty()) {
        return nullptr;
    }

    Long size = objArray.GetLength();
    InterfaceID iid;
    objArray[0]->GetInterfaceID(objArray[0], iid);
    AutoPtr<IArray> arrObj;
    CArray::New(iid, size, IID_IArray, (IInterface**)&arrObj);
    for (Long i = 0; i < size; i++) {
        arrObj->Set(i, objArray[i]);
    }
    return arrObj;
}

Char CoreUtils::Unbox(
    /* [in] */ IChar* ch)
{
    Char c;
    ch->GetValue(c);
    return c;
}

Boolean CoreUtils::Unbox(
    /* [in] */ IBoolean* bo)
{
    Boolean b;
    bo->GetValue(b);
    return b;
}

Integer CoreUtils::Unbox(
    /* [in] */ IInteger* io)
{
    Integer i;
    io->GetValue(i);
    return i;
}

Long CoreUtils::Unbox(
    /* [in] */ ILong* lo)
{
    Long l;
    lo->GetValue(l);
    return l;
}

Double CoreUtils::Unbox(
    /* [in] */ IDouble* dobj)
{
    Double d;
    dobj->GetValue(d);
    return d;
}

String CoreUtils::Unbox(
    /* [in] */ ICharSequence* seq)
{
    String str;
    if (seq != nullptr) {
        seq->ToString(str);
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

Array<IInterface*> CoreUtils::Unbox(
    /* [in] */ IArray* arrObj)
{
    if (arrObj == nullptr) {
        return Array<IInterface*>::Null();
    }

    Long size;
    arrObj->GetLength(size);
    Array<IInterface*> objArray(size);
    for (Long i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        arrObj->Get(i, obj);
        objArray.Set(i, obj);
    }
    return objArray;
}

Array<Short> CoreUtils::ToUTF16Chars(
    /* [in] */ const Array<Char>& chars)
{
    Array<Short> utf16chars;
    if (chars.IsNull()) {
        return utf16chars;
    }

    Long size = chars.GetLength();
    utf16chars = Array<Short>(size);
    for (Long i = 0; i < size; i++) {
        utf16chars[i] = chars[i];
    }
    return utf16chars;
}

ECode CoreUtils::ToChars(
    /* [in] */ const Array<Short>& utf16chars,
    /* [out] */ Array<Char>& chars)
{
    if (utf16chars.IsNull() || chars.IsNull()) {
        return NOERROR;
    }

    Long size = chars.GetLength() <= utf16chars.GetLength() ?
            chars.GetLength() : utf16chars.GetLength();
    for (Long i = 0; i < size; i++) {
        chars[i] = utf16chars[i];
    }
    return NOERROR;
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
