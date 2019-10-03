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

#include "metadata/MetadataDumper.h"
#include "util/Properties.h"
#include "util/StringBuilder.h"
#include <memory>

namespace cdlc {

String MetadataDumper::Dump(
    /* [in] */ const String& prefix)
{
    if (mComponent == nullptr) {
        return prefix + "";
    }

    return DumpMetaComponent(mComponent, prefix);
}

String MetadataDumper::DumpMetaComponent(
    /* [in] */ como::MetaComponent* mc,
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append("{\n");
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mMagic\":\"0x%x\",\n", mc->mMagic);
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mSize\":\"%d\",\n", mc->mSize);
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mUuid\":\"%s\",\n", DumpUUID(mc->mUuid).string());
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mName\":\"%s\",\n", mc->mName);
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mUri\":\"%s\",\n", mc->mUri);
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mNamespaceNumber\":\"%d\",\n", mc->mNamespaceNumber);
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mConstantNumber\":\"%d\",\n", mc->mConstantNumber);
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mCoclassNumber\":\"%d\",\n", mc->mCoclassNumber);
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mEnumerationNumber\":\"%d\",\n", mc->mEnumerationNumber);
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mInterfaceNumber\":\"%d\",\n", mc->mInterfaceNumber);
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mTypeNumber\":\"%d\"", mc->mTypeNumber);

    if (mc->mNamespaceNumber > 0) {
        builder.Append(",\n");
        builder.Append(prefix + Properties::INDENT).AppendFormat("\"mNamespaces\":[\n");
        for (int i = 0; i < mc->mNamespaceNumber; i++) {
            String namespaceInfo = DumpMetaNamespace(mc->mNamespaces[i],
                    prefix + Properties::INDENT + Properties::INDENT);
            builder.Append(namespaceInfo);
            builder.AppendFormat("%s", i < mc->mNamespaceNumber - 1 ? ",\n" : "\n");
        }
        builder.Append(prefix + Properties::INDENT).Append("]");
    }
    builder.Append("\n");
    builder.Append(prefix).Append("}\n");

    return builder.ToString();
}

String MetadataDumper::DumpMetaNamespace(
    /* [in] */ como::MetaNamespace* mn,
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append("{\n");
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mName\":\"%s\",\n", mn->mName);
    if (mn->mInterfaceWrappedIndex != -1) {
        builder.Append(prefix + Properties::INDENT).AppendFormat("\"InterfaceWrapped\":\"%s\",\n",
                mComponent->mInterfaces[mn->mInterfaceWrappedIndex]->mName);
    }
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mNamespaceNumber\":\"%d\",\n", mn->mNamespaceNumber);
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mConstantNumber\":\"%d\",\n", mn->mConstantNumber);
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mCoclassNumber\":\"%d\",\n", mn->mCoclassNumber);
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mEnumerationNumber\":\"%d\",\n", mn->mEnumerationNumber);
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mInterfaceNumber\":\"%d\"", mn->mInterfaceNumber);

    if (mn->mConstantNumber > 0) {
        builder.Append(",\n");
        builder.Append(prefix + Properties::INDENT).Append("\"mConstants\":[\n");
        for (int i = 0; i < mn->mConstantNumber; i++) {
            como::MetaConstant* constant = mComponent->mConstants[mn->mConstantIndexes[i]];
            String constantInfo = DumpMetaConstant(constant,
                    prefix + Properties::INDENT + Properties::INDENT);
            builder.Append(constantInfo);
            builder.AppendFormat("%s", i < mn->mConstantNumber - 1 ? ",\n" : "\n");
        }
        builder.Append(prefix + Properties::INDENT).Append("]");
    }

    if (mn->mEnumerationNumber > 0) {
        builder.Append(",\n");
        builder.Append(prefix + Properties::INDENT).Append("\"mEnumerations\":[\n");
        for (int i = 0; i < mn->mEnumerationNumber; i++) {
            como::MetaEnumeration* enumeration = mComponent->mEnumerations[mn->mEnumerationIndexes[i]];
            String enumerationInfo = DumpMetaEnumeration(enumeration,
                    prefix + Properties::INDENT + Properties::INDENT);
            builder.Append(enumerationInfo);
            builder.AppendFormat("%s", i < mn->mEnumerationNumber - 1 ? ",\n" : "\n");
        }
        builder.Append(prefix + Properties::INDENT).Append("]");
    }

    if (mn->mInterfaceNumber > 0) {
        builder.Append(",\n");
        builder.Append(prefix + Properties::INDENT).Append("\"mInterfaces\":[\n");
        for (int i = 0; i < mn->mInterfaceNumber; i++) {
            como::MetaInterface* interface = mComponent->mInterfaces[mn->mInterfaceIndexes[i]];
            String interfaceInfo = DumpMetaInterface(interface,
                    prefix + Properties::INDENT + Properties::INDENT);
            builder.Append(interfaceInfo);
            builder.AppendFormat("%s", i < mn->mInterfaceNumber - 1 ? ",\n" : "\n");
        }
        builder.Append(prefix + Properties::INDENT).Append("]");
    }

    if (mn->mCoclassNumber > 0) {
        builder.Append(",\n");
        builder.Append(prefix + Properties::INDENT).Append("\"mCoclasss\":[\n");
        for (int i = 0; i < mn->mCoclassNumber; i++) {
            como::MetaCoclass* klass = mComponent->mCoclasses[mn->mCoclassIndexes[i]];
            String klassInfo = DumpMetaCoclass(klass, prefix + Properties::INDENT);
            builder.Append(klassInfo);
            builder.AppendFormat("%s", i < mn->mCoclassNumber - 1 ? ",\n" : "\n");
        }
        builder.Append(prefix + Properties::INDENT).Append("]");
    }
    builder.Append("\n");
    builder.Append(prefix).Append("}");

    return builder.ToString();
}

