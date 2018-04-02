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

#ifndef __CCDL_METADATA_BUILDER_H__
#define __CCDL_METADATA_BUILDER_H__

#include "../../runtime/metadata/Component.h"
#include "../ast/Coclass.h"
#include "../ast/Constant.h"
#include "../ast/Enumeration.h"
#include "../ast/Interface.h"
#include "../ast/Method.h"
#include "../ast/Module.h"
#include "../ast/Namespace.h"
#include "../ast/Parameter.h"
#include "../ast/Type.h"
#include "../util/String.h"
#include "../util/StringPool.h"

#include <memory>

using ccdl::ast::Coclass;
using ccdl::ast::Constant;
using ccdl::ast::Enumeration;
using ccdl::ast::Interface;
using ccdl::ast::Method;
using ccdl::ast::Module;
using ccdl::ast::Namespace;
using ccdl::ast::Parameter;
using ccdl::ast::Type;
using ccm::metadata::MetaCoclass;
using ccm::metadata::MetaComponent;
using ccm::metadata::MetaConstant;
using ccm::metadata::MetaEnumeration;
using ccm::metadata::MetaEnumerator;
using ccm::metadata::MetaInterface;
using ccm::metadata::MetaMethod;
using ccm::metadata::MetaNamespace;
using ccm::metadata::MetaParameter;
using ccm::metadata::MetaType;
using ccm::metadata::CcdlType;

namespace ccdl {
namespace metadata {

class MetaBuilder
{
public:
    MetaBuilder(
        /* [in] */ std::shared_ptr<Module> module)
        : mModule(module)
        , mBasePtr(0)
    {}

    bool IsValidate();

    std::shared_ptr<MetaComponent> Build();

private:
    size_t CalculateMetadataSize();

    void CalculateMetaComponent(
        /* [in] */ Module* module);

    void CalculateMetaCoclass(
        /* [in] */ Coclass* klass);

    void CalculateMetaEnumeration(
        /* [in] */ Enumeration* enumn);

    void CalculateMetaEnumerator(
        /* [in] */ Enumeration::Enumerator* enumr);

    void CalculateMetaInterface(
        /* [in] */ Interface* itf);

    void CalculateMetaConstant(
        /* [in] */ Constant* constant);

    void CalculateMetaMethod(
        /* [in] */ Method* method);

    void CalculateMetaNamespace(
        /* [in] */ Namespace* ns);

    void CalculateMetaParameter(
        /* [in] */ Parameter* param);

    void CalculateMetaType(
        /* [in] */ Type* type);

    void CalculateStringPool();

    void WriteMetadata(
        /* [in] */ uintptr_t base);

    void WriteMetaComponent(
        /* [in] */ Module* module);

    MetaCoclass* WriteMetaCoclass(
        /* [in] */ Coclass* klass);

    MetaConstant* WriteMetaConstant(
        /* [in] */ Constant* constant);

    MetaEnumeration* WriteMetaEnumeration(
        /* [in] */ Enumeration* enumn);

    MetaEnumerator* WriteMetaEnumerator(
        /* [in] */ Enumeration::Enumerator* enumr);

    MetaInterface* WriteMetaInterface(
        /* [in] */ Interface* itf);

    MetaMethod* WriteMetaMethod(
        /* [in] */ Method* method);

    MetaNamespace* WriteMetaNamespace(
        /* [in] */ Namespace* ns);

    MetaParameter* WriteMetaParameter(
        /* [in] */ Parameter* param);

    MetaType* WriteMetaType(
        /* [in] */ Type* type);

    char* WriteString(
        /* [in] */ const String& string);

    CcdlType Type2CcdlType(
        /* [in] */ Type* type);

private:
    static const String TAG;
    std::shared_ptr<Module> mModule;
    std::shared_ptr<MetaComponent> mMetaComponet;
    uintptr_t mBasePtr;
    int mSize;
    StringPool mStringPool;
};

}
}

#endif // __CCDL_METADATA_BUILDER_H__
