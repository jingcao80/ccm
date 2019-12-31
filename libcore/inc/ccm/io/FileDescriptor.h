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

#ifndef __CCM_IO_FILEDESCRIPTOR_H__
#define __CCM_IO_FILEDESCRIPTOR_H__

#include "ccm/core/SyncObject.h"
#include "ccm.io.IFileDescriptor.h"
#include <ccmautoptr.h>

using ccm::core::SyncObject;

namespace ccm {
namespace io {

class FileDescriptor
    : public SyncObject
    , public IFileDescriptor
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor();

    ECode Constructor(
        /* [in] */ Integer descriptor);

    static AutoPtr<IFileDescriptor> GetIN();

    static AutoPtr<IFileDescriptor> GetOUT();

    static AutoPtr<IFileDescriptor> GetERR();

    ECode Valid(
        /* [out] */ Boolean* valid) override;

    ECode Sync() override;

    ECode GetInt(
        /* [out] */ Integer* fd) override;

    ECode SetInt(
        /* [in] */ Integer fd) override;

    ECode IsSocket(
        /* [out] */ Boolean* socket) override;

private:
    static AutoPtr<IFileDescriptor> DupFd(
        /* [in] */ Integer fd);

private:
    Integer mDescriptor;
};

}
}

#endif // __CCM_IO_FILEDESCRIPTOR_H__
