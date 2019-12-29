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
#include <comorpc.h>
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

    ReleaseComponentID(ipack->mCid.mCid);
    ReleaseComponentID(ipack->mIid.mCid);
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
            AutoPtr<IParcel> parcel;
            CoCreateParcel(RPCType::Local, parcel);
            Long size = data.dataAvail();
            const void* args = data.readInplace(size);
            parcel->SetData(reinterpret_cast<HANDLE>(args), size);
            parcel->ReadCoclassID(ipack.mCid);
            parcel->ReadInterfaceID(ipack.mIid);
            parcel->ReadBoolean(ipack.mIsParcelable);
            ipack.mCid.mCid = CloneComponentID(ipack.mCid.mCid);
            ipack.mIid.mCid = CloneComponentID(ipack.mIid.mCid);

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
                AutoPtr<IParcel> parcel;
                CoCreateParcel(RPCType::Local, parcel);
                parcel->WriteCoclassID(ipack->mCid);
                parcel->WriteInterfaceID(ipack->mIid);
                parcel->WriteBoolean(ipack->mIsParcelable);
                HANDLE replyData;
                parcel->GetData(replyData);
                reply->writeStrongBinder(ipack->mBinder);
                size_t size = reinterpret_cast<android::Parcel*>(replyData)->dataAvail();
                const void* result = reinterpret_cast<android::Parcel*>(replyData)->readInplace(size);
                reply->write(result, size);
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

}
