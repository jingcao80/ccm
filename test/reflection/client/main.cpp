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

#include "ReflectionTestUnit.h"
#include <ccmautoptr.h>
#include <ccmapi.h>

#include <stdio.h>

int main(int argv, char** argc)
{
    AutoPtr<IMetaComponent> mc;
    CoGetComponentMetadata(CID_ReflectionTestUnit, (IMetaComponent**)&mc);
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

    AutoPtr<IMetaCoclass> klass = klasses[0];
    String clsName, clsNs;
    klass->GetName(&clsName);
    klass->GetNamespace(&clsNs);
    Integer methodNumber;
    klass->GetMethodNumber(&methodNumber);
    printf("==== class %s has %d methods ====\n", (clsNs + clsName).string(), methodNumber);
    Array<IMetaMethod*> methods(methodNumber);
    klass->GetAllMethods(methods);
    for (Integer i = 0; i < methodNumber; i++) {
        IMetaMethod* method = methods[i];
        String mthName, mthSig;
        method->GetName(&mthName);
        method->GetSignature(&mthSig);
        printf("==== [%d] method name: %s, signature: %s ====\n", i, mthName.string(), mthSig.string());
    }
    AutoPtr<IMetaMethod> method;
    klass->GetMethod(String("TestMethod1"), String("(I)E"), (IMetaMethod**)&method);
    Integer paramNumber;
    method->GetParameterNumber(&paramNumber);
    printf("==== method TestMethod1 has %d parameters ====\n", paramNumber);
    Array<IMetaParameter*> params(paramNumber);
    method->GetAllParameters(params);
    for (Integer i = 0; i < paramNumber; i++) {
        IMetaParameter* param = params[i];
        String pname;
        param->GetName(&pname);
        Integer pidx;
        param->GetIndex(&pidx);
        IOAttribute attr;
        param->GetIOAttribute(&attr);
        AutoPtr<IMetaType> type;
        param->GetType((IMetaType**)&type);
        String tname;
        type->GetName(&tname);
        printf("==== [%d] parameter name: %s, index: %d, attr: %d, type: %s ====\n",
                i, pname.string(), pidx, attr, tname.string());
    }
    AutoPtr<IArgumentList> args;
    method->CreateArgumentList((IArgumentList**)&args);
    args->SetInputArgumentOfInteger(0, 9);
    method->Invoke(obj, args);
    return 0;
}