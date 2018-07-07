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

#include "CBootClassLoader.h"
#include "reflection/ccmreflectionapi.h"
#include "reflection/CMetaComponent.h"
#include "metadata/Component.h"
#include "util/ccmlogger.h"
#include "util/elf.h"
#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

using ccm::metadata::MetaComponent;

namespace ccm {

const CoclassID CID_CBootClassLoader =
        {{0x861efebf,0x54c8,0x4939,0xa2ab,{0x4,0xb,0xa,0xc,0xf,0x2,0xc,0xa,0xf,0xa,0x1,0xe}}, &CID_CCMRuntime};

AutoPtr<IClassLoader> CBootClassLoader::sInstance = new CBootClassLoader();
const String CBootClassLoader::TAG("CBootClassLoader");

CCM_OBJECT_IMPL(CBootClassLoader);
CCM_INTERFACE_IMPL_1(CBootClassLoader, Object, IClassLoader);

CBootClassLoader::CBootClassLoader()
    : mDebug(false)
{
    InitComponentPath();
}

AutoPtr<IClassLoader> CBootClassLoader::GetInstance()
{
    return sInstance;
}

ECode CBootClassLoader::LoadComponent(
    /* [in] */ const String& path,
    /* [out] */ IMetaComponent** component)
{
    VALIDATE_NOT_NULL(component);
    *component = nullptr;

    {
        Mutex::AutoLock lock(mComponentsLock);
        IMetaComponent* mc = mComponentPathMap.Get(path);
        if (mc != nullptr) {
            *component = mc;
            REFCOUNT_ADD(*component);
            return NOERROR;
        }
    }

    void* handle = dlopen(path.string(), RTLD_NOW);
    if (handle == nullptr) {
        Logger::E(TAG, "Dlopen \"%s\" failed. The reason is %s.",
                path.string(), strerror(errno));
        return E_COMPONENT_IO_EXCEPTION;
    }

    {
        // Dlopening a component maybe cause to run its initialization which
        // running nested LoadComponent about itself, so we check mComponents again.
        Mutex::AutoLock lock(mComponentsLock);
        IMetaComponent* mc = mComponentPathMap.Get(path);
        if (mc != nullptr) {
            *component = mc;
            REFCOUNT_ADD(*component);
            return NOERROR;
        }
    }

    ECode ec = CoGetComponentMetadataFromFile(
            reinterpret_cast<HANDLE>(handle), this, component);
    if (FAILED(ec)) {
        dlclose(handle);
        return ec;
    }

    ComponentID compId;
    (*component)->GetComponentID(&compId);

    {
        Mutex::AutoLock lock(mComponentsLock);
        mComponents.Put(compId.mUuid, *component);
        mComponentPathMap.Put(path, *component);
    }

    return NOERROR;
}

ECode CBootClassLoader::LoadComponent(
    /* [in] */ const ComponentID& compId,
    /* [out] */ IMetaComponent** component)
{
    VALIDATE_NOT_NULL(component);
    *component = nullptr;

    {
        Mutex::AutoLock lock(mComponentsLock);
        IMetaComponent* mc = mComponents.Get(compId.mUuid);
        if (mc != nullptr) {
            *component = mc;
            REFCOUNT_ADD(*component);
            return NOERROR;
        }
    }

    String compPath;
    ECode ec = FindComponent(compId, &compPath);
    if (FAILED(ec)) {
        return ec;
    }

    void* handle = dlopen(compPath.string(), RTLD_NOW);
    if (handle == nullptr) {
        Logger::E(TAG, "Dlopen \"%s\" failed. The reason is %s.",
                compPath.string(), strerror(errno));
        return E_COMPONENT_IO_EXCEPTION;
    }

    {
        // Dlopening a component maybe cause to run its initialization which
        // running nested LoadComponent about itself, so we check mComponents again.
        Mutex::AutoLock lock(mComponentsLock);
        IMetaComponent* mc = mComponents.Get(compId.mUuid);
        if (mc != nullptr) {
            *component = mc;
            REFCOUNT_ADD(*component);
            return NOERROR;
        }
    }

    ec = CoGetComponentMetadataFromFile(
            reinterpret_cast<HANDLE>(handle), this, component);
    if (FAILED(ec)) {
        dlclose(handle);
        return ec;
    }

    {
        Mutex::AutoLock lock(mComponentsLock);
        mComponents.Put(compId.mUuid, *component);
        mComponentPathMap.Put(compPath, *component);
    }

    return NOERROR;
}

ECode CBootClassLoader::FindComponent(
    /* [in] */ const ComponentID& compId,
    /* [out] */ String* compPath)
{
    VALIDATE_NOT_NULL(compPath);

    String url(compId.mUrl);
    *compPath = nullptr;
    if (mDebug) {
        Logger::D(TAG, "The url of the component which will be loaded is \"%s\".",
                url.string());
    }

    Integer index = url.LastIndexOf("/");
    String compFile = index != -1 ? url.Substring(index + 1) : url;
    if (compFile.IsNullOrEmpty()) {
        Logger::E(TAG, "The name of component is null or empty.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FILE* fd = nullptr;
    for (Long i = 0; i < mComponentPath.GetSize(); i++) {
        String filePath = mComponentPath.Get(i) + "/" + compFile;
        fd = fopen(filePath.string(), "rb");
        if (fd != nullptr) {
            if (mDebug) {
                Logger::D(TAG, "Find \"%\" component in directory \"%s\".",
                        compFile.string(), mComponentPath.Get(i).string());
            }
            *compPath = filePath;
            break;
        }
    }
    if (fd == nullptr) {
        Logger::E(TAG, "Cannot find \"%s\" component.", compFile.string());
        *compPath = nullptr;
        return E_COMPONENT_NOT_FOUND_EXCEPTION;
    }

    if (fseek(fd, 0, SEEK_SET) == -1) {
        Logger::E(TAG, "Seek \"%s\" file failed.", compFile.string());
        fclose(fd);
        *compPath = nullptr;
        return E_COMPONENT_IO_EXCEPTION;
    }

    Elf64_Ehdr ehdr;

    if (fread((void *)&ehdr, sizeof(Elf64_Ehdr), 1, fd) < 1) {
        Logger::E(TAG, "Read \"%s\" file failed.", compFile.string());
        fclose(fd);
        *compPath = nullptr;
        return E_COMPONENT_IO_EXCEPTION;
    }

    if (fseek(fd, ehdr.e_shoff, SEEK_SET) == -1) {
        Logger::E(TAG, "Seek \"%s\" file failed.", compFile.string());
        fclose(fd);
        *compPath = nullptr;
        return E_COMPONENT_IO_EXCEPTION;
    }

    Elf64_Shdr* shdrs = (Elf64_Shdr *)malloc(sizeof(Elf64_Shdr) * ehdr.e_shnum);
    if (shdrs == nullptr) {
        Logger::E(TAG, "Malloc Elf64_Shdr failed.");
        fclose(fd);
        *compPath = nullptr;
        return E_COMPONENT_IO_EXCEPTION;
    }

    if (fread((void*)shdrs, sizeof(Elf64_Shdr), ehdr.e_shnum, fd) < ehdr.e_shnum) {
        Logger::E(TAG, "Read \"%s\" file failed.", compFile.string());
        free(shdrs);
        fclose(fd);
        *compPath = nullptr;
        return E_COMPONENT_IO_EXCEPTION;
    }

    Elf64_Shdr* strShdr = shdrs + ehdr.e_shstrndx;
    char* strTable = (char *)malloc(strShdr->sh_size);
    if (strTable == nullptr) {
        Logger::E(TAG, "Malloc string table failed.");
        free(shdrs);
        fclose(fd);
        *compPath = nullptr;
        return E_OUT_OF_MEMORY_ERROR;
    }

    if (fseek(fd, strShdr->sh_offset, SEEK_SET) == -1) {
        Logger::E(TAG, "Seek \"%s\" file failed.", compFile.string());
        free(shdrs);
        free(strTable);
        fclose(fd);
        *compPath = nullptr;
        return E_COMPONENT_IO_EXCEPTION;
    }

    if (fread((void*)strTable, 1, strShdr->sh_size, fd) < strShdr->sh_size) {
        Logger::E(TAG, "Read \"%s\" file failed.", compFile.string());
        free(shdrs);
        free(strTable);
        fclose(fd);
        *compPath = nullptr;
        return E_COMPONENT_IO_EXCEPTION;
    }

    Elf64_Shdr* mdSec = nullptr;
    for (int i = 0; i < ehdr.e_shnum; i++) {
        const char* secName = strTable + shdrs[i].sh_name;
        if (!strncmp(secName, ".metadata", 9)) {
            mdSec = shdrs + i;
            break;
        }
    }

    if (mdSec == nullptr) {
        Logger::E(TAG, "Find .metadata section of \"%s\" file failed.", compFile.string());
        free(shdrs);
        free(strTable);
        fclose(fd);
        *compPath = nullptr;
        return E_COMPONENT_IO_EXCEPTION;
    }

    if (fseek(fd, mdSec->sh_offset + sizeof(int), SEEK_SET) < 0) {
        Logger::E(TAG, "Seek \"%s\" file failed.", compFile.string());
        free(shdrs);
        free(strTable);
        fclose(fd);
        *compPath = nullptr;
        return E_COMPONENT_IO_EXCEPTION;
    }

    free(shdrs);
    free(strTable);

    MetaComponent metadata;

    if (fread((void*)&metadata, sizeof(MetaComponent), 1, fd) < 1) {
        Logger::E(TAG, "Read \"%s\" file failed.", compFile.string());
        fclose(fd);
        *compPath = nullptr;
        return E_COMPONENT_IO_EXCEPTION;
    }

    fclose(fd);

    if (memcmp(&metadata.mUuid, &compId.mUuid, sizeof(Uuid)) == 0) {
        return NOERROR;
    }
    else {
        *compPath = nullptr;
        return E_COMPONENT_NOT_FOUND_EXCEPTION;
    }
}

ECode CBootClassLoader::UnloadComponent(
    /* [in] */ const ComponentID& compId)
{
    Mutex::AutoLock lock(mComponentsLock);
    CMetaComponent* mcObj = (CMetaComponent*)mComponents.Get(compId.mUuid);
    if (mcObj == nullptr) return E_COMPONENT_NOT_FOUND_EXCEPTION;
    Boolean canUnload;
    mcObj->CanUnload(&canUnload);
    if (canUnload) {
        int ret = dlclose(mcObj->mComponent->mSoHandle);
        if (ret == 0) {
            mComponents.Remove(compId.mUuid);
            mComponentPathMap.Remove(mcObj->mUrl);
            return NOERROR;
        }
    }
    return E_COMPONENT_UNLOAD_EXCEPTION;
}

ECode CBootClassLoader::LoadCoclass(
    /* [in] */ const String& fullName,
    /* [out] */ IMetaCoclass** klass)
{
    VALIDATE_NOT_NULL(klass);

    Array<IMetaComponent*> components;
    {
        Mutex::AutoLock lock(mComponentsLock);
        components = mComponents.GetValues();
    }
    for (Integer i = 0; i < components.GetLength(); i++) {
        components[i]->GetCoclass(fullName, klass);
        if (*klass != nullptr) return NOERROR;
    }
    *klass = nullptr;
    return E_CLASS_NOT_FOUND_EXCEPTION;
}

ECode CBootClassLoader::LoadInterface(
    /* [in] */ const String& fullName,
    /* [out] */ IMetaInterface** intf)
{
    VALIDATE_NOT_NULL(intf);

    Array<IMetaComponent*> components;
    {
        Mutex::AutoLock lock(mComponentsLock);
        components = mComponents.GetValues();
    }
    for (Integer i = 0; i < components.GetLength(); i++) {
        components[i]->GetInterface(fullName, intf);
        if (*intf != nullptr) return NOERROR;
    }
    *intf = nullptr;
    return E_INTERFACE_NOT_FOUND_EXCEPTION;
}

ECode CBootClassLoader::GetParent(
    /* [out] */ IClassLoader** parent)
{
    VALIDATE_NOT_NULL(parent);

    *parent = nullptr;
    return NOERROR;
}

void CBootClassLoader::InitComponentPath()
{
    String cpath(getenv("COMPONENT_PATH"));
    if (!cpath.IsNullOrEmpty()) {
        Integer index = cpath.IndexOf(":");
        while (index != -1) {
            mComponentPath.Add(cpath.Substring(0, index - 1));
            cpath = cpath.Substring(index + 1);
            index = cpath.IndexOf(":");
        }
        if (!cpath.IsNullOrEmpty()) mComponentPath.Add(cpath);
    }
    else {
        char* cwd = getcwd(nullptr, 0);
        mComponentPath.Add(String(cwd));
        free(cwd);
    }
}

}
