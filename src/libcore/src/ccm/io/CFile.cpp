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

#include "ccm/io/CFile.h"

namespace ccm {
namespace io {

CCM_INTERFACE_IMPL_1(CFile, SyncObject, IFile);
CCM_OBJECT_IMPL(CFile);

ECode CFile::Constructor(
    /* [in] */ const String& pathname)
{
    return NOERROR;
}

ECode CFile::Exists(
    /* [out] */ Boolean* existed)
{
    return NOERROR;
}

ECode CFile::GetName(
    /* [out] */ String* name)
{
    return NOERROR;
}

ECode CFile::GetPath(
    /* [out] */ String* path)
{
    return NOERROR;
}

ECode CFile::GetPrefixLength(
    /* [out] */ Integer* length)
{
    return NOERROR;
}

ECode CFile::IsDirectory(
    /* [out] */ Boolean* directory)
{
    return NOERROR;
}

}
}
