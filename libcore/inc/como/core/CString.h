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

#ifndef __COMO_CORE_CSTRING_H__
#define __COMO_CORE_CSTRING_H__

#include "como/core/SyncObject.h"
#include "como.core.ICharSequence.h"
#include "como.core.IComparable.h"
#include "como.core.IString.h"
#include "como.io.ISerializable.h"
#include "_como_core_CString.h"
#include <comostring.h>

using como::io::ISerializable;

namespace como {
namespace core {

Coclass(CString)
    , public SyncObject
    , public ICharSequence
    , public IComparable
    , public ISerializable
    , public IString
{
public:
    COMO_INTERFACE_DECL();

    COMO_OBJECT_DECL();

    ECode Constructor(
        /* [in] */ const String& str);

    ECode GetLength(
        /* [out] */ Integer& number) override;

    ECode GetCharAt(
        /* [in] */ Integer index,
        /* [out] */ Char& c) override;

    ECode SubSequence(
        /* [in] */ Integer start,
        /* [in] */ Integer end,
        /* [out] */ AutoPtr<ICharSequence>& subcsq) override;

    ECode CompareTo(
        /* [in] */ IInterface* other,
        /* [out] */ Integer& result) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& result) override;

    ECode GetHashCode(
        /* [out] */ Integer& hash) override;

    ECode ToString(
        /* [out] */ String& str) override;

private:
    String mString;
};

}
}

#endif // __COMO_CORE_CSTRING_H__
