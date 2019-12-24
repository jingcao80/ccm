//=========================================================================
// Copyright (C) 2018 The C++ Component Model(COMO) Open Source Project
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

#ifndef __COMO_COMORPC_H__
#define __COMO_COMORPC_H__

#include "comointfs.h"

namespace como {

constexpr Integer RPC_MAGIC_NUMBER = 0x7a79636f;

EXTERN_C COM_PUBLIC ECode CoCreateParcel(
    /* [in] */ RPCType type,
    /* [out] */ AutoPtr<IParcel>& parcel);

EXTERN_C COM_PUBLIC ECode CoCreateInterfacePack(
    /* [in] */ RPCType type,
    /* [out] */ AutoPtr<IInterfacePack>& ipack);

EXTERN_C COM_PUBLIC ECode CoCreateProxy(
    /* [in] */ IInterfacePack* ipack,
    /* [in] */ RPCType type,
    /* [in] */ IClassLoader* loader,
    /* [out] */ AutoPtr<IProxy>& proxy);

EXTERN_C COM_PUBLIC ECode CoCreateStub(
    /* [in] */ IInterface* object,
    /* [in] */ RPCType type,
    /* [out] */ AutoPtr<IStub>& stub);

EXTERN_C COM_PUBLIC ECode CoMarshalInterface(
    /* [in] */ IInterface* object,
    /* [in] */ RPCType type,
    /* [out] */ AutoPtr<IInterfacePack>& ipack);

EXTERN_C COM_PUBLIC ECode CoUnmarshalInterface(
    /* [in] */ IInterfacePack* ipack,
    /* [in] */ RPCType type,
    /* [out] */ AutoPtr<IInterface>& object);

} // namespace como

#endif // __COMO_COMORPC_H__
