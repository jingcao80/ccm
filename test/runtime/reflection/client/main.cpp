//=========================================================================
// Copyright (C) 2018 The C++ Component Model(COMO) Open Source Project
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
#include <comoapi.h>
#include <gtest/gtest.h>

TEST(ReflectionTest, TestComponentGetName)
{
    AutoPtr<IMetaComponent> mc;
    CoGetComponentMetadata(CID_ReflectionTestUnit, nullptr, mc);
    String name;
    mc->GetName(name);
    EXPECT_STREQ("ReflectionTestUnit", name.string());
}

TEST(ReflectionTest, TestComponentGetComponentID)
{
    AutoPtr<IMetaComponent> mc;
    CoGetComponentMetadata(CID_ReflectionTestUnit, nullptr, mc);
    ComponentID cid;
    mc->GetComponentID(cid);
    EXPECT_STREQ("42197c0a-0de1-4c11-8a35-4ed719e72695", DumpUUID(cid.mUuid).string());
    EXPECT_STREQ("http://como.org/component/test/reflection/ReflectionTestUnit.so", cid.mUri);
}

TEST(ReflectionTest, TestComponentGetConstants)
{
    AutoPtr<IMetaComponent> mc;
    CoGetComponentMetadata(CID_ReflectionTestUnit, nullptr, mc);
    Integer constantNumber;
    mc->GetConstantNumber(constantNumber);
    EXPECT_EQ(8, constantNumber);
    Array<IMetaConstant*> constants(constantNumber);
    mc->GetAllConstants(constants);
    for (Integer i = 0; i < constants.GetLength(); i++) {
        String name, ns;
        constants[i]->GetName(name);
        constants[i]->GetNamespace(ns);
        AutoPtr<IMetaType> type;
        constants[i]->GetType(type);
        AutoPtr<IMetaValue> value;
        constants[i]->GetValue(value);
        TypeKind kind;
        type->GetTypeKind(kind);
        switch (i) {
            case 0: {
                EXPECT_STREQ("", ns.string());
                EXPECT_STREQ("ICONST1", name.string());
                EXPECT_EQ(TypeKind::Integer, kind);
                Integer iv;
                value->GetIntegerValue(iv);
                EXPECT_EQ(1, iv);
                break;
            }
            case 1: {
                EXPECT_STREQ("como::test", ns.string());
                EXPECT_STREQ("ICONST2", name.string());
                EXPECT_EQ(TypeKind::Integer, kind);
                Integer iv;
                value->GetIntegerValue(iv);
                EXPECT_EQ(2, iv);
                break;
            }
            case 2: {
                EXPECT_STREQ("como::test::reflection", ns.string());
                EXPECT_STREQ("ICONST3", name.string());
                EXPECT_EQ(TypeKind::Integer, kind);
                Integer iv;
                value->GetIntegerValue(iv);
                EXPECT_EQ(3, iv);
                break;
            }
            case 3: {
                EXPECT_STREQ("como::test::reflection", ns.string());
                EXPECT_STREQ("FCONST", name.string());
                EXPECT_EQ(TypeKind::Float, kind);
                Float fv;
                value->GetFloatValue(fv);
                EXPECT_FLOAT_EQ(3.0, fv);
                break;
            }
            case 4: {
                EXPECT_STREQ("como::test::reflection", ns.string());
                EXPECT_STREQ("DCONST", name.string());
                EXPECT_EQ(TypeKind::Double, kind);
                Double dv;
                value->GetDoubleValue(dv);
                EXPECT_DOUBLE_EQ(3.0, dv);
                break;
            }
            case 5: {
                EXPECT_STREQ("", ns.string());
                EXPECT_STREQ("BCONST", name.string());
                EXPECT_EQ(TypeKind::Byte, kind);
                Byte bv;
                value->GetByteValue(bv);
                EXPECT_EQ(6, bv);
                break;
            }
            case 6: {
                EXPECT_STREQ("", ns.string());
                EXPECT_STREQ("CCONST", name.string());
                EXPECT_EQ(TypeKind::Char, kind);
                Char cv;
                value->GetCharValue(cv);
                EXPECT_EQ(6, cv);
                break;
            }
            case 7: {
                EXPECT_STREQ("", ns.string());
                EXPECT_STREQ("SCONST", name.string());
                EXPECT_EQ(TypeKind::Short, kind);
                Short sv;
                value->GetShortValue(sv);
                EXPECT_EQ(60, sv);
                break;
            }
        }
    }
}

