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

#ifndef __CCM_CORE_CCHAR_H__
#define __CCM_CORE_CCHAR_H__

#include "ccm.core.IChar.h"
#include "ccm.core.IComparable.h"
#include "ccm.io.ISerializable.h"
#include "_ccm_core_CChar.h"
#include "ccm/core/SyncObject.h"

using ccm::io::ISerializable;

namespace ccm {
namespace core {

Coclass(CChar)
    , public SyncObject
    , public IChar
    , public ISerializable
    , public IComparable
{
public:
    CCM_INTERFACE_DECL();

    CCM_OBJECT_DECL();

    ECode Constructor(
        /* [in] */ Char value);

    ECode GetValue(
        /* [out] */ Char* value) override;

    ECode CompareTo(
        /* [in] */ IInterface* other,
        /* [out] */ Integer* result) override;

    ECode Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result) override;

    ECode GetHashCode(
        /* [out] */ Integer* hash) override;

    ECode ToString(
        /* [out] */ String* str) override;

private:
    Char mValue;
};

}
}

#endif // __CCM_CORE_CCHAR_H__
