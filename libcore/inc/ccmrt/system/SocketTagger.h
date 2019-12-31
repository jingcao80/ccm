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

#ifndef __CCMRT_SYSTEM_SOCKETTAGGER_H__
#define __CCMRT_SYSTEM_SOCKETTAGGER_H__

#include "ccm/core/SyncObject.h"
#include "ccm.net.IDatagramSocket.h"
#include "ccm.net.ISocket.h"
#include "ccmrt.system.ISocketTagger.h"
#include <ccmautoptr.h>

using ccm::core::SyncObject;
using ccm::net::IDatagramSocket;
using ccm::net::ISocket;

namespace ccmrt {
namespace system {

class SocketTagger
    : public SyncObject
    , public ISocketTagger
{
public:
    CCM_INTERFACE_DECL();

    ECode Tag(
        /* [in] */ ISocket* socket) override final;

    ECode Untag(
        /* [in] */ ISocket* socket) override final;

    ECode Tag(
        /* [in] */ IDatagramSocket* socket) override final;

    ECode Untag(
        /* [in] */ IDatagramSocket* socket) override final;

    using ISocketTagger::Tag;
    using ISocketTagger::Untag;

    static ECode Set(
        /* [in] */ ISocketTagger* tagger);

    static AutoPtr<ISocketTagger> Get();

private:
    static AutoPtr<ISocketTagger> GetOrSet(
        /* [in] */ ISocketTagger* tagger);
};

inline AutoPtr<ISocketTagger> SocketTagger::Get()
{
    return GetOrSet(nullptr);
}

}
}

#endif // __CCMRT_SYSTEM_SOCKETTAGGER_H__
