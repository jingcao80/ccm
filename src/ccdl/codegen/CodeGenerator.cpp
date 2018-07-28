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
#include "../../runtime/metadata/MetaSerializer.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

using ccdl::ast::Parameter;
using ccm::CcmTypeKind;
using ccm::metadata::MetaCoclass;
using ccm::metadata::MetaEnumerator;
using ccm::metadata::MetaNamespace;
using ccm::metadata::MetaSerializer;

namespace ccdl {
namespace codegen {

const String CodeGenerator::TAG("CodeGenerator");

CodeGenerator::CodeGenerator()
    : mSparseMode(false)
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

void CodeGenerator::GenerateOnCcmrtMode()
{
    if (!ResolveDirectory()) return;

    GenConstantsAndTypesOnCcmrtMode();
    GenIds();
    GenMetadataWrapper();
}

void CodeGenerator::GenerateOnComponentMode()
{
    if (!ResolveDirectory()) return;

    GenConstantsAndTypesOnComponentMode();
    if (mSparseMode) {
        GenInterfaceDeclarationsSparsely();
    }
    GenCoclasses();
    GenComponentCpp();
    GenMetadataWrapper();
}

void CodeGenerator::GenerateOnUserMode()
{
    if (!ResolveDirectory()) return;

    GenConstantsAndTypesOnUserMode();
    if (mSparseMode) {
        GenInterfaceDeclarationsSparsely();
        GenCoclassDeclarationsSparselyOnUserMode();
    }
    GenComponentCppOnUserMode();
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

void CodeGenerator::GenConstantsAndTypesOnCcmrtMode()
{
    String filePath = String::Format("%s/ccmintfs.h", mDirectory.string());
    File file(filePath, File::WRITE);

    StringBuilder builder;

    builder.Append(mLicense);
    builder.Append("\n");
    builder.Append("#ifndef __CCM_CCMRUNTIMEINTERFACES_H_GEN__\n"
                   "#define __CCM_CCMRUNTIMEINTERFACES_H_GEN__\n"
                   "\n"
                   "#include \"ccmtypes.h\"\n"
                   "\n"
                   "namespace ccm {\n"
                   "\n");

    MetaComponent* mc = mMetaComponent;
    builder.AppendFormat("COM_PUBLIC extern const ComponentID CID_%s;\n\n", mc->mName);
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        MetaNamespace* mn = mc->mNamespaces[i];
        builder.Append(GenConstantsInHeader(mn));
        if (i != mc->mNamespaceNumber - 1) builder.Append("\n");
        else builder.Append("\n\n");
    }
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        MetaNamespace* mn = mc->mNamespaces[i];
        if (mn->mEnumerationNumber - mn->mExternalEnumerationNumber == 0) {
            continue;
        }
        builder.Append(GenEnumerationPredeclarations(mn));
        if (i != mc->mNamespaceNumber - 1) builder.Append("\n");
        else builder.Append("\n\n");
    }
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        MetaNamespace* mn = mc->mNamespaces[i];
        if (mn->mEnumerationNumber - mn->mExternalEnumerationNumber == 0) {
            continue;
        }
        builder.Append(GenEnumerationDeclarations(mn));
        if (i != mc->mNamespaceNumber - 1) builder.Append("\n");
        else builder.Append("\n\n");
    }
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        MetaNamespace* mn = mc->mNamespaces[i];
        if (mn->mInterfaceNumber - mn->mExternalInterfaceNumber == 0) {
            continue;
        }
        builder.Append(GenInterfaceIDPredeclarations(mn));
        if (i != mc->mNamespaceNumber - 1) builder.Append("\n");
        else builder.Append("\n\n");
    }
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        MetaNamespace* mn = mc->mNamespaces[i];
        if (mn->mInterfaceNumber - mn->mExternalInterfaceNumber == 0) {
            continue;
        }
        builder.Append(GenInterfacePredeclarations(mn));
        if (i != mc->mNamespaceNumber - 1) builder.Append("\n");
        else builder.Append("\n\n");
    }
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        MetaNamespace* mn = mc->mNamespaces[i];
        if (mn->mInterfaceNumber - mn->mExternalInterfaceNumber == 0) {
            continue;
        }
        builder.Append(GenInterfaceDeclarations(mn));
        if (i != mc->mNamespaceNumber - 1) builder.Append("\n");
    }
    builder.Append("\n"
                   "}\n"
                   "\n"
                   "#endif // __CCM_CCMRUNTIMEINTERFACES_H_GEN__");

    String data = builder.ToString();
    file.Write(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

String CodeGenerator::GenConstantsInHeader(
    /* [in] */ MetaNamespace* mn)
{
    StringBuilder builder;

    if (mn->mConstantNumber == 0) return String();

    for (int i = 0; i < mn->mConstantNumber; i++) {
        MetaConstant* mc = mMetaComponent->mConstants[mn->mConstantIndexes[i]];
        builder.Append(GenConstant(mc));
    }
    builder.Append("\n");

    return builder.ToString();
}

String CodeGenerator::GenConstant(
    /* [in] */ MetaConstant* mc)
{
    StringBuilder builder;

    MetaType* mt = mMetaComponent->mTypes[mc->mTypeIndex];
    if (mt->mKind == CcmTypeKind::String) {
        builder.AppendFormat("extern const %s %s;\n", GenType(mt).string(),
                mc->mName);
    }
    else {
        builder.AppendFormat("constexpr %s %s = %s;\n", GenType(mt).string(),
                mc->mName, GenValue(mt, mc->mValue).string());
    }

    return builder.ToString();
}

String CodeGenerator::GenEnumerationPredeclarations(
    /* [in] */ MetaNamespace* mn)
{
    StringBuilder builder;

    if (mn->mEnumerationNumber - mn->mExternalEnumerationNumber == 0) return String();

    for (int i = 0; i < mn->mEnumerationNumber; i++) {
        MetaEnumeration* me = mMetaComponent->mEnumerations[mn->mEnumerationIndexes[i]];
        if (me->mExternal) continue;
        builder.AppendFormat("enum class %s;\n", me->mName);
    }
    builder.Append("\n");

    return builder.ToString();
}

String CodeGenerator::GenEnumerationDeclarations(
    /* [in] */ MetaNamespace* mn)
{
    StringBuilder builder;

    if (mn->mEnumerationNumber - mn->mExternalEnumerationNumber == 0) return String();

    for (int i = 0; i < mn->mEnumerationNumber; i++) {
        MetaEnumeration* me = mMetaComponent->mEnumerations[mn->mEnumerationIndexes[i]];
        if (me->mExternal) continue;
        builder.Append(GenEnumerationDeclaration(me));
        if (i != mn->mEnumerationNumber - 1) builder.Append("\n");
    }
    builder.Append("\n");

    return builder.ToString();
}

String CodeGenerator::GenEnumerationDeclaration(
    /* [in] */ MetaEnumeration* me)
{
    StringBuilder builder;

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

    return builder.ToString();
}

String CodeGenerator::GenInterfaceIDPredeclarations(
    /* [in] */ MetaNamespace* mn)
{
    StringBuilder builder;

    if (mn->mInterfaceNumber - mn->mExternalInterfaceNumber == 0) {
        return String();
    }

    for (int i = 0; i < mn->mInterfaceNumber; i++) {
        MetaInterface* mi = mMetaComponent->mInterfaces[mn->mInterfaceIndexes[i]];
        if (mi->mExternal) continue;
        builder.AppendFormat("extern const InterfaceID IID_%s;\n", mi->mName);
    }

    return builder.ToString();
}

String CodeGenerator::GenInterfacePredeclarations(
    /* [in] */ MetaNamespace* mn)
{
    StringBuilder builder;

    if (mn->mInterfaceNumber - mn->mExternalInterfaceNumber == 0) {
        return String();
    }

    for (int i = 0; i < mn->mInterfaceNumber; i++) {
        MetaInterface* mi = mMetaComponent->mInterfaces[mn->mInterfaceIndexes[i]];
        if (mi->mExternal) continue;
        builder.AppendFormat("interface %s;\n", mi->mName);
    }
    builder.Append("\n");

    return builder.ToString();
}

String CodeGenerator::GenInterfaceDeclarations(
    /* [in] */ MetaNamespace* mn)
{
    StringBuilder builder;

    if (mn->mInterfaceNumber - mn->mExternalInterfaceNumber == 0) {
        return String();
    }

    for (int i = 0; i < mn->mInterfaceNumber; i++) {
        MetaInterface* mi = mMetaComponent->mInterfaces[mn->mInterfaceIndexes[i]];
        if (mi->mExternal) continue;
        builder.Append(GenInterfaceDeclaration(mi));
        if (i != mn->mInterfaceNumber - 1) builder.Append("\n");
    }
    builder.Append("\n");

    return builder.ToString();
}

String CodeGenerator::GenInterfaceDeclaration(
    /* [in] */ MetaInterface* mi)
{
    StringBuilder builder;

    builder.AppendFormat("INTERFACE_ID(%s)\n", Uuid(mi->mUuid).Dump().string());
    builder.AppendFormat("interface %s : public ", mi->mName);
    builder.Append(
            mMetaComponent->mInterfaces[mi->mBaseInterfaceIndex]->mName).Append("\n");
    builder.Append("{\n");
    builder.AppendFormat("    using IInterface::Probe;\n\n"
                         "    inline static %s* Probe(\n"
                         "        /* [in] */ IInterface* object)\n"
                         "    {\n"
                         "        if (object == nullptr) return nullptr;\n"
                         "        return (%s*)object->Probe(IID_%s);\n"
                         "    }\n", mi->mName, mi->mName, mi->mName);
    builder.Append("\n");
    for (int i = 0; i < mi->mConstantNumber; i++) {
        builder.Append(GenInterfaceConstant(mi->mConstants[i]));
    }
    if (mi->mConstantNumber > 0 && mi->mMethodNumber > 0) builder.Append("\n");
    for (int i = 0; i < mi->mMethodNumber; i++) {
        builder.Append(GenInterfaceMethod(mi->mMethods[i]));
        if (i != mi->mMethodNumber - 1) builder.Append("\n");
    }
    builder.Append("};\n\n");

    return builder.ToString();
}

String CodeGenerator::GenInterfaceConstant(
    /* [in] */ MetaConstant* mc)
{
    StringBuilder builder;

    MetaType* mt = mMetaComponent->mTypes[mc->mTypeIndex];
    if ((mt->mKind == CcmTypeKind::String) ||
            ((mt->mKind == CcmTypeKind::Float || mt->mKind == CcmTypeKind::Double) &&
            (mc->mValue.mAttributes & FP_MASK))) {
        builder.AppendFormat("    static const %s %s;\n", GenType(mt).string(),
                mc->mName);
    }
    else {
        builder.AppendFormat("    static constexpr %s %s = %s;\n", GenType(mt).string(),
                mc->mName, GenValue(mt, mc->mValue).string());
    }

    return builder.ToString();
}

String CodeGenerator::GenInterfaceMethod(
    /* [in] */ MetaMethod* mm)
{
    StringBuilder builder;

    builder.AppendFormat("    virtual ECode %s(", mm->mName);
    for (int i = 0; i < mm->mParameterNumber; i++) {
        builder.AppendFormat("\n        %s", GenParameter(mm->mParameters[i]).string());
        if (i != mm->mParameterNumber - 1) builder.Append(",");
    }
    builder.Append(") = 0;\n");

    return builder.ToString();
}

String CodeGenerator::GenParameter(
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
    MetaType* mt = mMetaComponent->mTypes[mp->mTypeIndex];
    builder.AppendFormat("%s %s", GenType(mt, mp->mAttribute).string(), mp->mName);
    if (mp->mHasDefaultValue) {
        builder.AppendFormat(" = %s", GenValue(mt, mp->mDefaultValue).string());
    }

    return builder.ToString();
}

String CodeGenerator::GenType(
    /* [in] */ MetaType* mt,
    /* [in] */ int attr,
    /* [in] */ bool inArray)
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
            if ((attr & Parameter::ATTR_MASK) == Parameter::IN && !inArray) {
                builder.Append("const String&");
            }
            else {
                builder.Append("String");
            }
            break;
        case CcmTypeKind::CoclassID:
            if ((attr & Parameter::ATTR_MASK) == Parameter::IN && !inArray) {
                builder.Append("const CoclassID&");
            }
            else {
                builder.Append("CoclassID");
            }
            break;
        case CcmTypeKind::ComponentID:
            if ((attr & Parameter::ATTR_MASK) == Parameter::IN && !inArray) {
                builder.Append("const ComponentID&");
            }
            else {
                builder.Append("ComponentID");
            }
            break;
        case CcmTypeKind::InterfaceID:
            if ((attr & Parameter::ATTR_MASK) == Parameter::IN && !inArray) {
                builder.Append("const InterfaceID&");
            }
            else {
                builder.Append("InterfaceID");
            }
            break;
        case CcmTypeKind::HANDLE:
            builder.Append("HANDLE");
            break;
        case CcmTypeKind::ECode:
            builder.Append("ECode");
            break;
        case CcmTypeKind::Enum:
            builder.Append(mc->mEnumerations[mt->mIndex]->mName);
            break;
        case CcmTypeKind::Array:
            if ((attr & Parameter::ATTR_MASK) == Parameter::IN) {
                if (mt->mPointerNumber == 0) {
                    builder.AppendFormat("const Array<%s>&",
                        GenType(mc->mTypes[mt->mNestedTypeIndex], attr, true).string());
                }
                else if (mt->mPointerNumber == 1) {
                    builder.AppendFormat("const Array<%s>",
                        GenType(mc->mTypes[mt->mNestedTypeIndex], attr, true).string());
                }
            }
            else if ((attr & Parameter::ATTR_MASK) == Parameter::OUT) {
                builder.AppendFormat("Array<%s>&",
                    GenType(mc->mTypes[mt->mNestedTypeIndex], attr, true).string());
            }
            else if ((attr & Parameter::ATTR_MASK) == (Parameter::OUT | Parameter::CALLEE)) {
                builder.AppendFormat("Array<%s>",
                    GenType(mc->mTypes[mt->mNestedTypeIndex], attr, true).string());
            }
            break;
        case CcmTypeKind::Interface:
            builder.AppendFormat("%s%s",
                    mc->mInterfaces[mt->mIndex]->mNamespace,
                    mc->mInterfaces[mt->mIndex]->mName);
            break;
        default:
            break;
    }

    for (int i = 0; i < mt->mPointerNumber; i++) {
        builder.Append("*");
    }

    return builder.ToString();
}

