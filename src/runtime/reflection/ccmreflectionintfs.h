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

#ifndef __CCM_CCMREFLECTIONINTFS_H__
#define __CCM_CCMREFLECTIONINTFS_H__

#include "ccmtypes.h"

namespace ccm {

extern const InterfaceID IID_IMetaComponent;
extern const InterfaceID IID_IMetaCoclass;
extern const InterfaceID IID_IMetaInterface;

interface IMetaCoclass;
interface IMetaInterface;

INTERFACE_ID(35b4d7ca-b66c-44d3-b4a6-007852953085)
interface IMetaComponent : public IInterface
{
    inline static IMetaComponent* Probe(
        /* [in] */ IInterface* object)
    {
        if (object == NULL) return NULL;
        return (IMetaComponent*)object->Probe(IID_IMetaComponent);
    }

    virtual ECode GetName(
        /* [ou] */ String* name) = 0;

    virtual ECode GetComponentID(
        /* [out] */ ComponentID* cid) = 0;

    virtual ECode GetCoclassNumber(
        /* [out] */ Integer* number) = 0;

    virtual ECode GetAllCoclasses(
        /* [out] */ Array<IMetaCoclass*>& klasses) = 0;

    virtual ECode GetInterfaceNumber(
        /* [out] */ Integer* number) = 0;

    virtual ECode GetAllInterfaces(
        /* [out] */ Array<IMetaInterface*>& intfs) = 0;
};

INTERFACE_ID(8fdbd639-39be-4e9f-801d-8a3b137d7c1f)
interface IMetaCoclass : public IInterface
{
    inline static IMetaCoclass* Probe(
        /* [in] */ IInterface* object)
    {
        if (object == NULL) return NULL;
        return (IMetaCoclass*)object->Probe(IID_IMetaCoclass);
    }

    virtual ECode GetName(
        /* [out] */ String* name) = 0;

    virtual ECode GetNamespace(
        /* [out] */ String* ns) = 0;

    virtual ECode GetCoclassID(
        /* [out] */ CoclassID* cid) = 0;

    virtual ECode GetInterfaceNumber(
        /* [out] */ Integer* number) = 0;

    virtual ECode GetAllInterfaces(
        /* [out] */ Array<IMetaInterface*>& intfs) = 0;

    virtual ECode CreateObject(
        /* [out] */ IInterface** object) = 0;
};

INTERFACE_ID(a9f54826-5903-4910-af5f-9703364663b2)
interface IMetaInterface : public IInterface
{
    inline static IMetaInterface* Probe(
        /* [in] */ IInterface* object)
    {
        if (object == NULL) return NULL;
        return (IMetaInterface*)object->Probe(IID_IMetaInterface);
    }

    virtual ECode GetName(
        /* [out] */ String* name) = 0;

    virtual ECode GetNamespace(
        /* [out] */ String* ns) = 0;

    virtual ECode GetInterfaceID(
        /* [out] */ InterfaceID* iid) = 0;
};

}

#endif // __CCM_CCMREFLECTIONINTFS_H__
