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

#include <comoapi.h>
#include <comosp.h>
#include <comoref.h>
#include <binder/Binder.h>

namespace jing {

class COM_PUBLIC ServiceManager
    : public LightRefBase
{
public:
    static AutoPtr<ServiceManager> GetInstance();

    ECode AddService(
        /* [in] */ const String& name,
        /* [in] */ IInterface* object);

    ECode GetService(
        /* [in] */ const String& name,
        /* [out] */ AutoPtr<IInterface>& object);

    ECode RemoveService(
        /* [in] */ const String& name);

private:
    ServiceManager() {}
    ServiceManager(
        /* [in] */ ServiceManager&) {}

public:
    static const char* NAME;

private:
    static AutoPtr<ServiceManager> sInstance;

    static constexpr int ADD_SERVICE { android::IBinder::FIRST_CALL_TRANSACTION + 0 };
    static constexpr int GET_SERVICE { android::IBinder::FIRST_CALL_TRANSACTION + 1 };
    static constexpr int REMOVE_SERVICE { android::IBinder::FIRST_CALL_TRANSACTION + 2 };
};

}

#endif // __JING_SERVICEMANAGER_H__
