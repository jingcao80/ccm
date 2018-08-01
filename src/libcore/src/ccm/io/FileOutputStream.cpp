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
#include "ccm/io/FileOutputStream.h"
#include "ccm.io.IFile.h"

namespace ccm {
namespace io {

CCM_INTERFACE_IMPL_1(FileOutputStream, OutputStream, IFileOutputStream);

ECode FileOutputStream::Constructor(
    /* [in] */ const String& name)
{
    AutoPtr<IFile> f;
    if (!name.IsNull()) {
        CFile::New(name, IID_IFile, (IInterface**)&f);
    }
    return Constructor(f, false);
}

ECode FileOutputStream::Constructor(
    /* [in] */ const String& name,
    /* [in] */ Boolean append)
{
    AutoPtr<IFile> f;
    if (!name.IsNull()) {
        CFile::New(name, IID_IFile, (IInterface**)&f);
    }
    return Constructor(f, append);
}

ECode FileOutputStream::Constructor(
    /* [in] */ IFile* file)
{
    return Constructor(file, false);
}

ECode FileOutputStream::Constructor(
    /* [in] */ IFile* file,
    /* [in] */ Boolean append)
{
    return NOERROR;
}

ECode FileOutputStream::Constructor(
    /* [in] */ IFileDescriptor* fdObj)
{
    return NOERROR;
}

ECode FileOutputStream::Constructor(
    /* [in] */ IFileDescriptor* fdObj,
    /* [in] */ Boolean isFdOwner)
{
    return NOERROR;
}

ECode FileOutputStream::Close()
{
    return NOERROR;
}

ECode FileOutputStream::Flush()
{
    return NOERROR;
}

ECode FileOutputStream::Write(
    /* [in] */ Integer byte)
{
    return NOERROR;
}

ECode FileOutputStream::Write(
    /* [in] */ const Array<Byte>& buffer)
{
    return NOERROR;
}

ECode FileOutputStream::Write(
    /* [in] */ const Array<Byte>& buffer,
    /* [in] */ Integer offset,
    /* [in] */ Integer size)
{
    return NOERROR;
}

}
}
