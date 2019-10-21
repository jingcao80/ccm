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
#include <set>
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
    switch (mMode & Properties::BUILD_MODE_MASK) {
        case Properties::BUILD_MODE_CLIENT:
            emitter = new ClientModeEmitter(this);
            break;
        case Properties::BUILD_MODE_COMPONENT:
            emitter = new ComponentModeEmitter(this);
            break;
        case Properties::BUILD_MODE_RUNTIME:
            emitter = new RuntimeModeEmitter(this);
            break;
        default:
            Logger::E(TAG, "Code generation mode not supported.");
            return;
    }

    emitter->Emit();
}

//=============================================================================

void CodeGenerator::ComponentModeEmitter::Emit()
{
    EmitConstantsAndTypes();
    if (mOwner->mMode & Properties::CODEGEN_INTERFACE_SPLIT) {
        EmitInterfaceDeclarationsSplitly();
    }
    EmitCoclasses();
}

void CodeGenerator::ComponentModeEmitter::EmitConstantsAndTypes()
{
    String path = String::Format("%s/%s.h", mOwner->mDirectory.string(), mComponent->mName);
    File file(path, File::WRITE);

    como::MetaComponent* mc = mComponent;

    StringBuilder builder;

    builder.Append(mOwner->mLicense);
    builder.Append("\n");
    String defMacro = String::Format("__%s_H_GEN__", mc->mName).ToUpperCase();
    builder.AppendFormat("#ifndef %s\n", defMacro.string());
    builder.AppendFormat("#define %s\n", defMacro.string());
    builder.Append("\n");
    builder.Append("#include <comotypes.h>\n");
    builder.Append("\n");
    builder.Append("using namespace como;\n");
    builder.Append("\n");
    builder.AppendFormat("extern const ComponentID CID_%s;\n", mc->mName);
    builder.Append("\n");
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        como::MetaNamespace* mn = mc->mNamespaces[i];
        builder.Append(EmitConstantsAndTypeForwardDeclarationsRecursively(mn));
    }
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        como::MetaNamespace* mn = mc->mNamespaces[i];
        builder.Append(EmitConstantsAndTypeDeclarationsRecursively(mn));
    }
    builder.Append("extern void AddComponentRefCount();\n");
    builder.Append("extern void ReleaseComponentRefCount();\n");
    builder.Append("\n");
    builder.AppendFormat("#endif // %s\n", defMacro.string());

    String data = builder.ToString();
    file.Write(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

String CodeGenerator::ComponentModeEmitter::EmitConstantsAndTypeForwardDeclarationsRecursively(
    /* [in] */ como::MetaNamespace* mn)
{
    StringBuilder builder;

    if (!(mOwner->mMode & Properties::CODEGEN_INTERFACE_SPLIT)) {
        if (!(mn->mInterfaceWrappedIndex != -1 || mn->mConstantNumber +
            (mn->mEnumerationNumber - mn->mExternalEnumerationNumber) +
            (mn->mInterfaceNumber - mn->mExternalInterfaceNumber) == 0)) {
            builder.Append(EmitNamespaceBegin(mn->mName));

            String infos = EmitConstantsInHeader(mn);
            if (!infos.IsEmpty()) {
                builder.Append(infos);
                builder.Append("\n");
            }

            infos = EmitEnumerationForwardDeclarations(mn);
            if (!infos.IsEmpty()) {
                builder.Append(infos);
                builder.Append("\n");
            }

            infos = EmitInterfaceIDForwardDeclarations(mn);
            if (!infos.IsEmpty()) {
                builder.Append(infos);
                builder.Append("\n");
            }

            infos = EmitInterfaceForwardDeclarations(mn);
            if (!infos.IsEmpty()) {
                builder.Append(infos);
                builder.Append("\n");
            }

            builder.Append(EmitNamespaceEnd(mn->mName));
            builder.Append("\n");
        }
    }
    else {
        if (!(mn->mInterfaceWrappedIndex != -1 || mn->mConstantNumber +
            (mn->mEnumerationNumber - mn->mExternalEnumerationNumber) +
            (mn->mInterfaceNumber - mn->mExternalInterfaceNumber) == 0)) {
            builder.Append(EmitNamespaceBegin(mn->mName));

            String infos = EmitConstantsInHeader(mn);
            if (!infos.IsEmpty()) {
                builder.Append(infos);
                builder.Append("\n");
            }

            infos = EmitEnumerationForwardDeclarations(mn);
            if (!infos.IsEmpty()) {
                builder.Append(infos);
                builder.Append("\n");
            }

            infos = EmitInterfaceForwardDeclarations(mn);
            if (!infos.IsEmpty()) {
                builder.Append(infos);
                builder.Append("\n");
            }

            builder.Append(EmitNamespaceEnd(mn->mName));
            builder.Append("\n");
        }
    }

    for (int i = 0; i < mn->mNamespaceNumber; i++) {
        como::MetaNamespace* nmn = mn->mNamespaces[i];
        builder.Append(EmitConstantsAndTypeForwardDeclarationsRecursively(nmn));
    }

    return builder.ToString();
}

String CodeGenerator::ComponentModeEmitter::EmitConstantsAndTypeDeclarationsRecursively(
    /* [in] */ como::MetaNamespace* mn)
{
    StringBuilder builder;

    if (!(mOwner->mMode & Properties::CODEGEN_INTERFACE_SPLIT)) {
        if (!(mn->mInterfaceWrappedIndex != -1 || mn->mConstantNumber +
            (mn->mEnumerationNumber - mn->mExternalEnumerationNumber) +
            (mn->mInterfaceNumber - mn->mExternalInterfaceNumber) == 0)) {
            builder.Append(EmitNamespaceBegin(mn->mName));

            String infos = EmitEnumerationDeclarations(mn);
            if (!infos.IsEmpty()) {
                builder.Append(infos);
            }

            infos = EmitInterfaceDeclarations(mn);
            if (!infos.IsEmpty()) {
                builder.Append(infos);
            }

            builder.Append(EmitNamespaceEnd(mn->mName));
            builder.Append("\n");
        }
    }
    else {
        if (!(mn->mInterfaceWrappedIndex != -1 || mn->mConstantNumber +
            (mn->mEnumerationNumber - mn->mExternalEnumerationNumber) == 0)) {
            builder.Append(EmitNamespaceBegin(mn->mName));

            String infos = EmitEnumerationDeclarations(mn);
            if (!infos.IsEmpty()) {
                builder.Append(infos);
            }

            builder.Append(EmitNamespaceEnd(mn->mName));
            builder.Append("\n");
        }
    }

    for (int i = 0; i < mn->mNamespaceNumber; i++) {
        como::MetaNamespace* nmn = mn->mNamespaces[i];
        builder.Append(EmitConstantsAndTypeDeclarationsRecursively(nmn));
    }

    return builder.ToString();
}

void CodeGenerator::ComponentModeEmitter::EmitInterfaceDeclarationsSplitly()
{
    como::MetaComponent* mc = mComponent;
    for (int i = 0; i < mc->mInterfaceNumber; i++) {
        como::MetaInterface* mi = mc->mInterfaces[i];
        if ((mi->mProperties & TYPE_EXTERNAL) || mi->mOuterInterfaceIndex != -1) {
            continue;
        }
        EmitInterfaceDeclarationSplitly(mi);
    }
}

void CodeGenerator::ComponentModeEmitter::EmitInterfaceDeclarationSplitly(
    /* [in] */ como::MetaInterface* mi)
{
    String path = String::Format("%s/%s.%s.h", mOwner->mDirectory.string(),
            String(mi->mNamespace).Replace("::", ".").string(), mi->mName);
    File file(path, File::WRITE);

    StringBuilder builder;

    builder.Append(mOwner->mLicense);
    builder.Append("\n");

    String defMacro = EmitDefineMacro(
            String::Format("%s::%s_H_GEN", mi->mNamespace, mi->mName));
    builder.AppendFormat("#ifndef %s\n", defMacro.string());
    builder.AppendFormat("#define %s\n", defMacro.string());
    builder.Append("\n");
    builder.AppendFormat("#include \"%s.h\"\n", mComponent->mName);
    como::MetaInterface* base = mComponent->mInterfaces[mi->mBaseInterfaceIndex];
    if (!String(base->mName).Equals("IInterface")) {
        String baseFile = String::Format("%s.%s.h",
                String(base->mNamespace).Replace("::", ".").string(), base->mName);
        builder.AppendFormat("#include \"%s\"\n", baseFile.string());
    }

    builder.Append(EmitIncludeForUsingNestedInterface(mi));
    builder.Append("\n");
    builder.Append("using namespace como;\n");
    builder.Append("\n");

    builder.Append(EmitNamespaceBegin(mi->mNamespace));
    builder.AppendFormat("extern const InterfaceID IID_%s;\n", mi->mName);
    builder.Append("\n");
    builder.Append(EmitInterfaceDeclaration(mi, ""));
    builder.Append(EmitNamespaceEnd(mi->mNamespace));
    builder.Append("\n");

    builder.AppendFormat("#endif // %s\n", defMacro.string());

    String data = builder.ToString();
    file.Write(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

void CodeGenerator::ComponentModeEmitter::EmitCoclasses()
{
    como::MetaComponent* mc = mComponent;
    for (int i = 0; i < mc->mCoclassNumber; i++) {
        EmitCoclass(mc->mCoclasses[i]);
    }
}

void CodeGenerator::ComponentModeEmitter::EmitCoclass(
    /* [in] */ como::MetaCoclass* mk)
{
    EmitCoclassHeader(mk);
    EmitCoclassCpp(mk);
}

void CodeGenerator::ComponentModeEmitter::EmitCoclassHeader(
    /* [in] */ como::MetaCoclass* mk)
{
    String path = String::Format("%s/_%s.%s.h", mOwner->mDirectory.string(),
            String(mk->mNamespace).Replace("::", "_").string(), mk->mName);
    File file(path, File::WRITE);

    StringBuilder builder;

    builder.Append(mOwner->mLicense);
    builder.Append("\n");

    String defMacro = EmitDefineMacro(
            String::Format("%s::%s_H_GEN", mk->mNamespace, mk->mName));
    builder.AppendFormat("#ifndef %s\n", defMacro.string());
    builder.AppendFormat("#define %s\n", defMacro.string());
    builder.Append("\n");
    builder.AppendFormat("#include \"%s.h\"\n", mComponent->mName);

    std::set<String, StringCompareFunc> includes;

    como::MetaInterface* mi = mComponent->mInterfaces[mk->mInterfaceIndexes[mk->mInterfaceNumber - 1]];
    int fromIndex = String(mi->mName).Equals("IClassObject") ? 2 : 0;

    for (int i = fromIndex; i < mi->mMethodNumber; i++) {
        como::MetaMethod* mm = mi->mMethods[i];
        if ((mm->mProperties & METHOD_DELETED) ||
                (!(mk->mProperties & COCLASS_CONSTRUCTOR_DEFAULT) &&
                    (mk->mProperties & COCLASS_CONSTRUCTOR_DELETED))) {
            continue;
        }
        for (int j = 0; j < mm->mParameterNumber; j++) {
            como::MetaParameter* mp = mm->mParameters[j];
            como::MetaType* mt = mComponent->mTypes[mp->mTypeIndex];
            while (mt->mKind == como::TypeKind::Array) {
                mt = mComponent->mTypes[mt->mIndex];
            }
            if (mt->mKind == como::TypeKind::Interface) {
                como::MetaInterface* mmi = mComponent->mInterfaces[mt->mIndex];
                if (mmi->mOuterInterfaceIndex != -1) {
                    while (mmi->mOuterInterfaceIndex != -1) {
                        mmi = mComponent->mInterfaces[mmi->mOuterInterfaceIndex];
                    }
                    String include = String::Format("#include \"%s.%s.h\"\n",
                            String(mmi->mNamespace).Replace("::", ".").string(), mmi->mName);
                    includes.insert(include);
                }
            }
        }
    }

    for (String include : includes) {
        builder.Append(include);
    }

    builder.Append("\n");
    builder.Append("using namespace como;\n");
    builder.Append("\n");

    builder.Append(EmitNamespaceBegin(mk->mNamespace));
    builder.AppendFormat("extern const CoclassID CID_%s;\n\n", mk->mName);
    builder.AppendFormat("COCLASS_ID(%s)\n", UUID::Parse(mk->mUuid)->Dump().string());
    builder.AppendFormat("class _%s\n", mk->mName);
    builder.Append("{\n");
    builder.Append("public:\n");
    builder.Append(Properties::INDENT).AppendFormat("_%s();\n", mk->mName);
    builder.Append("\n");
    builder.Append(Properties::INDENT).AppendFormat("virtual ~_%s();\n", mk->mName);
    builder.Append("\n");

    for (int i = fromIndex; i < mi->mMethodNumber; i++) {
        como::MetaMethod* mm = mi->mMethods[i];
        if ((mm->mProperties & METHOD_DELETED) ||
                (!(mk->mProperties & COCLASS_CONSTRUCTOR_DEFAULT) &&
                    (mk->mProperties & COCLASS_CONSTRUCTOR_DELETED))) {
            continue;
        }
        builder.Append(Properties::INDENT).Append("static ECode New(\n");
        for (int j = 0; j < mm->mParameterNumber; j++) {
            builder.Append(Properties::INDENT + Properties::INDENT).AppendFormat(EmitParameter(mm->mParameters[j]));
            if (j != mm->mParameterNumber - 1) {
                builder.Append(",\n");
            }
        }
        builder.Append(");\n");
        if (i != mi->mMethodNumber - 1) {
            builder.Append("\n");
        }
    }
    builder.Append("};\n");
    builder.Append("\n");
    builder.Append(EmitNamespaceEnd(mk->mNamespace));

    builder.Append("\n");
    builder.AppendFormat("#endif // %s\n", defMacro.string());

    String data = builder.ToString();
    file.Write(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

void CodeGenerator::ComponentModeEmitter::EmitCoclassCpp(
    /* [in] */ como::MetaCoclass* mk)
{
    String path = String::Format("%s/_%s.%s.cpp", mOwner->mDirectory.string(),
            String(mk->mNamespace).Replace("::", "_").string(), mk->mName);
    File file(path, File::WRITE);

    StringBuilder builder;

    builder.Append(mOwner->mLicense);
    builder.Append("\n");
    builder.AppendFormat("#include \"%s.h\"\n", mk->mName);
    como::MetaInterface* mi = mComponent->mInterfaces[mk->mInterfaceIndexes[mk->mInterfaceNumber - 1]];
    if (!String(mi->mName).Equals("IClassObject")) {
        String classObjectInterfaceHeader = String::Format("%s.%s.h",
                String(mi->mNamespace).Replace("::", ".").string(), mi->mName);
        builder.AppendFormat("#include \"%s\"\n", classObjectInterfaceHeader.string());
    }
    builder.Append("#include <comoapi.h>\n");
    builder.Append("#include <comoclsobj.h>\n");
    builder.Append("#include <comolock.h>\n");
    builder.Append("#include <cstdlib>\n");
    builder.Append("#include <new>\n");
    builder.Append("\n");
    builder.Append("using namespace como;\n");
    builder.Append("\n");

    builder.Append(EmitNamespaceBegin(mk->mNamespace));
    builder.Append(EmitCoclassObject(mk));
    builder.Append(EmitCoclassMethods(mk));
    builder.Append(EmitNamespaceEnd(mk->mNamespace));

    String data = builder.ToString();
    file.Write(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

String CodeGenerator::ComponentModeEmitter::EmitCoclassObject(
    /* [in] */ como::MetaCoclass* mk)
{
    StringBuilder builder;

    builder.AppendFormat("class %sClassObject\n", mk->mName);
    builder.Append(Properties::INDENT).Append(": public ClassObject\n");
    como::MetaInterface* mi = mComponent->mInterfaces[mk->mInterfaceIndexes[mk->mInterfaceNumber - 1]];
    bool isIClassObject = String(mi->mName).Equals("IClassObject");
    if (!isIClassObject) {
        builder.Append(Properties::INDENT).AppendFormat(", public %s\n", mi->mName);
    }
    builder.Append("{\n");
    builder.Append("public:\n");
    builder.Append(Properties::INDENT).AppendFormat("%sClassObject();\n", mk->mName);
    builder.Append("\n");
    builder.Append(Properties::INDENT).AppendFormat("~%sClassObject();\n", mk->mName);
    builder.Append("\n");
    if (!isIClassObject) {
        builder.Append(Properties::INDENT).Append("COMO_INTERFACE_DECL();\n");
        builder.Append("\n");
    }
}

String CodeGenerator::ComponentModeEmitter::EmitCoclassMethods(
    /* [in] */ como::MetaCoclass* mk)
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
    String path = String::Format("%s/_comointfs.h", mOwner->mDirectory.string());
    File file(path, File::WRITE);

    StringBuilder builder;

    builder.Append(mOwner->mLicense);
    builder.Append("\n");
    builder.Append("#ifndef __COMO_COMORUNTIMEINTERFACES_H_GEN__\n"
                   "#define __COMO_COMORUNTIMEINTERFACES_H_GEN__\n"
                   "\n"
                   "#include \"comosp.h\"\n"
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

    como::MetadataSerializer serializer(mComponent);
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

String CodeGenerator::Emitter::EmitIncludeForUsingNestedInterface(
    /* [in] */ como::MetaInterface* mi)
{
    std::set<String, StringCompareFunc> includes;

    for (int i = 0; i < mi->mNestedInterfaceNumber; i++) {
        como::MetaInterface* nmi = mComponent->mInterfaces[mi->mNestedInterfaceIndexes[i]];
        includes.insert(EmitIncludeForUsingNestedInterface(nmi));
    }

    for (int i = 0; i < mi->mMethodNumber; i++) {
        como::MetaMethod* mm = mi->mMethods[i];
        for (int j = 0; j < mm->mParameterNumber; j++) {
            como::MetaParameter* mp = mm->mParameters[j];
            como::MetaType* mt = mComponent->mTypes[mp->mTypeIndex];
            while (mt->mKind == como::TypeKind::Array) {
                mt = mComponent->mTypes[mt->mIndex];
            }
            if (mt->mKind == como::TypeKind::Interface) {
                como::MetaInterface* mmi = mComponent->mInterfaces[mt->mIndex];
                if (mmi->mOuterInterfaceIndex != -1) {
                    while (mmi->mOuterInterfaceIndex != -1) {
                        mmi = mComponent->mInterfaces[mmi->mOuterInterfaceIndex];
                    }
                    String include = String::Format("#include \"%s.%s.h\"\n",
                            String(mmi->mNamespace).Replace("::", ".").string(), mmi->mName);
                    includes.insert(include);
                }
            }
        }
    }

    StringBuilder builder;

    for (String include : includes) {
        builder.Append(include);
    }

    return builder.ToString();
}

String CodeGenerator::Emitter::EmitNamespaceBegin(
    /* [in] */ const String& nsStr)
{
    String ns = nsStr;

    if (ns.IsEmpty()) {
        return nullptr;
    }

    StringBuilder builder;

    ns = ns + "::";
    int begin = 0;
    int index = ns.IndexOf("::");
    while (index != -1) {
        builder.AppendFormat("namespace %s {\n", ns.Substring(begin, index).string());
        begin = index + 2;
        index = ns.IndexOf("::", begin);
    }
    builder.Append("\n");

    return builder.ToString();
}

String CodeGenerator::Emitter::EmitNamespaceEnd(
    /* [in] */ const String& nsStr)
{
    String ns = nsStr;

    if (ns.IsEmpty()) {
        return nullptr;
    }

    StringBuilder builder;

    ns = "::" + ns;
    int end = ns.GetLength();
    int index = ns.LastIndexOf("::");
    while (index != -1) {
        builder.AppendFormat("} // namespace %s\n", ns.Substring(index + 2, end).string());
        end = index;
        index = ns.LastIndexOf("::", end - 1);
    }

    return builder.ToString();
}

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
        if (me->mProperties & TYPE_EXTERNAL) {
            continue;
        }
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
        if (me->mProperties & TYPE_EXTERNAL) {
            continue;
        }
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
    unsigned char properties = mt->mProperties;

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
                builder.AppendFormat("Array<%s>",
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
            int N = properties & TYPE_NUMBER_MASK;
            if ((properties >> (N * 2)) & TYPE_REFERENCE) {
                builder.AppendFormat("AutoPtr<%s::%s>", mi->mNamespace, mi->mName);
                properties &= ~((TYPE_REFERENCE) << (N * 2));
                properties -= 1;
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

    if ((properties & TYPE_NUMBER_MASK) > 0) {
        int N = properties & TYPE_NUMBER_MASK;
        for (int i = N; i >= 1; i--) {
            if ((properties >> (i * 2)) & TYPE_POINTER) {
                builder.Append("*");
            }
            else {
                builder.Append("&");
            }
        }
    }

    return builder.ToString();
}

String CodeGenerator::Emitter::EmitValue(
    /* [in] */ como::MetaType* mt,
    /* [in] */ como::MetaValue* mv)
{
    int N = mt->mProperties & TYPE_NUMBER_MASK;
    if (N > 0 && (mt->mProperties & (TYPE_POINTER << 2))) {
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

String CodeGenerator::Emitter::EmitDefineMacro(
    /* [in] */ const String& fullName)
{
    StringBuilder builder;

    builder.Append("__");
    builder.Append(fullName.Replace("::", "_").ToUpperCase());
    builder.Append("__");

    return builder.ToString();
}

}
