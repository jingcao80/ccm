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

#include "MetaDumper.h"
#include "../ast/Parameter.h"
#include "../util/StringBuilder.h"
#include "../util/Uuid.h"

using ccdl::ast::Parameter;
using ccm::CcmTypeKind;

namespace ccdl {
namespace metadata {

MetaDumper::MetaDumper()
    : mMetaComponet(nullptr)
{}

String MetaDumper::Dump(
    /* [in] */ MetaComponent* mc)
{
    if (mc == nullptr) return String();

    mMetaComponet = mc;
    return DumpMetaComponent(mMetaComponet, String(""));
}

String MetaDumper::DumpMetaComponent(
    /* [in] */ MetaComponent* mc,
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append("MetaComponent\n");
    builder.Append(prefix).Append("{\n");
    builder.Append(prefix).AppendFormat("    mMagic:0x%x\n", mc->mMagic);
    builder.Append(prefix).AppendFormat("    mSize:%d\n", mc->mSize);
    builder.Append(prefix).Append("    mUuid:").Append(Uuid(mc->mUuid).Dump()).Append("\n");
    builder.Append(prefix).Append("    mName:").Append(mc->mName).Append("\n");
    builder.Append(prefix).Append("    mUrl:").Append(mc->mUrl).Append("\n");
    builder.Append(prefix).AppendFormat("    mNamespaceNumber:%d\n", mc->mNamespaceNumber);
    builder.Append(prefix).AppendFormat("    mCoclassNumber:%d\n", mc->mCoclassNumber);
    builder.Append(prefix).AppendFormat("    mEnumerationNumber:%d\n", mc->mEnumerationNumber);
    builder.Append(prefix).AppendFormat("    mInterfaceNumber:%d\n", mc->mInterfaceNumber);
    builder.Append(prefix).AppendFormat("    mTypeNumber:%d\n", mc->mTypeNumber);
    builder.Append(prefix).Append("}\n");

    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        String dumpNS = DumpMetaNamespace(mc->mNamespaces[i], prefix + "  ");
        builder.Append(dumpNS);
    }

    for (int i = 0; i < mc->mEnumerationNumber; i++) {
        String dumpEnum = DumpMetaEnumeration(mc->mEnumerations[i], prefix + "  ");
        builder.Append(dumpEnum);
    }

    for (int i = 0; i < mc->mInterfaceNumber; i++) {
        if (mc->mInterfaces[i]->mExternal) continue;
        String dumpItf = DumpMetaInterface(mc->mInterfaces[i], prefix + "  ");
        builder.Append(dumpItf);
    }

    for (int i = 0; i < mc->mCoclassNumber; i++) {
        String dumpCls = DumpMetaCoclass(mc->mCoclasses[i], prefix + "  ");
        builder.Append(dumpCls);
    }

    for (int i = 0; i < mc->mConstantNumber; i++) {
        String dumpConst = DumpMetaConstant(mc->mConstants[i], prefix + "  ");
        builder.Append(dumpConst);
    }

