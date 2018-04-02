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

//===-- llvm/Support/ELF.h - ELF constants and data structures --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This header contains common, non-processor-specific data structures and
// constants for the ELF file format.
//
// The details of the ELF32 bits in this file are largely based on the Tool
// Interface Standard (TIS) Executable and Linking Format (ELF) Specification
// Version 1.2, May 1995. The ELF64 stuff is based on ELF-64 Object File Format
// Version 1.5, Draft 2, May 1998 as well as OpenBSD header files.
//
//===----------------------------------------------------------------------===//

#ifndef __CCM_ELF_H__
#define __CCM_ELF_H__

namespace ccm {

typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef int32_t  Elf64_Sword;
typedef uint64_t Elf64_Xword;
typedef int64_t  Elf64_Sxword;

// Object file magic string.
static constexpr char ElfMagic[] = { 0x7f, 'E', 'L', 'F', '\0' };

// e_ident size and indices.
enum {
  EI_MAG0       = 0,          // File identification index.
  EI_MAG1       = 1,          // File identification index.
  EI_MAG2       = 2,          // File identification index.
  EI_MAG3       = 3,          // File identification index.
  EI_CLASS      = 4,          // File class.
  EI_DATA       = 5,          // Data encoding.
  EI_VERSION    = 6,          // File version.
  EI_OSABI      = 7,          // OS/ABI identification.
  EI_ABIVERSION = 8,          // ABI version.
  EI_PAD        = 9,          // Start of padding bytes.
  EI_NIDENT     = 16          // Number of bytes in e_ident.
};

struct Elf64_Ehdr
{
    unsigned char   e_ident[EI_NIDENT]; // ELF Identification bytes
    Elf64_Half      e_type;             // Type of file
    Elf64_Half      e_machine;          // Required architecture for this file
    Elf64_Word      e_version;          // object file version
    Elf64_Addr      e_entry;            // Address to jump to in order to start program
    Elf64_Off       e_phoff;            // Program header table's file offset, in bytes
    Elf64_Off       e_shoff;            // Section header table's file offset, in bytes
    Elf64_Word      e_flags;            // Processor-specific flags
    Elf64_Half      e_ehsize;           // Size of ELF header, in bytes
    Elf64_Half      e_phentsize;        // Size of an entry in the program header table
    Elf64_Half      e_phnum;            // Number of entries in the program header table
    Elf64_Half      e_shentsize;        // Size of an entry in the section header table
    Elf64_Half      e_shnum;            // Number of entries in the section header table
    Elf64_Half      e_shstrndx;         // Sect hdr table index of sect name string table
    bool CheckMagic() const {
        return (memcmp(e_ident, ElfMagic, strlen(ElfMagic))) == 0;
    }
};

// Section header for ELF64 - same fields as ELF32, different types.
struct Elf64_Shdr
{
    Elf64_Word      sh_name;
    Elf64_Word      sh_type;
    Elf64_Xword     sh_flags;
    Elf64_Addr      sh_addr;
    Elf64_Off       sh_offset;
    Elf64_Xword     sh_size;
    Elf64_Word      sh_link;
    Elf64_Word      sh_info;
    Elf64_Xword     sh_addralign;
    Elf64_Xword     sh_entsize;
};

} // namespace ccm;

#endif // __CCM_ELF_H__
