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

#include "component/comocomp.h"
#include "component/CBootClassLoader.h"
#include "metadata/Component.h"
#include "metadata/MetadataSerializer.h"
#include "util/comolog.h"
#include "reflection/comoreflapi.h"
#include "reflection/CMetaComponent.h"
#include "reflection/reflection.h"
#include <dlfcn.h>
#include <cerrno>

namespace como {

const char* NAMESPACE_GLOBAL = "__global__";

ECode CoGetComponentMetadata(
    /* [in] */ const ComponentID& cid,
    /* [in] */ IClassLoader* loader,
    /* [out] */ AutoPtr<IMetaComponent>& mc)
{
    if (loader == nullptr) {
        loader = CBootClassLoader::GetInstance();
    }

    return loader->LoadComponent(cid, mc);
}

ECode CoGetComponentMetadataFromFile(
    /* [in] */ HANDLE fd,
    /* [in] */ IClassLoader* loader,
    /* [out] */ AutoPtr<IMetaComponent>& mc)
{
    mc = nullptr;

    void* handle = reinterpret_cast<void*>(fd);
    if (handle == nullptr) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    GetClassObjectPtr getFunc = (GetClassObjectPtr)dlsym(handle, "soGetClassObject");
    if (getFunc == nullptr) {
        Logger::E("COMORT", "Dlsym \"soGetClassObject\" function from "
                "component failed. The reason is %s.", strerror(errno));
        return E_COMPONENT_IO_EXCEPTION;
    }

    GetAllClassObjectsPtr getAllFunc = (GetAllClassObjectsPtr)dlsym(handle, "soGetAllClassObjects");
    if (getAllFunc == nullptr) {
        Logger::E("COMORT", "Dlsym \"soGetAllClassObjects\" function from "
                "component failed. The reason is %s.", strerror(errno));
        return E_COMPONENT_IO_EXCEPTION;
    }

    CanUnloadPtr canFunc = (CanUnloadPtr)dlsym(handle, "soCanUnload");
    if (canFunc == nullptr) {
        Logger::E("COMORT", "Dlsym \"soCanUnload\" function from "
                "component failed. The reason is %s.", strerror(errno));
        return E_COMPONENT_IO_EXCEPTION;
    }

    MetadataWrapper* metadata = *(MetadataWrapper**)(dlsym(handle, "soMetadataHandle"));
    if (metadata == nullptr) {
        Logger::E("COMORT", "Dlsym \"soMetadataHandle\" variable from "
                "component failed. The reason is %s.", strerror(errno));
        return E_COMPONENT_IO_EXCEPTION;
    }

    ComoComponent* component = (ComoComponent*)malloc(sizeof(ComoComponent));
    if (component == nullptr) {
        Logger::E("COMORT", "Malloc ComoComponent failed.");
        return E_OUT_OF_MEMORY_ERROR;
    }
    component->mSoHandle = handle;
    component->mSoGetClassObject = getFunc;
    component->mSoGetAllClassObjects = getAllFunc;
    component->mSoCanUnload = canFunc;
    component->mMetadataWrapper = metadata;

    MetaComponent* mmc = reinterpret_cast<MetaComponent*>(
            component->mMetadataWrapper->mMetadata);

    if (mmc->mMagic != COMO_MAGIC) {
        Logger::E("COMORT", "Metadata info is bad.");
        free(component);
        return E_COMPONENT_IO_EXCEPTION;
    }

    void* data = malloc(mmc->mSize);
    if (data == nullptr) {
        Logger::E("COMORT", "Malloc %lu size metadata failed.", mmc->mSize);
        free(component);
        return E_OUT_OF_MEMORY_ERROR;
    }
    memcpy(data, mmc, mmc->mSize);

    MetadataSerializer serializer;
    serializer.Deserialize(reinterpret_cast<uintptr_t>(data));
    mc = new CMetaComponent(loader, component, (MetaComponent*)data);
    return NOERROR;
}

ECode CoGetComponentMetadataFromBytes(
    /* [in] */ const Array<Byte>& bytes,
    /* [in] */ IClassLoader* loader,
    /* [out] */ AutoPtr<IMetaComponent>& mc)
{
    mc = nullptr;

    if (bytes.IsEmpty() || bytes.GetLength() < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    ComoComponent* component = (ComoComponent*)malloc(
            sizeof(ComoComponent) + sizeof(MetadataWrapper) + bytes.GetLength());
    if (component == nullptr) {
        Logger::E("COMORT", "Malloc ComoComponent failed.");
        return E_OUT_OF_MEMORY_ERROR;
    }
    component->mSoHandle = nullptr;
    component->mSoGetClassObject = nullptr;
    component->mSoGetAllClassObjects = nullptr;
    component->mSoCanUnload = nullptr;
    component->mMetadataWrapper = reinterpret_cast<MetadataWrapper*>(component + 1);
    component->mMetadataWrapper->mSize = bytes.GetLength();
    memcpy(&(component->mMetadataWrapper->mMetadata), bytes.GetPayload(), bytes.GetLength());

    MetaComponent* mmc = reinterpret_cast<MetaComponent*>(
            component->mMetadataWrapper->mMetadata);

    if (mmc->mMagic != COMO_MAGIC) {
        Logger::E("COMORT", "Metadata info is bad.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    void* data = malloc(mmc->mSize);
    if (data == nullptr) {
        Logger::E("COMORT", "Malloc %lu size metadata failed.", mmc->mSize);
        free(component);
        return E_OUT_OF_MEMORY_ERROR;
    }
    memcpy(data, mmc, mmc->mSize);

    MetadataSerializer serializer;
    serializer.Deserialize(reinterpret_cast<uintptr_t>(data));
    mc = new CMetaComponent(loader, component, (MetaComponent*)data);
    return NOERROR;
}

ECode CoGetCoclassMetadata(
    /* [in] */ const CoclassID& cid,
    /* [in] */ IClassLoader* loader,
    /* [out] */ AutoPtr<IMetaCoclass>& mc)
{
    if (loader == nullptr) {
        loader = CBootClassLoader::GetInstance();
    }

    AutoPtr<IMetaComponent> component;
    ECode ec = loader->LoadComponent(*cid.mCid, component);
    if (FAILED(ec)) {
        mc = nullptr;
        return ec;
    }
    return component->GetCoclass(cid, mc);
}

} // namespace como
