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

#ifndef __CCM_CORE_COREUTILS_H__
#define __CCM_CORE_COREUTILS_H__

#include "ccm.core.IArray.h"
#include "ccm.core.IChar.h"
#include "ccm.core.ICharSequence.h"
#include "ccm.core.IDouble.h"
#include "ccm.core.IInteger.h"
#include <ccmautoptr.h>

namespace ccm {
namespace core {

class CoreUtils
{
public:
    COM_PUBLIC static AutoPtr<IChar> Box(
        /* [in] */ Char c);

    COM_PUBLIC static AutoPtr<IBoolean> Box(
        /* [in] */ Boolean b);

    COM_PUBLIC static AutoPtr<IInteger> Box(
        /* [in] */ Integer i);

    COM_PUBLIC static AutoPtr<ILong> Box(
        /* [in] */ Long l);

    COM_PUBLIC static AutoPtr<IDouble> Box(
        /* [in] */ Double d);

    COM_PUBLIC static AutoPtr<ICharSequence> Box(
        /* [in] */ const String& str);

    COM_PUBLIC static Array<ICharSequence*> Box(
        /* [in] */ const Array<String>& strArray);

    COM_PUBLIC static AutoPtr<IArray> Box(
        /* [in] */ const Array<IInterface*>& objArray);

    COM_PUBLIC static Char Unbox(
        /* [in] */ IChar* ch);

    COM_PUBLIC static Boolean Unbox(
        /* [in] */ IBoolean* bo);

    COM_PUBLIC static Integer Unbox(
        /* [in] */ IInteger* io);

    COM_PUBLIC static Long Unbox(
        /* [in] */ ILong* lo);

    COM_PUBLIC static Double Unbox(
        /* [in] */ IDouble* dobj);

    COM_PUBLIC static String Unbox(
        /* [in] */ ICharSequence* seq);

    COM_PUBLIC static Array<String> Unbox(
        /* [in] */ const Array<ICharSequence*>& seqArray);

    COM_PUBLIC static Array<IInterface*> Unbox(
        /* [in] */ IArray* arrObj);

    COM_PUBLIC static Array<Short> ToUTF16Chars(
        /* [in] */ const Array<Char>& chars);

    COM_PUBLIC static ECode ToChars(
        /* [in] */ const Array<Short>& utf16chars,
        /* [out] */ Array<Char>& chars);

    COM_PUBLIC static Boolean GetBoolean(
        /* [in] */ const String& name);

    COM_PUBLIC static AutoPtr<IInterface> Clone(
        /* [in] */ IInterface* obj,
        /* [in] */ const InterfaceID& iid);
};

}
}

#endif // __CCM_CORE_COREUTILS_H__
