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

#ifndef __CCM_METADATA_METASERIALIZER_H__
#define __CCM_METADATA_METASERIALIZER_H__

#include "Component.h"
#include <stddef.h>
#include <stdint.h>

namespace ccm {
namespace metadata {

class MetaSerializer
{
public:
    MetaSerializer();

    MetaSerializer(
        /* [in] */ MetaComponent* mc);

    void Serialize();

    inline uintptr_t GetData()
    { return mBasePtr; }

    inline int GetDataSize()
    { return mDataSize; }

    void Deserialize(
        /* [in] */ uintptr_t basePtr);

private:
    void SerializeMetaCoclass(
        /* [in] */ MetaCoclass* mc);

    void SerializeMetaConstant(
        /* [in] */ MetaConstant* mc);

    void SerializeMetaEnumeration(
        /* [in] */ MetaEnumeration* me);

    void SerializeMetaEnumerator(
        /* [in] */ MetaEnumerator* me);

    void SerializeMetaInterface(
        /* [in] */ MetaInterface* mi);

    void SerializeMetaMethod(
        /* [in] */ MetaMethod* mm);

    void SerializeMetaNamespace(
        /* [in] */ MetaNamespace* mn);

    void SerializeMetaParameter(
        /* [in] */ MetaParameter* mp);

    void SerializeMetaType(
        /* [in] */ MetaType* mt);

    ptrdiff_t SerializeAdjust(
        /* [in] */ void* addr);

    void DeserializeMetaCoclass(
        /* [in] */ MetaCoclass* mc);

    void DeserializeMetaConstant(
        /* [in] */ MetaConstant* mc);

    void DeserializeMetaEnumeration(
        /* [in] */ MetaEnumeration* me);

    void DeserializeMetaEnumerator(
        /* [in] */ MetaEnumerator* mr);

    void DeserializeMetaInterface(
        /* [in] */ MetaInterface* mi);

    void DeserializeMetaMethod(
        /* [in] */ MetaMethod* mm);

    void DeserializeMetaNamespace(
        /* [in] */ MetaNamespace* mn);

    void DeserializeMetaParameter(
        /* [in] */ MetaParameter* mp);

    void DeserializeMetaType(
        /* [in] */ MetaType* mt);

    uintptr_t DeserializeAdjust(
        /* [in] */ void* addr);

private:
    MetaComponent* mMetaComponent;
    uintptr_t mBasePtr;
    int mDataSize;
};

}
}

#endif // __CCM_METADATA_METASERIALIZER_H__
