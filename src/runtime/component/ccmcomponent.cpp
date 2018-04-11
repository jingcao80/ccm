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
#include "ccmerror.h"
#include "metadata/Component.h"
#include "util/arraylist.h"
#include "util/ccmlogger.h"
#include "util/elf.h"
#include "util/hashmap.h"
#include "util/mutex.h"

#include <dlfcn.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>

#include <unistd.h>

using ccm::metadata::MetaComponent;

namespace ccm {

template<>
void DeleteValueImpl<CcmComponent*>(
    /* [in, out] */ CcmComponent** value)
{
    if (*value != nullptr) {
        free(*value);
        *value = nullptr;
    }
}

static INIT_PROI_1 HashMap<Uuid, CcmComponent*> sCcmComponents;
static INIT_PROI_1 Mutex sCcmComponentsLock;

static INIT_PROI_1 ArrayList<String> sCcmComponentSearchPaths;
static Boolean DebugComponentAPI = false;

void InitCompSearchPaths()
{
    char* cwd = getcwd(nullptr, 0);
    sCcmComponentSearchPaths.Add(String(cwd));
}

static CcmComponent* CoFindComponent(
    /* [in] */ const Uuid& compId)
{
    Mutex::AutoLock lock(sCcmComponentsLock);
    CcmComponent* ccmComp = sCcmComponents.Get(compId);
    return ccmComp;
}

static bool CheckComponentID(
    /* [in] */ const ComponentID& compId,
    /* [out] */ String* compPath);

static CcmComponent* CoLoadComponent(
    /* [in] */ const ComponentID& compId)
{
    String compPath;
    if (!CheckComponentID(compId, &compPath)) {
        return nullptr;
    }

    void* handle = dlopen(compPath.string(), RTLD_NOW);
    if (handle == nullptr) {
        Logger::E("CCMRT", "Dlopen \"%s\" failed. The reason is %s.",
                compPath.string(), strerror(errno));
        return nullptr;
    }

    GetClassObjectPtr func = (GetClassObjectPtr)dlsym(handle, "soGetClassObject");
    if (func == nullptr) {
        Logger::E("CCMRT", "Dlsym \"soGetClassObject\" function from \"%s\" \
                component failed. The reason is %s.", compPath.string(), strerror(errno));
        return nullptr;
    }

    CcmComponent* ccmComp = (CcmComponent*)malloc(sizeof(CcmComponent));
    if (ccmComp == nullptr) {
        Logger::E("CCMRT", "Malloc CcmComponent structure failed.");
        return nullptr;
    }

    ccmComp->mSoHandle = handle;
    ccmComp->mSoGetClassObject = func;
    {
        Mutex::AutoLock lock(sCcmComponentsLock);
        sCcmComponents.Put(compId.mUuid, ccmComp);
    }

    return ccmComp;
}

ECode CoGetComponent(
    /* [in] */ const ComponentID& compId,
    /* [out] */ CcmComponent** component)
{
    CcmComponent* ccmComp = CoFindComponent(compId.mUuid);
    if (ccmComp != nullptr) {
        *component = ccmComp;
        return NOERROR;
    }

    ccmComp = CoLoadComponent(compId);
    *component = ccmComp;
    return ccmComp != nullptr ? NOERROR : E_COMPONENT_NOT_FOUND_EXCEPTION;
}

static bool CheckComponentID(
    /* [in] */ const ComponentID& compId,
    /* [out] */ String* compPath)
{
    String url(compId.mUrl);
    *compPath = nullptr;
    if (DebugComponentAPI) {
        Logger::D("CCMRT", "The url of the component which will be loaded is \"%s\".",
                url.string());
    }

    Integer index = url.LastIndexOf("/");
    String compFile = index != -1 ? url.Substring(index + 1) : url;
    if (compFile.IsNullOrEmpty()) {
        Logger::E("CCMRT", "The name of component is null or empty.");
        return false;
    }

    FILE* fd = nullptr;
    for (Long i = 0; i < sCcmComponentSearchPaths.GetSize(); i++) {
        String filePath = sCcmComponentSearchPaths.Get(i) + "/" + compFile;
        fd = fopen(filePath.string(), "rb");
        if (fd != nullptr) {
            if (DebugComponentAPI) {
                Logger::D("CCMRT", "Find \"%\" component in directory \"%s\".",
                        compFile.string(), sCcmComponentSearchPaths.Get(i).string());
            }
            *compPath = filePath;
            break;
        }
    }
    if (fd == nullptr) {
        Logger::E("CCMRT", "Cannot find \"%s\" component.", compFile.string());
        return false;
    }

    if (fseek(fd, 0, SEEK_SET) == -1) {
        Logger::E("CCMRT", "Seek \"%s\" file failed.", compFile.string());
        return false;
    }

    Elf64_Ehdr ehdr;

    if (fread((void *)&ehdr, sizeof(Elf64_Ehdr), 1, fd) < 1) {
        Logger::E("CCMRT", "Read \"%s\" file failed.", compFile.string());
        return false;
    }

    if (fseek(fd, ehdr.e_shoff, SEEK_SET) == -1) {
        Logger::E("CCMRT", "Seek \"%s\" file failed.", compFile.string());
        return false;
    }

    Elf64_Shdr* shdrs = (Elf64_Shdr *)malloc(sizeof(Elf64_Shdr) * ehdr.e_shnum);
    if (shdrs == nullptr) {
        Logger::E("CCMRT", "Malloc Elf64_Shdr failed.");
        return false;
    }

    if (fread((void*)shdrs, sizeof(Elf64_Shdr), ehdr.e_shnum, fd) < ehdr.e_shnum) {
        Logger::E("CCMRT", "Read \"%s\" file failed.", compFile.string());
        free(shdrs);
        return false;
    }

    Elf64_Shdr* strShdr = shdrs + ehdr.e_shstrndx;
    char* strTable = (char *)malloc(strShdr->sh_size);
    if (strTable == nullptr) {
        Logger::E("CCMRT", "Malloc string table failed.");
        free(shdrs);
        return false;
    }

    if (fseek(fd, strShdr->sh_offset, SEEK_SET) == -1) {
        Logger::E("CCMRT", "Seek \"%s\" file failed.", compFile.string());
        free(shdrs);
        free(strTable);
        return false;
    }

    if (fread((void*)strTable, 1, strShdr->sh_size, fd) < strShdr->sh_size) {
        Logger::E("CCMRT", "Read \"%s\" file failed.", compFile.string());
        free(shdrs);
        free(strTable);
        return false;
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
        Logger::E("CCMRT", "Find .metadata section of \"%s\" file failed.", compFile.string());
        free(shdrs);
        free(strTable);
        return false;
    }

    if (fseek(fd, mdSec->sh_offset + sizeof(int), SEEK_SET) < 0) {
        Logger::E("CCMRT", "Seek \"%s\" file failed.", compFile.string());
        free(shdrs);
        free(strTable);
        return false;
    }

    free(shdrs);
    free(strTable);

    MetaComponent metadata;

    if (fread((void*)&metadata, sizeof(MetaComponent), 1, fd) < 1) {
        Logger::E("CCMRT", "Read \"%s\" file failed.", compFile.string());
        return false;
    }

    return memcmp(&metadata.mUuid, &compId.mUuid, sizeof(Uuid)) == 0;
}

} // namespace ccm
