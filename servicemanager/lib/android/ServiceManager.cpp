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

#include "mutex.h"
#include "ServiceManager.h"
#include <comolog.h>
#include <binder/IServiceManager.h>
#include <binder/Parcel.h>

namespace jing {

const char* ServiceManager::NAME = "jing.servicemanager";

AutoPtr<ServiceManager> ServiceManager::sInstance = new ServiceManager();

AutoPtr<ServiceManager> ServiceManager::GetInstance()
{
    return sInstance;
}

static android::sp<android::IBinder> get_service_manager()
{
    static android::sp<android::IBinder> sBinder;
    static Mutex sBinderLock;

    if (sBinder != nullptr) {
        return sBinder;
    }

    Logger::D("ServiceManager", "jing servicemanager try getting...\n");

    Mutex::AutoLock lock(sBinderLock);
    if (sBinder != nullptr) {
        return sBinder;
    }
    android::sp<android::IServiceManager> sm = android::defaultServiceManager();
    do {
        sBinder = sm->getService(android::String16(ServiceManager::NAME));
        if (sBinder != nullptr){
            break;
        }
        usleep(500000);
    } while (true);

    Logger::D("ServiceManager", "jing servicemanager gotten.\n");

    return sBinder;
}

ECode ServiceManager::AddService(
    /* [in] */ const String& name,
    /* [in] */ IInterface* object)
{
    if (name.IsEmpty() || object == nullptr) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInterfacePack> ipack;
    ECode ec = CoMarshalInterface(object, RPCType::Local, ipack);
    if (FAILED(ec)) {
        Logger::E("ServiceManager", "Marshal the interface which named \"%s\" failed.",
                name.string());
        return ec;
    }

    AutoPtr<IParcel> parcel;
    CoCreateParcel(RPCType::Local, parcel);
    parcel->WriteString(name);
    IParcelable::Probe(ipack)->WriteToParcel(parcel);
    HANDLE data;
    parcel->GetPayload(data);

    android::Parcel reply;
    if (get_service_manager()->transact(ADD_SERVICE,
            *reinterpret_cast<android::Parcel*>(data), &reply) != android::NO_ERROR) {
        Logger::E("ServiceManager", "AddService failed.");
        return E_REMOTE_EXCEPTION;
    }

    ec = reply.readInt32();
    if (FAILED(ec)) {
        Logger::E("ServiceManager", "Remote call failed with ec = 0x%x.", ec);
        return ec;
    }

    return NOERROR;
}

ECode ServiceManager::GetService(
    /* [in] */ const String& name,
    /* [out] */ AutoPtr<IInterface>& object)
{
    if (name.IsEmpty()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    android::Parcel data, reply;

    data.writeInt32(TAG_NOT_NULL);
    data.writeCString(name.string());
    if (get_service_manager()->transact(GET_SERVICE, data, &reply) != android::NO_ERROR) {
        Logger::E("ServiceManager", "GetService failed.");
        return E_REMOTE_EXCEPTION;
    }

    ECode ec = reply.readInt32();
    if (FAILED(ec)) {
        Logger::E("ServiceManager", "Remote call failed with ec = 0x%x.", ec);
        return ec;
    }

    AutoPtr<IParcel> parcel;
    CoCreateParcel(RPCType::Local, parcel);
    parcel->SetPayload(reinterpret_cast<HANDLE>(&reply), false);

    AutoPtr<IInterfacePack> ipack;
    CoCreateInterfacePack(RPCType::Local, ipack);
    IParcelable::Probe(ipack)->ReadFromParcel(parcel);
    return CoUnmarshalInterface(ipack, RPCType::Local, object);
}

ECode ServiceManager::RemoveService(
    /* [in] */ const String& name)
{
    if (name.IsEmpty()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    android::Parcel data, reply;

    data.writeInt32(TAG_NOT_NULL);
    data.writeCString(name.string());
    if (get_service_manager()->transact(REMOVE_SERVICE, data, &reply) != android::NO_ERROR) {
        Logger::E("ServiceManager", "RemoveService failed.");
        return E_REMOTE_EXCEPTION;
    }

    ECode ec = reply.readInt32();
    if (FAILED(ec)) {
        Logger::E("ServiceManager", "Remote call failed with ec = 0x%x.", ec);
        return ec;
    }

    return NOERROR;
}

}
