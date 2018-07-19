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

#ifndef __CCM_IO_CFILE_H__
#define __CCM_IO_CFILE_H__

#include "ccm/core/SyncObject.h"
#include "ccm/io/File.h"
#include "ccm.io.IFile.h"
#include "_ccm_io_CFile.h"

using ccm::core::SyncObject;

namespace ccm {
namespace io {

Coclass(CFile)
    , public SyncObject
    , public IFile
{
public:
    CCM_INTERFACE_DECL();

    CCM_OBJECT_DECL();

    ECode Constructor(
        /* [in] */ const String& pathname);

    ECode Exists(
        /* [out] */ Boolean* existed) override;

    ECode GetName(
        /* [out] */ String* name) override;

    ECode GetPath(
        /* [out] */ String* path) override;

    ECode GetPrefixLength(
        /* [out] */ Integer* length) override;

    ECode IsDirectory(
        /* [out] */ Boolean* directory) override;
};

}
}

#endif // __CCM_IO_CFILE_H__
