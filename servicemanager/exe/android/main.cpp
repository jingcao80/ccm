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
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <cutils/log.h>

using jing::ServiceManager;

int main(int argv, char** argc)
{
    android::sp<android::IServiceManager> sm = android::defaultServiceManager();
    int res = sm->addService(android::String16(ServiceManager::NAME), new ServiceManager());
    if (res != 0) {
        ALOGE("Add service \"%s\" failed, res=%d.\n", ServiceManager::NAME, res);
        return -1;
    }
    else {
        ALOGE("Add service \"%s\" succeeded.\n", ServiceManager::NAME);
    }

    android::IPCThreadState::self()->joinThreadPool();
    return 0;
};
