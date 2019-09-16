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

#ifndef __CDLC_METADATABUILDER_H__
#define __CDLC_METADATABUILDER_H__

#include "ast/CoclassType.h"
#include "ast/Constant.h"
#include "ast/EnumerationType.h"
#include "ast/Expression.h"
#include "ast/InterfaceType.h"
#include "ast/Method.h"
#include "ast/Module.h"
#include "ast/Namespace.h"
#include "ast/Parameter.h"
#include "ast/Type.h"
#include "util/AutoPtr.h"
#include "util/StringPool.h"
#include "runtime/metadata/Component.h"
#include <memory>

namespace cdlc {

class MetadataBuilder
{
public:
    inline MetadataBuilder(
        /* [in] */ AutoPtr<Module> module);

    std::shared_ptr<como::MetaComponent> Build();

private:
    size_t CalculateMetadata();

    void CalculateMetaComponent(
        /* [in] */ Module* module);

    void CalculateMetaNamespace(
        /* [in] */ Namespace* ns);

    void CalculateMetaConstant(
        /* [in] */ Constant* constant);

    void CalculateMetaCoclass(
        /* [in] */ CoclassType* klass);

    void CalculateMetaEnumeration(
        /* [in] */ EnumerationType* enumeration);

    void CalculateMetaEnumerator(
        /* [in] */ EnumerationType::Enumerator* enumerator);

    void CalculateMetaInterface(
        /* [in] */ InterfaceType* interface);

    void CalculateMetaMethod(
        /* [in] */ Method* method);

    void CalculateMetaParameter(
        /* [in] */ Parameter* parameter);

    void CalculateMetaValue(
        /* [in] */ Parameter* parameter);

    void CalculateMetaType(
        /* [in] */ Type* type);

    void CalculateStringPool();

    void WriteMetadata(
        /* [in] */ uintptr_t base);

    void WriteMetaComponent(
        /* [in] */ Module* module);

    como::MetaNamespace* WriteMetaNamespace(
        /* [in] */ Namespace* ns);

    como::MetaConstant* WriteMetaConstant(
        /* [in] */ Constant* constant);

    como::MetaCoclass* WriteMetaCoclass(
        /* [in] */ CoclassType* klass);

    como::MetaEnumeration* WriteMetaEnumeration(
        /* [in] */ EnumerationType* enumeration);

    como::MetaEnumerator* WriteMetaEnumerator(
        /* [in] */ EnumerationType::Enumerator* enumerator);

    como::MetaInterface* WriteMetaInterface(
        /* [in] */ InterfaceType* interface);

    como::MetaMethod* WriteMetaMethod(
        /* [in] */ Method* method);

    como::MetaParameter* WriteMetaParameter(
        /* [in] */ Parameter* parameter);

    como::MetaValue* WriteMetaValue(
        /* [in] */ Parameter* parameter);

    como::MetaType* WriteMetaType(
        /* [in] */ Type* type);

    char* WriteString(
        /* [in] */ const String& string);

    static void AssignFrom(
        /* [out]*/ como::UUID& target,
        /* [in] */ cdlc::UUID* src);

    static como::TypeKind ToTypeKind(
        /* [in] */ Type* type);

private:
    static const char* TAG;

    AutoPtr<Module> mModule;
    std::shared_ptr<como::MetaComponent> mMetaComponent;
    uintptr_t mBasePtr = 0;
    size_t mSize = 0;
    StringPool mPool;
};

MetadataBuilder::MetadataBuilder(
    /* [in] */ AutoPtr<Module> module)
    : mModule(module)
{}

}

#endif // __CDLC_METADATABUILDER_H__
