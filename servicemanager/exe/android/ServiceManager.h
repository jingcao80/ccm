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

#ifndef __JING_SERVICEMANAGER_H__
#define __JING_SERVICEMANAGER_H__

#include <comoref.h>
#include <comotypes.h>
#include "hashmap.h"
#include "mutex.h"
#include <binder/Binder.h>

namespace jing {

class ServiceManager
    : public LightRefBase
    , public android::BBinder
{
private:
    struct InterfacePack
    {
        android::sp<android::IBinder> mBinder;
        CoclassID mCid;
        InterfaceID mIid;
        Boolean mIsParcelable;
    };

public:
    static AutoPtr<ServiceManager> GetInstance();

    ECode AddService(
        /* [in] */ const String& name,
        /* [in] */ InterfacePack& object);

    ECode GetService(
        /* [in] */ const String& name,
        /* [out, callee] */ InterfacePack** object);

    ECode RemoveService(
        /* [in] */ const String& name);

protected:
    android::status_t onTransact(
        /* [in] */ uint32_t code,
        /* [in] */ const android::Parcel& data,
        /* [in] */ android::Parcel* reply,
        /* [in] */ uint32_t flags = 0) override;

public:
    static const char* NAME;

private:
    static constexpr int ADD_SERVICE { android::IBinder::FIRST_CALL_TRANSACTION + 0 };
    static constexpr int GET_SERVICE { android::IBinder::FIRST_CALL_TRANSACTION + 1 };
    static constexpr int REMOVE_SERVICE { android::IBinder::FIRST_CALL_TRANSACTION + 2 };

    static AutoPtr<ServiceManager> sInstance;

    HashMap<String, InterfacePack*> mServices;
    Mutex mServicesLock;
};

} // namespace jing

#endif // __JING_SERVICEMANAGER_H__
