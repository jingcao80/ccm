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

#include "ccmcomponent.h"
#include "ccmlogger.h"
#include "ccmreflectionapi.h"
#include "CMetaComponent.h"
#include "Component.h"
#include "CBootClassLoader.h"
#include "MetaSerializer.h"
#include <dlfcn.h>
#include <errno.h>

using ccm::metadata::MetaComponent;
using ccm::metadata::MetaSerializer;

namespace ccm {

ECode CoGetComponentMetadata(
    /* [in] */ const ComponentID& cid,
    /* [in] */ IClassLoader* loader,
    /* [out] */ IMetaComponent** mc)
{
    VALIDATE_NOT_NULL(mc);

    if (loader == nullptr) {
        loader = CBootClassLoader::GetInstance();
    }

    return loader->LoadComponent(cid, mc);
}

ECode CoGetComponentMetadataFromFile(
    /* [in] */ HANDLE fd,
    /* [in] */ IClassLoader* loader,
    /* [out] */ IMetaComponent** mc)
{
    VALIDATE_NOT_NULL(mc);
    *mc = nullptr;

    void* handle = reinterpret_cast<void*>(fd);
    if (handle == nullptr) return E_ILLEGAL_ARGUMENT_EXCEPTION;

    GetClassObjectPtr getFunc = (GetClassObjectPtr)dlsym(handle, "soGetClassObject");
    if (getFunc == nullptr) {
        Logger::E("CCMRT", "Dlsym \"soGetClassObject\" function from "
                "component failed. The reason is %s.", strerror(errno));
        return E_COMPONENT_IO_EXCEPTION;
    }

    GetAllClassObjectsPtr getAllFunc = (GetAllClassObjectsPtr)dlsym(handle, "soGetAllClassObjects");
    if (getAllFunc == nullptr) {
        Logger::E("CCMRT", "Dlsym \"soGetAllClassObjects\" function from "
                "component failed. The reason is %s.", strerror(errno));
        return E_COMPONENT_IO_EXCEPTION;
    }

    CanUnloadPtr canFunc = (CanUnloadPtr)dlsym(handle, "soCanUnload");
    if (canFunc == nullptr) {
        Logger::E("CCMRT", "Dlsym \"soCanUnload\" function from "
                "component failed. The reason is %s.", strerror(errno));
        return E_COMPONENT_IO_EXCEPTION;
    }

    MetadataWrapper* metadata = *(MetadataWrapper**)(dlsym(handle, "soMetadataHandle"));
    if (metadata == nullptr) {
        Logger::E("CCMRT", "Dlsym \"soMetadataHandle\" variable from "
                "component failed. The reason is %s.", strerror(errno));
        return E_COMPONENT_IO_EXCEPTION;
    }

    CcmComponent* ccmComp = (CcmComponent*)malloc(sizeof(CcmComponent));
    if (ccmComp == nullptr) {
        Logger::E("CCMRT", "Malloc CcmComponent failed.");
        return E_OUT_OF_MEMORY_ERROR;
    }
    ccmComp->mSoHandle = handle;
    ccmComp->mSoGetClassObject = getFunc;
    ccmComp->mSoGetAllClassObjects = getAllFunc;
    ccmComp->mSoCanUnload = canFunc;
    ccmComp->mMetadataWrapper = metadata;

    MetaComponent* mmc = reinterpret_cast<MetaComponent*>(
            ccmComp->mMetadataWrapper->mMetadata);

    void* data = malloc(mmc->mSize);
    if (data == nullptr) {
        Logger::E("CCMRT", "Malloc %d size metadata failed.", mmc->mSize);
        free(ccmComp);
        *mc = nullptr;
        return E_OUT_OF_MEMORY_ERROR;
    }
    memcpy(data, mmc, mmc->mSize);

    MetaSerializer serializer;
    serializer.Deserialize(reinterpret_cast<uintptr_t>(data));
    *mc = new CMetaComponent(loader, ccmComp, (MetaComponent*)data);
    REFCOUNT_ADD(*mc);
    return NOERROR;
}

ECode CoGetCoclassMetadata(
    /* [in] */ const CoclassID& cid,
    /* [in] */ IClassLoader* loader,
    /* [in] */ IMetaCoclass** mc)
{
    VALIDATE_NOT_NULL(mc);

    if (loader == nullptr) {
        loader = CBootClassLoader::GetInstance();
    }

    AutoPtr<IMetaComponent> component;
    ECode ec = loader->LoadComponent(*cid.mCid, &component);
    if (FAILED(ec)) {
        *mc = nullptr;
        return ec;
    }
    return component->GetCoclass(cid, mc);
}

}
