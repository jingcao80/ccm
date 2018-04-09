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

#include "elf.h"
#include "Logger.h"
#include "MetadataUtils.h"
#include "../../runtime/metadata/Component.h"

#include <stdio.h>
#include <stdlib.h>

using ccm::metadata::MetaComponent;

namespace ccdl {

void* MetadataUtils::ReadMetadataFromElf64(
    /* [in] */ const String& file)
{
    const char* comPath = file.string();

    if (comPath == nullptr || comPath[0] == '\0') {
        Logger::E("MetadataUtils", "The path of component is null or empty.");
        return nullptr;
    }

    FILE* fd = fopen(comPath, "rb");
    if (fd == nullptr) {
        Logger::E("MetadataUtils", "Open \"%s\" file failed.", comPath);
        return nullptr;
    }

    if (fseek(fd, 0, SEEK_SET) == -1) {
        Logger::E("MetadataUtils", "Seek \"%s\" file failed.", comPath);
        return nullptr;
    }

    Elf64_Ehdr ehdr;

    if (fread((void *)&ehdr, sizeof(Elf64_Ehdr), 1, fd) < 1) {
        Logger::E("MetadataUtils", "Read \"%s\" file failed.", comPath);
        return nullptr;
    }

    if (fseek(fd, ehdr.e_shoff, SEEK_SET) == -1) {
        Logger::E("MetadataUtils", "Seek \"%s\" file failed.", comPath);
        return nullptr;
    }

    Elf64_Shdr* shdrs = (Elf64_Shdr *)malloc(sizeof(Elf64_Shdr) * ehdr.e_shnum);
    if (shdrs == nullptr) {
        Logger::E("MetadataUtils", "Malloc Elf64_Shdr failed.");
        return nullptr;
    }

    if (fread((void*)shdrs, sizeof(Elf64_Shdr), ehdr.e_shnum, fd) < ehdr.e_shnum) {
        Logger::E("MetadataUtils", "Read \"%s\" file failed.", comPath);
        free(shdrs);
        return nullptr;
    }

    Elf64_Shdr* strShdr = shdrs + ehdr.e_shstrndx;
    char* strTable = (char *)malloc(strShdr->sh_size);
    if (strTable == nullptr) {
        Logger::E("MetadataUtils", "Malloc string table failed.");
        free(shdrs);
        return nullptr;
    }

    if (fseek(fd, strShdr->sh_offset, SEEK_SET) == -1) {
        Logger::E("MetadataUtils", "Seek \"%s\" file failed.", comPath);
        free(shdrs);
        free(strTable);
        return nullptr;
    }

    if (fread((void*)strTable, 1, strShdr->sh_size, fd) < strShdr->sh_size) {
        Logger::E("MetadataUtils", "Read \"%s\" file failed.", comPath);
        free(shdrs);
        free(strTable);
        return nullptr;
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
        Logger::E("MetadataUtils", "Find .metadata section of \"%s\" file failed.", comPath);
        free(shdrs);
        free(strTable);
        return nullptr;
    }

    if (fseek(fd, mdSec->sh_offset + sizeof(int), SEEK_SET) < 0) {
        Logger::E("MetadataUtils", "Seek \"%s\" file failed.", comPath);
        free(shdrs);
        free(strTable);
        return nullptr;
    }

    free(strTable);

    MetaComponent metadata;

    if (fread((void*)&metadata, sizeof(MetaComponent), 1, fd) < 1) {
        Logger::E("MetadataUtils", "Read \"%s\" file failed.", comPath);
        free(shdrs);
        return nullptr;
    }

    if (metadata.mMagic != CCM_MAGIC || metadata.mSize < 0) {
        Logger::E("MetadataUtils", ".metadata section in \"%s\" file is bad.", comPath);
        free(shdrs);
        return nullptr;
    }

    if (fseek(fd, mdSec->sh_offset + sizeof(int), SEEK_SET) < 0) {
        Logger::E("MetadataUtils", "Seek \"%s\" file failed.", comPath);
        free(shdrs);
        return nullptr;
    }

    free(shdrs);

    void* data = malloc(metadata.mSize);
    if (data == nullptr) {
        Logger::E("MetadataUtils", "Malloc metadata buffer failed.");
        return nullptr;
    }
    if (fread(data, metadata.mSize, 1, fd) < 1) {
        Logger::E("MetadataUtils", "Read metadata section of \"%s\" file failed.", comPath);
        return nullptr;
    }

    return data;
}

void* MetadataUtils::ReadMetadataFromFile(
    /* [in] */ const String& file)
{
    const char* comPath = file.string();

    if (comPath == nullptr || comPath[0] == '\0') {
        Logger::E("MetadataUtils", "The path of component is null or empty.");
        return nullptr;
    }

    FILE* fd = fopen(comPath, "rb");
    if (fd == nullptr) {
        Logger::E("MetadataUtils", "Open \"%s\" file failed.", comPath);
        return nullptr;
    }

    if (fseek(fd, 0, SEEK_SET) == -1) {
        Logger::E("MetadataUtils", "Seek \"%s\" file failed.", comPath);
        return nullptr;
    }

    MetaComponent metadata;

    if (fread((void*)&metadata, sizeof(MetaComponent), 1, fd) < 1) {
        Logger::E("MetadataUtils", "Read \"%s\" file failed.", comPath);
        return nullptr;
    }

    if (metadata.mMagic != CCM_MAGIC || metadata.mSize < 0) {
        Logger::E("MetadataUtils", ".metadata section in \"%s\" file is bad.", comPath);
        return nullptr;
    }

    if (fseek(fd, 0, SEEK_SET) == -1) {
        Logger::E("MetadataUtils", "Seek \"%s\" file failed.", comPath);
        return nullptr;
    }

    void* data = malloc(metadata.mSize);
    if (data == nullptr) {
        Logger::E("MetadataUtils", "Malloc metadata buffer failed.");
        return nullptr;
    }
    if (fread(data, metadata.mSize, 1, fd) < 1) {
        Logger::E("MetadataUtils", "Read metadata section of \"%s\" file failed.", comPath);
        return nullptr;
    }

    return data;
}

}