TEST(ReflectionTest, TestComponentGetCoclasses)
{
    AutoPtr<IMetaComponent> mc;
    CoGetComponentMetadata(CID_ReflectionTestUnit, nullptr, mc);
    Integer klassNumber;
    mc->GetCoclassNumber(klassNumber);
    EXPECT_EQ(1, klassNumber);
    Array<IMetaCoclass*> klasses(klassNumber);
    mc->GetAllCoclasses(klasses);
    for (Integer i = 0; i < klasses.GetLength(); i++) {
        String name, ns;
        klasses[i]->GetName(name);
        klasses[i]->GetNamespace(ns);
        if (i == 0) {
            EXPECT_STREQ("como::test::reflection", ns.string());
            EXPECT_STREQ("CMethodTester", name.string());
        }
    }
}

TEST(ReflectionTest, TestComponentGetInterfaces)
{
    AutoPtr<IMetaComponent> mc;
    CoGetComponentMetadata(CID_ReflectionTestUnit, nullptr, mc);
    Integer interfaceNumber;
    mc->GetInterfaceNumber(interfaceNumber);
    EXPECT_EQ(1, interfaceNumber);
    Array<IMetaInterface*> interfaces(interfaceNumber);
    mc->GetAllInterfaces(interfaces);
    for (Integer i = 0; i < interfaces.GetLength(); i++) {
        String name, ns;
        int totalNumber, declaredNumber;
        interfaces[i]->GetName(name);
        interfaces[i]->GetNamespace(ns);
        interfaces[i]->GetMethodNumber(totalNumber);
        interfaces[i]->GetDeclaredMethodNumber(declaredNumber);
        if (i == 0) {
            EXPECT_STREQ("como::test::reflection", ns.string());
            EXPECT_STREQ("IMethodTest", name.string());
            EXPECT_EQ(9, totalNumber);
            EXPECT_EQ(5, declaredNumber);
        }
    }
}

TEST(ReflectionTest, TestInterfaceGetDeclaredMethods)
{
    AutoPtr<IMetaComponent> mc;
    CoGetComponentMetadata(CID_ReflectionTestUnit, nullptr, mc);
    AutoPtr<IMetaInterface> intf;
    mc->GetInterface("como::test::reflection::IMethodTest", intf);
    Integer totalNumber;
    intf->GetMethodNumber(totalNumber);
    EXPECT_EQ(9, totalNumber);
    Integer declaredNumber;
    intf->GetDeclaredMethodNumber(declaredNumber);
    EXPECT_EQ(5, declaredNumber);
    Array<IMetaMethod*> declaredMethods(declaredNumber);
    intf->GetDeclaredMethods(declaredMethods);
    for (Integer i = 0; i < declaredNumber; i++) {
        String name, signature;
        declaredMethods[i]->GetName(name);
        declaredMethods[i]->GetSignature(signature);
        if (i == 0) {
            EXPECT_STREQ("TestMethod1", name.string());
            EXPECT_STREQ("(II&)E", signature.string());
        }
    }
}

TEST(ReflectionTest, TestCoclassCreateObject)
{
    AutoPtr<IMetaComponent> mc;
    CoGetComponentMetadata(CID_ReflectionTestUnit, nullptr, mc);
    AutoPtr<IMetaCoclass> klass;
    mc->GetCoclass("como::test::reflection::CMethodTester", klass);
    AutoPtr<IInterface> obj;
    klass->CreateObject(IID_IInterface, &obj);
    EXPECT_TRUE(obj != nullptr);
}

