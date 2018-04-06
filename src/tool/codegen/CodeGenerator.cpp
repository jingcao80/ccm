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

#include "CodeGenerator.h"
#include "../ast/Parameter.h"
#include "../util/File.h"
#include "../util/Logger.h"
#include "../util/StringBuilder.h"
#include "../util/Uuid.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

using ccdl::ast::Parameter;
using ccm::CcmTypeKind;
using ccm::metadata::MetaCoclass;
using ccm::metadata::MetaEnumerator;
using ccm::metadata::MetaNamespace;

namespace ccdl {
namespace codegen {

const String CodeGenerator::TAG("CodeGenerator");

CodeGenerator::CodeGenerator()
{
    mLicense = String("//=========================================================================\n"
           "// Copyright (C) 2018 The C++ Component Model(CCM) Open Source Project\n"
           "//\n"
           "// Licensed under the Apache License, Version 2.0 (the \"License\");\n"
           "// you may not use this file except in compliance with the License.\n"
           "// You may obtain a copy of the License at\n"
           "//\n"
           "//      http://www.apache.org/licenses/LICENSE-2.0\n"
           "//\n"
           "// Unless required by applicable law or agreed to in writing, software\n"
           "// distributed under the License is distributed on an \"AS IS\" BASIS,\n"
           "// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n"
           "// See the License for the specific language governing permissions and\n"
           "// limitations under the License.\n"
           "//=========================================================================\n");
}

void CodeGenerator::Generate()
{
    if (!ResolveDirectory()) return;

    GenerateTypeDeclarations();
    GenerateCoclasses();
    GenerateModule();
}

bool CodeGenerator::ResolveDirectory()
{
    if (!mDirectory.StartsWith("/")) {
        char* cwd = getcwd(nullptr, 0);
        mDirectory = String::Format("%s/%s", cwd, mDirectory.string());
        free(cwd);
    }

    if (!access(mDirectory.string(), R_OK | W_OK)) {
        return true;
    }

    if (mkdir(mDirectory.string(), S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
        Logger::E(TAG, "Create code directory failed.");
        return false;
    }

    return true;
}

void CodeGenerator::GenerateTypeDeclarations()
{
    String filePath = String::Format("%s/%s.h",
            mDirectory.string(), mMetaComponent->mName);
    File file(filePath, File::WRITE);

    StringBuilder builder;

    builder.Append(mLicense);
    builder.Append("\n");
    builder.Append("#include \"../../src/runtime/ccmtypes.h\"\n\n");
    builder.Append("using namespace ccm;\n\n");

    MetaComponent* mc = mMetaComponent;
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        MetaNamespace* mn = mc->mNamespaces[i];
        if (mn->mEnumerationNumber + (mn->mInterfaceNumber -
                mn->mSystemPreDeclaredInterfaceNumber) == 0) {
            continue;
        }
        builder.Append(GenerateNamespaceBegin(String(mn->mName)));
        builder.Append(GenerateEnumerationPreDeclarations(mn));
        builder.Append(GenerateInterfacePreDeclarations(mn));
        builder.Append(GenerateNamespaceEnd(String(mn->mName)));
        if (i != mc->mNamespaceNumber - 1) builder.Append("\n");
    }
    builder.Append("\n");
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        MetaNamespace* mn = mc->mNamespaces[i];
        if (mn->mEnumerationNumber + (mn->mInterfaceNumber -
                mn->mSystemPreDeclaredInterfaceNumber) == 0) {
            continue;
        }
        builder.Append(GenerateNamespaceBegin(String(mn->mName)));
        builder.Append(GenerateEnumerations(mn));
        builder.Append(GenerateInterfaces(mn));
        builder.Append(GenerateNamespaceEnd(String(mn->mName)));
        if (i != mc->mNamespaceNumber - 1) builder.Append("\n");
    }