String MetadataDumper::DumpMetaConstant(
    /* [in] */ como::MetaConstant* mc,
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append("{ ");
    builder.AppendFormat("\"mName\":\"%s\", ", mc->mName);
    builder.AppendFormat("\"mType\":\"%s\", ",
            DumpMetaType(mComponent->mTypes[mc->mTypeIndex]).string());
    builder.AppendFormat("\"mValue\":\"%s\"",
            DumpMetaValue(mComponent->mTypes[mc->mTypeIndex], &mc->mValue).string());
    builder.Append(" }");

    return builder.ToString();
}

String MetadataDumper::DumpMetaEnumeration(
    /* [in] */ como::MetaEnumeration* me,
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append("{\n");
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mName\":\"%s\",\n", me->mName);
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mNamespace\":\"%s\",\n", me->mNamespace);
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mEnumeratorNumber\":\"%d\",\n", me->mEnumeratorNumber);
    builder.Append(prefix + Properties::INDENT).Append("\"mEnumerators\":[\n");
    for (int i = 0; i < me->mEnumeratorNumber; i++) {
        builder.Append(prefix + Properties::INDENT + Properties::INDENT);
        builder.AppendFormat("{ \"%s\":\"%d\" }", me->mEnumerators[i]->mName, me->mEnumerators[i]->mValue);
        builder.AppendFormat("%s", i < me->mEnumeratorNumber - 1 ? ",\n" : "\n");
    }
    builder.Append(prefix + Properties::INDENT).Append("]\n");
    builder.Append(prefix).Append("}");

    return builder.ToString();
}

