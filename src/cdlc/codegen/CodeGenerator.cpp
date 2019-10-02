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

#include "codegen/CodeGenerator.h"
#include "util/AutoPtr.h"
#include "util/File.h"
#include "util/Logger.h"
#include "util/Properties.h"
#include "util/StringBuilder.h"
#include "util/UUID.h"
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>

namespace cdlc {

const char* CodeGenerator::TAG = "CodeGenerator";

CodeGenerator::CodeGenerator()
{
    mLicense =
            "//=========================================================================\n"
            "// Copyright (C) 2018 The C++ Component Model(COMO) Open Source Project\n"
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
            "//=========================================================================\n";
}

bool CodeGenerator::CheckDirectory()
{
    if (!mDirectory.StartsWith("/")) {
        char* cwd = getcwd(nullptr, 0);
        mDirectory = String::Format("%s/%s", cwd, mDirectory.string());
        free(cwd);
    }

    if (access(mDirectory.string(), R_OK | W_OK) == 0) {
        return true;
    }

    if (mkdir(mDirectory.string(), S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
        Logger::E(TAG, "Create the \"%s\" directory failed.", mDirectory.string());
        return false;
    }

    return true;
}

void CodeGenerator::Generate()
{
    if (!CheckDirectory()) {
        return;
    }

    AutoPtr<Emitter> emitter;
    switch (mMode & MODE_MASK) {
        case MODE_CLIENT:
            emitter = new ClientModeEmitter(this);
            break;
        case MODE_COMPONENT:
            emitter = new ComponentModeEmitter(this);
            break;
        case MODE_RUNTIME:
            emitter = new RuntimeModeEmitter(this);
            break;
        default:
            Logger::E(TAG, "Code generation mode not supported.");
            break;
    }

    emitter->Emit();
}

//=============================================================================

void CodeGenerator::ComponentModeEmitter::Emit()
{

}

//=============================================================================

void CodeGenerator::ClientModeEmitter::Emit()
{

}

//=============================================================================

void CodeGenerator::RuntimeModeEmitter::Emit()
{
    EmitConstantsAndTypes();
    EmitUUIDs();
    EmitMetadataWrapper();
}

void CodeGenerator::RuntimeModeEmitter::EmitConstantsAndTypes()
{
    String path = String::Format("%s/comointfs.h", mOwner->mDirectory.string());
    File file(path, File::WRITE);

    StringBuilder builder;

    builder.Append(mOwner->mLicense);
    builder.Append("\n");
    builder.Append("#ifndef __COMO_COMORUNTIMEINTERFACES_H_GEN__\n"
                   "#define __COMO_COMORUNTIMEINTERFACES_H_GEN__\n"
                   "\n"
                   "#include <comotypes.h>\n"
                   "\n"
                   "namespace como {\n"
                   "\n");

    como::MetaComponent* mc = mComponent;
    builder.AppendFormat("COM_PUBLIC extern const ComponentID CID_%s;\n\n", mc->mName);
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        como::MetaNamespace* mn = mc->mNamespaces[i];
        String constantInfos = EmitConstantsInHeader(mn);
        if (!constantInfos.IsEmpty()) {
            builder.Append(constantInfos);
            builder.Append("\n");
        }
    }
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        como::MetaNamespace* mn = mc->mNamespaces[i];
        String enumerationInfos = EmitEnumerationForwardDeclarations(mn);
        if (!enumerationInfos.IsEmpty()) {
            builder.Append(enumerationInfos);
            builder.Append("\n");
        }
    }
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        como::MetaNamespace* mn = mc->mNamespaces[i];
        String enumerationInfos = EmitEnumerationDeclarations(mn);
        if (!enumerationInfos.IsEmpty()) {
            builder.Append(enumerationInfos);
        }
    }
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        como::MetaNamespace* mn = mc->mNamespaces[i];
        String interfaceIDInfos = EmitInterfaceIDForwardDeclarations(mn);
        if (!interfaceIDInfos.IsEmpty()) {
            builder.Append(interfaceIDInfos);
            builder.Append("\n");
        }
    }
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        como::MetaNamespace* mn = mc->mNamespaces[i];
        String interfaceInfos = EmitInterfaceForwardDeclarations(mn);
        if (!interfaceInfos.IsEmpty()) {
            builder.Append(interfaceInfos);
            builder.Append("\n");
        }
    }
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        como::MetaNamespace* mn = mc->mNamespaces[i];
        String interfaceInfos = EmitInterfaceDeclarations(mn);
        if (!interfaceInfos.IsEmpty()) {
            builder.Append(interfaceInfos);
        }
    }

    builder.Append("} // namespace como\n"
                   "\n"
                   "#endif // __COMO_COMORUNTIMEINTERFACES_H_GEN__\n");

    String data = builder.ToString();
    file.Write(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

void CodeGenerator::RuntimeModeEmitter::EmitUUIDs()
{
    String path = String::Format("%s/comouuids.cpp", mOwner->mDirectory.string());
    File file(path, File::WRITE);

    StringBuilder builder;

    builder.Append(mOwner->mLicense);
    builder.Append("\n");
    builder.Append("#include <comotypes.h>\n"
                   "\n"
                   "namespace como {\n"
                   "\n");

    como::MetaComponent* mc = mComponent;
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        como::MetaNamespace* mn = mc->mNamespaces[i];
        String constantInfos = EmitConstantsInCpp(mn);
        if (!constantInfos.IsEmpty()) {
            builder.Append(constantInfos);
        }
        String interfaceIDInfos = EmitInterfaceIDsInCpp(mn);
        if (!interfaceIDInfos.IsEmpty()) {
            builder.Append(interfaceIDInfos);
        }
    }

    builder.Append("} // namespace como\n");

    String data = builder.ToString();
    file.Write(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

void CodeGenerator::RuntimeModeEmitter::EmitMetadataWrapper()
{
    String path = String::Format("%s/MetadataWrapper.cpp", mOwner->mDirectory.string());
    File file(path, File::WRITE);

    StringBuilder builder;

    builder.Append(mOwner->mLicense);
    builder.Append("\n");

    como::metadata::MetadataSerializer serializer(mComponent);
    serializer.Serialize();
    size_t metadataSize = serializer.GetSize();
    uintptr_t metadata = serializer.GetSerializedMetadata();

    builder.Append("#include <comodef.h>\n");
    builder.Append("#include <cstdint>\n\n");
    builder.AppendFormat("struct MetadataWrapper\n"
                   "{\n"
                   "    size_t          mSize;\n"
                   "    unsigned char   mMetadata[%d];\n"
                   "};\n\n", metadataSize);
    builder.Append("static const MetadataWrapper comMetadata __attribute__ ((used,__section__ (\".metadata\"))) = {\n");
    builder.Append(Properties::INDENT).AppendFormat("%lu, {\n", metadataSize);
    int lineSize = 0;
    for (size_t i = 0; i < metadataSize; i++, lineSize++) {
        if (lineSize == 0) {
            builder.Append(Properties::INDENT);
        }
        builder.AppendFormat("0x%02x", ((unsigned char*)metadata)[i]);
        if (i == metadataSize - 1) {
            break;
        }
        if (lineSize < 15) {
            builder.Append(", ");
        }
        else {
            builder.Append(",\n");
            lineSize = -1;
        }
    }
    builder.Append(" }};\n\n");
    builder.Append("COM_PUBLIC extern const uintptr_t soMetadataHandle = reinterpret_cast<uintptr_t>(&comMetadata);\n");

    String data = builder.ToString();
    file.Write(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

//=============================================================================

String CodeGenerator::Emitter::EmitConstantsInHeader(
    /* [in] */ como::MetaNamespace* mn)
{
    if (mn->mConstantNumber == 0) {
        return nullptr;
    }

    StringBuilder builder;

    for (int i = 0; i < mn->mConstantNumber; i++) {
        como::MetaConstant* mc = mComponent->mConstants[mn->mConstantIndexes[i]];
        builder.Append(EmitConstantForHeader(mc));
    }

    return builder.ToString();
}

String CodeGenerator::Emitter::EmitConstantForHeader(
    /* [in] */ como::MetaConstant* mc)
{
    StringBuilder builder;

    como::MetaType* mt = mComponent->mTypes[mc->mTypeIndex];
    if (mt->mKind == como::TypeKind::String || mt->mKind == como::TypeKind::Float ||
            mt->mKind == como::TypeKind::Double) {
        builder.AppendFormat("extern const %s %s;\n", EmitType(mt, MODE_VARIABLE).string(),
                mc->mName);
    }
    else {
        builder.AppendFormat("constexpr %s %s = %s;\n", EmitType(mt, MODE_VARIABLE).string(),
                mc->mName, EmitValue(mt, &mc->mValue).string());
    }

    return builder.ToString();
}

String CodeGenerator::Emitter::EmitEnumerationForwardDeclarations(
    /* [in] */ como::MetaNamespace* mn)
{
    StringBuilder builder;

    for (int i = 0; i < mn->mEnumerationNumber; i++) {
        como::MetaEnumeration* me = mComponent->mEnumerations[mn->mEnumerationIndexes[i]];
        builder.AppendFormat("enum class %s;\n", me->mName);
    }

    return builder.ToString();
}

String CodeGenerator::Emitter::EmitEnumerationDeclarations(
    /* [in] */ como::MetaNamespace* mn)
{
    StringBuilder builder;

    for (int i = 0; i < mn->mEnumerationNumber; i++) {
        como::MetaEnumeration* me = mComponent->mEnumerations[mn->mEnumerationIndexes[i]];
        builder.Append(EmitEnumerationDeclaration(me));
        builder.Append("\n");
    }

    return builder.ToString();
}

String CodeGenerator::Emitter::EmitEnumerationDeclaration(
    /* [in] */ como::MetaEnumeration* me)
{
    StringBuilder builder;

    builder.AppendFormat("enum class %s\n{\n", me->mName);
    int value = 0;
    for (int i = 0; i < me->mEnumeratorNumber; i++, value++) {
        como::MetaEnumerator* mr = me->mEnumerators[i];
        builder.Append(Properties::INDENT).Append(mr->mName);
        if (mr->mValue != value) {
            builder.AppendFormat(" = %d", mr->mValue);
            value = mr->mValue;
        }
        builder.Append(",\n");
    }
    builder.Append("};\n");

    return builder.ToString();
}

String CodeGenerator::Emitter::EmitInterfaceIDForwardDeclarations(
    /* [in] */ como::MetaNamespace* mn)
{
    StringBuilder builder;

    for (int i = 0; i < mn->mInterfaceNumber; i++) {
        como::MetaInterface* mi = mComponent->mInterfaces[mn->mInterfaceIndexes[i]];
        if (mi->mProperties & TYPE_EXTERNAL) {
            continue;
        }
        builder.AppendFormat("extern const InterfaceID IID_%s;\n", mi->mName);
    }

    return builder.ToString();
}

String CodeGenerator::Emitter::EmitInterfaceForwardDeclarations(
    /* [in] */ como::MetaNamespace* mn)
{
    StringBuilder builder;

    for (int i = 0; i < mn->mInterfaceNumber; i++) {
        como::MetaInterface* mi = mComponent->mInterfaces[mn->mInterfaceIndexes[i]];
        if (mi->mProperties & TYPE_EXTERNAL) {
            continue;
        }
        builder.AppendFormat("interface %s;\n", mi->mName);
    }

    return builder.ToString();
}

String CodeGenerator::Emitter::EmitInterfaceDeclarations(
    /* [in] */ como::MetaNamespace* mn)
{
    StringBuilder builder;

    for (int i = 0; i < mn->mInterfaceNumber; i++) {
        como::MetaInterface* mi = mComponent->mInterfaces[mn->mInterfaceIndexes[i]];
        if (mi->mProperties & TYPE_EXTERNAL) {
            continue;
        }
        builder.Append(EmitInterfaceDeclaration(mi, ""));
        builder.Append("\n");
    }

    return builder.ToString();
}

String CodeGenerator::Emitter::EmitInterfaceDeclaration(
    /* [in] */ como::MetaInterface* mi,
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).AppendFormat("INTERFACE_ID(%s)\n",
            UUID::Parse(mi->mUuid)->Dump().string());
    builder.Append(prefix).AppendFormat("interface %s\n", mi->mName);
    builder.Append(prefix + Properties::INDENT).AppendFormat(": public %s\n",
            mComponent->mInterfaces[mi->mBaseInterfaceIndex]->mName);
    builder.Append(prefix).Append("{\n");
    for (int i = 0; i < mi->mNestedInterfaceNumber; i++) {
        como::MetaInterface* nmi = mComponent->mInterfaces[mi->mNestedInterfaceIndexes[i]];
        builder.Append(prefix + Properties::INDENT).AppendFormat("static const InterfaceID IID_%s;\n\n", nmi->mName);
        builder.Append(EmitInterfaceDeclaration(nmi, prefix + Properties::INDENT));
    }
    builder.Append(prefix + Properties::INDENT).Append("using IInterface::Probe;\n\n");
    builder.Append(prefix + Properties::INDENT).AppendFormat("inline static %s* Probe(\n", mi->mName);
    builder.Append(prefix + Properties::INDENT + Properties::INDENT).Append("/* [in] */ IInterface* object)\n");
    builder.Append(prefix + Properties::INDENT).Append("{\n");
    builder.Append(prefix + Properties::INDENT + Properties::INDENT).Append("if (object == nullptr) {\n");
    builder.Append(prefix + Properties::INDENT + Properties::INDENT + Properties::INDENT).Append("return nullptr;\n");
    builder.Append(prefix + Properties::INDENT + Properties::INDENT).Append("}\n");
    builder.Append(prefix + Properties::INDENT + Properties::INDENT).AppendFormat("return (%s*)object->Probe(IID_%s);\n", mi->mName, mi->mName);
    builder.Append(prefix + Properties::INDENT).Append("}\n");
    builder.Append("\n");
    builder.Append(prefix + Properties::INDENT).Append("inline static const InterfaceID& GetInterfaceID()\n");
    builder.Append(prefix + Properties::INDENT).Append("{\n");
    builder.Append(prefix + Properties::INDENT + Properties::INDENT).AppendFormat("return IID_%s;\n", mi->mName);
    builder.Append(prefix + Properties::INDENT).Append("}\n");
    builder.Append("\n");

    for (int i = 0; i < mi->mConstantNumber; i++) {
        builder.Append(EmitInterfaceConstant(mi->mConstants[i], prefix + Properties::INDENT));
    }
    if (mi->mConstantNumber > 0 && mi->mMethodNumber > 0) {
        builder.Append("\n");
    }
    for (int i = 0; i < mi->mMethodNumber; i++) {
        builder.Append(EmitInterfaceMethod(mi->mMethods[i], prefix + Properties::INDENT));
        if (i != mi->mMethodNumber - 1) {
            builder.Append("\n");
        }
    }
    builder.Append(prefix).Append("};\n");

    return builder.ToString();
}

String CodeGenerator::Emitter::EmitInterfaceConstant(
    /* [in] */ como::MetaConstant* mc,
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    como::MetaType* mt = mComponent->mTypes[mc->mTypeIndex];
    if (mt->mKind == como::TypeKind::String || mt->mKind == como::TypeKind::Float ||
            mt->mKind == como::TypeKind::Double) {
        builder.Append(prefix).AppendFormat("static const %s %s;\n", EmitType(mt, MODE_VARIABLE).string(),
                mc->mName);
    }
    else {
        builder.Append(prefix).AppendFormat("static constexpr %s %s = %s;\n", EmitType(mt, MODE_VARIABLE).string(),
                mc->mName, EmitValue(mt, &mc->mValue).string());
    }

    return builder.ToString();
}

String CodeGenerator::Emitter::EmitInterfaceMethod(
    /* [in] */ como::MetaMethod* mm,
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).AppendFormat("virtual ECode %s(", mm->mName);
    for (int i = 0; i < mm->mParameterNumber; i++) {
        builder.Append("\n");
        builder.Append(prefix + Properties::INDENT).AppendFormat("%s",
                EmitParameter(mm->mParameters[i]).string());
        if (i != mm->mParameterNumber - 1) {
            builder.Append(",");
        }
    }
    builder.Append(") = 0;\n");

    return builder.ToString();
}

String CodeGenerator::Emitter::EmitConstantsInCpp(
    /* [in] */ como::MetaNamespace* mn)
{
    if (mn->mConstantNumber == 0) {
        return nullptr;
    }

    StringBuilder builder;

    bool needBlankLine = false;
    for (int i = 0; i < mn->mConstantNumber; i++) {
        como::MetaConstant* mc = mComponent->mConstants[mn->mConstantIndexes[i]];
        como::MetaType* mt = mComponent->mTypes[mc->mTypeIndex];
        if (mt->mKind == como::TypeKind::String) {
            builder.AppendFormat("const %s %s(%s);\n", EmitType(mt, MODE_VARIABLE).string(),
                    mc->mName, EmitValue(mt, &mc->mValue).string());
            needBlankLine = true;
        }
        else if ((mt->mKind == como::TypeKind::Float || mt->mKind == como::TypeKind::Double)) {
            builder.AppendFormat("const %s %s = %s;\n", EmitType(mt, MODE_VARIABLE).string(),
                    mc->mName, EmitValue(mt, &mc->mValue).string());
            needBlankLine = true;
        }
    }
    if (needBlankLine) {
        builder.Append("\n");
    }

    return builder.ToString();
}

String CodeGenerator::Emitter::EmitInterfaceIDsInCpp(
    /* [in] */ como::MetaNamespace* mn)
{
    if (mn->mInterfaceNumber == 0) {
        return nullptr;
    }

    StringBuilder builder;

    bool needBlankLine = false;
    for (int i = 0; i < mn->mInterfaceNumber; i++) {
        como::MetaInterface* mi = mComponent->mInterfaces[mn->mInterfaceIndexes[i]];
        if (mi->mProperties & TYPE_EXTERNAL) {
            continue;
        }
        if (mn->mInterfaceWrappedIndex == -1) {
            builder.AppendFormat("COM_PUBLIC extern const InterfaceID IID_%s =\n", mi->mName);
            builder.Append(Properties::INDENT + Properties::INDENT).AppendFormat("{%s, &CID_%s};\n",
                    UUID::Parse(mi->mUuid)->ToString().string(), mComponent->mName);
        }
        else {
            String ns = mn->mName;
            ns = ns.Substring(ns.LastIndexOf("::") + 2, ns.GetLength());
            builder.AppendFormat("COM_PUBLIC extern const InterfaceID %s::IID_%s =\n",
                    ns.string(), mi->mName);
            builder.Append(Properties::INDENT + Properties::INDENT).AppendFormat("{%s, &CID_%s};\n",
                    UUID::Parse(mi->mUuid)->ToString().string(), mComponent->mName);
        }
        needBlankLine = true;
    }
    if (needBlankLine) {
        builder.Append("\n");
    }

    return builder.ToString();
}

String CodeGenerator::Emitter::EmitParameter(
    /* [in] */ como::MetaParameter* mp)
{
    StringBuilder builder;

    if (mp->mProperties & PARAMETER_IN) {
        if (mp->mProperties & PARAMETER_OUT) {
            builder.Append("/* [in, out] */ ");
        }
        else {
            builder.Append("/* [in] */ ");
        }
    }
    else if (mp->mProperties & PARAMETER_OUT) {
        if (mp->mProperties & PARAMETER_CALLEE) {
            builder.Append("/* [out, callee] */ ");
        }
        else {
            builder.Append("/* [out] */ ");
        }
    }
    como::MetaType* mt = mComponent->mTypes[mp->mTypeIndex];
    builder.AppendFormat("%s %s", EmitType(mt, mp->mProperties).string(), mp->mName);
    if (mp->mProperties & PARAMETER_VALUE_DEFAULT) {
        como::MetaValue* value = reinterpret_cast<como::MetaValue*>(ALIGN((uintptr_t)mp + sizeof(como::MetaParameter)));
        builder.AppendFormat(" = %s", EmitValue(mt, value).string());
    }

    return builder.ToString();
}

String CodeGenerator::Emitter::EmitType(
    /* [in] */ como::MetaType* mt,
    /* [in] */ int mode)
{
    StringBuilder builder;

    switch (mt->mKind) {
        case como::TypeKind::Char:
            builder.Append("Char");
            break;
        case como::TypeKind::Byte:
            builder.Append("Byte");
            break;
        case como::TypeKind::Short:
            builder.Append("Short");
            break;
        case como::TypeKind::Integer:
            builder.Append("Integer");
            break;
        case como::TypeKind::Long:
            builder.Append("Long");
            break;
        case como::TypeKind::Float:
            builder.Append("Float");
            break;
        case como::TypeKind::Double:
            builder.Append("Double");
            break;
        case como::TypeKind::Boolean:
            builder.Append("Boolean");
            break;
        case como::TypeKind::String:
            if (mode == MODE_PARAMETER_IN) {
                builder.Append("const String&");
            }
            else {
                builder.Append("String");
            }
            break;
        case como::TypeKind::CoclassID:
            if (mode == MODE_PARAMETER_IN) {
                builder.Append("const CoclassID&");
            }
            else {
                builder.Append("CoclassID");
            }
            break;
        case como::TypeKind::ComponentID:
            if (mode == MODE_PARAMETER_IN) {
                builder.Append("const ComponentID&");
            }
            else {
                builder.Append("ComponentID");
            }
            break;
        case como::TypeKind::InterfaceID:
            if (mode == MODE_PARAMETER_IN) {
                builder.Append("const InterfaceID&");
            }
            else {
                builder.Append("InterfaceID");
            }
            break;
        case como::TypeKind::HANDLE:
            builder.Append("HANDLE");
            break;
        case como::TypeKind::ECode:
            builder.Append("ECode");
            break;
        case como::TypeKind::Enum:
            builder.AppendFormat("%s::%s",
                    mComponent->mEnumerations[mt->mIndex]->mNamespace,
                    mComponent->mEnumerations[mt->mIndex]->mName);
            break;
        case como::TypeKind::Array: {
            como::MetaType* emt = mComponent->mTypes[mt->mIndex];
            if (mode == MODE_PARAMETER_IN) {
                builder.AppendFormat("const Array<%s>&",
                        EmitType(emt, MODE_VARIABLE).string());
            }
            else if (mode == (MODE_PARAMETER_IN | MODE_PARAMETER_OUT) ||
                     mode == MODE_PARAMETER_OUT) {
                builder.AppendFormat("Array<%s>&",
                        EmitType(emt, MODE_VARIABLE).string());
            }
            else if (mode == (MODE_PARAMETER_OUT | PARAMETER_CALLEE)) {
                builder.AppendFormat("Array<%s>",
                        EmitType(emt, MODE_VARIABLE).string());
            }
            else if (mode == MODE_VARIABLE) {
                builder.AppendFormat("Array<%s>",
                        EmitType(emt, MODE_VARIABLE).string());
            }
            break;
        }
        case como::TypeKind::Interface: {
            como::MetaInterface* mi = mComponent->mInterfaces[mt->mIndex];
            if (mt->mProperties & TYPE_REFERENCE) {
                builder.AppendFormat("AutoPtr<%s::%s>", mi->mNamespace, mi->mName);
            }
            else {
                builder.AppendFormat("%s::%s", mi->mNamespace, mi->mName);
            }
            break;
        }
        case como::TypeKind::Triple:
            if (mode == MODE_PARAMETER_IN) {
                builder.Append("const Triple&");
            }
            else {
                builder.Append("Triple");
            }
            break;
        default:
            break;
    }

    if (mt->mProperties & TYPE_REFERENCE) {
        builder.Append("&");
    }

    int N = (mt->mProperties & TYPE_POINTER_NUMBER_MASK) >> 2;
    for (int i = 0; i < N; i++) {
        builder.Append("*");
    }

    return builder.ToString();
}

String CodeGenerator::Emitter::EmitValue(
    /* [in] */ como::MetaType* mt,
    /* [in] */ como::MetaValue* mv)
{
    int N = (mt->mProperties & TYPE_POINTER_NUMBER_MASK) >> 2;
    if (N > 0) {
        return mv->mIntegralValue == 0
                ? "nullptr"
                : String::Format("0x%16x", mv->mIntegralValue);
    }

    switch (mt->mKind) {
        case como::TypeKind::Char:
            if (32 <= mv->mIntegralValue && mv->mIntegralValue <= 126) {
                return String::Format("\'%c\'", (char)mv->mIntegralValue);
            }
            else {
                return String::Format("0x%x", (int)mv->mIntegralValue);
            }
        case como::TypeKind::Byte: {
            int radix = mv->mProperties & VALUE_RADIX_MASK;
            const char* format = radix == 8
                    ? "%o"
                    : radix == 10 ? "%d" : "0x%x";
            return String::Format(format, (char)mv->mIntegralValue);
        }
        case como::TypeKind::Short: {
            int radix = mv->mProperties & VALUE_RADIX_MASK;
            const char* format = radix == 8
                    ? "%o"
                    : radix == 10 ? "%d" : "0x%x";
            return String::Format(format, (short)mv->mIntegralValue);
        }
        case como::TypeKind::Integer: {
            int radix = mv->mProperties & VALUE_RADIX_MASK;
            const char* format = radix == 8
                    ? "%o"
                    : radix == 10 ? "%d" : "0x%x";
            return String::Format(format, (int)mv->mIntegralValue);
        }
        case como::TypeKind::Long: {
            int radix = mv->mProperties & VALUE_RADIX_MASK;
            const char* format = radix == 8
                    ? "%lloll"
                    : radix == 10 ? "%lldll" : "0x%llxll";
            return String::Format(format, mv->mIntegralValue);
        }
        case como::TypeKind::Float: {
            if (mv->mProperties & VALUE_POSITIVE_INFINITY) {
                return String("1.0f / 0.0f");
            }
            else if (mv->mProperties & VALUE_NEGATIVE_INFINITY) {
                return String("-1.0f / 0.0f");
            }
            else if (mv->mProperties & VALUE_NAN) {
                return String("0.0f / 0.0f");
            }
            return String::Format(mv->mProperties & VALUE_SCIENTIFIC_NOTATION
                    ? "%e" : "%f", (float)mv->mFloatingPointValue);
        }
        case como::TypeKind::Double: {
            if (mv->mProperties & VALUE_POSITIVE_INFINITY) {
                return String("1.0d / 0.0d");
            }
            else if (mv->mProperties & VALUE_NEGATIVE_INFINITY) {
                return String("-1.0d / 0.0d");
            }
            else if (mv->mProperties & VALUE_NAN) {
                return String("0.0d / 0.0d");
            }
            return String::Format(mv->mProperties & VALUE_SCIENTIFIC_NOTATION
                    ? "%.16e" : "%f", mv->mFloatingPointValue);
        }
        case como::TypeKind::Boolean:
            return mv->mBooleanValue ? String("true") : String("false");
        case como::TypeKind::String:
            return String::Format("\"%s\"", mv->mStringValue);
        case como::TypeKind::Enum:
            return String::Format("%s::%s",
                    EmitType(mt, MODE_VARIABLE).string(), mv->mStringValue);
        case como::TypeKind::ECode:
            return String::Format("0x%08x", (int)mv->mIntegralValue);
        case como::TypeKind::HANDLE:
            return String::Format("0x%llx", mv->mIntegralValue);
        case como::TypeKind::Array:
        case como::TypeKind::CoclassID:
        case como::TypeKind::ComponentID:
        case como::TypeKind::InterfaceID:
        case como::TypeKind::Interface:
        case como::TypeKind::Triple:
        default:
            break;
    }

    return nullptr;
}

}
