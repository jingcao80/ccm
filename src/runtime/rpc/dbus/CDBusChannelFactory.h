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

#ifndef __CCM_CDBUSCHANNELFACTORY_H__
#define __CCM_CDBUSCHANNELFACTORY_H__

#include "ccmrefbase.h"

namespace ccm {

class CDBusChannelFactory
    : public LightRefBase
    , public IRPCChannelFactory
{
public:
    CDBusChannelFactory(
        /* [in] */ RPCType type);

    CCM_INTERFACE_DECL();

    ECode CreateInterfacePack(
        /* [out] */ IInterfacePack** ipack) override;

    ECode CreateParcel(
        /* [out] */ IParcel** parcel) override;

    ECode CreateChannel(
        /* [in] */ RPCPeer peer,
        /* [out] */ IRPCChannel** channel);

    ECode MarshalInterface(
        /* [in] */ IInterface* object,
        /* [out] */ IInterfacePack** ipack) override;

    ECode UnmarshalInterface(
        /* [in] */ IInterfacePack* ipack,
        /* [out] */ IInterface** object) override;

private:
    RPCType mType;
};

}

#endif // __CCM_CDBUSCHANNELFACTORY_H__