String MetadataDumper::DumpMetaCoclass(
    /* [in] */ como::MetaCoclass* mk,
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append("{\n");
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mUuid\":\"%s\",\n", DumpUUID(mk->mUuid).string());
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mName\":\"%s\",\n", mk->mName);
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mNamespace\":\"%s\",\n", mk->mNamespace);
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mInterfaceNumber\":\"%d\"\n", mk->mInterfaceNumber);
    if (mk->mInterfaceNumber > 0) {
        builder.Append(",\n");
        builder.Append(prefix + Properties::INDENT).Append("\"mInterfaces\":[\n");
        for (int i = 0; i < mk->mInterfaceNumber; i++) {
            builder.Append(prefix + Properties::INDENT).AppendFormat("{ \"Interface\":\"%s\" }",
                    mComponent->mInterfaces[mk->mInterfaceIndexes[i]]->mName);
            builder.AppendFormat("%s", i < mk->mInterfaceNumber - 1 ? ",\n" : "\n");
        }
        builder.Append(prefix + Properties::INDENT).Append("]");
    }

    if (mk->mProperties & (COCLASS_CONSTRUCTOR_DEFAULT | COCLASS_CONSTRUCTOR_DELETED)) {
        builder.Append(",\n");
        builder.Append(prefix + Properties::INDENT).Append("\"mProperties\":");
        if (mk->mProperties & COCLASS_CONSTRUCTOR_DEFAULT) {
            builder.Append("\"COCLASS_CONSTRUCTOR_DEFAULT\"");
        }
        if (mk->mProperties & COCLASS_CONSTRUCTOR_DELETED) {
            builder.Append("\"COCLASS_CONSTRUCTOR_DELETED\"");
        }
        builder.Append("\n");
    }
    builder.Append(prefix).Append("}");

    return builder.ToString();
}

String MetadataDumper::DumpMetaInterface(
    /* [in] */ como::MetaInterface* mi,
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append("{\n");
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mUuid\":\"%s\",\n", DumpUUID(mi->mUuid).string());
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mName\":\"%s\",\n", mi->mName);
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mNamespace\":\"%s\",\n", mi->mNamespace);
    if (mi->mBaseInterfaceIndex != -1) {
        builder.Append(prefix + Properties::INDENT).AppendFormat("\"BaseInterface\":\"%s\",\n",
                mComponent->mInterfaces[mi->mBaseInterfaceIndex]->mName);
    }
    if (mi->mOuterInterfaceIndex != -1) {
        builder.Append(prefix + Properties::INDENT).AppendFormat("\"OuterInterface\":\"%s\",\n",
                mComponent->mInterfaces[mi->mOuterInterfaceIndex]->mName);
    }
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mNestedInterfaceNumber\":\"%d\",\n", mi->mNestedInterfaceNumber);
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mConstantNumber\":\"%d\",\n", mi->mConstantNumber);
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mMethodNumber\":\"%d\"", mi->mMethodNumber);

    if (mi->mConstantNumber > 0) {
        builder.Append(",\n");
        builder.Append(prefix + Properties::INDENT).Append("\"mConstants\":[\n");
        for (int i = 0; i < mi->mConstantNumber; i++) {
            String constantInfo = DumpMetaConstant(mi->mConstants[i],
                    prefix + Properties::INDENT + Properties::INDENT);
            builder.Append(constantInfo);
            builder.AppendFormat("%s", i < mi->mConstantNumber - 1 ? ",\n" : "\n");
        }
        builder.Append(prefix + Properties::INDENT).Append("]");
    }

    if (mi->mMethodNumber > 0) {
        builder.Append(",\n");
        builder.Append(prefix + Properties::INDENT).Append("\"mMethods\":[\n");
        for (int i = 0; i < mi->mMethodNumber; i++) {
            String methodInfo = DumpMetaMethod(mi->mMethods[i],
                    prefix + Properties::INDENT + Properties::INDENT);
            builder.Append(methodInfo);
            builder.AppendFormat("%s", i < mi->mMethodNumber - 1 ? ",\n" : "\n");
        }
        builder.Append(prefix + Properties::INDENT).Append("]");
    }

    if (mi->mNestedInterfaceNumber > 0) {
        builder.Append(",\n");
        builder.Append(prefix + Properties::INDENT).Append("\"mNestedInterfaces\":[\n");
        for (int i = 0; i < mi->mNestedInterfaceNumber; i++) {
            como::MetaInterface* interface = mComponent->mInterfaces[mi->mNestedInterfaceIndexes[i]];
            String interfaceInfo = DumpMetaInterface(interface,
                    prefix + Properties::INDENT + Properties::INDENT);
            builder.Append(interfaceInfo);
            builder.AppendFormat("%s", i < mi->mNestedInterfaceNumber - 1 ? ",\n" : "\n");
        }
        builder.Append(prefix + Properties::INDENT).Append("]");
    }
    builder.Append("\n");
    builder.Append(prefix).Append("}");

    return builder.ToString();
}

