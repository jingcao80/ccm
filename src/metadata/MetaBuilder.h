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

#ifndef __CCM_METADATA_BUILDER_H__
#define __CCM_METADATA_BUILDER_H__

#include "Component.h"
#include "../ccdl/Coclass.h"
#include "../ccdl/Constant.h"
#include "../ccdl/Enumeration.h"
#include "../ccdl/Interface.h"
#include "../ccdl/Method.h"
#include "../ccdl/Module.h"
#include "../ccdl/Namespace.h"
#include "../ccdl/Parameter.h"
#include "../ccdl/Type.h"
#include "../util/String.h"
#include "../util/StringPool.h"

#include <memory>

using ccm::ccdl::Coclass;
using ccm::ccdl::Constant;
using ccm::ccdl::Enumeration;
using ccm::ccdl::Interface;
using ccm::ccdl::Method;
using ccm::ccdl::Module;
using ccm::ccdl::Namespace;
using ccm::ccdl::Parameter;
using ccm::ccdl::Type;

namespace ccm {
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

private:
    static const String TAG;
    std::shared_ptr<Module> mModule;
    std::shared_ptr<MetaComponent> mMetaComponet;
    uintptr_t mBasePtr;
    StringPool mStringPool;
};

}
}

#endif // __CCM_METADATA_BUILDER_H__
