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

#ifndef __COMO_UTIL_ABSTRACTMAP_H__
#define __COMO_UTIL_ABSTRACTMAP_H__

#include "como/core/SyncObject.h"
#include "como.io.ISerializable.h"
#include "como.util.ICollection.h"
#include "como.util.IMap.h"
#include "como.util.IMapEntry.h"
#include "como.util.ISet.h"
#include <comosp.h>

using como::core::SyncObject;
using como::io::ISerializable;

namespace como {
namespace util {

class AbstractMap
    : public SyncObject
    , public IMap
{
public:
    class SimpleEntry
        : public Object
        , public IMapEntry
        , public ISerializable
    {
    public:
        COMO_INTERFACE_DECL()

        ECode Constructor(
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* value);

        ECode Constructor(
            /* [in] */ IMapEntry* entry);

        ECode GetKey(
            /* [out] */ IInterface** key) override;

        ECode GetValue(
            /* [out] */ IInterface** value) override;

        ECode SetValue(
            /* [in] */ IInterface* value,
            /* [out] */ IInterface** prevValue = nullptr) override;

        ECode Equals(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean& result) override;

        ECode GetHashCode(
            /* [out] */ Integer& hashCode) override;

        ECode ToString(
            /* [out] */ String& str) override;

    private:
        AutoPtr<IInterface> mKey;
        AutoPtr<IInterface> mValue;
    };

    class SimpleImmutableEntry
        : public Object
        , public IMapEntry
        , public ISerializable
    {
    public:
        COMO_INTERFACE_DECL()

        ECode Constructor(
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* value);

        ECode Constructor(
            /* [in] */ IMapEntry* entry);

        ECode GetKey(
            /* [out] */ IInterface** key) override;

        ECode GetValue(
            /* [out] */ IInterface** value) override;

        ECode SetValue(
            /* [in] */ IInterface* value,
            /* [out] */ IInterface** prevValue = nullptr) override;

        ECode Equals(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean& result) override;

        ECode GetHashCode(
            /* [out] */ Integer& hashCode) override;

        ECode ToString(
            /* [out] */ String& str) override;

    private:
        AutoPtr<IInterface> mKey;
        AutoPtr<IInterface> mValue;
    };

public:
    COMO_INTERFACE_DECL();

    ECode GetSize(
        /* [out] */ Integer* size) override;

    ECode IsEmpty(
        /* [out] */ Boolean* result) override;

    ECode ContainsValue(
        /* [in] */ IInterface* value,
        /* [out] */ Boolean* result) override;

    ECode ContainsKey(
        /* [in] */ IInterface* key,
        /* [out] */ Boolean* result) override;

    ECode Get(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** value) override;

    ECode Put(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** prevValue = nullptr) override;

    ECode Remove(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** prevValue = nullptr) override;

    ECode PutAll(
        /* [in] */ IMap* m) override;

    ECode PutIfAbsent(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** prevValue = nullptr) override;

    ECode Clear() override;

    ECode GetKeySet(
        /* [out] */ ISet** keys) override;

    ECode GetValues(
        /* [out] */ ICollection** values) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean& result) override;

    ECode GetHashCode(
        /* [out] */ Integer& hash) override;

    ECode ToString(
        /* [out] */ String& str) override;

protected:
    AutoPtr<ISet> mKeySet;

    AutoPtr<ICollection> mValues;
};

}
}

#endif // __COMO_UTIL_ABSTRACTMAP_H__
