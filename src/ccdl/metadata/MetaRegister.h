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

#ifndef __CCDL_METADATA_METAREGISTER_H__
#define __CCDL_METADATA_METAREGISTER_H__

#include "../ast/Namespace.h"
#include "../ast/Pool.h"
#include "../../runtime/metadata/Component.h"

using ccdl::ast::Method;
using ccdl::ast::Namespace;
using ccdl::ast::Parameter;
using ccdl::ast::Pool;
using ccdl::ast::Type;

using ccm::metadata::MetaComponent;
using ccm::metadata::MetaInterface;
using ccm::metadata::MetaMethod;
using ccm::metadata::MetaParameter;
using ccm::metadata::MetaType;

namespace ccdl {
namespace metadata {

class MetaRegister
{
public:
    MetaRegister(
        /* [in] */ Pool* pool,
        /* [in] */ void* metadata);

    bool Register();

private:
    void RegisterInterface(
        /* [in] */ MetaInterface* mi);

    Namespace* RegisterNamespace(
        /* [in] */ const String& ns);

    Method* BuildMethod(
        /* [in] */ MetaMethod* mm);

    Parameter* BuildParameter(
        /* [in] */ MetaParameter* mp);

    Type* BuildType(
        /* [in] */ MetaType* mt);

private:
    Pool* mPool;
    MetaComponent* mMetaComponent;
};

}
}

#endif // __CCDL_METADATA_METAREGISTER_H__
