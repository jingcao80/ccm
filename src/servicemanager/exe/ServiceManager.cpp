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

namespace xos {

AutoPtr<ServiceManager> ServiceManager::sInstance = new ServiceManager();

AutoPtr<ServiceManager> ServiceManager::GetInstance()
{
    return sInstance;
}

ECode ServiceManager::AddService(
    /* [in] */ const String& name,
    /* [in] */ InterfacePack& object)
{
    if (name.IsNullOrEmpty()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    InterfacePack* ipack = new InterfacePack();
    ipack->mDBusName = object.mDBusName;
    ipack->mCid = object.mCid;
    ipack->mIid = object.mIid;

    Mutex::AutoLock lock(mServicesLock);
    mServices.Put(name, ipack);
    return NOERROR;
}

ECode ServiceManager::GetService(
    /* [in] */ const String& name,
    /* [out, callee] */ InterfacePack** object)
{
    if (name.IsNullOrEmpty()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Mutex::AutoLock lock(mServicesLock);
    *object = mServices.Get(name);
    return NOERROR;
}

static void ReleaseComponentID(
    /* [in] */ const ComponentID* cid)
{
    if (cid != nullptr) {
        if (cid->mUrl != nullptr) {
            free(const_cast<char*>(cid->mUrl));
        }
        free(const_cast<ComponentID*>(cid));
    }
}

ECode ServiceManager::RemoveService(
    /* [in] */ const String& name)
{
    if (name.IsNullOrEmpty()) {
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

}
