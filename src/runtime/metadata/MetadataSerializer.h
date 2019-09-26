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

#ifndef __CCM_METADATA_METADATASERIALIZER_H__
#define __CCM_METADATA_METADATASERIALIZER_H__

#include "Component.h"
#include <stddef.h>
#include <stdint.h>

namespace como {
namespace metadata {

class MetadataSerializer
{
public:
    MetadataSerializer() = default;

    MetadataSerializer(
        /* [in] */ MetaComponent* mc);

    void Serialize();

    void Deserialize(
        /* [in] */ uintptr_t basePtr);

    inline uintptr_t GetSerializedMetadata();

    inline size_t GetSize();

private:
    void SerializeMetaNamespace(
        /* [in] */ MetaNamespace* mn);

    void SerializeMetaConstant(
        /* [in] */ MetaConstant* mc);

    void SerializeMetaCoclass(
        /* [in] */ MetaCoclass* mc);

    void SerializeMetaEnumeration(
        /* [in] */ MetaEnumeration* me);

    void SerializeMetaEnumerator(
        /* [in] */ MetaEnumerator* me);

    void SerializeMetaInterface(
        /* [in] */ MetaInterface* mi);

    void SerializeMetaMethod(
        /* [in] */ MetaMethod* mm);

    void SerializeMetaParameter(
        /* [in] */ MetaParameter* mp);

    void SerializeMetaValue(
        /* [in] */ MetaType* mt,
        /* [in] */ MetaValue* mv);

    void SerializeMetaType(
        /* [in] */ MetaType* mt);

    ptrdiff_t SerializeAdjust(
        /* [in] */ void* addr);

    void DeserializeMetaNamespace(
        /* [in] */ MetaNamespace* mn);

    void DeserializeMetaConstant(
        /* [in] */ MetaConstant* mc);

    void DeserializeMetaCoclass(
        /* [in] */ MetaCoclass* mc);

    void DeserializeMetaEnumeration(
        /* [in] */ MetaEnumeration* me);

    void DeserializeMetaEnumerator(
        /* [in] */ MetaEnumerator* mr);

    void DeserializeMetaInterface(
        /* [in] */ MetaInterface* mi);

    void DeserializeMetaMethod(
        /* [in] */ MetaMethod* mm);

    void DeserializeMetaParameter(
        /* [in] */ MetaParameter* mp);

    void DeserializeMetaValue(
        /* [in] */ MetaType* mt,
        /* [in] */ MetaValue* mv);

    void DeserializeMetaType(
        /* [in] */ MetaType* mt);

    uintptr_t DeserializeAdjust(
        /* [in] */ void* addr);

private:
    MetaComponent* mComponent = nullptr;
    uintptr_t mBasePtr = 0;
    size_t mMetadataSize = 0;
};

uintptr_t MetadataSerializer::GetSerializedMetadata()
{
    return mBasePtr;
}

size_t MetadataSerializer::GetSize()
{
    return mMetadataSize;
}

} // metadata
} // como

#endif // __CCM_METADATA_METADATASERIALIZER_H__
