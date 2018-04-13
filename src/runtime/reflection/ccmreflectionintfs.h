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

extern const InterfaceID IID_IMetaCoclass;
extern const InterfaceID IID_IMetaComponent;
extern const InterfaceID IID_IMetaConstructor;
extern const InterfaceID IID_IMetaEnumeration;
extern const InterfaceID IID_IMetaEnumerator;
extern const InterfaceID IID_IMetaInterface;

interface IMetaCoclass;
interface IMetaConstructor;
interface IMetaEnumeration;
interface IMetaEnumerator;
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

    virtual ECode GetCoclass(
        /* [in] */ const String& fullName,
        /* [out] */ IMetaCoclass** metaKls) = 0;

    virtual ECode GetInterfaceNumber(
        /* [out] */ Integer* number) = 0;

    virtual ECode GetAllInterfaces(
        /* [out] */ Array<IMetaInterface*>& intfs) = 0;

    virtual ECode GetInterface(
        /* [in] */ const String& fullName,
        /* [out] */ IMetaInterface** metaIntf) = 0;
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

    virtual ECode GetMetaComponent(
        /* [out] */ IMetaComponent** metaComp) = 0;

    virtual ECode GetName(
        /* [out] */ String* name) = 0;

    virtual ECode GetNamespace(
        /* [out] */ String* ns) = 0;

    virtual ECode GetCoclassID(
        /* [out] */ CoclassID* cid) = 0;

    virtual ECode GetConstructorNumber(
        /* [out] */ Integer* number) = 0;

    virtual ECode GetAllConstructors(
        /* [out] */ Array<IMetaConstructor*>& constrs) = 0;

    virtual ECode GetConstructors(
        /* [in] */ Integer paramNumber,
        /* [out] */ IMetaConstructor** constr) = 0;

    virtual ECode GetInterfaceNumber(
        /* [out] */ Integer* number) = 0;

    virtual ECode GetAllInterfaces(
        /* [out] */ Array<IMetaInterface*>& intfs) = 0;

    virtual ECode CreateObject(
        /* [out] */ IInterface** object) = 0;

    virtual ECode CreateObject(
        /* [in] */ const InterfaceID& iid,
        /* [out] */ IInterface** object) = 0;
};

INTERFACE_ID(ba7707a3-e2f8-4318-8559-f12940f02b2e)
interface IMetaConstructor : public IInterface
{

};

INTERFACE_ID(f23514ea-5b7f-4a16-85da-102decb85d38)
interface IMetaEnumeration : public IInterface
{
    inline static IMetaEnumeration* Probe(
        /* [in] */ IInterface* object)
    {
        if (object == NULL) return NULL;
        return (IMetaEnumeration*)object->Probe(IID_IMetaEnumeration);
    }

    virtual ECode GetMetaComponent(
        /* [out] */ IMetaComponent** metaComp) = 0;

    virtual ECode GetName(
        /* [out] */ String* name) = 0;

    virtual ECode GetNamespace(
        /* [out] */ String* ns) = 0;

    virtual ECode GetEnumeratorNumber(
        /* [out] */ Integer* number) = 0;

    virtual ECode GetAllEnumerators(
        /* [out] */ Array<IMetaEnumerator*> enumrs) = 0;
};

INTERFACE_ID(f508e549-ba3f-4a6e-ad4b-b8bdd3c3fe73)
interface IMetaEnumerator : public IInterface
{

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

    virtual ECode GetMetaComponent(
        /* [out] */ IMetaComponent** metaComp) = 0;

    virtual ECode GetName(
        /* [out] */ String* name) = 0;

    virtual ECode GetNamespace(
        /* [out] */ String* ns) = 0;

    virtual ECode GetInterfaceID(
        /* [out] */ InterfaceID* iid) = 0;
};

}

#endif // __CCM_CCMREFLECTIONINTFS_H__