TEST(ReflectionTest, TestCoclassGetMethods)
{
    AutoPtr<IMetaComponent> mc;
    CoGetComponentMetadata(CID_ReflectionTestUnit, nullptr, mc);
    AutoPtr<IMetaCoclass> klass;
    mc->GetCoclass("como::test::reflection::CMethodTester", klass);
    Integer methodNumber;
    klass->GetMethodNumber(methodNumber);
    EXPECT_EQ(9, methodNumber);
    Array<IMetaMethod*> methods(methodNumber);
    klass->GetAllMethods(methods);
    for (Integer i = 0; i < methodNumber; i++) {
        IMetaMethod* method = methods[i];
        String name, sig;
        method->GetName(name);
        method->GetSignature(sig);
        switch (i) {
            case 0:
                EXPECT_STREQ("AddRef", name.string());
                EXPECT_STREQ("(H)I", sig.string());
                break;
            case 1:
                EXPECT_STREQ("Release", name.string());
                EXPECT_STREQ("(H)I", sig.string());
                break;
            case 2:
                EXPECT_STREQ("Probe", name.string());
                EXPECT_STREQ("(U)Lcomo/IInterface*", sig.string());
                break;
            case 3:
                EXPECT_STREQ("GetInterfaceID", name.string());
                EXPECT_STREQ("(Lcomo/IInterface*U&)E", sig.string());
                break;
            case 4:
                EXPECT_STREQ("TestMethod1", name.string());
                EXPECT_STREQ("(II&)E", sig.string());
                break;
            case 5:
                EXPECT_STREQ("TestMethod2", name.string());
                EXPECT_STREQ("(FF&)E", sig.string());
                break;
            case 6:
                EXPECT_STREQ("TestMethod3", name.string());
                EXPECT_STREQ("(ILZCSI)E", sig.string());
                break;
            case 7:
                EXPECT_STREQ("TestMethod4", name.string());
                EXPECT_STREQ("(ILZCSDFID&)E", sig.string());
                break;
            case 8:
                EXPECT_STREQ("TestMethod5", name.string());
                EXPECT_STREQ("(ILZCSDFIFDDFFDDFD&)E", sig.string());
                break;
            default:
                break;
        }
    }
}

TEST(ReflectionTest, TestMethodGetParameters)
{
    AutoPtr<IMetaComponent> mc;
    CoGetComponentMetadata(CID_ReflectionTestUnit, nullptr, mc);
    AutoPtr<IMetaCoclass> klass;
    mc->GetCoclass("como::test::reflection::CMethodTester", klass);
    AutoPtr<IMetaMethod> method;
    klass->GetMethod("TestMethod1", "(II&)E", method);
    Integer paramNumber;
    method->GetParameterNumber(paramNumber);
    EXPECT_EQ(2, paramNumber);
    Array<IMetaParameter*> params(paramNumber);
    method->GetAllParameters(params);
    for (Integer i = 0; i < paramNumber; i++) {
        IMetaParameter* param = params[i];
        String name;
        param->GetName(name);
        Integer index;
        param->GetIndex(index);
        IOAttribute attr;
        param->GetIOAttribute(attr);
        AutoPtr<IMetaType> type;
        param->GetType(type);
        String tname;
        type->GetName(tname);
        switch (i) {
            case 0:
                EXPECT_STREQ("arg", name.string());
                EXPECT_EQ(0, index);
                EXPECT_TRUE(attr == IOAttribute::IN);
                EXPECT_STREQ("Integer", tname.string());
                break;
            case 1:
                EXPECT_STREQ("result", name.string());
                EXPECT_EQ(1, index);
                EXPECT_TRUE(attr == IOAttribute::OUT);
                EXPECT_STREQ("Integer&", tname.string());
                break;
            default:
                break;
        }
    }
}

TEST(ReflectionTest, TestMethodInvokeTestMethod1)
{
    AutoPtr<IMetaComponent> mc;
    CoGetComponentMetadata(CID_ReflectionTestUnit, nullptr, mc);
    AutoPtr<IMetaCoclass> klass;
    mc->GetCoclass("como::test::reflection::CMethodTester", klass);
    AutoPtr<IInterface> obj;
    klass->CreateObject(IID_IInterface, &obj);
    AutoPtr<IMetaMethod> method;
    klass->GetMethod("TestMethod1", "(II&)E", method);
    AutoPtr<IArgumentList> args;
    method->CreateArgumentList(args);
    Integer arg = 9, result;
    args->SetInputArgumentOfInteger(0, arg);
    args->SetOutputArgumentOfInteger(1, reinterpret_cast<HANDLE>(&result));
    method->Invoke(obj, args);
    IObject::Probe(obj)->GetCoclass(klass);
    EXPECT_EQ(arg, result);
    String name, ns;
    klass->GetName(name);
    klass->GetNamespace(ns);
    EXPECT_STREQ("como::test::reflection", ns.string());
    EXPECT_STREQ("CMethodTester", name.string());
}