String MetadataDumper::DumpMetaMethod(
    /* [in] */ como::MetaMethod* mm,
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append("{\n");
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mName\":\"%s\",\n", mm->mName);
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mSignature\":\"%s\",\n", mm->mSignature);
    como::MetaType* type = mComponent->mTypes[mm->mReturnTypeIndex];
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"ReturnType\":\"%s\",\n", DumpMetaType(type).string());
    builder.Append(prefix + Properties::INDENT).AppendFormat("\"mParameterNumber\":\"%d\"", mm->mParameterNumber);
    if (mm->mProperties & (METHOD_DELETED | METHOD_RETURN_REFERENCE)) {
        builder.Append(",\n");
        builder.Append(prefix + Properties::INDENT).Append("\"mProperties\":");
        if (mm->mProperties & METHOD_DELETED) {
            builder.Append(prefix + Properties::INDENT).AppendFormat("\"METHOD_DELETED\"");
        }
        if (mm->mProperties & METHOD_RETURN_REFERENCE) {
            builder.Append(prefix + Properties::INDENT).AppendFormat("\"METHOD_RETURN_REFERENCE\"");
        }
    }

    if (mm->mParameterNumber > 0) {
        builder.Append(",\n");
        builder.Append(prefix + Properties::INDENT).Append("\"mParameters\":[\n");
        for (int i = 0; i < mm->mParameterNumber; i++) {
            String parameterInfo = DumpMetaParameter(mm->mParameters[i],
                    prefix + Properties::INDENT + Properties::INDENT);
            builder.Append(parameterInfo);
            builder.AppendFormat("%s", i < mm->mParameterNumber - 1 ? ",\n" : "\n");
        }
        builder.Append(prefix + Properties::INDENT).Append("]");
    }
    builder.Append("\n");
    builder.Append(prefix).Append("}");

    return builder.ToString();
}

String MetadataDumper::DumpMetaParameter(
    /* [in] */ como::MetaParameter* mp,
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append("{ ");
    builder.AppendFormat("\"mName\":\"%s\", ", mp->mName);
    como::MetaType* type = mComponent->mTypes[mp->mTypeIndex];
    builder.AppendFormat("\"Type\":\"%s\", ",
            DumpMetaType(type).string());
    builder.AppendFormat("\"Attributes\":\"");
    bool needComma = false;
    if (mp->mProperties & PARAMETER_IN) {
        builder.Append("in");
        needComma = true;
    }
    if (mp->mProperties & PARAMETER_OUT) {
        if (needComma) builder.Append(" | ");
        builder.Append("out");
        needComma = true;
    }
    if (mp->mProperties & PARAMETER_CALLEE) {
        if (needComma) builder.Append(" | ");
        builder.Append("callee");
    }
    builder.Append("\"");
    if (mp->mProperties & PARAMETER_VALUE_DEFAULT) {
        builder.Append(", ");
        como::MetaType* type = mComponent->mTypes[mp->mTypeIndex];
        como::MetaValue* value = reinterpret_cast<como::MetaValue*>(ALIGN((uintptr_t)mp + sizeof(como::MetaParameter)));
        builder.AppendFormat("\"value\":\"%s\"",
                DumpMetaValue(type, value).string());
    }
    builder.Append(" }");

    return builder.ToString();
}

