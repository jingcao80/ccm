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

#include "RPCTestUnit.h"
#include <ccmapi.h>
#include <ccmautoptr.h>
#include <ServiceManager.h>

#include <stdio.h>
#include <unistd.h>

using ccm::test::rpc::CService;
using ccm::test::rpc::IService;
using ccm::test::rpc::IID_IService;
using pisces::ServiceManager;

int main(int argv, char** argc)
{
    AutoPtr<IService> srv;
    CService::New(IID_IService, (IInterface**)&srv);

    ServiceManager::GetInstance()->AddService(String("rpcservice"), srv);

    printf("==== rpc service wait for calling ====\n");
    while (true) {
        sleep(5);
    }

    return 0;
}