    return builder.ToString();
}

String MetaDumper::DumpMetaCoclass(
    /* [in] */ MetaCoclass* mc,
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append("MetaCoclass\n");
    builder.Append(prefix).Append("{\n");
    builder.Append(prefix).Append("    mName:").Append(mc->mName).Append("\n");
    builder.Append(prefix).Append("    mNamespace:").Append(mc->mNamespace).Append("\n");
    builder.Append(prefix).AppendFormat("    mInterfaceNumber:%d\n", mc->mInterfaceNumber);
    for (int i = 0; i < mc->mInterfaceNumber; i++) {
        builder.Append(prefix).AppendFormat("        %s\n",
                mMetaComponet->mInterfaces[mc->mInterfaceIndexes[i]]->mName);
    }
    builder.Append(prefix).Append("}\n");

    return builder.ToString();
}

String MetaDumper::DumpMetaConstant(
    /* [in] */ MetaConstant* mc,
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append("MetaConstant\n");
    builder.Append(prefix).Append("{\n");
    builder.Append(prefix).Append("    mName:").Append(mc->mName).Append("\n");
    builder.Append(prefix).Append("    mType:").Append(
            DumpMetaType(mMetaComponet->mTypes[mc->mTypeIndex])).Append("\n");
    builder.Append(prefix).Append("    mValue:").Append(
            DumpConstantValue(mc)).Append("\n");
    if (mc->mValue.mAttributes & RADIX_MASK) {
        builder.Append(prefix).AppendFormat("    mRadix:%d\n", mc->mValue.mAttributes & RADIX_MASK);
    }
    builder.Append(prefix).Append("}\n");

    return builder.ToString();
}

String MetaDumper::DumpConstantValue(
    /* [in] */ MetaConstant* mc)
{
    StringBuilder builder;

    MetaType* mt = mMetaComponet->mTypes[mc->mTypeIndex];
    switch(mt->mKind) {
        case CcmTypeKind::Char:
            builder.AppendFormat("%c", mc->mValue.mInteger);
            break;
        case CcmTypeKind::Byte:
            builder.AppendFormat("%d", (unsigned char)mc->mValue.mInteger);
            break;
        case CcmTypeKind::Short:
            builder.AppendFormat("%d", (short)mc->mValue.mInteger);
            break;
        case CcmTypeKind::Integer:
            builder.AppendFormat("%d", mc->mValue.mInteger);
            break;
        case CcmTypeKind::Long:
            builder.AppendFormat("%lld", mc->mValue.mLong);
            break;
        case CcmTypeKind::Float:
            builder.AppendFormat("%f", mc->mValue.mFloat);
            break;
        case CcmTypeKind::Double:
            builder.AppendFormat("%e", mc->mValue.mDouble);
            break;
        case CcmTypeKind::Boolean:
            builder.Append(mc->mValue.mBoolean ? "true" : "false");
            break;
        case CcmTypeKind::String:
            builder.Append(mc->mValue.mString);
            break;
        case CcmTypeKind::Enum: {
            builder.Append(mc->mValue.mString);
            break;
        }
        default:
            break;
    }

    return builder.ToString();
}

String MetaDumper::DumpMetaEnumeration(
    /* [in] */ MetaEnumeration* me,
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append("MetaEnumeration\n");
    builder.Append(prefix).Append("{\n");
    builder.Append(prefix).Append("    mName:").Append(me->mName).Append("\n");
    builder.Append(prefix).Append("    mNamespace:").Append(me->mNamespace).Append("\n");
    builder.Append(prefix).AppendFormat("    mEnumeratorNumber:%d\n", me->mEnumeratorNumber);
    for (int i = 0; i < me->mEnumeratorNumber; i++) {
        builder.Append(prefix).AppendFormat("        %s = %d\n",
                me->mEnumerators[i]->mName, me->mEnumerators[i]->mValue);
    }
    builder.Append(prefix).Append("}\n");

    return builder.ToString();
}

String MetaDumper::DumpMetaInterface(
    /* [in] */ MetaInterface* mi,
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append("MetaInterface\n");
    builder.Append(prefix).Append("{\n");
    builder.Append(prefix).Append("    mName:").Append(mi->mName).Append("\n");
    builder.Append(prefix).Append("    mNamespace:").Append(mi->mNamespace).Append("\n");
    builder.Append(prefix).AppendFormat("    mConstantNumber:%d\n", mi->mConstantNumber);
    builder.Append(prefix).AppendFormat("    mMethodNumber:%d\n", mi->mMethodNumber);
    builder.Append(prefix).Append("}\n");

    for (int i = 0; i < mi->mConstantNumber; i++) {
        String dumpConst = DumpMetaConstant(mi->mConstants[i], prefix + "  ");
        builder.Append(dumpConst);
    }

    for (int i = 0; i < mi->mMethodNumber; i++) {
        String dumpMeth = DumpMetaMethod(mi->mMethods[i], prefix + "  ");
        builder.Append(dumpMeth);
    }

    return builder.ToString();
}

String MetaDumper::DumpMetaMethod(
    /* [in] */ MetaMethod* mm,
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append("MetaMethod\n");
    builder.Append(prefix).Append("{\n");
    builder.Append(prefix).Append("    mName:").Append(mm->mName).Append("\n");
    builder.Append(prefix).Append("    mSignature:").Append(mm->mSignature).Append("\n");
    builder.Append(prefix).AppendFormat("    mParameterNumber:%d\n", mm->mParameterNumber);
    for (int i = 0; i < mm->mParameterNumber; i++) {
        builder.Append(prefix).AppendFormat("        %s\n",
                DumpMetaParameter(mm->mParameters[i]).string());
    }
    builder.Append(prefix).Append("}\n");

    return builder.ToString();
}

String MetaDumper::DumpMetaNamespace(
    /* [in] */ MetaNamespace* mn,
    /* [in] */ const String& prefix)
{
    StringBuilder builder;

    builder.Append(prefix).Append("MetaNamespace\n");
    builder.Append(prefix).Append("{\n");
    builder.Append(prefix).Append("    mName:").Append(mn->mName).Append("\n");
    builder.Append(prefix).AppendFormat("    mCoclassNumber:%d\n", mn->mCoclassNumber);
    for (int i = 0; i < mn->mCoclassNumber; i++) {
        builder.Append(prefix).AppendFormat("        %s\n",
                mMetaComponet->mCoclasses[mn->mCoclassIndexes[i]]->mName);
    }
    builder.Append(prefix).AppendFormat("    mEnumerationNumber:%d\n", mn->mEnumerationNumber);
    for (int i = 0; i < mn->mEnumerationNumber; i++) {
        builder.Append(prefix).AppendFormat("        %s\n",
                mMetaComponet->mEnumerations[mn->mEnumerationIndexes[i]]->mName);
    }
    builder.Append(prefix).AppendFormat("    mInterfaceNumber:%d\n", mn->mInterfaceNumber);
    for (int i = 0; i < mn->mInterfaceNumber; i++) {
        builder.Append(prefix).AppendFormat("        %s\n",
                mMetaComponet->mInterfaces[mn->mInterfaceIndexes[i]]->mName);
    }
    builder.Append(prefix).Append("}\n");

    return builder.ToString();
}

String MetaDumper::DumpMetaParameter(
    /* [in] */ MetaParameter* mp)
{
    StringBuilder builder;

    builder.Append("mName:").Append(DumpMetaType(mMetaComponet->mTypes[mp->mTypeIndex], mp->mAttribute));
    builder.Append("[");
    bool needComma = false;
    if ((mp->mAttribute & Parameter::IN) != 0) {
        builder.Append("in");
        needComma = true;
    }
    if ((mp->mAttribute & Parameter::OUT) != 0) {
        if (needComma) builder.Append(",");
        builder.Append("out");
        needComma = true;
    }
    if ((mp->mAttribute & Parameter::CALLEE) != 0) {
        if (needComma) builder.Append(",");
        builder.Append("callee");
    }
    builder.Append("]");

    return builder.ToString();
}

String MetaDumper::DumpMetaType(
    /* [in] */ MetaType* mt,
    /* [in] */ int attr)
{
    StringBuilder builder;

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
        case CcmTypeKind::CoclassID:
            if ((attr & Parameter::ATTR_MASK) == Parameter::IN) {
                builder.Append("const CoclassID&");
            }
            else if ((attr & Parameter::ATTR_MASK) == Parameter::OUT) {
                builder.Append("CoclassID");
            }
            break;
        case CcmTypeKind::ComponentID:
            if ((attr & Parameter::ATTR_MASK) == Parameter::IN) {
                builder.Append("const ComponentID&");
            }
            else if ((attr & Parameter::ATTR_MASK) == Parameter::OUT) {
                builder.Append("ComponentID");
            }
            break;
        case CcmTypeKind::InterfaceID:
            if ((attr & Parameter::ATTR_MASK) == Parameter::IN) {
                builder.Append("const InterfaceID&");
            }
            else if ((attr & Parameter::ATTR_MASK) == Parameter::OUT) {
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
            builder.Append(mMetaComponet->mEnumerations[mt->mIndex]->mName);
            break;
        case CcmTypeKind::Array:
            if ((attr & Parameter::ATTR_MASK) == Parameter::IN) {
                builder.AppendFormat("const Array<%s>&",
                    DumpMetaType(mMetaComponet->mTypes[mt->mNestedTypeIndex], attr).string());
            }
            else if ((attr & Parameter::ATTR_MASK) == Parameter::OUT) {
                builder.AppendFormat("Array<%s>&",
                    DumpMetaType(mMetaComponet->mTypes[mt->mNestedTypeIndex], attr).string());
            }
            else if ((attr & Parameter::ATTR_MASK) == (Parameter::OUT | Parameter::CALLEE)) {
                builder.AppendFormat("Array<%s>",
                    DumpMetaType(mMetaComponet->mTypes[mt->mNestedTypeIndex], attr).string());
            }
            break;
        case CcmTypeKind::Interface:
            builder.Append(mMetaComponet->mInterfaces[mt->mIndex]->mName);
            break;
        case CcmTypeKind::Triple:
            builder.Append("Triple");
            break;
        default:
            break;
    }

    for (int i = 0; i < mt->mPointerNumber; i++) {
        builder.Append("*");
    }

    return builder.ToString();
}

}
}
