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

#include "ServiceManager.h"
#include <binder/Parcel.h>

namespace jing {

const char* ServiceManager::NAME = "jing.servicemanager";

AutoPtr<ServiceManager> ServiceManager::sInstance = new ServiceManager();

AutoPtr<ServiceManager> ServiceManager::GetInstance()
{
    return sInstance;
}

ECode ServiceManager::AddService(
    /* [in] */ const String& name,
    /* [in] */ InterfacePack& object)
{
    if (name.IsEmpty()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    InterfacePack* ipack = new InterfacePack();
    ipack->mBinder = object.mBinder;
    ipack->mCid = object.mCid;
    ipack->mIid = object.mIid;
    ipack->mIsParcelable = object.mIsParcelable;

    Mutex::AutoLock lock(mServicesLock);
    mServices.Put(name, ipack);
    return NOERROR;
}

ECode ServiceManager::GetService(
    /* [in] */ const String& name,
    /* [out, callee] */ InterfacePack** object)
{
    if (name.IsEmpty()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Mutex::AutoLock lock(mServicesLock);
    *object = mServices.Get(name);
    return NOERROR;
}

ECode ServiceManager::RemoveService(
    /* [in] */ const String& name)
{
    if (name.IsEmpty()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    InterfacePack* ipack;
    {
        Mutex::AutoLock lock(mServicesLock);
        ipack = mServices.Get(name);
        mServices.Remove(name);
    }

    ReleaseCoclassID(ipack->mCid);
    ReleaseInterfaceID(ipack->mIid);
    delete ipack;
    return NOERROR;
}

android::status_t ServiceManager::onTransact(
    /* [in] */ uint32_t code,
    /* [in] */ const android::Parcel& data,
    /* [in] */ android::Parcel* reply,
    /* [in] */ uint32_t flags)
{
    switch (code) {
        case ADD_SERVICE: {
            data.readInt32(); // TAG_NOT_NULL
            const char* str = data.readCString();
            InterfacePack ipack;
            ipack.mBinder = data.readStrongBinder();
            ipack.mCid = ReadCoclassID(data);
            ipack.mIid = ReadInterfaceID(data);
            ipack.mIsParcelable = (Boolean)data.readInt32();

            ECode ec = ServiceManager::GetInstance()->AddService(str, ipack);

            reply->writeInt32(ec);
            return android::NO_ERROR;
        }
        case GET_SERVICE: {
            data.readInt32(); // TAG_NOT_NULL
            const char* str = data.readCString();

            InterfacePack* ipack = nullptr;
            ECode ec = ServiceManager::GetInstance()->GetService(str, &ipack);

            reply->writeInt32(ec);
            if (SUCCEEDED(ec) && ipack != nullptr) {
                reply->writeStrongBinder(ipack->mBinder);
                WriteCoclassID(ipack->mCid, reply);
                WriteInterfaceID(ipack->mIid, reply);
                reply->writeInt32(ipack->mIsParcelable);
            }

            return android::NO_ERROR;
        }
        case REMOVE_SERVICE: {
            data.readInt32(); // TAG_NOT_NULL
            const char* str = data.readCString();

            ECode ec = ServiceManager::GetInstance()->RemoveService(str);

            reply->writeInt32(ec);
            return android::NO_ERROR;
        }
        default:
            break;
    }

    return android::BBinder::onTransact(code, data, reply, flags);
}

CoclassID ServiceManager::ReadCoclassID(
    /* [in] */ const android::Parcel& source)
{
    CoclassID value;
    android::status_t st = source.read((void*)&value, sizeof(CoclassID));
    if (st != android::NO_ERROR) {
        return CoclassID::Null;
    }

    Integer tag = source.readInt32();
    if (tag == TAG_NULL) {
        return value;
    }

    value.mCid = (ComponentID*)malloc(sizeof(ComponentID));
    if (value.mCid == nullptr) {
        return value;
    }
    st = source.read(const_cast<ComponentID*>(value.mCid), sizeof(ComponentID));
    if (st != android::NO_ERROR) {
        return value;
    }

    tag = source.readInt32();
    if (tag == TAG_NOT_NULL) {
        const char* uri = source.readCString();
        Integer size = strlen(uri);
        const_cast<ComponentID*>(value.mCid)->mUri = (const char*)malloc(size + 1);
        if (value.mCid->mUri != nullptr) {
            memcpy(const_cast<char*>(value.mCid->mUri), uri, size + 1);
        }
    }

    return value;
}

void ServiceManager::WriteCoclassID(
    /* [in] */ const CoclassID& value,
    /* [in] */ android::Parcel* dest)
{
    CoclassID* cid = (CoclassID*)dest->writeInplace(sizeof(CoclassID));
    if (cid == nullptr) {
        return;
    }
    memcpy(cid, &value, sizeof(CoclassID));
    cid->mCid = nullptr;

    if (value.mCid == nullptr) {
        dest->writeInt32(TAG_NULL);
        return;
    }

    dest->writeInt32(TAG_NOT_NULL);
    return WriteComponentID(*value.mCid, dest);
}

InterfaceID ServiceManager::ReadInterfaceID(
    /* [in] */ const android::Parcel& source)
{
    InterfaceID value;
    android::status_t st = source.read((void*)&value, sizeof(InterfaceID));
    value.mCid = nullptr;
    if (st != android::NO_ERROR) {
        return InterfaceID::Null;
    }

    Integer tag = source.readInt32();
    if (tag == TAG_NULL) {
        return value;
    }

    value.mCid = (ComponentID*)malloc(sizeof(ComponentID));
    if (value.mCid == nullptr) {
        return value;
    }
    st = source.read(const_cast<ComponentID*>(value.mCid), sizeof(ComponentID));
    if (st != android::NO_ERROR) {
        return value;
    }

    tag = source.readInt32();
    if (tag == TAG_NOT_NULL) {
        const char* uri = source.readCString();
        Integer size = strlen(uri);
        const_cast<ComponentID*>(value.mCid)->mUri = (const char*)malloc(size + 1);
        if (value.mCid->mUri != nullptr) {
            memcpy(const_cast<char*>(value.mCid->mUri), uri, size + 1);
        }
    }

    return value;
}

void ServiceManager::WriteInterfaceID(
    /* [in] */ const InterfaceID& value,
    /* [in] */ android::Parcel* dest)
{
    InterfaceID* iid = (InterfaceID*)dest->writeInplace(sizeof(InterfaceID));
    if (iid == nullptr) {
        return;
    }
    memcpy(iid, &value, sizeof(InterfaceID));
    iid->mCid = nullptr;

    if (value.mCid == nullptr) {
        dest->writeInt32(TAG_NULL);
        return;
    }

    dest->writeInt32(TAG_NOT_NULL);
    return WriteComponentID(*value.mCid, dest);
}

void ServiceManager::WriteComponentID(
    /* [in] */ const ComponentID& value,
    /* [in] */ android::Parcel* dest)
{
    ComponentID* cid = (ComponentID*)dest->writeInplace(sizeof(ComponentID));
    if (cid == nullptr) {
        return;
    }
    memcpy(cid, &value, sizeof(ComponentID));
    cid->mUri = nullptr;

    if (value.mUri != nullptr) {
        dest->writeInt32(TAG_NOT_NULL);
        dest->writeCString(value.mUri);
    }
    else {
        dest->writeInt32(TAG_NULL);
    }
}

}