TEST(ReflectionTest, TestMethodInvokeTestMethod2)
{
    AutoPtr<IMetaComponent> mc;
    CoGetComponentMetadata(CID_ReflectionTestUnit, nullptr, mc);
    AutoPtr<IMetaCoclass> klass;
    mc->GetCoclass("como::test::reflection::CMethodTester", klass);
    AutoPtr<IInterface> obj;
    klass->CreateObject(IID_IInterface, &obj);
    AutoPtr<IMetaMethod> method;
    klass->GetMethod("TestMethod2", "(FF&)E", method);
    AutoPtr<IArgumentList> args;
    method->CreateArgumentList(args);
    Float arg = 9.9, result;
    args->SetInputArgumentOfFloat(0, arg);
    args->SetOutputArgumentOfFloat(1, reinterpret_cast<HANDLE>(&result));
    method->Invoke(obj, args);
    IObject::Probe(obj)->GetCoclass(klass);
    EXPECT_FLOAT_EQ(arg, result);
    String name, ns;
    klass->GetName(name);
    klass->GetNamespace(ns);
    EXPECT_STREQ("como::test::reflection", ns.string());
    EXPECT_STREQ("CMethodTester", name.string());
}

TEST(ReflectionTest, TestMethodInvokeTestMethod3)
{
    AutoPtr<IMetaComponent> mc;
    CoGetComponentMetadata(CID_ReflectionTestUnit, nullptr, mc);
    AutoPtr<IMetaCoclass> klass;
    mc->GetCoclass("como::test::reflection::CMethodTester", klass);
    AutoPtr<IInterface> obj;
    klass->CreateObject(IID_IInterface, &obj);
    AutoPtr<IMetaMethod> method;
    klass->GetMethod("TestMethod3", "(ILZCSI)E", method);
    Integer arg1 = 9;
    Long arg2 = 99;
    Boolean arg3 = true;
    Char arg4 = U'C';
    Short arg5 = 999;
    Integer arg6 = 9999;
    AutoPtr<IArgumentList> args;
    method->CreateArgumentList(args);
    args->SetInputArgumentOfInteger(0, arg1);
    args->SetInputArgumentOfLong(1, arg2);
    args->SetInputArgumentOfBoolean(2, arg3);
    args->SetInputArgumentOfChar(3, arg4);
    args->SetInputArgumentOfShort(4, arg5);
    args->SetInputArgumentOfInteger(5, arg6);
    method->Invoke(obj, args);
    IObject::Probe(obj)->GetCoclass(klass);
    String name, ns;
    klass->GetName(name);
    klass->GetNamespace(ns);
    EXPECT_STREQ("como::test::reflection", ns.string());
    EXPECT_STREQ("CMethodTester", name.string());
}

TEST(ReflectionTest, TestMethodInvokeTestMethod4)
{
    AutoPtr<IMetaComponent> mc;
    CoGetComponentMetadata(CID_ReflectionTestUnit, nullptr, mc);
    AutoPtr<IMetaCoclass> klass;
    mc->GetCoclass("como::test::reflection::CMethodTester", klass);
    AutoPtr<IInterface> obj;
    klass->CreateObject(IID_IInterface, &obj);
    AutoPtr<IMetaMethod> method;
    klass->GetMethod("TestMethod4", "(ILZCSDFID&)E", method);
    Integer arg1 = 9;
    Long arg2 = 99;
    Boolean arg3 = true;
    Char arg4 = U'C';
    Short arg5 = 999;
    Double arg6 = 9.9;
    Float arg7 = 9.99;
    Integer arg8 = 999;
    Double result;
    AutoPtr<IArgumentList> args;
    method->CreateArgumentList(args);
    args->SetInputArgumentOfInteger(0, arg1);
    args->SetInputArgumentOfLong(1, arg2);
    args->SetInputArgumentOfBoolean(2, arg3);
    args->SetInputArgumentOfChar(3, arg4);
    args->SetInputArgumentOfShort(4, arg5);
    args->SetInputArgumentOfDouble(5, arg6);
    args->SetInputArgumentOfFloat(6, arg7);
    args->SetInputArgumentOfInteger(7, arg8);
    args->SetOutputArgumentOfDouble(8, reinterpret_cast<HANDLE>(&result));
    method->Invoke(obj, args);
    IObject::Probe(obj)->GetCoclass(klass);
    EXPECT_EQ(arg2, result);
    String name, ns;
    klass->GetName(name);
    klass->GetNamespace(ns);
    EXPECT_STREQ("como::test::reflection", ns.string());
    EXPECT_STREQ("CMethodTester", name.string());
}

