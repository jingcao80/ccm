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

#ifndef __CCM_CORE_CSHORT_H__
#define __CCM_CORE_CSHORT_H__

#include "ccm.core.IShort.h"
#include "ccm.core.INumber.h"
#include "ccm.core.IComparable.h"
#include "ccm.io.ISerializable.h"
#include "_ccm_core_CShort.h"
#include "core/SyncObject.h"

using ccm::io::ISerializable;

namespace ccm {
namespace core {

Coclass(CShort)
    , public SyncObject
    , public IShort
    , public INumber
    , public ISerializable
    , public IComparable
{
public:
    CCM_INTERFACE_DECL();

    CCM_OBJECT_DECL();

    ECode constructor(
        /* [in] */ Short value);

    ECode ByteValue(
        /* [out] */ Byte* value) override;

    ECode ShortValue(
        /* [out] */ Short* value) override;

    ECode IntValue(
        /* [out] */ Integer* value) override;

    ECode LongValue(
        /* [out] */ Long* value) override;

    ECode FloatValue(
        /* [out] */ Float* value) override;

    ECode DoubleValue(
        /* [out] */ Double* value) override;

    ECode GetValue(
        /* [out] */ Short* value) override;

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
    Short mValue;
};

}
}

#endif // __CCM_CORE_CSHORT_H__
