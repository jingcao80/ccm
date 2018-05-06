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

#include "CDBusChannelFactory.h"
#include "CDBusChannel.h"
#include "util/ccmautoptr.h"

namespace ccm {

CCM_INTERFACE_IMPL_LIGHT_1(CDBusChannelFactory, IRPCChannelFactory);

CDBusChannelFactory::CDBusChannelFactory(
    /* [in] */ RPCType type)
    : mType(type)
{}

ECode CDBusChannelFactory::CreateChannel(
    /* [in] */ RPCPeer peer,
    /* [out] */ IRPCChannel** channel)
{
    VALIDATE_NOT_NULL(channel);

    AutoPtr<CDBusChannel> channelObj = new CDBusChannel();
    ECode ec = channelObj->Initialize(mType, peer);
    if (FAILED(ec)) {
        *channel = nullptr;
        return ec;
    }
    *channel = channelObj;
    REFCOUNT_ADD(*channel);
    return NOERROR;
}

}
