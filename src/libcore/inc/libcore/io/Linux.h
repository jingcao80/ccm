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

#ifndef __LIBCORE_IO_LINUX_H__
#define __LIBCORE_IO_LINUX_H__

#include "ccm/core/SyncObject.h"
#include "ccm.io.IFileDescriptor.h"
#include "ccm.net.ISocketAddress.h"
#include "libcore.io.IOs.h"

using ccm::core::SyncObject;
using ccm::io::IFileDescriptor;
using ccm::net::ISocketAddress;

namespace libcore {
namespace io {

class Linux
    : public SyncObject
    , public IOs
{
public:
    Linux();

    ECode Accept(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ISocketAddress* peerAddress,
        /* [out] */ IFileDescriptor** retFd) override;

    ECode Access(
        /* [in] */ const String& path,
        /* [in] */ Integer mode,
        /* [out] */ Boolean* result) override;
};

inline Linux::Linux()
{}

}
}

#endif // __LIBCORE_IO_LINUX_H__
