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

#include "registry.h"
#include "util/hashmap.h"
#include "util/mutex.h"

namespace ccm {

template<>
struct HashFunc<IObject*>
{
    inline Integer operator()(
        /* [in] */ IObject* data)
    {
        Integer hash;
        data->GetHashCode(&hash);
        return hash;
    }
};

template<>
struct HashFunc<IInterfacePack*>
{
    inline Integer operator()(
        /* [in] */ IInterfacePack* data)
    {
        Integer hash;
        data->GetHashCode(&hash);
        return hash;
    }
};

static HashMap<IObject*, IStub*> sLocalExportRegistry;
static Mutex sLocalExportRegistryLock;
static HashMap<IObject*, IStub*> sRemoteExportRegistry;
static Mutex sRemoteExportRegistryLock;

static HashMap<IInterfacePack*, IObject*> sLocalImportRegistry;
static Mutex sLocalImportRegistryLock;
static HashMap<IInterfacePack*, IObject*> sRemoteImportRegistry;
static Mutex sRemoteImportRegistryLock;

ECode RegisterExportObject(
    /* [in] */ RPCType type,
    /* [in] */ IObject* object,
    /* [in] */ IStub* stub)
{
    if (object == nullptr || stub == nullptr) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    HashMap<IObject*, IStub*>& registry = type == RPCType::Local ?
            sLocalExportRegistry : sRemoteExportRegistry;
    Mutex& registryLock = type == RPCType::Local ?
            sLocalExportRegistryLock : sRemoteExportRegistryLock;

    Mutex::AutoLock lock(registryLock);
    registry.Put(object, stub);
    return NOERROR;
}

ECode UnregisterExportObject(
    /* [in] */ RPCType type,
    /* [in] */ IObject* object)
{
    if (object == nullptr) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    HashMap<IObject*, IStub*>& registry = type == RPCType::Local ?
            sLocalExportRegistry : sRemoteExportRegistry;
    Mutex& registryLock = type == RPCType::Local ?
            sLocalExportRegistryLock : sRemoteExportRegistryLock;

    Mutex::AutoLock lock(registryLock);
    if (registry.ContainsKey(object)) {
        registry.Remove(object);
        return NOERROR;
    }
    return E_NOT_FOUND_EXCEPTION;
}

ECode FindExportObject(
    /* [in] */ RPCType type,
    /* [in] */ IObject* object,
    /* [out] */ IStub** stub)
{
    VALIDATE_NOT_NULL(stub);

    if (object == nullptr) {
        *stub = nullptr;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    HashMap<IObject*, IStub*>& registry = type == RPCType::Local ?
            sLocalExportRegistry : sRemoteExportRegistry;
    Mutex& registryLock = type == RPCType::Local ?
            sLocalExportRegistryLock : sRemoteExportRegistryLock;

    Mutex::AutoLock lock(registryLock);
    if (registry.ContainsKey(object)) {
        *stub = registry.Get(object);
        REFCOUNT_ADD(*stub);
        return NOERROR;
    }
    *stub = nullptr;
    return E_NOT_FOUND_EXCEPTION;
}

ECode FindExportObject(
    /* [in] */ RPCType type,
    /* [in] */ IInterfacePack* ipack,
    /* [out] */ IStub** stub)
{
    VALIDATE_NOT_NULL(stub);

    if (ipack == nullptr) {
        *stub = nullptr;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    HashMap<IObject*, IStub*>& registry = type == RPCType::Local ?
            sLocalExportRegistry : sRemoteExportRegistry;
    Mutex& registryLock = type == RPCType::Local ?
            sLocalExportRegistryLock : sRemoteExportRegistryLock;

    Mutex::AutoLock lock(registryLock);
    Array<IStub*> stubs = registry.GetValues();
    for (Long i = 0; i < stubs.GetLength(); i++) {
        IStub* stubObj = stubs[i];
        Boolean matched;
        stubObj->Match(ipack, &matched);
        if (matched) {
            *stub = stubObj;
            REFCOUNT_ADD(*stub);
            return NOERROR;
        }
    }
    *stub = nullptr;
    return E_NOT_FOUND_EXCEPTION;
}

ECode RegisterImportObject(
    /* [in] */ RPCType type,
    /* [in] */ IInterfacePack* ipack,
    /* [in] */ IObject* object)
{
    if (ipack == nullptr || object == nullptr) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    HashMap<IInterfacePack*, IObject*>& registry = type == RPCType::Local ?
            sLocalImportRegistry : sRemoteImportRegistry;
    Mutex& registryLock = type == RPCType::Local ?
            sLocalImportRegistryLock : sRemoteImportRegistryLock;

    Mutex::AutoLock lock(registryLock);
    registry.Put(ipack, object);
    return NOERROR;
}

ECode UnregisterImportObject(
    /* [in] */ RPCType type,
    /* [in] */ IInterfacePack* ipack)
{
    if (ipack == nullptr) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    HashMap<IInterfacePack*, IObject*>& registry = type == RPCType::Local ?
            sLocalImportRegistry : sRemoteImportRegistry;
    Mutex& registryLock = type == RPCType::Local ?
            sLocalImportRegistryLock : sRemoteImportRegistryLock;

    Mutex::AutoLock lock(registryLock);
    if (registry.ContainsKey(ipack)) {
        registry.Remove(ipack);
        return NOERROR;
    }
    return E_NOT_FOUND_EXCEPTION;
}

ECode FindImportObject(
    /* [in] */ RPCType type,
    /* [in] */ IInterfacePack* ipack,
    /* [out] */ IObject** object)
{
    VALIDATE_NOT_NULL(object);

    if (ipack == nullptr) {
        *object = nullptr;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    HashMap<IInterfacePack*, IObject*>& registry = type == RPCType::Local ?
            sLocalImportRegistry : sRemoteImportRegistry;
    Mutex& registryLock = type == RPCType::Local ?
            sLocalImportRegistryLock : sRemoteImportRegistryLock;

    Mutex::AutoLock lock(registryLock);
    if (registry.ContainsKey(ipack)) {
        *object = registry.Get(ipack);
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    *object = nullptr;
    return E_NOT_FOUND_EXCEPTION;
}

}