String MetadataDumper::DumpMetaType(
    /* [in] */ como::MetaType* mt)
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
            builder.Append("String");
            break;
        case como::TypeKind::CoclassID:
            builder.Append("CoclassID");
            break;
        case como::TypeKind::ComponentID:
            builder.Append("ComponentID");
            break;
        case como::TypeKind::InterfaceID:
            builder.Append("InterfaceID");
            break;
        case como::TypeKind::HANDLE:
            builder.Append("HANDLE");
            break;
        case como::TypeKind::ECode:
            builder.Append("ECode");
            break;
        case como::TypeKind::Enum:
            builder.Append(mComponent->mEnumerations[mt->mIndex]->mName);
            break;
        case como::TypeKind::Array:
            builder.AppendFormat("Array<%s>", DumpMetaType(mComponent->mTypes[mt->mIndex]).string());
            break;
        case como::TypeKind::Interface:
            builder.Append(mComponent->mInterfaces[mt->mIndex]->mName);
            break;
        case como::TypeKind::Coclass:
            builder.Append(mComponent->mCoclasses[mt->mIndex]->mName);
            break;
        case como::TypeKind::Triple:
            builder.Append("Triple");
            break;
        default:
            builder.Append("Unknown");
            break;
    }

    if ((mt->mProperties & TYPE_NUMBER_MASK) > 0) {
        int N = mt->mProperties & TYPE_NUMBER_MASK;
        for (int i = N; i >= 1; i--) {
            if ((mt->mProperties >> (i * 2)) & TYPE_POINTER) {
                builder.Append("*");
            }
            else {
                builder.Append("&");
            }
        }
    }

    if (mt->mProperties & TYPE_EXTERNAL) {
        char** externalPtr = reinterpret_cast<char**>(ALIGN((uintptr_t)mt + sizeof(como::MetaType)));
        builder.AppendFormat("(in %s)", *externalPtr);
    }

    return builder.ToString();
}

String MetadataDumper::DumpMetaValue(
    /* [in] */ como::MetaType* mt,
    /* [in] */ como::MetaValue* mv)
{
    StringBuilder builder;

    if (mt->mProperties & TYPE_POINTER) {
        if (mv->mIntegralValue == 0) {
            builder.Append("nullptr");
        }
        else {
            builder.AppendFormat("0x%16x", mv->mIntegralValue);
        }
        return builder.ToString();
    }

    switch (mt->mKind) {
        case como::TypeKind::Boolean:
            builder.Append(mv->mBooleanValue ? "true" : "false");
            break;
        case como::TypeKind::Char:
            builder.AppendFormat("%c", (char)mv->mIntegralValue);
            break;
        case como::TypeKind::Byte:
            builder.AppendFormat("%d", (unsigned char)mv->mIntegralValue);
            break;
        case como::TypeKind::Short:
            builder.AppendFormat("%d", (short)mv->mIntegralValue);
            break;
        case como::TypeKind::Integer:
            builder.AppendFormat("%d", (int)mv->mIntegralValue);
            break;
        case como::TypeKind::Long:
        case como::TypeKind::HANDLE:
            builder.AppendFormat("%lld", mv->mIntegralValue);
            break;
        case como::TypeKind::Float:
            builder.AppendFormat("%f", (float)mv->mFloatingPointValue);
            break;
        case como::TypeKind::Double:
            builder.AppendFormat("%e", mv->mFloatingPointValue);
            break;
        case como::TypeKind::String:
        case como::TypeKind::Enum:
            builder.Append(mv->mStringValue);
            break;
        default:
            break;
    }

    return builder.ToString();
}

String MetadataDumper::DumpUUID(
    /* [in] */ const como::UUID& uuid)
{
    String uuidStr = String::Format("%08x-%04x-%04x-%04x-%x%x%x%x%x%x%x%x%x%x%x%x",
            uuid.mData1, uuid.mData2, uuid.mData3, uuid.mData4, uuid.mData5[0], uuid.mData5[1],
            uuid.mData5[2], uuid.mData5[3], uuid.mData5[4], uuid.mData5[5], uuid.mData5[6],
            uuid.mData5[7], uuid.mData5[8], uuid.mData5[9], uuid.mData5[10], uuid.mData5[11]);
    return uuidStr;
}

}
