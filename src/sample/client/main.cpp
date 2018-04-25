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

#include "FooBarDemo.h"
#include <ccmapi.h>
#include <ccmautoptr.h>

#include <stdio.h>
#include <stdlib.h>

using namespace ccm;

using ccm::demo::CFoo;
using ccm::demo::IFoo;
using ccm::demo::IID_IFoo;

int main(int argc, char** argv)
{
    AutoPtr<IFoo> foo;
    CFoo::New(IID_IFoo, (IInterface**)&foo);
    foo->Foo(9);

    AutoPtr<IMetaComponent> mc;
    CoGetComponentMetadata(CID_FooBarDemo, nullptr, (IMetaComponent**)&mc);
    String name;
    mc->GetName(&name);
    printf("==== component name: %s ====\n\n", name.string());

    Integer clsNumber;
    mc->GetCoclassNumber(&clsNumber);
    printf("==== component class number: %d ====\n", clsNumber);
    Array<IMetaCoclass*> klasses(clsNumber);
    mc->GetAllCoclasses(klasses);
    for (Integer i = 0; i < klasses.GetLength(); i++) {
        String clsName, clsNs;
        klasses[i]->GetName(&clsName);
        klasses[i]->GetNamespace(&clsNs);
        printf("==== [%d] class name: %s, namespace: %s ====\n",
                i, clsName.string(), clsNs.string());
    }
    printf("\n");

    Integer intfNumber;
    mc->GetInterfaceNumber(&intfNumber);
    printf("==== component interface number: %d ====\n", intfNumber);
    Array<IMetaInterface*> intfs(intfNumber);
    mc->GetAllInterfaces(intfs);
    for (Integer i = 0; i < intfs.GetLength(); i++) {
        String intfName, intfNs;
        intfs[i]->GetName(&intfName);
        intfs[i]->GetNamespace(&intfNs);
        printf("==== [%d] interface name: %s, namespace: %s ====\n",
                i, intfName.string(), intfNs.string());
    }
    printf("\n");

    AutoPtr<IInterface> obj;
    klasses[0]->CreateObject(IID_IInterface, (IInterface**)&obj);

    return 0;
}