String CodeGenerator::GenValue(
    /* [in] */ MetaType* mt,
    /* [in] */ MetaValue& mv)
{
    StringBuilder builder;

    if (mt->mPointerNumber > 0) {
        return mv.mLong == 0 ? String("nullptr") :
                String::Format("0x%llx", mv.mLong);
    }

    switch(mt->mKind) {
        case CcmTypeKind::Char:
            return String::Format("\'%c\'", mv.mInteger);
        case CcmTypeKind::Byte: {
            int radix = mv.mAttributes & RADIX_MASK;
            const char* format = radix == 8 ? "%o" :
                    radix == 10 ? "%d" : "0x%x";
            return String::Format(format, (char)mv.mInteger);
        }
        case CcmTypeKind::Short: {
            int radix = mv.mAttributes & RADIX_MASK;
            const char* format = radix == 8 ? "%o" :
                    radix == 10 ? "%d" : "0x%x";
            return String::Format(format, (short)mv.mInteger);
        }
        case CcmTypeKind::Integer: {
            int radix = mv.mAttributes & RADIX_MASK;
            const char* format = radix == 8 ? "%o" :
                    radix == 10 ? "%d" : "0x%x";
            return String::Format(format, mv.mInteger);
        }
        case CcmTypeKind::Long: {
            int radix = mv.mAttributes & RADIX_MASK;
            const char* format = radix == 8 ? "%lloll" :
                    radix == 10 ? "%lldll" : "0x%llxll";
            return String::Format(format, mv.mLong);
        }
        case CcmTypeKind::Float: {
            if (mv.mAttributes & POSITIVE_INFINITY_MASK) {
                return String("1.0f / 0.0f");
            }
            else if (mv.mAttributes & NEGATIVE_INFINITY_MASK) {
                return String("-1.0f / 0.0f");
            }
            else if (mv.mAttributes & NAN_MASK) {
                return String("0.0f / 0.0f");
            }
            return String::Format(mv.mAttributes & SCIENTIFIC_NOTATION_MASK ?
                    "%e" : "%f", mv.mFloat);
        }
        case CcmTypeKind::Double: {
            if (mv.mAttributes & POSITIVE_INFINITY_MASK) {
                return String("1.0d / 0.0d");
            }
            else if (mv.mAttributes & NEGATIVE_INFINITY_MASK) {
                return String("-1.0d / 0.0d");
            }
            else if (mv.mAttributes & NAN_MASK) {
                return String("0.0d / 0.0d");
            }
            return String::Format(mv.mAttributes & SCIENTIFIC_NOTATION_MASK ?
                    "%e" : "%f", mv.mDouble);
        }
        case CcmTypeKind::Boolean:
            return mv.mBoolean ? String("true") : String("false");
        case CcmTypeKind::String:
            return String::Format("\"%s\"", mv.mString);
        case CcmTypeKind::Enum:
            return String::Format("%s::%s", GenType(mt).string(), mv.mString);
        case CcmTypeKind::ECode:
            return String::Format("0x%08x", mv.mInteger);
        case CcmTypeKind::HANDLE:
            return String::Format("%lld", mv.mLong);
        case CcmTypeKind::Array:
        case CcmTypeKind::CoclassID:
        case CcmTypeKind::ComponentID:
        case CcmTypeKind::InterfaceID:
        case CcmTypeKind::Interface:
        default:
            break;
    }

    return builder.ToString();
}

