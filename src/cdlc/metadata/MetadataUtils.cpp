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

#include "metadata/MetadataUtils.h"
#include "util/File.h"
#include "util/Logger.h"
#include "runtime/util/elf.h"
#include <cstdlib>

namespace cdlc {

void* MetadataUtils::ReadMetadata(
    /* [in] */ const String& filePath,
    /* [in] */ int fileType)
{
    switch (fileType) {
        case TYPE_METADATA:
            return ReadMetadataFromFile(filePath);
        case TYPE_SO:
            return ReadMetadataFromElf64(filePath);
        default:
            Logger::E("MetadataUtils", "Do not support read metadata from file \"%s\".",
                    filePath.string());
            return nullptr;
    }
}

void* MetadataUtils::ReadMetadataFromElf64(
    /* [in] */ const String& filePath)
{
    if (filePath.IsEmpty()) {
        Logger::E("MetadataUtils", "The file path is null or empty.");
        return nullptr;
    }

    File file(filePath, File::READ);
    if (!file.IsValid()) {
        Logger::E("MetadataUtils", "Open \"%s\" file failed.", file.GetPath().string());
        return nullptr;
    }

    if (!file.Seek(0, File::SEEK_FROM_BEGIN)) {
        Logger::E("MetadataUtils", "Seek \"%s\" file failed.", filePath.string());
        return nullptr;
    }

    como::Elf64_Ehdr ehdr;

    if (file.Read((void *)&ehdr, sizeof(como::Elf64_Ehdr)) < sizeof(como::Elf64_Ehdr)) {
        Logger::E("MetadataUtils", "Read \"%s\" file failed.", filePath.string());
        return nullptr;
    }

    if (!file.Seek(ehdr.e_shoff, File::SEEK_FROM_BEGIN)) {
        Logger::E("MetadataUtils", "Seek \"%s\" file failed.", filePath.string());
        return nullptr;
    }

    como::Elf64_Shdr* shdrs = (como::Elf64_Shdr *)malloc(sizeof(como::Elf64_Shdr) * ehdr.e_shnum);
    if (shdrs == nullptr) {
        Logger::E("MetadataUtils", "Malloc Elf64_Shdr failed.");
        return nullptr;
    }

    if (file.Read((void*)shdrs, sizeof(como::Elf64_Shdr) * ehdr.e_shnum) <
            sizeof(como::Elf64_Shdr) * ehdr.e_shnum) {
        Logger::E("MetadataUtils", "Read \"%s\" file failed.", filePath.string());
        free(shdrs);
        return nullptr;
    }

    como::Elf64_Shdr* strShdr = shdrs + ehdr.e_shstrndx;
    char* strTable = (char *)malloc(strShdr->sh_size);
    if (strTable == nullptr) {
        Logger::E("MetadataUtils", "Malloc string table failed.");
        free(shdrs);
        return nullptr;
    }

    if (!file.Seek(strShdr->sh_offset, File::SEEK_FROM_BEGIN)) {
        Logger::E("MetadataUtils", "Seek \"%s\" file failed.", filePath.string());
        free(shdrs);
        free(strTable);
        return nullptr;
    }

    if (file.Read((void*)strTable, strShdr->sh_size) < strShdr->sh_size) {
        Logger::E("MetadataUtils", "Read \"%s\" file failed.", filePath.string());
        free(shdrs);
        free(strTable);
        return nullptr;
    }

    como::Elf64_Shdr* mdSec = nullptr;
    for (int i = 0; i < ehdr.e_shnum; i++) {
        const char* secName = strTable + shdrs[i].sh_name;
        if (!strncmp(secName, ".metadata", 9)) {
            mdSec = shdrs + i;
            break;
        }
    }

    if (mdSec == nullptr) {
        Logger::E("MetadataUtils", "Find .metadata section of \"%s\" file failed.", filePath.string());
        free(shdrs);
        free(strTable);
        return nullptr;
    }

    free(strTable);

    como::MetaComponent component;

    if (file.Read(reinterpret_cast<void*>(&component), sizeof(como::MetaComponent))
            < sizeof(como::MetaComponent)) {
        Logger::E("MetadataUtils", "Read \"%s\" file failed.", filePath.string());
        free(shdrs);
        return nullptr;
    }

    if (component.mMagic != COMO_MAGIC) {
        Logger::E("MetadataUtils", ".metadata section in \"%s\" file is bad.", filePath.string());
        free(shdrs);
        return nullptr;
    }

    if (!file.Seek(mdSec->sh_offset + sizeof(int), File::SEEK_FROM_BEGIN)) {
        Logger::E("MetadataUtils", "Seek \"%s\" file failed.", filePath.string());
        free(shdrs);
        return nullptr;
    }

    free(shdrs);

    void* metadata = malloc(component.mSize);
    if (metadata == nullptr) {
        Logger::E("MetadataUtils", "Malloc metadata buffer failed.");
        return nullptr;
    }
    if (file.Read(metadata, component.mSize) < component.mSize) {
        Logger::E("MetadataUtils", "Read .metadata section of \"%s\" file failed.", filePath.string());
        return nullptr;
    }

    return metadata;
}

void* MetadataUtils::ReadMetadataFromFile(
    /* [in] */ const String& filePath)
{
    if (filePath.IsEmpty()) {
        Logger::E("MetadataUtils", "The file path is null or empty.");
        return nullptr;
    }

    File file(filePath, File::READ);
    if (!file.IsValid()) {
        Logger::E("MetadataUtils", "Open \"%s\" file failed.", file.GetPath().string());
        return nullptr;
    }

    if (!file.Seek(0, File::SEEK_FROM_BEGIN)) {
        Logger::E("MetadataUtils", "Seek \"%s\" file failed.", filePath.string());
        return nullptr;
    }

    como::MetaComponent component;

    if (file.Read(reinterpret_cast<void*>(&component), sizeof(como::MetaComponent))
            < sizeof(como::MetaComponent)) {
        Logger::E("MetadataUtils", "Read \"%s\" file failed.", filePath.string());
        return nullptr;
    }

    if (component.mMagic != COMO_MAGIC) {
        Logger::E("MetadataUtils", "Metadata info in \"%s\" file is bad.", filePath.string());
        return nullptr;
    }

    if (!file.Seek(0, File::SEEK_FROM_BEGIN)) {
        Logger::E("MetadataUtils", "Seek \"%s\" file failed.", filePath.string());
        return nullptr;
    }

    void* metadata = malloc(component.mSize);
    if (metadata == nullptr) {
        Logger::E("MetadataUtils", "Malloc metadata buffer failed.");
        return nullptr;
    }
    if (file.Read(metadata, component.mSize) < component.mSize) {
        Logger::E("MetadataUtils", "Read metadata content of \"%s\" file failed.", filePath.string());
        return nullptr;
    }

    return metadata;
}

}

