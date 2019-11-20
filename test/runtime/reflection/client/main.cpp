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
            EXPECT_EQ(5, totalNumber);
            EXPECT_EQ(1, declaredNumber);
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
    EXPECT_EQ(5, totalNumber);
    Integer declaredNumber;
    intf->GetDeclaredMethodNumber(declaredNumber);
    EXPECT_EQ(1, declaredNumber);
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
    EXPECT_EQ(5, methodNumber);
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

TEST(ReflectionTest, TestMethodInvoke)
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