    String data = builder.ToString();
    file.Write(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

String CodeGenerator::GenerateEnumerationPreDeclarations(
    /* [in] */ MetaNamespace* mn)
{
    StringBuilder builder;

    if (mn->mEnumerationNumber == 0) return String();

    for (int i = 0; i < mn->mEnumerationNumber; i++) {
        MetaEnumeration* me = mMetaComponent->mEnumerations[mn->mEnumerationIndexes[i]];
        builder.AppendFormat("enum class %s;\n", me->mName);
    }
    builder.Append("\n");

    return builder.ToString();
}

String CodeGenerator::GenerateInterfacePreDeclarations(
    /* [in] */ MetaNamespace* mn)
{
    StringBuilder builder;

    if (mn->mInterfaceNumber - mn->mSystemPreDeclaredInterfaceNumber == 0) {
        return String();
    }

    for (int i = 0; i < mn->mInterfaceNumber; i++) {
        MetaInterface* mi = mMetaComponent->mInterfaces[mn->mInterfaceIndexes[i]];
        if (mi->mSystemPreDeclared) continue;
        builder.AppendFormat("interface %s;\n", mi->mName);
    }
    builder.Append("\n");

    return builder.ToString();
}

String CodeGenerator::GenerateEnumerations(
    /* [in] */ MetaNamespace* mn)
{
    StringBuilder builder;

    if (mn->mEnumerationNumber == 0) return String();

    for (int i = 0; i < mn->mEnumerationNumber; i++) {
        builder.Append(GenerateEnumerationDeclaration(
                mMetaComponent->mEnumerations[mn->mEnumerationIndexes[i]]));
        if (i != mn->mEnumerationNumber - 1) builder.Append("\n");
    }
    builder.Append("\n");

    return builder.ToString();
}

String CodeGenerator::GenerateInterfaces(
    /* [in] */ MetaNamespace* mn)
{
    StringBuilder builder;

    if (mn->mInterfaceNumber - mn->mSystemPreDeclaredInterfaceNumber == 0) {
        return String();
    }

    for (int i = 0; i < mn->mInterfaceNumber; i++) {
        MetaInterface* mi = mMetaComponent->mInterfaces[mn->mInterfaceIndexes[i]];
        if (mi->mSystemPreDeclared) continue;
        builder.Append(GenerateInterfaceDeclaration(mi));
        if (i != mn->mInterfaceNumber - 1) builder.Append("\n");
    }
    builder.Append("\n");

    return builder.ToString();
}

String CodeGenerator::GenerateEnumerationDeclaration(
    /* [in] */ MetaEnumeration* me)
{
    StringBuilder builder;

    String defMacro = GenerateDefineMacro(
            String::Format("%s%s", me->mNamespace, me->mName));
    builder.AppendFormat("#ifndef %s\n", defMacro.string());
    builder.AppendFormat("#define %s\n\n", defMacro.string());
    builder.AppendFormat("enum class %s\n{\n", me->mName);
    int j = 0;
    for (int i = 0; i < me->mEnumeratorNumber; i++, j++) {
        MetaEnumerator* mr = me->mEnumerators[i];
        builder.Append("    ").Append(mr->mName);
        if (mr->mValue != j) {
            builder.AppendFormat(" = %d", mr->mValue);
            j = mr->mValue;
        }
        if (i == me->mEnumeratorNumber - 1) builder.Append("\n");
        else builder.Append(",\n");
    }
    builder.Append("};\n\n");
    builder.AppendFormat("#endif // %s\n", defMacro.string());

    return builder.ToString();
}

String CodeGenerator::GenerateInterfaceDeclaration(
    /* [in] */ MetaInterface* mi)
{
    StringBuilder builder;

    String defMacro = GenerateDefineMacro(
            String::Format("%s%s", mi->mNamespace, mi->mName));
    builder.AppendFormat("#ifndef %s\n", defMacro.string());
    builder.AppendFormat("#define %s\n\n", defMacro.string());
    builder.AppendFormat("extern const InterfaceID IID_%s;\n\n", mi->mName);
    builder.AppendFormat("INTERFACE_ID(%s)\n", Uuid(mi->mUuid).Dump().string());
    builder.AppendFormat("interface %s : public ", mi->mName);
    if (mi->mBaseInterfaceIndex != -1) {
        builder.Append(
                mMetaComponent->mInterfaces[mi->mBaseInterfaceIndex]->mName).Append("\n");
    }
    else {
        builder.Append("IInterface\n");
    }
    builder.Append("{\n");
    for (int i = 0; i < mi->mConstantNumber; i++) {
        builder.Append(GenerateConstantDeclaration(mi->mConstants[i]));
    }
    if (mi->mConstantNumber > 0 && mi->mMethodNumber > 0) builder.Append("\n");
    for (int i = 0; i < mi->mMethodNumber; i++) {
        builder.Append(GenerateMethodDeclaration(mi->mMethods[i]));
        if (i != mi->mMethodNumber - 1) builder.Append("\n");
    }
    builder.Append("};\n\n");
    builder.AppendFormat("#endif // %s\n", defMacro.string());

    return builder.ToString();
}

String CodeGenerator::GenerateConstantDeclaration(
    /* [in] */ MetaConstant* mc)
{
    StringBuilder builder;

    MetaType* mt = mMetaComponent->mTypes[mc->mTypeIndex];
    if (mt->mKind == CcmTypeKind::String) {
        builder.Append("    static const ");
    }
    else {
        builder.Append("    static constexpr ");
    }
    builder.AppendFormat("%s %s = %s;\n", GenerateType(mt).string(),
            mc->mName, GenerateValue(mc).string());

    return builder.ToString();
}

String CodeGenerator::GenerateMethodDeclaration(
    /* [in] */ MetaMethod* mm)
{
    StringBuilder builder;

    builder.AppendFormat("    virtual ECode %s(", mm->mName);
    for (int i = 0; i < mm->mParameterNumber; i++) {
        builder.AppendFormat("\n        %s", GenerateParameter(mm->mParameters[i]).string());
        if (i != mm->mParameterNumber - 1) builder.Append(",");
    }
    builder.Append(") = 0;\n");

    return builder.ToString();
}

String CodeGenerator::GenerateDefineMacro(
    /* [in] */ const String& fullName)
{
    String defMacro = fullName.Replace("::", "_");
    defMacro = defMacro.ToUpperCase();
    defMacro = String("__") + defMacro + String("__");
    return defMacro;
}

String CodeGenerator::GenerateNamespaceBegin(
    /* [in] */ const String& ns)
{
    StringBuilder builder;

    if (ns.IsNullOrEmpty()) return String();

    int begin = 0;
    int index = ns.IndexOf("::");
    while (index != -1) {
        builder.AppendFormat("namespace %s {\n", ns.Substring(begin, index - 1).string());
        begin = index + 2;
        index = ns.IndexOf("::", begin);
    }
    builder.Append("\n");

    return builder.ToString();
}

String CodeGenerator::GenerateNamespaceEnd(
    /* [in] */ const String& ns)
{
    StringBuilder builder;

    if (ns.IsNullOrEmpty()) return String();

    int end = ns.LastIndexOf("::") - 1;
    int fromIndex = ns.LastIndexOf("::", end);
    while (fromIndex != -1) {
        builder.AppendFormat("} // namespace %s\n", ns.Substring(fromIndex + 2, end).string());
        end = fromIndex - 1;
        fromIndex = ns.LastIndexOf("::", end);
    }
    builder.AppendFormat("} // namespace %s\n", ns.Substring(0, end).string());

    return builder.ToString();
}

String CodeGenerator::GenerateParameter(
    /* [in] */ MetaParameter* mp)
{
    StringBuilder builder;

    if (mp->mAttribute & Parameter::IN) {
        if (mp->mAttribute & Parameter::OUT) {
            builder.Append("/* [in, out] */ ");
        }
        else {
            builder.Append("/* [in] */ ");
        }
    }
    else if (mp->mAttribute & Parameter::OUT) {
        if (mp->mAttribute & Parameter::CALLEE) {
            builder.Append("/* [out, callee] */ ");
        }
        else {
            builder.Append("/* [out] */ ");
        }
    }
    builder.AppendFormat("%s %s",
            GenerateType(mMetaComponent->mTypes[mp->mTypeIndex]).string(),
            mp->mName);

    return builder.ToString();
}

String CodeGenerator::GenerateType(
    /* [in] */ MetaType* mt)
{
    StringBuilder builder;

    MetaComponent* mc = mMetaComponent;
    switch(mt->mKind) {
        case CcmTypeKind::Char:
            builder.Append("Char");
            break;
        case CcmTypeKind::Byte:
            builder.Append("Byte");
            break;
        case CcmTypeKind::Short:
            builder.Append("Short");
            break;
        case CcmTypeKind::Integer:
            builder.Append("Integer");
            break;
        case CcmTypeKind::Long:
            builder.Append("Long");
            break;
        case CcmTypeKind::Float:
            builder.Append("Float");
            break;
        case CcmTypeKind::Double:
            builder.Append("Double");
            break;
        case CcmTypeKind::Boolean:
            builder.Append("Boolean");
            break;
        case CcmTypeKind::String:
            builder.Append("String");
            break;
        case CcmTypeKind::HANDLE:
            builder.Append("HANDLE");
            break;
        case CcmTypeKind::Enum:
            builder.Append(mc->mEnumerations[mt->mIndex]->mName);
            break;
        case CcmTypeKind::Array:
            builder.AppendFormat("Array<%s>",
                    GenerateType(mc->mTypes[mt->mNestedTypeIndex]).string());
            break;
        case CcmTypeKind::Interface:
            builder.Append(mc->mInterfaces[mt->mIndex]->mName);
            break;
        case CcmTypeKind::CoclassID:
            builder.Append(mt->mPointerNumber > 0 ? "CoclassID" : "const CoclassID&");
            break;
        case CcmTypeKind::InterfaceID:
            builder.Append(mt->mPointerNumber > 0 ? "InterfaceID" : "const InterfaceID&");
            break;
        default:
            break;
    }

    for (int i = 0; i < mt->mPointerNumber; i++) {
        builder.Append("*");
    }

    return builder.ToString();
}

String CodeGenerator::GenerateValue(
    /* [in] */ MetaConstant* mc)
{
    StringBuilder builder;

    MetaType* mt = mMetaComponent->mTypes[mc->mTypeIndex];
    switch(mt->mKind) {
        case CcmTypeKind::Char:
            return String::Format("\'%c\'", mc->mValue.mInteger);
        case CcmTypeKind::Byte: {
            const char* format = mc->mRadix == 8 ? "%o" :
                    mc->mRadix == 10 ? "%d" : "%x";
            return String::Format(format, (unsigned char)mc->mValue.mInteger);
        }
        case CcmTypeKind::Short: {
            const char* format = mc->mRadix == 8 ? "%o" :
                    mc->mRadix == 10 ? "%d" : "%x";
            return String::Format(format, (short)mc->mValue.mInteger);
        }
        case CcmTypeKind::Integer: {
            const char* format = mc->mRadix == 8 ? "%o" :
                    mc->mRadix == 10 ? "%d" : "%x";
            return String::Format(format, mc->mValue.mInteger);
        }
        case CcmTypeKind::Long: {
            const char* format = mc->mRadix == 8 ? "%llo" :
                    mc->mRadix == 10 ? "%lld" : "%llx";
            return String::Format(format, mc->mValue.mLong);
        }
        case CcmTypeKind::Float:
            return String::Format("%f", mc->mValue.mFloat);
        case CcmTypeKind::Double:
            return String::Format("%e", mc->mValue.mDouble);
        case CcmTypeKind::Boolean:
            return mc->mValue.mBoolean ? String("true") : String("false");
        case CcmTypeKind::String:
            return String::Format("\"%s\"", mc->mValue.mString);
        case CcmTypeKind::Enum:
            return String::Format("%s::%s", GenerateType(mt).string(), mc->mValue.mString);
        case CcmTypeKind::Array:
        case CcmTypeKind::HANDLE:
        case CcmTypeKind::Interface:
        case CcmTypeKind::CoclassID:
        case CcmTypeKind::InterfaceID:
        default:
            break;
    }

    return builder.ToString();
}

void CodeGenerator::GenerateCoclasses()
{
    MetaComponent* mc = mMetaComponent;
    for (int i = 0; i < mc->mCoclassNumber; i++) {
        GenerateCoclass(mc->mCoclasses[i]);
    }
}

void CodeGenerator::GenerateCoclass(
    /* [in] */ MetaCoclass* mk)
{
    GenerateCoclassHeader(mk);
    GenerateCoclassCpp(mk);
}

void CodeGenerator::GenerateCoclassHeader(
    /* [in] */ MetaCoclass* mk)
{
    String filePath =
            String::Format("%s/%s.h", mDirectory.string(), mk->mName);
    File file(filePath, File::WRITE);

    StringBuilder builder;

    builder.Append(mLicense);
    builder.Append("\n");

    String defMacro = GenerateDefineMacro(
            String::Format("%s%s::H", mk->mNamespace, mk->mName));
    builder.AppendFormat("#ifndef %s\n", defMacro.string());
    builder.AppendFormat("#define %s\n\n", defMacro.string());

    builder.AppendFormat("#endif // %s\n", defMacro.string());

    String data = builder.ToString();
    file.Write(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

void CodeGenerator::GenerateCoclassCpp(
    /* [in] */ MetaCoclass* mk)
{
    String filePath =
            String::Format("%s/%s.cpp", mDirectory.string(), mk->mName);
    File file(filePath, File::WRITE);

    StringBuilder builder;

    builder.Append(mLicense);
    builder.Append("\n");

    builder.Append(GenerateCoclassObject(mk));

    String data = builder.ToString();
    file.Write(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

String CodeGenerator::GenerateCoclassObject(
    /* [in] */ MetaCoclass* mk)
{
    StringBuilder builder;

    builder.Append(GenerateNamespaceBegin(String(mk->mNamespace)));

    builder.AppendFormat("class %sClassObject\n", mk->mName);
    builder.Append("    : public Object\n");
    MetaInterface* mi = mMetaComponent->mInterfaces[mk->mInterfaceIndexes[mk->mInterfaceNumber - 1]];
    builder.AppendFormat("    , public %s\n", mi->mName);
    builder.Append("{\n");
    builder.Append("public:\n");
    for (int i = 0; i < mi->mMethodNumber; i++) {
        MetaMethod* mm = mi->mMethods[i];
        builder.AppendFormat("    ECode %s(", mm->mName);
        for (int i = 0; i < mm->mParameterNumber; i++) {
            builder.AppendFormat("\n        %s", GenerateParameter(mm->mParameters[i]).string());
            if (i != mm->mParameterNumber - 1) builder.Append(",");
        }
        builder.Append(");\n");
    }
    builder.Append("};\n\n");

    for (int i = 0; i < mi->mMethodNumber; i++) {
        MetaMethod* mm = mi->mMethods[i];
        builder.AppendFormat("ECode %s::%s(", mk->mName, mm->mName);
        for (int i = 0; i < mm->mParameterNumber; i++) {
            builder.AppendFormat("\n    %s", GenerateParameter(mm->mParameters[i]).string());
            if (i != mm->mParameterNumber - 1) builder.Append(",");
        }
        builder.Append(")\n");
        builder.Append("{\n");
        builder.Append("    VALIDATE_NOT_NULL(object);\n\n");
        builder.AppendFormat("    void* addr = calloc(sizeof(%s), 1);\n", mk->mName);
        builder.Append("    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;\n\n");
        builder.AppendFormat("    %s* _obj = new(addr) %s();\n", mk->mName, mk->mName);
        builder.Append("    ECode ec = _obj->constructor(");
        for (int i = 0; i < mm->mParameterNumber; i++) {
            builder.Append(mm->mParameters[i]->mName);
            if (i != mm->mParameterNumber - 1) builder.Append(", ");
        }
        builder.Append(");\n");
        builder.Append("    if (FAILED(ec)) {\n"
                       "        free(addr);\n"
                       "        return ec;\n"
                       "    }\n"
                       "    *object = _obj;\n"
                       "    REFCOUNT_ADD(*object);\n");
        builder.Append("    return NOERROR;\n");
        builder.Append("}\n");
    }

    builder.Append("\n");
    builder.AppendFormat("ECode Get%sClassObject(IInterface** classObject)\n", mk->mName);
    builder.Append("{\n");
    builder.Append("    VALIDATE_NOT_NULL(classObject);\n\n");
    builder.AppendFormat("    static AutoPtr<%s> s%sClassObject;\n", mi->mName, mk->mName);
    builder.AppendFormat("    if (s%sClassObject == nullptr) {\n", mk->mName);
    builder.AppendFormat("        s%sClassObject = new %sClassObject();\n", mk->mName, mk->mName);
    builder.Append("    }\n");
    builder.AppendFormat("    *classObject = s%sClassObject;\n", mk->mName);
    builder.Append("    REFCOUNT_ADD(*classObject);\n"
                   "    return NOERROR;\n");
    builder.Append("}\n\n");

    builder.Append(GenerateNamespaceEnd(String(mk->mNamespace)));

    return builder.ToString();
}

void CodeGenerator::GenerateModule()
{
    String filePath =
            String::Format("%s/%sPub.cpp", mDirectory.string(), mMetaComponent->mName);
    File file(filePath, File::WRITE);

    StringBuilder builder;

    builder.Append(mLicense);
    builder.Append("\n");
    builder.Append("#include \"../../src/runtime/ccmcomponent.h\"\n\n"
                   "using namespace ccm;\n\n");

    MetaComponent* mc = mMetaComponent;
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        MetaNamespace* mn = mc->mNamespaces[i];
        if (mn->mCoclassNumber + mn->mEnumerationNumber +
                (mn->mInterfaceNumber - mn->mSystemPreDeclaredInterfaceNumber) == 0) {
            continue;
        }
        builder.Append(GenerateNamespaceBegin(String(mn->mName)));
        builder.Append(GenerateInterfaceIDs(mn));
        builder.Append(GenerateCoclassIDs(mn));
        builder.Append(GenerateNamespaceEnd(String(mn->mName)));
        if (i != mc->mNamespaceNumber - 1) builder.Append("\n");
    }

    builder.Append("\n");
    builder.Append(GenerateClassObjectGetterArray());
    builder.Append("\n");
    builder.Append(GenerateSoGetClassObject());

    String data = builder.ToString();
    file.Write(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

String CodeGenerator::GenerateInterfaceIDs(
    /* [in] */ MetaNamespace* mn)
{
    StringBuilder builder;

    if (mn->mInterfaceNumber == 0) return String();

    for (int i = 0; i < mn->mInterfaceNumber; i++) {
        MetaInterface* mi = mMetaComponent->mInterfaces[mn->mInterfaceIndexes[i]];
        if (mi->mSystemPreDeclared) continue;
        builder.AppendFormat("const InterfaceID IID_%s =\n        %s;\n",
                mi->mName, Uuid(mi->mUuid).ToString().string());
    }
    builder.Append("\n");

    return builder.ToString();
}

String CodeGenerator::GenerateCoclassIDs(
    /* [in] */ MetaNamespace* mn)
{
    StringBuilder builder;

    if (mn->mCoclassNumber == 0) return String();

    for (int i = 0; i < mn->mCoclassNumber; i++) {
        MetaCoclass* mc = mMetaComponent->mCoclasses[mn->mCoclassIndexes[i]];
        builder.AppendFormat("const CoclassID CID_%s =\n        %s;\n",
                mc->mName, Uuid(mc->mUuid).ToString().string());
    }
    builder.Append("\n");

    return builder.ToString();
}

String CodeGenerator::GenerateClassObjectGetterArray()
{
    StringBuilder builder;

    MetaComponent* mc = mMetaComponent;

    if (mc->mCoclassNumber == 0) return String();

    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        MetaNamespace* mn = mc->mNamespaces[i];
        if (mn->mCoclassNumber == 0) continue;
        builder.Append(GenerateNamespaceBegin(String(mn->mName)));
        for (int i = 0; i < mn->mCoclassNumber; i++) {
            MetaCoclass* mk = mc->mCoclasses[mn->mCoclassIndexes[i]];
            builder.AppendFormat("extern ECode Get%sClassObject(IInterface** classObject);\n", mk->mName);
        }
        builder.Append("\n");
        builder.Append(GenerateNamespaceEnd(String(mn->mName)));
    }
    builder.Append("\n");
    builder.AppendFormat("static ClassObjectGetter co%sGetters[%d] = {\n", mc->mName, mc->mCoclassNumber);
    for (int i = 0; i < mc->mCoclassNumber; i++) {
        MetaCoclass* mk = mc->mCoclasses[i];
        builder.AppendFormat("        {%s, %sGet%sClassObject}",
                Uuid(mk->mUuid).ToString().string(), mk->mNamespace, mk->mName);
        if (i != mc->mCoclassNumber - 1) builder.Append(",\n");
    }
    builder.Append("};\n\n");
    builder.Append(GenerateSoGetAllClassObjects());

    return builder.ToString();
}

String CodeGenerator::GenerateSoGetClassObject()
{
    StringBuilder builder;

    MetaComponent* mc = mMetaComponent;

    builder.Append("ECode soGetClassObject(const CoclassID& cid, IInterface** object)\n");
    builder.Append("{\n"
                   "    VALIDATE_NOT_NULL(object);\n\n");
    for (int i = 0; i < mc->mCoclassNumber; i++) {
        MetaCoclass* mk = mc->mCoclasses[i];
        if (i == 0) {
            builder.AppendFormat("    if (%sCID_%s == cid) {\n"
                                 "        return %sGet%sClassObject(object);\n"
                                 "    }\n", mk->mNamespace, mk->mName, mk->mNamespace, mk->mName);
        }
        else {
            builder.AppendFormat("    else if (%sCID_%s == cid) {\n"
                                 "        return %sGet%sClassObject(object);\n"
                                 "    }\n", mk->mNamespace, mk->mName, mk->mNamespace, mk->mName);
        }
    }
    builder.Append("\n    *object = nullptr;\n"
                   "    return NOERROR;\n}\n");

    return builder.ToString();
}

String CodeGenerator::GenerateSoGetAllClassObjects()
{
    StringBuilder builder;

    builder.AppendFormat("ClassObjectGetter* soGetAllClassObjects(int* size)\n"
                         "{\n"
                         "    *size = sizeof(co%sGetters) / sizeof(ClassObjectGetter);\n"
                         "    return co%sGetters;\n"
                         "}\n", mMetaComponent->mName, mMetaComponent->mName);

    return builder.ToString();
}

}
}