void CodeGenerator::GenIds()
{
    String filePath =
            String::Format("%s/ccmids.cpp", mDirectory.string());
    File file(filePath, File::WRITE);

    StringBuilder builder;

    builder.Append(mLicense);
    builder.Append("\n");
    builder.Append("#include \"ccmtypes.h\"\n"
                   "\n"
                   "namespace ccm {\n"
                   "\n");

    MetaComponent* mc = mMetaComponent;
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        MetaNamespace* mn = mc->mNamespaces[i];
        if (mn->mConstantNumber + mn->mInterfaceNumber == 0) {
            continue;
        }
        builder.Append(GenConstantsInCpp(mn));
        builder.Append(GenInterfaceIDsOnCcmrtMode(mn));
    }
    builder.Append("\n"
                   "}\n"
                   "\n");

    String data = builder.ToString();
    file.Write(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

String CodeGenerator::GenComponentID()
{
    StringBuilder builder;

    MetaComponent* mc = mMetaComponent;
    builder.AppendFormat("extern const ComponentID CID_%s =\n"
                         "        {%s,\n"
                         "        \"%s\"};\n",
            mc->mName, Uuid(mc->mUuid).ToString().string(),
            mc->mUrl);

    return builder.ToString();
}

String CodeGenerator::GenInterfaceIDsOnCcmrtMode(
    /* [in] */ MetaNamespace* mn)
{
    StringBuilder builder;

    if (mn->mInterfaceNumber == 0) return String();

    for (int i = 0; i < mn->mInterfaceNumber; i++) {
        MetaInterface* mi = mMetaComponent->mInterfaces[mn->mInterfaceIndexes[i]];
        if (mi->mExternal) continue;
        builder.AppendFormat("COM_PUBLIC extern const InterfaceID IID_%s =\n"
                             "        {%s, &CID_%s};\n",
                mi->mName, Uuid(mi->mUuid).ToString().string(),
                mMetaComponent->mName);
    }
    builder.Append("\n");

    return builder.ToString();
}

void CodeGenerator::GenConstantsAndTypesOnComponentMode()
{
    MetaComponent* mc = mMetaComponent;
    String filePath = String::Format("%s/%s.h",
            mDirectory.string(), mc->mName);
    File file(filePath, File::WRITE);

    String defMacro = String::Format("__%s_H_GEN__", mc->mName).ToUpperCase();

    StringBuilder builder;

    builder.Append(mLicense);
    builder.Append("\n");
    builder.AppendFormat("#ifndef %s\n"
                         "#define %s\n\n", defMacro.string(), defMacro.string());
    builder.Append("#include <ccmtypes.h>\n\n");
    builder.Append("using namespace ccm;\n\n");

    builder.AppendFormat("extern const ComponentID CID_%s;\n\n", mc->mName);
    if (!mSparseMode) {
        for (int i = 0; i < mc->mNamespaceNumber; i++) {
            MetaNamespace* mn = mc->mNamespaces[i];
            if (mn->mConstantNumber +
                    (mn->mEnumerationNumber - mn->mExternalEnumerationNumber) +
                    (mn->mInterfaceNumber - mn->mExternalInterfaceNumber) == 0) {
                continue;
            }
            builder.Append(GenNamespaceBegin(String(mn->mName)));
            builder.Append(GenConstantsInHeader(mn));
            builder.Append(GenEnumerationPredeclarations(mn));
            builder.Append(GenInterfaceIDPredeclarations(mn));
            builder.Append(GenInterfacePredeclarations(mn));
            builder.Append(GenNamespaceEnd(String(mn->mName)));
            if (i != mc->mNamespaceNumber - 1) builder.Append("\n");
            else builder.Append("\n\n");
        }
        for (int i = 0; i < mc->mNamespaceNumber; i++) {
            MetaNamespace* mn = mc->mNamespaces[i];
            if ((mn->mEnumerationNumber - mn->mExternalEnumerationNumber) +
                    (mn->mInterfaceNumber - mn->mExternalInterfaceNumber) == 0) {
                continue;
            }
            builder.Append(GenNamespaceBegin(String(mn->mName)));
            builder.Append(GenEnumerationDeclarations(mn));
            builder.Append(GenInterfaceDeclarations(mn));
            builder.Append(GenNamespaceEnd(String(mn->mName)));
            if (i != mc->mNamespaceNumber - 1) builder.Append("\n");
            else builder.Append("\n");
        }
    }
    else {
        for (int i = 0; i < mc->mNamespaceNumber; i++) {
            MetaNamespace* mn = mc->mNamespaces[i];
            if (mn->mConstantNumber +
                    (mn->mEnumerationNumber - mn->mExternalEnumerationNumber) +
                    (mn->mInterfaceNumber - mn->mExternalInterfaceNumber) == 0) {
                continue;
            }
            builder.Append(GenNamespaceBegin(String(mn->mName)));
            builder.Append(GenConstantsInHeader(mn));
            builder.Append(GenEnumerationPredeclarations(mn));
            builder.Append(GenInterfacePredeclarations(mn));
            builder.Append(GenNamespaceEnd(String(mn->mName)));
            if (i != mc->mNamespaceNumber - 1) builder.Append("\n");
            else builder.Append("\n\n");
        }
        for (int i = 0; i < mc->mNamespaceNumber; i++) {
            MetaNamespace* mn = mc->mNamespaces[i];
            if ((mn->mEnumerationNumber - mn->mExternalEnumerationNumber) == 0) {
                continue;
            }
            builder.Append(GenNamespaceBegin(String(mn->mName)));
            builder.Append(GenEnumerationDeclarations(mn));
            builder.Append(GenNamespaceEnd(String(mn->mName)));
            if (i != mc->mNamespaceNumber - 1) builder.Append("\n");
            else builder.Append("\n");
        }
    }
    builder.Append("extern void AddComponentCount();\n"
                   "extern void ReleaseComponentCount();\n\n");
    builder.AppendFormat("#endif // %s\n", defMacro.string());

    String data = builder.ToString();
    file.Write(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

String CodeGenerator::GenNamespaceBegin(
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

String CodeGenerator::GenNamespaceEnd(
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

void CodeGenerator::GenInterfaceDeclarationsSparsely()
{
    MetaComponent* mc = mMetaComponent;
    for (int i = 0; i < mc->mInterfaceNumber; i++) {
        MetaInterface* mi = mc->mInterfaces[i];
        if (mi->mExternal) continue;
        GenInterfaceDeclarationSparsely(mc->mInterfaces[i]);
    }
}

void CodeGenerator::GenInterfaceDeclarationSparsely(
    /* [in] */ MetaInterface* mi)
{
    String filePath = String::Format("%s/%s%s.h", mDirectory.string(),
            String(mi->mNamespace).Replace("::", ".").string(), mi->mName);
    File file(filePath, File::WRITE);

    StringBuilder builder;

    builder.Append(mLicense);
    builder.Append("\n");

    String defMacro = GenDefineMacro(
            String::Format("%s%s_H_GEN", mi->mNamespace, mi->mName));
    builder.AppendFormat("#ifndef %s\n", defMacro.string());
    builder.AppendFormat("#define %s\n\n", defMacro.string());
    builder.AppendFormat("#include \"%s.h\"\n", mMetaComponent->mName);
    MetaInterface* bmi = mMetaComponent->mInterfaces[mi->mBaseInterfaceIndex];
    if (!String("IInterface").Equals(bmi->mName)) {
        String bfilePath = String::Format("%s%s.h",
                String(bmi->mNamespace).Replace("::", ".").string(), bmi->mName);
        builder.AppendFormat("#include \"%s\"\n\n", bfilePath.string());
    }
    else {
        builder.Append("\n");
    }

    builder.Append("using namespace ccm;\n\n");

    builder.Append(GenNamespaceBegin(String(mi->mNamespace)));
    builder.AppendFormat("extern const InterfaceID IID_%s;\n\n", mi->mName);
    builder.Append(GenInterfaceDeclaration(mi));
    builder.Append(GenNamespaceEnd(String(mi->mNamespace)));

    builder.AppendFormat("\n#endif // %s\n", defMacro.string());

    String data = builder.ToString();
    file.Write(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

void CodeGenerator::GenCoclasses()
{
    MetaComponent* mc = mMetaComponent;
    for (int i = 0; i < mc->mCoclassNumber; i++) {
        GenCoclass(mc->mCoclasses[i]);
    }
}

void CodeGenerator::GenCoclass(
    /* [in] */ MetaCoclass* mk)
{
    GenCoclassHeader(mk);
    GenCoclassCpp(mk);
}

void CodeGenerator::GenCoclassHeader(
    /* [in] */ MetaCoclass* mk)
{
    String filePath =
            String::Format("%s/_%s%s.h", mDirectory.string(),
            String(mk->mNamespace).Replace("::", "_").string(), mk->mName);
    File file(filePath, File::WRITE);

    StringBuilder builder;

    builder.Append(mLicense);
    builder.Append("\n");

    String defMacro = GenDefineMacro(
            String::Format("%s%s_H_GEN", mk->mNamespace, mk->mName));
    builder.AppendFormat("#ifndef %s\n", defMacro.string());
    builder.AppendFormat("#define %s\n\n", defMacro.string());
    builder.AppendFormat("#include \"%s.h\"\n\n", mMetaComponent->mName);
    builder.Append("using namespace ccm;\n\n");

    builder.Append(GenNamespaceBegin(String(mk->mNamespace)));
    builder.AppendFormat("extern const CoclassID CID_%s;\n\n", mk->mName);
    builder.AppendFormat("COCLASS_ID(%s)\n", Uuid(mk->mUuid).Dump().string());
    builder.AppendFormat("class _%s\n", mk->mName);
    builder.AppendFormat("{\n"
                         "public:\n"
                         "    _%s();\n\n"
                         "    virtual ~_%s();\n\n", mk->mName, mk->mName);
    MetaInterface* mi = mMetaComponent->mInterfaces[mk->mInterfaceIndexes[mk->mInterfaceNumber - 1]];
    bool isIClassObject = String("IClassObject").Equals(mi->mName);
    int start = isIClassObject ? 1 : 0;
    for (int i = start; i < mi->mMethodNumber; i++) {
        MetaMethod* mm = mi->mMethods[i];
        builder.Append("    static ECode New(\n");
        for (int j = 0; j < mm->mParameterNumber; j++) {
            builder.AppendFormat("        %s", GenParameter(mm->mParameters[j]).string());
            if (j != mm->mParameterNumber - 1) builder.Append(",\n");
        }
        builder.Append(");\n");
        if (i != mi->mMethodNumber - 1) builder.Append("\n");
    }
    builder.Append("};\n\n");
    builder.Append(GenNamespaceEnd(String(mk->mNamespace)));

    builder.AppendFormat("\n#endif // %s\n", defMacro.string());

    String data = builder.ToString();
    file.Write(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

void CodeGenerator::GenCoclassCpp(
    /* [in] */ MetaCoclass* mk)
{
    String filePath =
            String::Format("%s/_%s%s.cpp", mDirectory.string(),
            String(mk->mNamespace).Replace("::", "_").string(), mk->mName);
    File file(filePath, File::WRITE);

    StringBuilder builder;

    builder.Append(mLicense);
    builder.Append("\n");
    builder.AppendFormat("#include \"%s.h\"\n", mk->mName);
    MetaInterface* mi = mMetaComponent->mInterfaces[mk->mInterfaceIndexes[mk->mInterfaceNumber - 1]];
    bool isIClassObject = String("IClassObject").Equals(mi->mName);
    if (!isIClassObject) {
        String classObjectInterfaceHeader = String::Format("%s%s.h",
                String(mi->mNamespace).Replace("::", ".").string(), mi->mName);
        builder.AppendFormat("#include \"%s\"\n", classObjectInterfaceHeader.string());
    }
    builder.Append("#include <ccmapi.h>\n"
                   "#include <ccmautoptr.h>\n"
                   "#include <ccmclassobject.h>\n"
                   "#include <ccmobject.h>\n"
                   "#include <ccmspinlock.h>\n\n"
                   "#include <stdlib.h>\n"
                   "#include <new>\n\n");
    builder.Append("using namespace ccm;\n\n");

    builder.Append(GenNamespaceBegin(String(mk->mNamespace)));
    builder.Append(GenCoclassObject(mk));
    builder.Append(GenCoclassMethods(mk));
    builder.Append(GenNamespaceEnd(String(mk->mNamespace)));

    String data = builder.ToString();
    file.Write(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

String CodeGenerator::GenCoclassObject(
    /* [in] */ MetaCoclass* mk)
{
    StringBuilder builder;

    builder.AppendFormat("class %sClassObject\n", mk->mName);
    builder.Append("    : public ClassObject\n");
    MetaInterface* mi = mMetaComponent->mInterfaces[mk->mInterfaceIndexes[mk->mInterfaceNumber - 1]];
    bool isIClassObject = String("IClassObject").Equals(mi->mName);
    if (!isIClassObject) {
        builder.AppendFormat("    , public %s\n", mi->mName);
    }
    builder.Append("{\n");
    builder.AppendFormat("public:\n"
                         "    %sClassObject();\n\n"
                         "    ~%sClassObject();\n\n", mk->mName, mk->mName);
    if (!isIClassObject) {
        builder.Append("    CCM_INTERFACE_DECL();\n\n");
    }
    bool hasConstructorWithoutArgu = false;
    int start = isIClassObject ? 1 : 0;
    for (int i = start; i < mi->mMethodNumber; i++) {
        MetaMethod* mm = mi->mMethods[i];
        if (mm->mParameterNumber == 2) hasConstructorWithoutArgu = true;
        builder.AppendFormat("    ECode %s(\n", mm->mName);
        for (int j = 0; j < mm->mParameterNumber; j++) {
            builder.AppendFormat("        %s", GenParameter(mm->mParameters[j]).string());
            if (j != mm->mParameterNumber - 1) builder.Append(",\n");
        }
        builder.Append(");\n");
        if (i != mi->mMethodNumber - 1) builder.Append("\n");
    }
    if (!isIClassObject && !hasConstructorWithoutArgu) {
        builder.Append("\n    ECode CreateObject(\n"
                       "        /* [in] */ const InterfaceID& iid,\n"
                       "        /* [out] */ IInterface** object);\n\n");
    }
    builder.Append("};\n\n");
    builder.AppendFormat("static %s* s%sClassObject = nullptr;\n", mi->mName, mk->mName);
    builder.AppendFormat("static Spinlock& Get%sClassObjectLock()\n"
                         "{\n"
                         "    static Spinlock s%sClassObjectLock;\n"
                         "    return s%sClassObjectLock;\n"
                         "}\n\n", mk->mName, mk->mName, mk->mName);
    if (!isIClassObject) {
        builder.AppendFormat("CCM_INTERFACE_IMPL_1(%sClassObject, ClassObject, %s);\n\n",
                mk->mName, mi->mName);
    }
    builder.AppendFormat("%sClassObject::%sClassObject()\n"
                         "{\n"
                         "    AddComponentCount();\n"
                         "}\n\n", mk->mName, mk->mName);
    builder.AppendFormat("%sClassObject::~%sClassObject()\n"
                         "{\n"
                         "    Spinlock& lock = Get%sClassObjectLock();\n"
                         "    lock.Lock();\n"
                         "    s%sClassObject = nullptr;\n"
                         "    lock.Unlock();\n"
                         "    ReleaseComponentCount();\n"
                         "}\n\n", mk->mName, mk->mName, mk->mName, mk->mName);
    if (!isIClassObject && !hasConstructorWithoutArgu) {
        builder.AppendFormat("ECode %sClassObject::CreateObject(\n"
                             "    /* [in] */ const InterfaceID& iid,\n"
                             "    /* [out] */ IInterface** object)\n", mk->mName);
        builder.Append("{\n"
                       "    *object = nullptr;\n"
                       "    return E_UNSUPPORTED_OPERATION_EXCEPTION;\n"
                       "}\n\n");
    }
    for (int i = start; i < mi->mMethodNumber; i++) {
        MetaMethod* mm = mi->mMethods[i];
        builder.AppendFormat("ECode %sClassObject::%s(\n", mk->mName, mm->mName);
        for (int j = 0; j < mm->mParameterNumber; j++) {
            builder.AppendFormat("    %s", GenParameter(mm->mParameters[j]).string());
            if (j != mm->mParameterNumber - 1) builder.Append(",\n");
        }
        builder.Append(")\n"
                       "{\n");
        builder.Append("    VALIDATE_NOT_NULL(object);\n\n");
        builder.AppendFormat("    void* addr = calloc(sizeof(%s), 1);\n", mk->mName);
        builder.Append("    if (addr == nullptr) return E_OUT_OF_MEMORY_ERROR;\n\n");
        builder.AppendFormat("    %s* _obj = new(addr) %s();\n", mk->mName, mk->mName);
        if (mm->mParameterNumber != 2 || !mk->mConstructorDefault) {
            builder.Append("    ECode ec = _obj->Constructor(");
            for (int i = 0; i < mm->mParameterNumber - 2; i++) {
                builder.Append(mm->mParameters[i]->mName);
                if (i != mm->mParameterNumber - 3) builder.Append(", ");
            }
            builder.Append(");\n");
            builder.Append("    if (FAILED(ec)) {\n"
                           "        free(addr);\n"
                           "        return ec;\n"
                           "    }\n");
        }
        builder.AppendFormat("    _obj->AttachMetadata(mComponent, String(\"%s%s\"));\n",
                mk->mNamespace, mk->mName);
        builder.AppendFormat("    *object = _obj->Probe(%s);\n"
                             "    REFCOUNT_ADD(*object);\n",
                             mm->mParameters[mm->mParameterNumber - 2]->mName);
        builder.Append("    return NOERROR;\n");
        builder.Append("}\n\n");
    }

    builder.AppendFormat("ECode Get%sClassObject(IClassObject** classObject)\n"
                         "{\n"
                         "    VALIDATE_NOT_NULL(classObject);\n\n"
                         "    Spinlock& lock = Get%sClassObjectLock();\n"
                         "    lock.Lock();\n"
                         "    if (s%sClassObject == nullptr) {\n"
                         "        s%sClassObject = new %sClassObject();\n"
                         "    }\n"
                         "    lock.Unlock();\n"
                         "    *classObject = IClassObject::Probe(s%sClassObject);\n"
                         "    REFCOUNT_ADD(*classObject);\n"
                         "    return NOERROR;\n"
                         "}\n\n", mk->mName, mk->mName, mk->mName, mk->mName, mk->mName, mk->mName);

    return builder.ToString();
}

String CodeGenerator::GenCoclassMethods(
    /* [in] */ MetaCoclass* mk)
{
    StringBuilder builder;

    builder.AppendFormat("_%s::_%s()\n"
                         "{\n"
                         "    s%sClassObject->AddRef();\n"
                         "}\n"
                         "\n"
                         "_%s::~_%s()\n"
                         "{\n"
                         "    s%sClassObject->Release();\n"
                         "}\n\n", mk->mName, mk->mName, mk->mName, mk->mName, mk->mName, mk->mName);
    MetaInterface* mi = mMetaComponent->mInterfaces[mk->mInterfaceIndexes[mk->mInterfaceNumber - 1]];
    bool isIClassObject = String("IClassObject").Equals(mi->mName);
    int start = isIClassObject ? 1 : 0;
    for (int i = start; i < mi->mMethodNumber; i++) {
        MetaMethod* mm = mi->mMethods[i];
        builder.AppendFormat("ECode _%s::New(\n", mk->mName);
        for (int j = 0; j < mm->mParameterNumber; j++) {
            builder.AppendFormat("    %s", GenParameter(mm->mParameters[j]).string());
            if (j != mm->mParameterNumber - 1) builder.Append(",\n");
        }
        builder.Append(")\n"
                       "{\n");
        if (mm->mParameterNumber > 2) {
            builder.Append("    AutoPtr<IClassObject> clsObject;\n");
            builder.AppendFormat("    ECode ec = CoAcquireClassFactory(CID_%s, nullptr, (IClassObject**)&clsObject);\n",
                    mk->mName);
            builder.Append("    if (FAILED(ec)) return ec;\n");
            builder.AppendFormat("    return %s::Probe(clsObject)->CreateObject(", mi->mName);
            for (int j = 0; j < mm->mParameterNumber; j++) {
                builder.AppendFormat("%s", mm->mParameters[j]->mName);
                if (j != mm->mParameterNumber - 1) builder.Append(", ");
            }
            builder.Append(");\n");
        }
        else {
            builder.AppendFormat("    return CoCreateObjectInstance(CID_%s, %s, nullptr, %s);\n",
                    mk->mName, mm->mParameters[0]->mName, mm->mParameters[1]->mName);
        }
        builder.Append("};\n\n");
    }

    return builder.ToString();
}

void CodeGenerator::GenComponentCpp()
{
    String filePath =
            String::Format("%s/%sPub.cpp", mDirectory.string(), mMetaComponent->mName);
    File file(filePath, File::WRITE);

    StringBuilder builder;

    builder.Append(mLicense);
    builder.Append("\n");
    if (mSparseMode) {
        for (int i = 0; i < mMetaComponent->mInterfaceNumber; i++) {
            MetaInterface* mi = mMetaComponent->mInterfaces[i];
            for (int j = 0; j < mi->mConstantNumber; j++) {
                MetaConstant* mc = mi->mConstants[j];
                MetaType* mt = mMetaComponent->mTypes[mc->mTypeIndex];
                if ((mt->mKind == CcmTypeKind::String) ||
                        ((mt->mKind == CcmTypeKind::Float || mt->mKind == CcmTypeKind::Double) &&
                        (mc->mValue.mAttributes & FP_MASK))) {
                    builder.AppendFormat("#include \"%s%s.h\"\n",
                            String(mi->mNamespace).Replace("::", ".").string(), mi->mName);
                    break;
                }
            }
        }
    }
    builder.AppendFormat("#include \"%s.h\"\n"
                         "#include <ccmcomponent.h>\n"
                         "#include <ccmrefbase.h>\n\n"
                         "using namespace ccm;\n\n", mMetaComponent->mName);

    MetaComponent* mc = mMetaComponent;
    builder.Append(GenComponentID());
    builder.Append("\n");
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        MetaNamespace* mn = mc->mNamespaces[i];
        if (mn->mConstantNumber +
                (mn->mInterfaceNumber - mn->mExternalInterfaceNumber) +
                mn->mCoclassNumber == 0) {
            continue;
        }
        builder.Append(GenNamespaceBegin(String(mn->mName)));
        builder.Append(GenConstantsInCpp(mn));
        builder.Append(GenInterfaceConstantsInCpp(mn));
        builder.Append(GenInterfaceIDsInCpp(mn));
        builder.Append(GenCoclassIDsInCpp(mn));
        builder.Append(GenNamespaceEnd(String(mn->mName)));
        if (i != mc->mNamespaceNumber - 1) builder.Append("\n");
    }

    builder.Append("\n");
    builder.Append(GenClassObjectGetterArray());
    builder.Append("\n");
    builder.Append(GenSoGetClassObject());
    builder.Append("\n");
    builder.Append(GenComponentObject());

    String data = builder.ToString();
    file.Write(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

String CodeGenerator::GenConstantsInCpp(
    /* [in] */ MetaNamespace* mn)
{
    StringBuilder builder;

    if (mn->mConstantNumber == 0) return String();

    for (int i = 0; i < mn->mConstantNumber; i++) {
        MetaConstant* mc = mMetaComponent->mConstants[mn->mConstantIndexes[i]];
        MetaType* mt = mMetaComponent->mTypes[mc->mTypeIndex];
        if (mt->mKind == CcmTypeKind::String) {
            builder.AppendFormat("const %s %s(%s);\n", GenType(mt).string(),
                    mc->mName, GenValue(mt, mc->mValue).string());
        }
    }
    builder.Append("\n");

    return builder.ToString();
}

String CodeGenerator::GenInterfaceConstantsInCpp(
    /* [in] */ MetaNamespace* mn)
{
    StringBuilder builder;

    if (mn->mInterfaceNumber == 0) return String();

    for (int i = 0; i < mn->mInterfaceNumber; i++) {
        MetaInterface* mi = mMetaComponent->mInterfaces[mn->mInterfaceIndexes[i]];
        if (mi->mExternal) continue;
        for (int j = 0; j < mi->mConstantNumber; j++) {
            MetaConstant* mc = mi->mConstants[j];
            MetaType* mt = mMetaComponent->mTypes[mc->mTypeIndex];
            if (mt->mKind == CcmTypeKind::String) {
                builder.AppendFormat("const %s %s::%s(%s);\n", GenType(mt).string(),
                        mi->mName, mc->mName, GenValue(mt, mc->mValue).string());
            }
            else if ((mt->mKind == CcmTypeKind::Float || mt->mKind == CcmTypeKind::Double) &&
                    (mc->mValue.mAttributes & FP_MASK)) {
                builder.AppendFormat("const %s %s::%s = %s;\n", GenType(mt).string(),
                        mi->mName, mc->mName, GenValue(mt, mc->mValue).string());
            }
        }
    }
    builder.Append("\n");

    return builder.ToString();
}

String CodeGenerator::GenInterfaceIDsInCpp(
    /* [in] */ MetaNamespace* mn)
{
    StringBuilder builder;

    if (mn->mInterfaceNumber == 0) return String();

    for (int i = 0; i < mn->mInterfaceNumber; i++) {
        MetaInterface* mi = mMetaComponent->mInterfaces[mn->mInterfaceIndexes[i]];
        if (mi->mExternal) continue;
        builder.AppendFormat("extern const InterfaceID IID_%s =\n"
                             "        {%s, &CID_%s};\n",
                mi->mName, Uuid(mi->mUuid).ToString().string(),
                mMetaComponent->mName);
    }
    builder.Append("\n");

    return builder.ToString();
}

String CodeGenerator::GenCoclassIDsInCpp(
    /* [in] */ MetaNamespace* mn)
{
    StringBuilder builder;

    if (mn->mCoclassNumber == 0) return String();

    for (int i = 0; i < mn->mCoclassNumber; i++) {
        MetaCoclass* mc = mMetaComponent->mCoclasses[mn->mCoclassIndexes[i]];
        builder.AppendFormat("extern const CoclassID CID_%s =\n"
                             "        {%s, &CID_%s};\n",
                mc->mName, Uuid(mc->mUuid).ToString().string(),
                mMetaComponent->mName);
    }
    builder.Append("\n");

    return builder.ToString();
}

String CodeGenerator::GenClassObjectGetterArray()
{
    StringBuilder builder;

    MetaComponent* mc = mMetaComponent;

    if (mc->mCoclassNumber == 0) return String();

    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        MetaNamespace* mn = mc->mNamespaces[i];
        if (mn->mCoclassNumber == 0) continue;
        builder.Append(GenNamespaceBegin(String(mn->mName)));
        for (int i = 0; i < mn->mCoclassNumber; i++) {
            MetaCoclass* mk = mc->mCoclasses[mn->mCoclassIndexes[i]];
            builder.AppendFormat("extern ECode Get%sClassObject(IClassObject** classObject);\n", mk->mName);
        }
        builder.Append(GenNamespaceEnd(String(mn->mName)));
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
    builder.Append(GenSoGetAllClassObjects());

    return builder.ToString();
}

String CodeGenerator::GenSoGetAllClassObjects()
{
    StringBuilder builder;

    builder.AppendFormat("EXTERN_C COM_PUBLIC ClassObjectGetter* soGetAllClassObjects(int* size)\n"
                         "{\n"
                         "    *size = sizeof(co%sGetters) / sizeof(ClassObjectGetter);\n"
                         "    return co%sGetters;\n"
                         "}\n", mMetaComponent->mName, mMetaComponent->mName);

    return builder.ToString();
}

String CodeGenerator::GenSoGetClassObject()
{
    StringBuilder builder;

    MetaComponent* mc = mMetaComponent;

    builder.Append("EXTERN_C COM_PUBLIC ECode soGetClassObject(const CoclassID& cid, IClassObject** object)\n");
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

String CodeGenerator::GenComponentObject()
{
    StringBuilder builder;

    builder.AppendFormat("class C%s\n"
                         "    : public LightRefBase\n"
                         "{\n"
                         "public:\n"
                         "    C%s();\n"
                         "};\n"
                         "C%s::C%s()\n"
                         "{\n"
                         "    AddRef();\n"
                         "}\n\n",
                         mMetaComponent->mName, mMetaComponent->mName,
                         mMetaComponent->mName, mMetaComponent->mName);
    builder.AppendFormat("static __attribute__ ((init_priority (200))) C%s sComponentObject;\n\n", mMetaComponent->mName);
    builder.Append("EXTERN_C COM_PUBLIC Boolean soCanUnload()\n"
                   "{\n"
                   "    return sComponentObject.GetStrongCount() == 1;\n"
                   "}\n"
                   "\n"
                   "void AddComponentCount()\n"
                   "{\n"
                   "    sComponentObject.AddRef();\n"
                   "}\n"
                   "\n"
                   "void ReleaseComponentCount()\n"
                   "{\n"
                   "    sComponentObject.Release();\n"
                   "}\n");
    return builder.ToString();
}

void CodeGenerator::GenConstantsAndTypesOnUserMode()
{
    MetaComponent* mc = mMetaComponent;
    String filePath = String::Format("%s/%s.h",
            mDirectory.string(), mc->mName);
    File file(filePath, File::WRITE);

    String defMacro = String::Format("__%s_H_GEN__", mc->mName).ToUpperCase();

    StringBuilder builder;

    builder.Append(mLicense);
    builder.Append("\n");
    builder.AppendFormat("#ifndef %s\n"
                         "#define %s\n\n", defMacro.string(), defMacro.string());
    builder.Append("#include <ccmtypes.h>\n\n");
    builder.Append("using namespace ccm;\n\n");

    builder.AppendFormat("extern const ComponentID CID_%s;\n\n", mc->mName);
    if (!mSparseMode) {
        for (int i = 0; i < mc->mNamespaceNumber; i++) {
            MetaNamespace* mn = mc->mNamespaces[i];
            if (mn->mConstantNumber +
                    (mn->mEnumerationNumber - mn->mExternalEnumerationNumber) +
                    (mn->mInterfaceNumber - mn->mExternalInterfaceNumber) == 0) {
                continue;
            }
            builder.Append(GenNamespaceBegin(String(mn->mName)));
            builder.Append(GenConstantsInHeader(mn));
            builder.Append(GenEnumerationPredeclarations(mn));
            builder.Append(GenInterfaceIDPredeclarations(mn));
            builder.Append(GenInterfacePredeclarations(mn));
            builder.Append(GenNamespaceEnd(String(mn->mName)));
            if (i != mc->mNamespaceNumber - 1) builder.Append("\n");
            else builder.Append("\n\n");
        }
        for (int i = 0; i < mc->mNamespaceNumber; i++) {
            MetaNamespace* mn = mc->mNamespaces[i];
            if ((mn->mEnumerationNumber - mn->mExternalEnumerationNumber)  +
                    (mn->mInterfaceNumber - mn->mExternalInterfaceNumber) +
                    mn->mCoclassNumber == 0) {
                continue;
            }
            builder.Append(GenNamespaceBegin(String(mn->mName)));
            builder.Append(GenEnumerationDeclarations(mn));
            builder.Append(GenInterfaceDeclarations(mn));
            builder.Append(GenCoclassDeclarations(mn));
            builder.Append(GenNamespaceEnd(String(mn->mName)));
            if (i != mc->mNamespaceNumber - 1) builder.Append("\n");
            else builder.Append("\n\n");
        }
    }
    else {
        for (int i = 0; i < mc->mNamespaceNumber; i++) {
            MetaNamespace* mn = mc->mNamespaces[i];
            if (mn->mConstantNumber +
                    (mn->mEnumerationNumber - mn->mExternalEnumerationNumber) +
                    (mn->mInterfaceNumber - mn->mExternalInterfaceNumber) == 0) {
                continue;
            }
            builder.Append(GenNamespaceBegin(String(mn->mName)));
            builder.Append(GenConstantsInHeader(mn));
            builder.Append(GenEnumerationPredeclarations(mn));
            builder.Append(GenInterfacePredeclarations(mn));
            builder.Append(GenNamespaceEnd(String(mn->mName)));
            if (i != mc->mNamespaceNumber - 1) builder.Append("\n");
            else builder.Append("\n\n");
        }
        for (int i = 0; i < mc->mNamespaceNumber; i++) {
            MetaNamespace* mn = mc->mNamespaces[i];
            if ((mn->mEnumerationNumber - mn->mExternalEnumerationNumber) == 0) {
                continue;
            }
            builder.Append(GenNamespaceBegin(String(mn->mName)));
            builder.Append(GenEnumerationDeclarations(mn));
            builder.Append(GenNamespaceEnd(String(mn->mName)));
            if (i != mc->mNamespaceNumber - 1) builder.Append("\n");
            else builder.Append("\n\n");
        }
    }
    builder.AppendFormat("#endif // %s\n", defMacro.string());

    String data = builder.ToString();
    file.Write(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

String CodeGenerator::GenCoclassDeclarations(
    /* [in] */ MetaNamespace* mn)
{
    StringBuilder builder;

    if (mn->mCoclassNumber == 0) return String();

    for (int i = 0; i < mn->mCoclassNumber; i++) {
        MetaCoclass* mc = mMetaComponent->mCoclasses[i];
        builder.Append(GenCoclassDeclaration(mc));
        if (i != mn->mCoclassNumber - 1) builder.Append("\n");
    }
    builder.Append("\n");

    return builder.ToString();
}

String CodeGenerator::GenCoclassDeclaration(
    /* [in] */ MetaCoclass* mc)
{
    StringBuilder builder;

    builder.AppendFormat("extern const CoclassID CID_%s;\n\n", mc->mName);
    builder.AppendFormat("COCLASS_ID(%s)\n", Uuid(mc->mUuid).Dump().string());
    builder.AppendFormat("class %s\n", mc->mName);
    builder.Append("{\n"
                   "public:\n");
    MetaInterface* mi = mMetaComponent->mInterfaces[mc->mInterfaceIndexes[mc->mInterfaceNumber - 1]];
    bool isIClassObject = String("IClassObject").Equals(mi->mName);
    int start = isIClassObject ? 1 : 0;
    for (int i = start; i < mi->mMethodNumber; i++) {
        MetaMethod* mm = mi->mMethods[i];
        builder.Append("    static ECode New(\n");
        for (int j = 0; j < mm->mParameterNumber; j++) {
            builder.AppendFormat("        %s", GenParameter(mm->mParameters[j]).string());
            if (j != mm->mParameterNumber - 1) builder.Append(",\n");
        }
        builder.Append(");\n");
        if (i != mi->mMethodNumber - 1) builder.Append("\n");
    }
    builder.Append("};\n\n");

    return builder.ToString();
}

void CodeGenerator::GenCoclassDeclarationsSparselyOnUserMode()
{
    MetaComponent* mc = mMetaComponent;
    for (int i = 0; i < mc->mCoclassNumber; i++) {
        MetaCoclass* mk = mc->mCoclasses[i];
        GenCoclassDeclarationSparselyOnUserMode(mk);
    }
}

void CodeGenerator::GenCoclassDeclarationSparselyOnUserMode(
    /* [in] */ MetaCoclass* mc)
{
    String filePath = String::Format("%s/%s%s.h", mDirectory.string(),
            String(mc->mNamespace).Replace("::", ".").string(), mc->mName);
    File file(filePath, File::WRITE);

    StringBuilder builder;

    builder.Append(mLicense);
    builder.Append("\n");

    String defMacro = GenDefineMacro(
            String::Format("%s%s_H_GEN", mc->mNamespace, mc->mName));
    builder.AppendFormat("#ifndef %s\n", defMacro.string());
    builder.AppendFormat("#define %s\n\n", defMacro.string());
    builder.AppendFormat("#include \"%s.h\"\n\n", mMetaComponent->mName);

    builder.Append("using namespace ccm;\n\n");

    builder.Append(GenNamespaceBegin(String(mc->mNamespace)));
    builder.Append(GenCoclassDeclaration(mc));
    builder.Append(GenNamespaceEnd(String(mc->mNamespace)));

    builder.AppendFormat("\n#endif // %s\n", defMacro.string());

    String data = builder.ToString();
    file.Write(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

void CodeGenerator::GenComponentCppOnUserMode()
{
    MetaComponent* mc = mMetaComponent;

    String filePath =
            String::Format("%s/%s.cpp", mDirectory.string(), mc->mName);
    File file(filePath, File::WRITE);

    StringBuilder builder;

    builder.Append(mLicense);
    builder.Append("\n");
    builder.AppendFormat("#include \"%s.h\"\n", mc->mName);
    if (mSparseMode) {
        for (int i = 0; i < mc->mCoclassNumber; i++) {
            MetaCoclass* mk = mc->mCoclasses[i];
            String clsHeader = String::Format("%s%s.h",
                    String(mk->mNamespace).Replace("::", ".").string(), mk->mName);
            builder.AppendFormat("#include \"%s\"\n", clsHeader.string());
        }
        for (int i = 0; i < mc->mInterfaceNumber; i++) {
            MetaInterface* mi = mc->mInterfaces[i];
            if (mi->mExternal) continue;
            if (!String(mi->mName).EndsWith("ClassObject")) {
                for (int j = 0; j < mi->mConstantNumber; j++) {
                    MetaConstant* mc = mi->mConstants[j];
                    MetaType* mt = mMetaComponent->mTypes[mc->mTypeIndex];
                    if ((mt->mKind == CcmTypeKind::String) ||
                            ((mt->mKind == CcmTypeKind::Float || mt->mKind == CcmTypeKind::Double) &&
                            (mc->mValue.mAttributes & FP_MASK))) {
                        String intfHeader = String::Format("%s%s.h",
                                String(mi->mNamespace).Replace("::", ".").string(), mi->mName);
                        builder.AppendFormat("#include \"%s\"\n", intfHeader.string());
                        break;
                    }
                }
            }
            else {
                String intfHeader = String::Format("%s%s.h",
                        String(mi->mNamespace).Replace("::", ".").string(), mi->mName);
                builder.AppendFormat("#include \"%s\"\n", intfHeader.string());
            }
        }
    }
    builder.Append("\n#include <ccmapi.h>\n"
                     "#include <ccmautoptr.h>\n\n"
                     "using namespace ccm;\n\n");

    builder.Append(GenComponentID());
    builder.Append("\n");
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        MetaNamespace* mn = mc->mNamespaces[i];
        if (mn->mConstantNumber +
                (mn->mInterfaceNumber - mn->mExternalInterfaceNumber) +
                mn->mCoclassNumber == 0) {
            continue;
        }
        builder.Append(GenNamespaceBegin(String(mn->mName)));
        builder.Append(GenConstantsInCpp(mn));
        builder.Append(GenInterfaceConstantsInCpp(mn));
        builder.Append(GenInterfaceIDsInCpp(mn));
        builder.Append(GenCoclassIDsInCpp(mn));
        builder.Append(GenCoclassesOnUserMode(mn));
        builder.Append(GenNamespaceEnd(String(mn->mName)));
        if (i != mc->mNamespaceNumber - 1) builder.Append("\n");
    }

    String data = builder.ToString();
    file.Write(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

String CodeGenerator::GenCoclassesOnUserMode(
    /* [in] */ MetaNamespace* mn)
{
    StringBuilder builder;

    if (mn->mCoclassNumber == 0) return String();

    for (int i = 0; i < mn->mCoclassNumber; i++) {
        MetaCoclass* mc = mMetaComponent->mCoclasses[mn->mCoclassIndexes[i]];
        builder.Append(GenCoclassOnUserMode(mc));
        if (i != mn->mCoclassNumber - 1) builder.Append("\n");
    }
    builder.Append("\n");

    return builder.ToString();
}

String CodeGenerator::GenCoclassOnUserMode(
    /* [in] */ MetaCoclass* mc)
{
    StringBuilder builder;

    builder.AppendFormat("// %s\n", mc->mName);
    MetaInterface* mi = mMetaComponent->mInterfaces[mc->mInterfaceIndexes[mc->mInterfaceNumber - 1]];
    bool isIClassObject = String("IClassObject").Equals(mi->mName);
    int start = isIClassObject ? 1 : 0;
    for (int i = start; i < mi->mMethodNumber; i++) {
        MetaMethod* mm = mi->mMethods[i];
        builder.AppendFormat("ECode %s::New(\n", mc->mName);
        for (int j = 0; j < mm->mParameterNumber; j++) {
            builder.AppendFormat("    %s", GenParameter(mm->mParameters[j]).string());
            if (j != mm->mParameterNumber - 1) builder.Append(",\n");
        }
        builder.Append(")\n"
                       "{\n");
        if (mm->mParameterNumber > 2) {
            builder.Append("    AutoPtr<IClassObject> clsObject;\n");
            builder.AppendFormat("    ECode ec = CoAcquireClassFactory(CID_%s, nullptr, (IClassObject**)&clsObject);\n",
                    mc->mName);
            builder.Append("    if (FAILED(ec)) return ec;\n");
            builder.AppendFormat("    return %s::Probe(clsObject)->CreateObject(", mi->mName);
            for (int j = 0; j < mm->mParameterNumber; j++) {
                builder.AppendFormat("%s", mm->mParameters[j]->mName);
                if (j != mm->mParameterNumber - 1) builder.Append(", ");
            }
            builder.Append(");\n");
        }
        else {
            builder.AppendFormat("    return CoCreateObjectInstance(CID_%s, %s, nullptr, %s);\n",
                    mc->mName, mm->mParameters[0]->mName, mm->mParameters[1]->mName);
        }
        builder.Append("};\n");
        if (i != mi->mMethodNumber - 1) builder.Append("\n");
    }
    builder.Append("\n");

    return builder.ToString();
}

String CodeGenerator::GenDefineMacro(
    /* [in] */ const String& fullName)
{
    String defMacro = fullName.Replace("::", "_");
    defMacro = defMacro.ToUpperCase();
    defMacro = String("__") + defMacro + String("__");
    return defMacro;
}

void CodeGenerator::GenMetadataWrapper()
{
    String filePath =
            String::Format("%s/MetadataWrapper.cpp", mDirectory.string());
    File file(filePath, File::WRITE);

    StringBuilder builder;

    builder.Append(mLicense);
    builder.Append("\n");

    MetaSerializer serializer(mMetaComponent);
    serializer.Serialize();
    int dataSize = serializer.GetDataSize();
    uintptr_t data = serializer.GetData();

    builder.Append("#include <ccmdef.h>\n");
    builder.Append("#include <stdint.h>\n\n");
    builder.AppendFormat("struct MetadataWrapper\n"
                   "{\n"
                   "    int             mSize;\n"
                   "    unsigned char   mMetadata[%d];\n"
                   "};\n\n", dataSize);
    builder.Append("static const MetadataWrapper comMetadata __attribute__ ((used,__section__ (\".metadata\"))) = {\n");
    builder.AppendFormat("    %d, {\n", dataSize);
    int lineSize = 0;
    for (int i = 0; i < dataSize; i++, lineSize++) {
        if (lineSize == 0) builder.Append("    ");
        builder.AppendFormat("0x%02x", ((unsigned char*)data)[i]);
        if (i == dataSize - 1) break;
        if (lineSize < 15) {
            builder.Append(",");
        }
        else {
            builder.Append(",\n");
            lineSize = -1;
        }
    }
    builder.Append("}};\n\n");
    builder.Append("COM_PUBLIC extern const uintptr_t soMetadataHandle = reinterpret_cast<uintptr_t>(&comMetadata);\n");

    String strData = builder.ToString();
    file.Write(strData.string(), strData.GetLength());
    file.Flush();
    file.Close();
}

}
}
