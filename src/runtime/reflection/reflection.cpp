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

#include "ccmcomponent.h"
#include "ccmlogger.h"
#include "ccmreflectionapi.h"
#include "CMetaComponent.h"
#include "Component.h"
#include "MetaSerializer.h"

#include <stdlib.h>

using ccm::metadata::MetaSerializer;

using ccm::metadata::MetaComponent;

namespace ccm {

ECode CoGetComponentMetadata(
    /* [in] */ const ComponentID& cid,
    /* [out] */ IMetaComponent** mc)
{
    VALIDATE_NOT_NULL(mc);

    CcmComponent* ccmComp;
    ECode ec = CoGetComponent(cid, &ccmComp);
    if (FAILED(ec)) {
        *mc = nullptr;
        return ec;
    }

    if (ccmComp->mMetaComponent != nullptr) {
        *mc = ccmComp->mMetaComponent;
        REFCOUNT_ADD(*mc);
        return NOERROR;
    }

    MetaComponent* mmc = reinterpret_cast<MetaComponent*>(
            ccmComp->mMetadataWrapper->mMetadata);

    void* metadata = malloc(mmc->mSize);
    if (metadata == nullptr) {
        Logger::E("CCMRT", "Malloc %d size metadata failed.", mmc->mSize);
        *mc = nullptr;
        return E_OUT_OF_MEMORY_ERROR;
    }
    memcpy(metadata, mmc, mmc->mSize);

    MetaSerializer serializer;
    serializer.Deserialize(reinterpret_cast<uintptr_t>(metadata));
    ccmComp->mMetaComponent = new CMetaComponent((MetaComponent*)metadata);
    REFCOUNT_ADD(ccmComp->mMetaComponent);

    *mc = ccmComp->mMetaComponent;
    REFCOUNT_ADD(*mc);

    return NOERROR;
}

}