TEST(ReflectionTest, TestMethodInvokeTestMethod5)
{
    AutoPtr<IMetaComponent> mc;
    CoGetComponentMetadata(CID_ReflectionTestUnit, nullptr, mc);
    AutoPtr<IMetaCoclass> klass;
    mc->GetCoclass("como::test::reflection::CMethodTester", klass);
    AutoPtr<IInterface> obj;
    klass->CreateObject(IID_IInterface, &obj);
    AutoPtr<IMetaMethod> method;
    klass->GetMethod("TestMethod5", "(ILZCSDFIFDDFFDDFD&)E", method);
    Integer arg1 = 9;
    Long arg2 = 99;
    Boolean arg3 = true;
    Char arg4 = U'C';
    Short arg5 = 999;
    Double arg6 = 9.9;
    Float arg7 = 9.99;
    Integer arg8 = 999;
    Float arg9 = 99.9;
    Double arg10 = 9.009;
    Double arg11 = 0.009;
    Float arg12 = 9.09;
    Float arg13 = 0.09;
    Double arg14 = 99.009;
    Double arg15 = -999.009;
    Float arg16 = -0.09;
    Double result;
    AutoPtr<IArgumentList> args;
    method->CreateArgumentList(args);
    args->SetInputArgumentOfInteger(0, arg1);
    args->SetInputArgumentOfLong(1, arg2);
    args->SetInputArgumentOfBoolean(2, arg3);
    args->SetInputArgumentOfChar(3, arg4);
    args->SetInputArgumentOfShort(4, arg5);
    args->SetInputArgumentOfDouble(5, arg6);
    args->SetInputArgumentOfFloat(6, arg7);
    args->SetInputArgumentOfInteger(7, arg8);
    args->SetInputArgumentOfFloat(8, arg9);
    args->SetInputArgumentOfDouble(9, arg10);
    args->SetInputArgumentOfDouble(10, arg11);
    args->SetInputArgumentOfFloat(11, arg12);
    args->SetInputArgumentOfFloat(12, arg13);
    args->SetInputArgumentOfDouble(13, arg14);
    args->SetInputArgumentOfDouble(14, arg15);
    args->SetInputArgumentOfFloat(15, arg16);
    args->SetOutputArgumentOfDouble(16, reinterpret_cast<HANDLE>(&result));
    method->Invoke(obj, args);
    IObject::Probe(obj)->GetCoclass(klass);
    EXPECT_EQ(arg16, result);
    String name, ns;
    klass->GetName(name);
    klass->GetNamespace(ns);
    EXPECT_STREQ("como::test::reflection", ns.string());
    EXPECT_STREQ("CMethodTester", name.string());
}

TEST(ReflectionTest, TestModuleUnload)
{
    AutoPtr<IMetaComponent> mc;
    CoGetComponentMetadata(CID_ReflectionTestUnit, nullptr, mc);
    Boolean canUnload;
    mc->CanUnload(canUnload);
    EXPECT_TRUE(canUnload);
    AutoPtr<IMetaCoclass> klass;
    mc->GetCoclass("como::test::reflection::CMethodTester", klass);
    AutoPtr<IInterface> obj;
    klass->CreateObject(IID_IInterface, &obj);
    mc->CanUnload(canUnload);
    EXPECT_FALSE(canUnload);
    obj = nullptr;
    mc->CanUnload(canUnload);
    EXPECT_TRUE(canUnload);
    ECode ec = mc->Unload();
    EXPECT_TRUE(SUCCEEDED(ec));
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
