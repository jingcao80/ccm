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

#ifndef __COMO_CORE_CINTEGER_H__
#define __COMO_CORE_CINTEGER_H__

#include "como.core.IInteger.h"
#include "como.core.INumber.h"
#include "como.core.IComparable.h"
#include "como.io.ISerializable.h"
#include "_como_core_CInteger.h"
#include "como/core/SyncObject.h"

using como::io::ISerializable;

namespace como {
namespace core {

Coclass(CInteger)
    , public SyncObject
    , public IInteger
    , public INumber
    , public ISerializable
    , public IComparable
{
public:
    COMO_INTERFACE_DECL();

    COMO_OBJECT_DECL();

    ECode Constructor(
        /* [in] */ Integer value);

    ECode ByteValue(
        /* [out] */ Byte* value) override;

    ECode ShortValue(
        /* [out] */ Short* value) override;

    ECode IntegerValue(
        /* [out] */ Integer* value) override;

    ECode LongValue(
        /* [out] */ Long* value) override;

    ECode FloatValue(
        /* [out] */ Float* value) override;

    ECode DoubleValue(
        /* [out] */ Double* value) override;

    ECode GetValue(
        /* [out] */ Integer* value) override;

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
    Integer mValue;
};

}
}

#endif // __COMO_CORE_CINTEGER_H__
