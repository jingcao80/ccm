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

#ifndef __PISCES_SERVICEMANAGER_H__
#define __PISCES_SERVICEMANAGER_H__

#include <comoapi.h>
#include <comosp.h>
#include <comoref.h>
#include "hashmap.h"
#include "mutex.h"
#include <dbus/dbus.h>

namespace pisces {

class ServiceManager
    : public LightRefBase
{
public:
    struct InterfacePack
    {
        String mDBusName;
        CoclassID mCid;
        InterfaceID mIid;
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

    static DBusHandlerResult HandleMessage(
            /* [in] */ DBusConnection* conn,
            /* [in] */ DBusMessage* msg,
            /* [in] */ void* arg);

private:
    ServiceManager() {}
    ServiceManager(
        /* [in] */ ServiceManager&) {}

public:
    static constexpr const char* DBUS_NAME = "pisces.servicemanager";
    static constexpr const char* OBJECT_PATH = "/pisces/servicemanager";
    static constexpr const char* INTERFACE_PATH = "pisces.servicemanager.IServiceManager";

private:
    static AutoPtr<ServiceManager> sInstance;

    HashMap<String, InterfacePack*> mServices;
    Mutex mServicesLock;
};

}

#endif // __PISCES_SERVICEMANAGER_H__
