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

#ifndef __LIBCORE_IO_BLOCKGUARDOS_H__
#define __LIBCORE_IO_BLOCKGUARDOS_H__

#include "libcore/io/ForwardingOs.h"

namespace libcore {
namespace io {

class BlockGuardOs
    : public ForwardingOs
{
public:
    BlockGuardOs(
        /* [in] */ IOs* os);

    ECode Accept(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ISocketAddress* peerAddress,
        /* [out] */ IFileDescriptor** retFd) override;

    ECode Access(
        /* [in] */ const String& path,
        /* [in] */ Integer mode,
        /* [out] */ Boolean* result) override;

    ECode Chmod(
        /* [in] */ const String& path,
        /* [in] */ Integer mode) override;

    ECode Chown(
        /* [in] */ const String& path,
        /* [in] */ Integer uid,
        /* [in] */ Integer gid) override;

    ECode Close(
        /* [in] */ IFileDescriptor* fd) override;

private:
    ECode TagSocket(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ IFileDescriptor** taggedFd);

    ECode UntagSocket(
        /* [in] */ IFileDescriptor* fd);

    static Boolean IsInetSocket(
        /* [in] */ IFileDescriptor* fd);

    static Boolean IsInetDomain(
        /* [in] */ Integer domain);

    static Boolean IsLingerSocket(
        /* [in] */ IFileDescriptor* fd);
};

inline BlockGuardOs::BlockGuardOs(
    /* [in] */ IOs* os)
    : ForwardingOs(os)
{}

}
}

#endif // __LIBCORE_IO_BLOCKGUARDOS_H__
